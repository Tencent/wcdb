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

#include <WCDB/Core.h>
#include <WCDB/FileManager.hpp>

namespace WCDB {

HandlePools *HandlePools::defaultPools()
{
    static HandlePools *s_handlePools = new HandlePools;
    return s_handlePools;
}

HandlePools::HandlePools()
{
    //init
    SQLiteGlobal::shared();
}

RecyclableHandlePool HandlePools::getPool(const std::string &path,
                                          const Generator &generator)
{
    std::shared_ptr<HandlePool> pool = nullptr;
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    auto iter = m_pools.find(path);
    bool generate = false;
    if (iter != m_pools.end()) {
        //check path is still sanity
        auto pair = FileManager::shared()->isExists(path);
        if (!pair.first) {
            return nullptr;
        }
        if (!pair.second && !iter->second.first->isDrained()) {
            m_pools.erase(iter);
            generate = true;
        }
    } else {
        generate = true;
    }
    if (generate) {
        pool = generator(path);
        if (pool == nullptr) {
            return nullptr;
        }
        iter = m_pools.insert({path, {pool, 0}}).first;
    }
    return getExistingPool(iter);
}

RecyclableHandlePool HandlePools::getExistingPool(HandlePool::Tag tag)
{
    WCTAssert(tag != Handle::invalidTag, "Tag invalid");
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    auto iter = m_pools.end();
    for (iter = m_pools.begin(); iter != m_pools.end(); ++iter) {
        if (iter->second.first->getTag() == tag) {
            break;
        }
    }
    return getExistingPool(iter);
}

RecyclableHandlePool HandlePools::getExistingPool(const std::string &path)
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    auto iter = m_pools.begin();
    for (; iter != m_pools.end(); ++iter) {
        if (iter->second.first->path == path) {
            break;
        }
    }
    return getExistingPool(iter);
}

RecyclableHandlePool HandlePools::getExistingPool(
    const std::unordered_map<
        std::string,
        std::pair<std::shared_ptr<HandlePool>, int>>::iterator &iter)
{
    if (iter == m_pools.end()) {
        return nullptr;
    }
    ++iter->second.second;
    const std::string path = iter->second.first->path;
    return RecyclableHandlePool(iter->second.first, [path, this]() {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        const auto &iter = m_pools.find(path);
        if (iter == m_pools.end()) {
            //drop it
            return;
        }
        if (--iter->second.second == 0) {
            m_pools.erase(iter);
        }
    });
}

void HandlePools::purge()
{
    std::list<std::shared_ptr<HandlePool>> handlePools;
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        for (const auto &iter : m_pools) {
            handlePools.push_back(iter.second.first);
        }
    }
    for (const auto &handlePool : handlePools) {
        handlePool->purgeFreeHandles();
    }
}

void HandlePools::drain()
{
    std::list<std::shared_ptr<HandlePool>> handlePools;
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);
        for (const auto &iter : m_pools) {
            handlePools.push_back(iter.second.first);
        }
    }
    for (const auto &handlePool : handlePools) {
        handlePool->drain(nullptr);
    }
}

} //namespace WCDB
