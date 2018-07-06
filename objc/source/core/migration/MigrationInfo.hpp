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

#include <WCDB/Abstract.h>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationInfo {
public:
#pragma mark - Initialize
    static std::shared_ptr<MigrationInfo> info(const std::string &targetTable,
                                               const std::string &sourceTable,
                                               const std::string &sourceDatabasePath);

protected:
    MigrationInfo(const std::string &targetTable,
                  const std::string &sourceTable,
                  const std::string &sourceDatabasePath);

#pragma mark - Basic
public:
    const std::string targetTable;
    const std::string sourceTable;
    const std::string sourceDatabasePath;
    const std::string schema;

    bool isSameDatabaseMigration() const;

    QualifiedTableName getQualifiedSourceTable() const;

    TableOrSubquery getSourceTable() const;

    static const std::string schemaPrefix();

    static std::string resolvedSchema(const std::string &path);

    bool isInited() const;
    void initialize(const std::set<std::string> &columnNames);

#pragma mark - Statement
public:
    /**
     SELECT rowid FROM [sourceTable]
     LIMIT 1

     @return StatementSelect
     */
    const StatementSelect &getStatementForPickingRowIDs() const;

    /**
     DELETE FROM [sourceTable]
     LIMIT 1
     
     @return StatementDelete
     */
    const StatementDelete &getStatementForDeletingMigratedRow() const;

    /**
     DROP TABLE IF EXISTS [sourceTable]
     
     @return StatementDropTable
     */
    StatementDropTable getStatementForDroppingOldTable() const;

    /**
     CREATE VIEW IF NOT EXISTS [unionedView] 
     AS(SELECT * FROM [sourceTable] UNION SELECT * FROM [targetTable])

     @return StatementCreateView
     */
    StatementCreateView getStatementForCreatingUnionedView() const;

    /**
     DROP VIEW IF EXISTS [unionedView]

     @return StatementDropView
     */
    StatementDropView getStatementForDroppingUnionedView() const;

    const std::string unionedViewName;

    /**
     INSERT [CONFLICT TYPE depends on the origin insertion] INTO [targetTable]
     SELECT * FROM [sourceTable] WHERE rowid = ?

     @param type type
     @return StatementInsert; 
     */
    StatementInsert
    getStatementForTamperingConflictType(const Lang::InsertSTMT::Type &type) const;

protected:
    static const std::string getUnionedViewPrefix();

    StatementInsert m_statementForMigration;
    StatementDelete m_statementForDeletingMigratedRow;
    StatementSelect m_statementForPickingRowIDs;
    StatementCreateView m_statementForCreatingUnionedView;

    std::string m_unionedViewName;
    SpinLock m_spin;
    std::atomic<bool> m_inited;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationInfo_hpp */
