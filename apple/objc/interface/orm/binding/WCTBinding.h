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

#import <WCDB/CaseInsensiveList.hpp>
#import <WCDB/StringView.hpp>
#import <WCDB/WCTCoding.h>
#import <WCDB/WCTCommon.h>
#import <WCDB/WCTProperty.h>
#import <map>

class WCTBinding final {
#pragma mark - Binding
public:
    WCTBinding(Class cls);
    static void assertNoInheritance(const WCTProperty &property, Class cls);

private:
    Class m_cls;

#pragma mark - Property
public:
    const WCTProperties &getProperties() const;

private:
    WCTProperties m_properties;

#pragma mark - Column Def
public:
    const WCDB::CaseInsensiveList<WCDB::ColumnDef> &getColumnDefs() const;
    WCDB::ColumnDef *getColumnDef(const WCTProperty &property);

private:
    WCDB::CaseInsensiveList<WCDB::ColumnDef> m_columnDefs;

#pragma mark - Table
public:
    WCDB::StatementCreateTable
    generateCreateTableStatement(const WCDB::UnsafeStringView &tableName) const;

    WCDB::StatementCreateVirtualTable
    generateCreateVirtualTableStatement(const WCDB::UnsafeStringView &tableName) const;

    WCDB::StatementCreateVirtualTable statementVirtualTable;

#pragma mark - Table Constraint
public:
    WCDB::TableConstraint &getOrCreateTableConstraint(const WCDB::UnsafeStringView &name);

private:
    /* constraint name */
    WCDB::StringViewMap<WCDB::TableConstraint> m_constraints;

#pragma mark - Index
public:
    struct Index {
        Index(const WCDB::UnsafeStringView &suffix);
        const WCDB::StringView suffix;
        enum class Action {
            Create,
            CreateForNewlyCreatedTableOnly, // create if and only if the table is newly created by createTable:withClass:
            Drop,
        } action;
        WCDB::StatementCreateIndex statement;
    };
    typedef struct Index Index;
    Index &getOrCreateIndex(const WCDB::UnsafeStringView &suffix);

    std::pair<std::list<WCDB::StatementCreateIndex>, std::list<WCDB::StatementDropIndex>>
    generateIndexStatements(const WCDB::UnsafeStringView &tableName,
                            bool isTableNewlyCreated) const;

private:
    /* index suffix */
    WCDB::StringViewMap<Index> m_indexes;
};
