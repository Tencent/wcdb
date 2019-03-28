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
#import <WCDB/String.hpp>
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
    generateCreateTableStatement(const WCDB::String &tableName) const;

    WCDB::StatementCreateVirtualTable
    generateCreateVirtualTableStatement(const WCDB::String &tableName) const;

    WCDB::StatementCreateVirtualTable statementVirtualTable;

#pragma mark - Table Constraint
public:
    WCDB::TableConstraint &getOrCreateTableConstraint(const WCDB::String &name);

private:
    std::map<WCDB::String /* constraint name */, WCDB::TableConstraint> m_constraints;

#pragma mark - Index
public:
    struct Index {
        Index();
        WCDB::StatementCreateIndex statement;
        bool forNewlyCreatedTableOnly; // create if and only if the table is newly created by createTable:withClass:
    };
    typedef struct Index Index;
    Index &getOrCreateIndex(const WCDB::String &suffix);

    std::list<Index> generateIndexes(const WCDB::String &tableName) const;

private:
    std::map<WCDB::String /* index suffix */, Index> m_indexes;
};
