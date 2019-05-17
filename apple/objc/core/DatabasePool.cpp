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
#include <WCDB/DatabasePool.hpp>

namespace WCDB {

DatabasePoolEvent::~DatabasePoolEvent()
{
}

#pragma mark - DatabasePool
DatabasePool::DatabasePool(DatabasePoolEvent *event) : m_event(event)
{
    WCTInnerAssert(m_event != nullptr);
}

RecyclableDatabase DatabasePool::getOrCreate(const String &path)
{
    String normalized = Path::normalize(path);
    {
        SharedLockGuard lockGuard(m_lock);
        auto iter = m_databases.find(normalized);
        if (iter != m_databases.end()) {
            return get(iter);
        }
    }
    LockGuard lockGuard(m_lock);
    auto iter = m_databases.find(normalized);
    if (iter != m_databases.end()) {
        return get(iter);
    }
    ReferencedDatabase referencedDatabase(std::shared_ptr<Database>(new Database(normalized)));
    auto result = m_databases.emplace(normalized, std::move(referencedDatabase));
    WCTInnerAssert(result.second);
    m_event->databaseDidCreate(result.first->second.database.get());
    return get(result.first);
}

RecyclableDatabase DatabasePool::get(const String &path)
{
    String normalized = Path::normalize(path);
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_databases.find(normalized);
    // get referenced database only
    if (iter != m_databases.end() && iter->second.reference != 0) {
        return get(iter);
    }
    return nullptr;
}

DatabasePool::ReferencedDatabase::ReferencedDatabase(std::shared_ptr<Database> &&database_)
: database(std::move(database_)), reference(0)
{
}

DatabasePool::ReferencedDatabase::ReferencedDatabase(ReferencedDatabase &&other)
: database(std::move(other.database)), reference(other.reference.load())
{
    other.reference = 0;
}

RecyclableDatabase
DatabasePool::get(const std::map<String, ReferencedDatabase>::iterator &iter)
{
    WCTInnerAssert(m_lock.readSafety());
    WCTInnerAssert(iter != m_databases.end());
    ++iter->second.reference;
    return RecyclableDatabase(
    iter->second.database.get(),
    std::bind(&DatabasePool::flowBack, this, std::placeholders::_1));
}

void DatabasePool::flowBack(Database *database)
{
    // shared lock is enough.
    SharedLockGuard lockGuard(m_lock);
    const auto &iter = m_databases.find(database->getPath());
    WCTInnerAssert(iter != m_databases.end());
    WCTInnerAssert(iter->second.database.get() == database);
    if (--iter->second.reference == 0) {
        // A created database will never be erased. Instead, it will be empty so that the memory used will be very low.
        iter->second.database->close(nullptr);
    }
}

void DatabasePool::purge()
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_databases) {
        iter.second.database->purge();
    }
}

} //namespace WCDB
