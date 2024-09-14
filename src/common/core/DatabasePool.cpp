//
// Created by sanhuazhang on 2019/05/02
//

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

#include "DatabasePool.hpp"
#include "Assertion.hpp"

namespace WCDB {

DatabasePoolEvent::~DatabasePoolEvent() = default;

#pragma mark - DatabasePool
DatabasePool::DatabasePool(DatabasePoolEvent *event) : m_event(event)
{
    WCTAssert(m_event != nullptr);
}

RecyclableDatabase DatabasePool::getOrCreate(const UnsafeStringView &path)
{
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_databases.find(path);
        if (iter != m_databases.end()) {
            return get(iter);
        }
    }
    LockGuard lockGuard(m_lock);
    auto iter = m_databases.find(path);
    if (iter != m_databases.end()) {
        return get(iter);
    }
    ReferencedDatabase referencedDatabase(std::make_shared<InnerDatabase>(path));
    auto result = m_databases.emplace(path, std::move(referencedDatabase));
    WCTAssert(result.second);
    m_event->databaseDidCreate(result.first->second.database.get());
    return get(result.first);
}

Tag DatabasePool::getTag(const UnsafeStringView &path)
{
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_databases.find(path);
    if (iter != m_databases.end()) {
        return get(iter).get()->getTag();
    } else {
        return Tag::invalid();
    }
}

DatabasePool::ReferencedDatabase::ReferencedDatabase(std::shared_ptr<InnerDatabase> &&database_)
: database(std::move(database_)), reference(0)
{
}

DatabasePool::ReferencedDatabase::ReferencedDatabase(ReferencedDatabase &&other)
: database(std::move(other.database)), reference(other.reference.load())
{
    other.reference = 0;
}

RecyclableDatabase
DatabasePool::get(const StringViewMap<ReferencedDatabase>::iterator &iter)
{
    WCTAssert(m_lock.readSafety());
    WCTAssert(iter != m_databases.end());
    ++iter->second.reference;
    return RecyclableDatabase(
    iter->second.database.get(),
    std::bind(&DatabasePool::flowBack, this, std::placeholders::_1));
}

void DatabasePool::flowBack(InnerDatabase *database)
{
    // shared lock is enough.
    SharedLockGuard lockGuard(m_lock);
    const auto &iter = m_databases.find(database->getPath());
    WCTAssert(iter != m_databases.end());
    WCTAssert(iter->second.database.get() == database);
    if (--iter->second.reference == 0) {
        // A created database will never be erased. Instead, it will be empty so that the memory used will be very low.
        iter->second.database->close(nullptr);
    }
}

void DatabasePool::purge()
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_databases) {
        if (!iter.second.database->isBlockaded()) {
            iter.second.database->purge();
        }
    }
}

} //namespace WCDB
