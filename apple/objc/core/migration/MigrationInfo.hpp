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

#include <WCDB/StringView.hpp>
#include <WCDB/WINQ.h>
#include <set>

namespace WCDB {

#pragma mark - MigrationBaseInfo
class MigrationBaseInfo {
public:
    MigrationBaseInfo();
    MigrationBaseInfo(const UnsafeStringView& database, const UnsafeStringView& table);
    virtual ~MigrationBaseInfo() = 0;

    const StringView& getTable() const;
    const StringView& getDatabase() const;
    const StringView& getSourceTable() const;
    const StringView& getSourceDatabase() const;

    bool shouldMigrate() const;
    bool isCrossDatabase() const;

    // wcdb_migration_
    static const char* getSchemaPrefix();

protected:
    static Schema getSchemaForDatabase(const UnsafeStringView& database);
    void setSource(const UnsafeStringView& table, const UnsafeStringView& database = "");

private:
    StringView m_database;
    StringView m_table;
    StringView m_sourceDatabase;
    StringView m_sourceTable;
};

#pragma mark - MigrationUserInfo
class MigrationUserInfo final : public MigrationBaseInfo {
public:
    using MigrationBaseInfo::MigrationBaseInfo;
    ~MigrationUserInfo() override final;

    using MigrationBaseInfo::setSource;

    /*
     ATTACH [sourceDatabase]
     AS [schemaForSourceDatabase]
     */
    StatementAttach getStatementForAttachingSchema() const;

    Schema getSchemaForSourceDatabase() const;
};

#pragma mark - MigrationInfo
class MigrationInfo final : public MigrationBaseInfo {
public:
    MigrationInfo(const MigrationUserInfo& userInfo,
                  const std::set<StringView>& columns,
                  bool integerPrimaryKey);
    ~MigrationInfo() override final;

protected:
    bool m_integerPrimaryKey;

#pragma mark - Schema
public:
    // Schema
    const Schema& getSchemaForSourceDatabase() const;

    /*
     ATTACH [sourceDatabase]
     AS [schemaForSourceDatabase]
     */
    const StatementAttach& getStatementForAttachingSchema() const;

    /*
     DETACH [schemaForSourceDatabase]
     */
    static StatementDetach getStatementForDetachingSchema(const Schema& schema);

    /*
     PRAGMA main.database_list
     */
    static StatementPragma getStatementForSelectingDatabaseList();

protected:
    // wcdb_migration_ + hash([sourceDatabase])
    Schema m_schemaForSourceDatabase;
    StatementAttach m_statementForAttachingSchema;

#pragma mark - View
public:
    // wcdb_union_
    static const char* getUnionedViewPrefix();

    const StringView& getUnionedView() const;

    /*
     CREATE TEMP VIEW IF NOT EXISTS [unionedView] AS
     SELECT rowid, [columns]
     FROM [schemaForSourceDatabase].[sourceTable]
     UNION/UNION ALL
     SELECT rowid, [columns]
     FROM main.[table]
     ORDER BY rowid ASC
     */
    const StatementCreateView& getStatementForCreatingUnionedView() const;

    /*
     DROP VIEW IF EXISTS temp.[unionedView]
     */
    static const StatementDropView
    getStatementForDroppingUnionedView(const UnsafeStringView& unionedView);

    /*
     SELECT name
     FROM temp.sqlite_master
     WHERE type == "view" AND name LIKE "wcdb_union_%"
     */
    static StatementSelect getStatementForSelectingUnionedView();

protected:
    // wcdb_union_ + [table]
    StringView m_unionedView;
    StatementCreateView m_statementForCreatingUnionedView;

#pragma mark - Compatible
public:
    /*
     DELETE FROM [schemaForSourceDatabase].[sourceTable] WHERE rowid == ?1
     */
    const StatementDelete& getStatementForDeletingSpecifiedRow() const;

    /*
     INSERT rowid, [columns]
     ...
     VALUES (?rowidIndex, ...)
     
     Note that newRowid is
     1. rowid in source table when it's an integer primary key table
     2. SELECT max(rowid)+1 FROM temp.[unionedView] when the table does not contain an integer primary key
     */
    StatementInsert getStatementForMigrating(const Syntax::InsertSTMT& stmt) const;

    int getRowIDIndexOfMigratingStatement() const;

    /*
     UPDATE ...
     SET ...
     WHERE rowid IN(
     SELECT rowid FROM temp.[unionedView] WHERE ... ORDER BY ... LIMIT ... OFFSET ...
     )
     */
    StatementUpdate
    getStatementForLimitedUpdatingTable(const Statement& sourceStatement) const;

    /*
     DELETE FROM ...
     WHERE rowid IN(
     SELECT rowid FROM temp.[unionedView] WHERE ... ORDER BY ... LIMIT ... OFFSET ...
     )
     */
    StatementDelete
    getStatementForLimitedDeletingFromTable(const Statement& sourceStatement) const;

    StatementDelete getStatementForDeletingFromTable(const Statement& sourceStatement) const;

protected:
    StatementDelete m_statementForDeletingSpecifiedRow;
    StatementSelect m_statementForSelectingMaxRowID;

#pragma mark - Migrate
public:
    /*
     INSERT [columns]
     INTO rowid, main.[table]
     SELECT rowid, [columns]
     FROM [schemaForSourceDatabase].[sourceTable]
     ORDER BY rowid DESC
     LIMIT 1
     */
    const StatementInsert& getStatementForMigratingOneRow() const;

    /*
     DELETE FROM [schemaForSourceDatabase].[sourceTable]
     ORDER BY rowid DESC
     LIMIT 1
     */
    const StatementDelete& getStatementForDeletingMigratedOneRow() const;

    /*
     DROP TABLE IF EXISTS [schemaForSourceDatabase].[sourceTable]
     */
    const StatementDropTable& getStatementForDroppingSourceTable() const;

protected:
    StatementInsert m_statementForMigratingOneRow;
    StatementDelete m_statementForDeletingMigratedOneRow;
    StatementDropTable m_statementForDroppingSourceTable;
};

} // namespace WCDB
