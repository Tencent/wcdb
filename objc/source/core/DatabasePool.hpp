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

#ifndef _WCDB_DATABASEPOOL_HPP
#define _WCDB_DATABASEPOOL_HPP

#include <WCDB/Lock.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/RecyclableDatabase.hpp>

namespace WCDB {

class DatabasePoolEvent {
public:
    virtual ~DatabasePoolEvent();

protected:
    virtual void onDatabaseCreated(Database* database) = 0;
    friend class DatabasePool;
};

class DatabasePool final {
#pragma mark - DatabasePool
public:
    DatabasePool(DatabasePoolEvent* event);

    RecyclableDatabase getOrCreate(const String& path);
    RecyclableDatabase get(const String& path);
    RecyclableDatabase get(const Tag& tag);

    void purge();

protected:
    struct ReferencedDatabase {
        ReferencedDatabase(std::shared_ptr<Database>&& database);
        std::shared_ptr<Database> database;
        int reference;
    };
    typedef struct ReferencedDatabase ReferencedDatabase;

    RecyclableDatabase get(const std::map<String, ReferencedDatabase>::iterator& iter);
    void flowBack(Database* database);

    std::map<String, ReferencedDatabase> m_databases; //path->{database, reference}
    SharedLock m_lock;

    DatabasePoolEvent* m_event;
};

} //namespace WCDB

#endif /* _WCDB_DATABASEPOOL_HPP */
