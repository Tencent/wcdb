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

#include <WCDB/String.hpp>
#include <WCDB/WINQ.h>
#include <set>

namespace WCDB {

#pragma mark - MigrationBaseInfo
class MigrationBaseInfo {
public:
    MigrationBaseInfo();
    MigrationBaseInfo(const String& database, const String& table);
    virtual ~MigrationBaseInfo() = 0;

    const String& getTable() const;
    const String& getDatabase() const;
    const String& getSourceTable() const;
    const String& getSourceDatabase() const;

    bool shouldMigrate() const;
    bool isCrossDatabase() const;

    // WCDBMigration_
    static const char* getSchemaPrefix();

protected:
    static Schema getSchemaForDatabase(const String& database);
    void setSource(const String& table, const String& database = "");

private:
    String m_table;
    String m_database;
    String m_sourceTable;
    String m_sourceDatabase;
};

#pragma mark - MigrationUserInfo
class MigrationUserInfo final : public MigrationBaseInfo {
public:
    using MigrationBaseInfo::MigrationBaseInfo;

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
                  const std::set<String>& columns,
                  bool integerPrimaryKey);

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
    // WCDBMigration_ + hash([sourceDatabase])
    Schema m_schemaForSourceDatabase;
    StatementAttach m_statementForAttachingSchema;

#pragma mark - View
public:
    // WCDBUnioned_
    static const String& getUnionedViewPrefix();

    const String& getUnionedView() const;

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
    getStatementForDroppingUnionedView(const String& unionedView);

    /*
     SELECT name
     FROM temp.sqlite_master
     WHERE type == "view" AND name LIKE "WCDBUnioned_%"
     */
    static StatementSelect getStatementForSelectingUnionedView();

protected:
    // WCDBUnioned_ + [table]
    String m_unionedView;
    StatementCreateView m_statementForCreatingUnionedView;

#pragma mark - Compatible
public:
    /*
     DELETE FROM [schemaForSourceDatabase].[sourceTable] WHERE rowid == ?1
     */
    const StatementDelete& getStatementForDeletingSpecifiedRow() const;

    /*
     INSERT rowid, [columns]
     INTO main.[table]
     [OR ONCONFLICT ACTION]
     SELECT newRowid, [columns]
     FROM [schemaForSourceDatabase].[sourceTable]
     WHERE rowid == ?1
     
     Note that newRowid is
     1. rowid in source table when it's an integer primary key table
     2. SELECT max(rowid)+1 FROM temp.[unionedView] when the table does not contain an integer primary key
     */
    StatementInsert
    getStatementForMigratingSpecifiedRow(Syntax::ConflictAction conflictAction) const;
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
    StatementInsert m_statementForMigratingSpecifiedRowTemplate;
    StatementDelete m_statementForDeletingSpecifiedRow;

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
