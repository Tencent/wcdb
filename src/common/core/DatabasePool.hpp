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

#pragma once

#include "InnerDatabase.hpp"
#include "Lock.hpp"
#include "Path.hpp"
#include "Tag.hpp"

namespace WCDB {

typedef Recyclable<InnerDatabase*> RecyclableDatabase;

class DatabasePoolEvent {
public:
    virtual ~DatabasePoolEvent() = 0;

protected:
    virtual void databaseDidCreate(InnerDatabase* database) = 0;
    friend class DatabasePool;
};

class DatabasePool final {
#pragma mark - DatabasePool
public:
    DatabasePool(DatabasePoolEvent* event);

    RecyclableDatabase getOrCreate(const UnsafeStringView& path);
    Tag getTag(const UnsafeStringView& path);

    void purge();

protected:
    struct ReferencedDatabase {
        ReferencedDatabase(std::shared_ptr<InnerDatabase>&& database);
        ReferencedDatabase(ReferencedDatabase&& other);
        std::shared_ptr<InnerDatabase> database;
        std::atomic<int> reference;
    };
    typedef struct ReferencedDatabase ReferencedDatabase;

    RecyclableDatabase get(const StringViewMap<ReferencedDatabase>::iterator& iter);
    void flowBack(InnerDatabase* database);

    StringViewMap<ReferencedDatabase> m_databases; //path->{database, reference}
    SharedLock m_lock;

    DatabasePoolEvent* m_event;
};

} //namespace WCDB
