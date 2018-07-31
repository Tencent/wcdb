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

#include <WCDB/Assertion.hpp>
#include <WCDB/Core.h>
#include <WCDB/CorruptionNotifier.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>

namespace WCDB {

HandlePools *HandlePools::defaultPools()
{
    static HandlePools *s_handlePools = new HandlePools;
    return s_handlePools;
}

HandlePools::HandlePools()
{
    SQLiteGlobal::shared()->setNotificationWhenFileCreated([](const char *path) {
        if (!path) {
            return;
        }
        FileManager::shared()->setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
    });
    CorruptionNotifier::shared();
}

RecyclableHandlePool
HandlePools::getOrGeneratePool(const std::string &path, const Generator &generator)
{
    std::string normalized = Path::normalize(path);
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_pools.find(normalized);
        if (iter != m_pools.end()) {
            return getExistingPool(iter);
        }
    }
    {
        LockGuard lockGuard(m_lock);
        auto iter = m_pools.find(normalized);
        if (iter == m_pools.end()) {
            std::shared_ptr<HandlePool> pool = generator(normalized);
            if (pool == nullptr) {
                return nullptr;
            }
            iter = m_pools.emplace(normalized, std::make_pair(pool, 0)).first;
        }
        return getExistingPool(iter);
    }
}

RecyclableHandlePool HandlePools::getExistingPool(Tag tag)
{
    WCTAssert(tag != Tag::invalid(), "Tag invalid");
    SharedLockGuard lockGuard(m_lock);
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
    std::string normalized = Path::normalize(path);
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_pools.begin();
    for (; iter != m_pools.end(); ++iter) {
        if (iter->second.first->path == normalized) {
            break;
        }
    }
    return getExistingPool(iter);
}

RecyclableHandlePool HandlePools::getExistingPool(
const std::map<std::string, std::pair<std::shared_ptr<HandlePool>, int>>::iterator &iter)
{
    SharedLockGuard lockGuard(m_lock);
    if (iter == m_pools.end()) {
        return nullptr;
    }
    ++iter->second.second;
    const std::string path = iter->second.first->path;
    return RecyclableHandlePool(
    iter->second.first,
    std::bind(&HandlePools::flowBackHandlePool, this, std::placeholders::_1));
}

void HandlePools::flowBackHandlePool(const std::shared_ptr<HandlePool> &handlePool)
{
    LockGuard lockGuard(m_lock);
    const auto &iter = m_pools.find(handlePool->path);
    if (iter == m_pools.end()) {
        //drop it
        return;
    }
    if (iter->second.first.get() == handlePool.get() && --iter->second.second == 0) {
        m_pools.erase(iter);
    }
}

void HandlePools::purge()
{
    std::list<std::shared_ptr<HandlePool>> handlePools;
    {
        SharedLockGuard lockGuard(m_lock);
        for (const auto &iter : m_pools) {
            handlePools.push_back(iter.second.first);
        }
    }
    for (const auto &handlePool : handlePools) {
        handlePool->purgeFreeHandles();
    }
}

} //namespace WCDB
