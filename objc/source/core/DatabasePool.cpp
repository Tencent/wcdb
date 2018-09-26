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

#pragma mark - DatabasePool
DatabasePool::DatabasePool() : m_event(nullptr)
{
}

RecyclableDatabase DatabasePool::getOrCreate(const std::string &path)
{
    std::string normalized = Path::normalize(path);
    {
        SharedLockGuard lockGuard(m_lock);
        RecyclableDatabase database = get(normalized);
        if (database != nullptr) {
            return database;
        }
    }
    LockGuard lockGuard(m_lock);
    RecyclableDatabase database = get(normalized);
    if (database != nullptr) {
        return database;
    }
    ReferencedDatabase referencedDatabase(std::shared_ptr<Database>(new Database(normalized)));
    ++referencedDatabase.reference;
    auto result = m_databases.emplace(path, std::move(referencedDatabase));
    WCTInnerAssert(result.second);
    onDatabaseCreated(result.first->second.database.get());
    return get(result.first);
}

RecyclableDatabase DatabasePool::get(const std::string &path)
{
    std::string normalized = Path::normalize(path);
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_databases.find(normalized);
    return get(iter);
}

RecyclableDatabase DatabasePool::get(const Tag &tag)
{
    WCTRemedialAssert(tag != Tag::invalid(), "Tag invalid", return nullptr;);
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_databases.end();
    for (iter = m_databases.begin(); iter != m_databases.end(); ++iter) {
        if (iter->second.database->getTag() == tag) {
            break;
        }
    }
    return get(iter);
}

DatabasePool::ReferencedDatabase::ReferencedDatabase(std::shared_ptr<Database> &&database_)
: database(std::move(database_)), reference(0)
{
}

RecyclableDatabase
DatabasePool::get(const std::map<std::string, ReferencedDatabase>::iterator &iter)
{
    WCTInnerAssert(m_lock.readSafety());
    if (iter == m_databases.end()) {
        return nullptr;
    }
    ++iter->second.reference;
    return RecyclableDatabase(
    iter->second.database.get(),
    std::bind(&DatabasePool::flowBack, this, std::placeholders::_1));
}

void DatabasePool::flowBack(Database *database)
{
    LockGuard lockGuard(m_lock);
    const auto &iter = m_databases.find(database->getPath());
    WCTInnerAssert(iter != m_databases.end());
#warning TODO will database be different?
    WCTInnerAssert(iter->second.database.get() == database);
    if (--iter->second.reference == 0) {
        m_databases.erase(iter);
    }
}

void DatabasePool::purge()
{
    SharedLockGuard lockGuard(m_lock);
    for (const auto &iter : m_databases) {
        iter.second.database->purge();
    }
}

#pragma mark - Event
void DatabasePool::setEvent(DatabasePoolEvent *event)
{
    LockGuard lockGuard(m_lock);
    m_event = event;
}

void DatabasePool::onDatabaseCreated(Database *database)
{
    WCTInnerAssert(m_lock.readSafety());
    if (m_event != nullptr) {
        m_event->onDatabaseCreated(database);
    }
}

} //namespace WCDB
