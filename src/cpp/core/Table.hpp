//
// Created by qiuwenchen on 2022/8/28.
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
#include "TableORMOperation.hpp"

namespace WCDB {

class BaseTable {
public:
    BaseTable() = delete;
    BaseTable(const BaseTable &);
    BaseTable &operator=(const BaseTable &);
    virtual ~BaseTable() = 0;

#pragma mark - Basic
    /**
     @brief Get the most recent error for the database of current table in the current thread.
            Since it is too cumbersome to get the error after every database operation, it‘s better to use monitoring interfaces to obtain database errors and print them to the log.
     @see   `static Database::globalTraceError()`
     @see   `Database::traceError()`
     @return WCDB::Error
     */
    const Error &getError() const;

    /**
     @brief Get the name of the table.
     */
    const StringView &getTableName() const;

    /**
     @brief Drop index with name tableName + subfix.
     */
    bool dropIndexWithSuffix(const UnsafeStringView &suffix);

protected:
    BaseTable(Recyclable<InnerDatabase *> database, const UnsafeStringView &tableName);
    RecyclableHandle getHandle(bool writeHint);
    StringView getIndexNameWithSuffix(const UnsafeStringView &suffix) const;

    StringView m_tableName;
    Recyclable<InnerDatabase *> m_databaseHolder;
    InnerDatabase *m_innerDatabase;
};

template<class ObjectType>
class Table final : public BaseTable, public TableORMOperation<ObjectType> {
    friend class HandleORMOperation;

    WCDB_CPP_ORM_STATIC_ASSERT_FOR_OBJECT_TYPE

protected:
    Table(Recyclable<InnerDatabase *> database, const UnsafeStringView &tableName)
    : BaseTable(database, tableName)
    {
    }

private:
    const UnsafeStringView &getTableName() const override final
    {
        return m_tableName;
    }
    RecyclableHandle getHandleHolder(bool writeHint) override final
    {
        return getHandle(writeHint);
    }
    Recyclable<InnerDatabase *> getDatabaseHolder() override final
    {
        return m_databaseHolder;
    }

public:
    Table() = delete;
    Table(const Table &) = default;
    Table &operator=(const Table &) = default;
    ~Table() override final = default;

    using ORMType = ObjectType;
};

} // namespace WCDB
