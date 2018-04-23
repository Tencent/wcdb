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
    static std::shared_ptr<MigrationInfo>
    info(const std::string &targetTable,
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

protected:
    static std::string resolvedSchema(const std::string &path);

#pragma mark - Statement
public:
    /**
     INSERT OR FAIL INTO [targetTable]
     SELECT *
     FROM [sourceTable]
     LIMIT 1
     
     @return StatementInsert
     */
    const StatementInsert &getStatementForMigration() const;

    /**
     DELETE FROM [sourceTable]
     LIMIT 1
     
     @return StatementDelete
     */
    const StatementDelete &getStatementForDeleteingMigratedRow() const;

    /**
     SELECT 1 FROM [sourceTable] LIMIT 1
     
     @return StatementSelect
     */
    const StatementSelect &getStatementForCheckingSourceTableEmpty() const;

    /**
     DROP TABLE IF EXISTS [sourceTable]
     
     @return StatementDropTable
     */
    StatementDropTable getStatementForDroppingOldTable() const;

    /**     
     SELECT max(rowid) FROM [sourceTable] 
     UNION
     SELECT max(rowid) FROM [targetTable] 
     
     @return StatementSelect
     */
    StatementSelect getStatementForGettingMaxRowID() const;

    /**     
     SELECT seq FROM sqlite_sequence WHERE name = [sourceTable]
     
     @return SelectCore
     */
    SelectCore getSelectionForGettingSourceSequence() const;

    /**     
     SELECT seq FROM sqlite_sequence WHERE name = [targetTable]
     
     @return SelectCore
     */
    SelectCore getSelectionForGettingTargetSequence() const;

    /**     
     INSERT INTO sqlite_sequence VALUES([targetTable], [sequence])
     
     @return StatementInsert
     */
    StatementInsert getStatementForInsertingSequence(int sequence) const;

    /**     
     UPDATE sqlite_sequence SET seq = [sequence] WHERE name = [targetTable]
     
     @return StatementUpdate
     */
    StatementUpdate getStatementForUpdatingSequence(int sequence) const;

    /**     
     WITH merged(seq) AS SELECT XXXX
     SELECT max(seq) FROM merged
     
     @return StatementSelect
     */
    StatementSelect
    getStatementForMergedSequence(const StatementSelect &statementSelect);

protected:
    void prepareForMigrating();

    StatementInsert m_statementForMigration;
    StatementDelete m_statementForDeleteingMigratedRow;
    StatementSelect m_statementForCheckingSourceTableEmpty;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationInfo_hpp */
