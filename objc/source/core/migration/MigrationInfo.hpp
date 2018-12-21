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

#ifndef _WCDB_MIGRATIONINFO_HPP
#define _WCDB_MIGRATIONINFO_HPP

#include <WCDB/DebugDescribable.hpp>
#include <WCDB/String.hpp>
#include <WCDB/WINQ.h>
#include <set>

namespace WCDB {

#pragma mark - MigrationBaseInfo
class MigrationBaseInfo : public DebugDescribable {
public:
    MigrationBaseInfo();
    MigrationBaseInfo(const String& migratedTable);
    virtual ~MigrationBaseInfo();

    const String& getMigratedTable() const;
    const String& getOriginTable() const;
    const String& getOriginDatabase() const;

    bool shouldMigrate() const;
    bool isSameDatabaseMigration() const;

    String getDebugDescription() const override final;

protected:
    String m_migratedTable;
    String m_originTable;
    String m_originDatabase;
};

#pragma mark - MigrationUserInfo
class MigrationUserInfo final : public MigrationBaseInfo {
public:
    using MigrationBaseInfo::MigrationBaseInfo;

    void setOrigin(const String& table, const String& database = "");
};

#pragma mark - MigrationInfo
class MigrationInfo final : public MigrationBaseInfo {
public:
    MigrationInfo(const MigrationUserInfo& userInfo, const std::set<String>& columns);

#pragma mark - Schema
public:
    const Schema& getSchemaForOriginDatabase() const;

    // WCDBMigration_
    static const String& getSchemaPrefix();

    static Schema getSchemaForDatabase(const String& database);

    /*
     ATTACH [originDatabase]
     AS [schemaForOriginDatabase]
     */
    const StatementAttach& getStatementForAttachingSchema() const;

    /*
     DETACH [schemaForOriginDatabase]
     */
    static StatementDetach getStatementForDetachingSchema(const Schema& schema);

    /*
     PRAGMA main.database_list
     */
    static StatementPragma getStatementForSelectingDatabaseList();

protected:
    // WCDBMigration_ + hash([originDatabase])
    Schema m_schemaForOriginDatabase;
    StatementAttach m_statementForAttachingSchema;

#pragma mark - View
public:
    // WCDBUnioned_
    static const String& getUnionedViewPrefix();

    const String& getUnionedView() const;

    /*
     CREATE TEMP VIEW IF NOT EXISTS [unionedView]
     SELECT rowid, [columns]
     FROM [schemaForOriginDatabase].[originTable]
     UNION/UNION ALL
     SELECT rowid, [columns]
     FROM main.[migratedTable]
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
    // WCDBUnioned_ + [migratedTable] + _ + [originTable]
    String m_unionedView;
    StatementCreateView m_statementForCreatingUnionedView;

#pragma mark - Migrate
public:
    /*
     INSERT rowid, [columns]
     INTO main.[migratedTable]
     SELECT rowid, [columns]
     FROM [schemaForOriginDatabase].[originTable]
     ORDER BY rowid DESC
     LIMIT 1
     */
    const StatementInsert& getStatementForMigratingOneRow() const;

    /*
     DELETE FROM [schemaForOriginDatabase].[originTable] WHERE rowid == ?1
     */
    const StatementDelete& getStatementForDeletingSpecifiedRow() const;

    /*
     INSERT rowid, [columns]
     INTO main.[migratedTable]
     [OR ONCONFLICT ACTION]
     SELECT rowid, [columns]
     FROM [schemaForOriginDatabase].[originTable]
     WHERE rowid == ?1
     */
    StatementInsert
    getStatementForMigratingSpecifiedRow(const Statement& originStatement) const;

    /*
     DROP TABLE IF EXISTS [schemaForOriginDatabase].[originTable]
     */
    const StatementDropTable& getStatementForDroppingOriginTable() const;

    /*
     UPDATE ...
     SET ...
     WHERE rowid IN(
        SELECT rowid FROM temp.[unionedView] WHERE ... ORDER BY ... LIMIT ... OFFSET ...
     )
     */
    StatementUpdate
    getStatementForLimitedUpdatingTable(const Statement& originStatement) const;

    /*
     DELETE FROM ...
     WHERE rowid IN(
        SELECT rowid FROM temp.[unionedView] WHERE ... ORDER BY ... LIMIT ... OFFSET ...
     )
     */
    StatementDelete
    getStatementForLimitedDeletingFromTable(const Statement& originStatement) const;

protected:
    StatementInsert m_statementForMigratingOneRow;
    StatementInsert m_statementForMigratingSpecifiedRowTemplate;
    StatementDelete m_statementForDeletingSpecifiedRow;
    StatementDropTable m_statementForDroppingOriginTable;
};

} // namespace WCDB

#endif /* _WCDB_MIGRATIONINFO_HPP */
