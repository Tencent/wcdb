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

#include <WCDB/HandlePool.hpp>
#include <algorithm>
#include <thread>
#include <unordered_map>
#include <WCDB/BuiltinConfig.hpp>

namespace WCDB {

std::unordered_map<std::string, std::pair<std::shared_ptr<HandlePool>, int>>
    HandlePool::s_pools;
std::mutex HandlePool::s_mutex;
const int HandlePool::s_hardwareConcurrency =
    std::thread::hardware_concurrency();
const int HandlePool::s_maxConcurrency =
    std::max<int>(std::thread::hardware_concurrency(), 64);

RecyclableHandlePool HandlePool::GetPool(const std::string &path)
{
    std::shared_ptr<HandlePool> pool = nullptr;
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_pools.find(path);
    if (iter == s_pools.end()) {
        pool.reset(new HandlePool(path, BuiltinConfigs::default_));
        iter = s_pools.insert({path, {pool, 0}}).first;
    }
    return HandlePool::GetExistingPool(iter);
}

RecyclableHandlePool HandlePool::GetExistingPool(Tag tag)
{
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_pools.end();
    if (tag != InvalidTag) {
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
    , tag(InvalidTag)
    , m_configs(configs)
    , m_handles(s_hardwareConcurrency)
    , m_aliveHandleCount(0)
{
}
    
HandlePool::~HandlePool()
{
    drain(nullptr);
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

void HandlePool::drain(const HandlePool::OnDrained& onDrained)
{
    m_rwlock.lockWrite();
    int size = (int) m_handles.clear();
    m_aliveHandleCount -= size;
    if (onDrained) {
        onDrained();
    }
    BuiltinConfig::removeKeyFromTimedQueue(path);
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

RecyclableHandle HandlePool::flowOut(Error &error)
{
    m_rwlock.lockRead();
    std::shared_ptr<HandleWrap> handleWrap = m_handles.popBack();
    if (handleWrap == nullptr) {
        if (m_aliveHandleCount < s_maxConcurrency) {
            handleWrap = generate(error);
            if (handleWrap) {
                ++m_aliveHandleCount;
                if (m_aliveHandleCount > s_hardwareConcurrency) {
                    WCDB::Error::Warning(
                        ("The concurrency of database:" +
                         std::to_string(tag.load()) + " with " +
                         std::to_string(m_aliveHandleCount) +
                         " exceeds the concurrency of hardware:" +
                         std::to_string(s_hardwareConcurrency))
                            .c_str());
                }
            }
        } else {
            Error::ReportCore(
                tag.load(), path, Error::CoreOperation::FlowOut,
                Error::CoreCode::Exceed,
                "The concurrency of database exceeds the max concurrency",
                &error);
        }
    }
    if (handleWrap) {
        handleWrap->handle->setTag(tag.load());
        if (invoke(handleWrap, error)) {
            return RecyclableHandle(
                handleWrap, [handleWrap, this]() { flowBack(handleWrap); });
        }
    }

    handleWrap = nullptr;
    m_rwlock.unlockRead();
    return RecyclableHandle(nullptr, nullptr);
}

void HandlePool::flowBack(const std::shared_ptr<HandleWrap> &handleWrap)
{
    if (handleWrap) {
        bool inserted = m_handles.pushBack(handleWrap);
        m_rwlock.unlockRead();
        if (!inserted) {
            --m_aliveHandleCount;
        }
    }
}

std::shared_ptr<HandleWrap> HandlePool::generate(Error &error)
{
    std::shared_ptr<Handle> handle(new Handle(path));
    handle->setTag(tag.load());
    Configs defaultConfigs =
        m_configs; //cache config to avoid multi-thread assigning
    if (!handle->open()) {
        error = handle->getError();
        return nullptr;
    }
    if (defaultConfigs.invoke(handle, error)) {
        return std::shared_ptr<HandleWrap>(
            new HandleWrap(handle, defaultConfigs));
    }
    return nullptr;
}

bool HandlePool::fillOne(Error &error)
{
    m_rwlock.lockRead();
    std::shared_ptr<HandleWrap> handleWrap = generate(error);
    bool result = false;
    if (handleWrap) {
        result = true;
        bool inserted = m_handles.pushBack(handleWrap);
        if (inserted) {
            ++m_aliveHandleCount;
        }
    }
    m_rwlock.unlockRead();
    return result;
}

bool HandlePool::invoke(std::shared_ptr<HandleWrap> &handleWrap, Error &error)
{
    Configs newConfigs =
        m_configs; //cache config to avoid multi-thread assigning
    if (newConfigs != handleWrap->configs) {
        if (!newConfigs.invoke(handleWrap->handle, error)) {
            return false;
        }
        handleWrap->configs = newConfigs;
    }
    error.reset();
    return true;
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

} //namespace WCDB
