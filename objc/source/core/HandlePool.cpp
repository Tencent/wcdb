/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <WCDB/BuiltinConfig.hpp>
#include <WCDB/HandlePool.hpp>
#include <algorithm>
#include <thread>
#include <unordered_map>

namespace WCDB {

std::unordered_map<std::string, std::pair<std::shared_ptr<HandlePool>, int>>
    HandlePool::s_pools;
std::mutex HandlePool::s_mutex;
const int HandlePool::s_hardwareConcurrency =
    std::thread::hardware_concurrency();
const int HandlePool::s_maxConcurrency =
    std::max<int>(std::thread::hardware_concurrency(), 64);
ThreadLocal<std::unordered_map<const HandlePool *, Error>>
    HandlePool::s_threadedErrors;

RecyclableHandlePool HandlePool::GetPool(const std::string &path)
{
    std::shared_ptr<HandlePool> pool = nullptr;
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_pools.find(path);
    if (iter == s_pools.end()) {
        pool.reset(new HandlePool(path, BuiltinConfigs::defaultConfig()));
        iter = s_pools.insert({path, {pool, 0}}).first;
    }
    return HandlePool::GetExistingPool(iter);
}

RecyclableHandlePool HandlePool::GetExistingPool(Tag tag)
{
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_pools.end();
    if (tag != Tag::Invalid) {
        for (iter = s_pools.begin(); iter != s_pools.end(); ++iter) {
            if (iter->second.first->tag == tag) {
                break;
            }
        }
    }
    return HandlePool::GetExistingPool(iter);
}

RecyclableHandlePool HandlePool::GetExistingPool(const std::string &path)
{
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_pools.begin();
    for (; iter != s_pools.end(); ++iter) {
        if (iter->second.first->path == path) {
            break;
        }
    }
    return HandlePool::GetExistingPool(iter);
}

RecyclableHandlePool HandlePool::GetExistingPool(
    const std::unordered_map<
        std::string,
        std::pair<std::shared_ptr<HandlePool>, int>>::iterator &iter)
{
    if (iter == s_pools.end()) {
        return RecyclableHandlePool(nullptr, nullptr);
    }
    ++iter->second.second;
    const std::string path = iter->second.first->path;
    return RecyclableHandlePool(iter->second.first, [path]() {
        std::lock_guard<std::mutex> lockGuard(s_mutex);
        const auto &iter = s_pools.find(path);
        if (iter == s_pools.end()) {
            Error::Abort("It should not be failed. If you think it's a bug, "
                         "please report an issue to us.");
        }
        if (--iter->second.second == 0) {
            s_pools.erase(iter);
        }
    });
}

void HandlePool::PurgeFreeHandlesInAllPool()
{
    std::list<std::shared_ptr<HandlePool>> handlePools;
    {
        std::lock_guard<std::mutex> lockGuard(s_mutex);
        for (const auto &iter : s_pools) {
            handlePools.push_back(iter.second.first);
        }
    }
    for (const auto &handlePool : handlePools) {
        handlePool->purgeFreeHandles();
    }
}

HandlePool::HandlePool(const std::string &thePath, const Configs &configs)
    : path(thePath)
    , tag(Tag::Invalid)
    , m_configs(configs)
    , m_handles(s_hardwareConcurrency)
    , m_aliveHandleCount(0)
{
}

HandlePool::~HandlePool()
{
    m_rwlock.lockWrite();
    BuiltinConfig::removeKeyFromTimedQueue(path);
    m_handles.clear();
    m_rwlock.unlockWrite();
}

void HandlePool::blockade()
{
    m_rwlock.lockWrite();
}

void HandlePool::unblockade()
{
    m_rwlock.unlockWrite();
}

bool HandlePool::isBlockaded() const
{
    return m_rwlock.isWriting();
}

void HandlePool::drain(const HandlePool::OnDrained &onDrained)
{
    m_rwlock.lockWrite();
    BuiltinConfig::removeKeyFromTimedQueue(path);
    ConcurrentList<ConfiguredHandle>::ElementType handle;
    while ((handle = m_handles.popBack())) {
        handle->getHandle()->close();
        --m_aliveHandleCount;
    }
    if (onDrained) {
        onDrained();
    }
    m_rwlock.unlockWrite();
}

void HandlePool::purgeFreeHandles()
{
    m_rwlock.lockRead();
    int size = (int) m_handles.clear();
    m_aliveHandleCount -= size;
    m_rwlock.unlockRead();
}

bool HandlePool::isDrained()
{
    return m_aliveHandleCount == 0;
}

RecyclableHandle HandlePool::flowOut()
{
    m_rwlock.lockRead();
    std::shared_ptr<ConfiguredHandle> configuredHandle = m_handles.popBack();
    if (!configuredHandle) {
        configuredHandle = generate();
    }
    if (configuredHandle) {
        Configs configs = m_configs;
        if (configuredHandle->configured(configs) ||
            configuredHandle->configure(m_configs)) {
            return RecyclableHandle(
                configuredHandle,
                [configuredHandle, this]() { flowBack(configuredHandle); });
        }
        setAndReportThreadedError(configuredHandle->getHandle()->getError());
    }
    --m_aliveHandleCount;
    m_rwlock.unlockRead();
    return RecyclableHandle(nullptr, nullptr);
}

void HandlePool::flowBack(
    const std::shared_ptr<ConfiguredHandle> &configuredHandle)
{
    if (configuredHandle) {
        bool inserted = m_handles.pushBack(configuredHandle);
        m_rwlock.unlockRead();
        if (!inserted) {
            --m_aliveHandleCount;
        }
    }
}

std::shared_ptr<ConfiguredHandle> HandlePool::generate()
{
    if (m_aliveHandleCount >= s_maxConcurrency) {
        Error error;
        error.setPath(path);
        error.setMessage(
            "The concurrency of database exceeds the max concurrency");
        setAndReportThreadedError(error);
        return nullptr;
    }
    std::shared_ptr<ConfiguredHandle> configuredHandle =
        ConfiguredHandle::configuredHandleWithPath(path);
    if (configuredHandle != nullptr) {
        ++m_aliveHandleCount;
        if (m_aliveHandleCount > s_hardwareConcurrency) {
            Error::Warning(
                ("The concurrency of database:" + std::to_string(tag.load()) +
                 " exceeds the concurrency of hardware.")
                    .c_str());
        }
    } else {
        Error error;
        error.setPath(path);
        error.setMessage("Out Of Memory");
        setAndReportThreadedError(error);
    }
    return configuredHandle;
}

bool HandlePool::fillOne()
{
    m_rwlock.lockRead();
    bool inserted = false;
    std::shared_ptr<ConfiguredHandle> configuredHandle = generate();
    //TODO
    if (configuredHandle) {
        inserted = m_handles.pushBack(configuredHandle);
        if (inserted) {
            ++m_aliveHandleCount;
        }
    }
    m_rwlock.unlockRead();
    return inserted;
}

void HandlePool::setConfig(const std::string &name,
                           const Config::Callback &callback)
{
    m_configs.setConfig(name, callback);
}

void HandlePool::setConfig(const Config &config)
{
    m_configs.setConfig(config);
}

void HandlePool::setAndReportThreadedError(const Error &error) const
{
    std::unordered_map<const HandlePool *, Error> *errors =
        s_threadedErrors.get();
    auto iter = errors->find(this);
    if (iter == errors->end()) {
        iter = errors->insert({this, error}).first;
    } else {
        iter->second = error;
    }
    iter->second.setTag(tag.load());
    iter->second.report();
}

void HandlePool::resetThreadedError() const
{
    std::unordered_map<const HandlePool *, Error> *errors =
        s_threadedErrors.get();
    auto iter = errors->find(this);
    if (iter == errors->end()) {
        iter = errors->insert({this, Error()}).first;
    }
    iter->second.reset();
}

const Error &HandlePool::getThreadedError() const
{
    std::unordered_map<const HandlePool *, Error> *errors =
        s_threadedErrors.get();
    auto iter = errors->find(this);
    assert(iter != errors->end());
    return iter->second;
}

} //namespace WCDB
