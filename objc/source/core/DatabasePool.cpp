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
#include <WCDB/CoreNotifier.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>

namespace WCDB {

RecyclableHandlePool
DatabasePool::getOrGeneratePool(const std::string &path, const Generator &generator)
{
    std::string normalized = Path::normalize(path);
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_databases.find(normalized);
        if (iter != m_databases.end()) {
            return getExistingPool(iter);
        }
    }
    {
        LockGuard lockGuard(m_lock);
        auto iter = m_databases.find(normalized);
        if (iter == m_databases.end()) {
            std::shared_ptr<HandlePool> pool = generator(normalized);
            if (pool == nullptr) {
                return nullptr;
            }
            iter = m_databases.emplace(normalized, std::make_pair(pool, 0)).first;
        }
        return getExistingPool(iter);
    }
}

RecyclableHandlePool DatabasePool::getExistingPool(Tag tag)
{
    WCTAssert(tag != Tag::invalid(), "Tag invalid");
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_databases.end();
    for (iter = m_databases.begin(); iter != m_databases.end(); ++iter) {
        if (iter->second.first->getTag() == tag) {
            break;
        }
    }
    return getExistingPool(iter);
}

RecyclableHandlePool DatabasePool::getExistingPool(const std::string &path)
{
    std::string normalized = Path::normalize(path);
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_databases.begin();
    for (; iter != m_databases.end(); ++iter) {
        if (iter->second.first->path == normalized) {
            break;
        }
    }
    return getExistingPool(iter);
}

RecyclableHandlePool DatabasePool::getExistingPool(
const std::map<std::string, std::pair<std::shared_ptr<HandlePool>, int>>::iterator &iter)
{
    WCTInnerAssert(m_lock.level() >= SharedLock::Level::Read);
    if (iter == m_databases.end()) {
        return nullptr;
    }
    ++iter->second.second;
    const std::string path = iter->second.first->path;
    return RecyclableHandlePool(
    iter->second.first,
    std::bind(&DatabasePool::flowBackHandlePool, this, std::placeholders::_1));
}

void DatabasePool::flowBackHandlePool(const std::shared_ptr<HandlePool> &handlePool)
{
    LockGuard lockGuard(m_lock);
    const auto &iter = m_databases.find(handlePool->path);
    if (iter == m_databases.end()) {
        //drop it
        return;
    }
    if (iter->second.first.get() == handlePool.get() && --iter->second.second == 0) {
        m_databases.erase(iter);
    }
}

void DatabasePool::purge()
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_databases) {
        iter.second.first->purgeFreeHandles();
    }
}

} //namespace WCDB
