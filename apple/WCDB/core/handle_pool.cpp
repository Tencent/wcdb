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

#include <WCDB/handle.hpp>
#include <WCDB/handle_pool.hpp>
#include <thread>
#include <unordered_map>

namespace WCDB {

std::unordered_map<std::string, std::pair<std::shared_ptr<HandlePool>, int>>
    HandlePool::s_pools;
std::mutex HandlePool::s_mutex;
const int HandlePool::s_hardwareConcurrency =
    std::thread::hardware_concurrency();
const int HandlePool::s_maxConcurrency = 64;

RecyclableHandlePool HandlePool::GetPool(const std::string &path,
                                         const Configs &defaultConfigs)
{
    std::shared_ptr<HandlePool> pool = nullptr;
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    auto iter = s_pools.find(path);
    if (iter == s_pools.end()) {
        pool.reset(new HandlePool(path, defaultConfigs));
        s_pools.insert({path, {pool, 1}});
    } else {
        pool = iter->second.first;
        ++iter->second.second;
    }
    //Can I store iter of unordered_map for later use?
    return RecyclableHandlePool(pool, [](std::shared_ptr<HandlePool> &pool) {
        std::lock_guard<std::mutex> lockGuard(s_mutex);
        const auto &iter = s_pools.find(pool->path);
        if (--iter->second.second == 0) {
            s_pools.erase(iter);
        }
    });
}

RecyclableHandlePool HandlePool::GetPool(Tag tag)
{
    std::lock_guard<std::mutex> lockGuard(s_mutex);
    if (tag != InvalidTag) {
        for (auto iter : s_pools) {
            if (iter.second.first->tag == tag) {
                ++iter.second.second;
                return RecyclableHandlePool(
                    iter.second.first, [](std::shared_ptr<HandlePool> &pool) {
                        std::lock_guard<std::mutex> lockGuard(s_mutex);
                        const auto &iter = s_pools.find(pool->path);
                        if (--iter->second.second == 0) {
                            s_pools.erase(iter);
                        }
                    });
            }
        }
    }
    return RecyclableHandlePool(nullptr, nullptr);
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

void HandlePool::drain(HandlePool::OnDrained onDrained)
{
    m_rwlock.lockWrite();
    int size = (int) m_handles.clear();
    m_aliveHandleCount -= size;
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
                handleWrap, [this](std::shared_ptr<HandleWrap> &handleWrap) {
                    flowBack(handleWrap);
                });
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
                           const Config &config,
                           Configs::Order order)
{
    m_configs.setConfig(name, config, order);
}

void HandlePool::setConfig(const std::string &name, const Config &config)
{
    m_configs.setConfig(name, config);
}

} //namespace WCDB
