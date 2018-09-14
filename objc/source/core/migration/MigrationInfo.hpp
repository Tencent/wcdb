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

#include <WCDB/Lock.hpp>
#include <WCDB/WINQ.h>
#include <string>

namespace WCDB {

class MigrationInfo {
#pragma mark - Initialize
public:
    MigrationInfo(const std::string& migratedTable,
                  const std::string& originTable,
                  const std::string& originDatabase);

    const std::string originTable;
    const std::string migratedTable;
    const std::string originDatabase;

    bool isSameDatabaseMigration() const;

    enum State : int {
        None = 0,
        Initialized,
        Migrated,
    };
    State getState() const;
    void markAsMigrated();
    void initialize(const std::list<std::string>& columns);

protected:
    bool isInitialized() const;
    State m_state;
    mutable SharedLock m_lock;

#pragma mark - Scheme
public:
    /*
     ATTACH [targetDatabase]
     AS [schema]
     */
    const StatementAttach& getStatementForAttachingSchema() const;

    /*
     DETACH [schema]
     */
    const StatementDetach& getStatementForDetachingSchema() const;

protected:
    // WCDBMigration_ + hash(targetDatabase)
    std::string m_schemaForOriginDatabase;
    StatementAttach m_statementForAttachingSchema;
    StatementDetach m_statementForDetachingSchema;

#warning TODO Does rowid need to be selected?
#pragma mark - View
public:
    /*
     CREATE VIEW IF NOT EXISTS [view]
     SELECT [columns]
     FROM [schema].sourceTable
     UNION/UNION ALL
     SELECT [columns]
     FROM main.targetTable
     */
    const StatementCreateView& getStatementForCreatingUnionedView() const;

    /*
     DROP VIEW IF EXISTS [view]
     */
    const StatementDropView& getStatementForDroppingUnionedView() const;

protected:
    // WCDBUnioned_ + targetTable + _ + sourceTable
    std::string m_unionedView;
    StatementCreateView m_statementForCreatingUnionedView;
    StatementDropView m_statementForDroppingUnionedView;

#pragma mark - Migrate
public:
    /*
     DELETE FROM sourceTable WHERE rowid = ?1
     */
    const StatementDelete& getStatementForDeletingMigratedRow() const;

    /*
     INSERT rowid, [columns]
     INTO [targetTable]
     SELECT rowid, [columns]
     FROM [sourceTable]
     LIMIT ?1
     */
    const StatementInsert getStatementForMigratingRow() const;

protected:
    StatementInsert m_statementForMigratingRow;
    StatementDelete m_statementForDeletingMigratedRow;
};

} // namespace WCDB

#endif /* MigrationInfo_hpp */
