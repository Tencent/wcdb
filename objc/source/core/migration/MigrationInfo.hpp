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

#ifndef MigrationInfo_hpp
#define MigrationInfo_hpp

#include <WCDB/DebugDescribable.hpp>
#include <WCDB/Lock.hpp>
#include <WCDB/WINQ.h>
#include <set>
#include <string>

namespace WCDB {

class MigrationUserInfo : public DebugDescribable {
public:
    MigrationUserInfo(const std::string& migratedTable);

    const std::string& getMigratedTable() const;
    const std::string& getOriginTable() const;
    const std::string& getOriginDatabase() const;

    bool shouldMigrate() const;
    bool isSameDatabaseMigration() const;

    void setOrigin(const std::string& table, const std::string& database = "");

    std::string getDebugDescription() const override;

protected:
    const std::string m_migratedTable;
    std::string m_originTable;
    std::string m_originDatabase;
};

class MigrationInfo : public MigrationUserInfo {
#pragma mark - Initialize
public:
    MigrationInfo(const MigrationUserInfo& userInfo, const std::set<std::string>& columns);

#pragma mark - Schema
public:
    const std::string& getSchemaForOriginDatabase() const;

    // WCDBMigration_
    static const std::string& getSchemaPrefix();

    static std::string getSchemaForDatabase(const std::string& database);

    /*
     ATTACH [originDatabase]
     AS [schemaForOriginDatabase]
     */
    const StatementAttach& getStatementForAttachingSchema() const;

    /*
     DETACH [schemaForOriginDatabase]
     */
    const StatementDetach& getStatementForDetachingSchema() const;

protected:
    // WCDBMigration_ + hash([originDatabase])
    std::string m_schemaForOriginDatabase;
    StatementAttach m_statementForAttachingSchema;
    StatementDetach m_statementForDetachingSchema;

#pragma mark - View
public:
    /*
     CREATE VIEW IF NOT EXISTS [unionedView]
     SELECT rowid, [columns]
     FROM [schemaForOriginDatabase].[originTable]
     UNION/UNION ALL
     SELECT rowid, [columns]
     FROM main.[migratedTable]
     */
    const StatementCreateView& getStatementForCreatingUnionedView() const;

    /*
     DROP VIEW IF EXISTS [unionedView]
     */
    const StatementDropView& getStatementForDroppingUnionedView() const;

protected:
    // WCDBUnioned_ + [migratedTable] + _ + [originTable]
    std::string m_unionedView;
    StatementCreateView m_statementForCreatingUnionedView;
    StatementDropView m_statementForDroppingUnionedView;

#pragma mark - Migrate
public:
    /*
     DELETE FROM [originTable] WHERE rowid = ?1
     */
    const StatementDelete& getStatementForDeletingMigratedRow() const;

    /*
     INSERT rowid, [columns]
     INTO [migratedTable]
     SELECT rowid, [columns]
     FROM [originTable]
     LIMIT ?1
     */
    const StatementInsert& getStatementForMigratingRow() const;

    /*
     DROP TABLE IF EXISTS [originTable]
     */
    const StatementDropTable& getStatementForDroppingOriginTable() const;

protected:
    StatementInsert m_statementForMigratingRow;
    StatementDelete m_statementForDeletingMigratedRow;
    StatementDropTable m_statementForDroppingOriginTable;
};

} // namespace WCDB

#endif /* MigrationInfo_hpp */
