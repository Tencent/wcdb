//
// Created by qiuwenchen on 2022/8/27.
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

#include "CaseInsensitiveList.hpp"
#include "WINQ.h"

namespace WCDB {

class InnerHandle;

class WCDB_API BaseBinding {
public:
    BaseBinding();
    virtual ~BaseBinding() = 0;
#pragma mark - Column Def
public:
    const CaseInsensitiveList<ColumnDef> &getColumnDefs() const;
    ColumnDef *getColumnDef(const UnsafeStringView &columnName);
    const ColumnDef *getColumnDef(const UnsafeStringView &columnName) const;
    void enableAutoIncrementForExistingTable();

protected:
    CaseInsensitiveList<ColumnDef> m_columnDefs;
    bool m_enableAutoIncrementForExistingTable = false;

private:
    bool configAutoincrementIfNeed(const UnsafeStringView &tableName, InnerHandle *handle) const;
    bool updateMasterTable(const UnsafeStringView &tableName, InnerHandle *handle) const;
    bool updateSequeceTable(const UnsafeStringView &tableName,
                            const UnsafeStringView &integerPrimaryKey,
                            InnerHandle *handle) const;
    void reportSqlParseFail(const UnsafeStringView &sql,
                            const UnsafeStringView &tableName,
                            InnerHandle *handle) const;

#pragma mark - Table
public:
    bool createTable(const UnsafeStringView &tableName, InnerHandle *handle) const;
    bool createVirtualTable(const UnsafeStringView &tableName, InnerHandle *handle) const;

    StatementCreateTable
    generateCreateTableStatement(const UnsafeStringView &tableName) const;

    StatementCreateVirtualTable
    generateCreateVirtualTableStatement(const UnsafeStringView &tableName) const;

    StatementCreateTable statementTable;
    StatementCreateVirtualTable statementVirtualTable;

    bool tryRecoverColumn(const UnsafeStringView &columnName,
                          const UnsafeStringView &tableName,
                          const UnsafeStringView &schemaName,
                          const UnsafeStringView &sql,
                          InnerHandle *handle) const;

#pragma mark - Table Constraint
public:
    TableConstraint &getOrCreateTableConstraint(const UnsafeStringView &name);

protected:
    /* constraint name */
    StringViewMap<TableConstraint> m_constraints;

#pragma mark - Index
public:
    struct Index {
        Index(const UnsafeStringView &nameOrSuffix, bool isFullName = false);
        const StringView nameOrSuffix;
        bool isFullName;
        enum class Action {
            Create,
            CreateForNewlyCreatedTableOnly, // create if and only if the table is newly created by createTable:withClass:
            Drop,
        } action;
        StatementCreateIndex statement;
        StringView getIndexName(const UnsafeStringView &tableName);
    };
    typedef struct Index Index;
    Index &getOrCreateIndex(const UnsafeStringView &nameOrSuffix, bool isFullName = false);

    std::pair<std::list<StatementCreateIndex>, std::list<StatementDropIndex>>
    generateIndexStatements(const UnsafeStringView &tableName, bool isTableNewlyCreated) const;

protected:
    /* index suffix */
    StringViewMap<Index> m_indexes;
};

} // namespace WCDB
