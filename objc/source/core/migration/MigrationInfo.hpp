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

#include <WCDB/Database.hpp>

namespace WCDB {

class MigrationInfo {
#pragma mark - Initialize
public:
    static std::shared_ptr<MigrationInfo>
    info(const std::string &targetTable,
         const std::string &sourceTable,
         const std::string &sourceDatabasePath = "");

    MigrationInfo() = delete;
    MigrationInfo(const MigrationInfo &) = delete;
    MigrationInfo &operator=(const MigrationInfo &) = delete;

#pragma mark - Basic
    const std::string &getSourceDatabasePath() const;
    bool isSameDatabaseMigration() const;

    const std::string &getSourceTableName() const;
    const std::string &getTargetTableName() const;
    TableOrSubquery getSourceTable() const;

    bool isMigrating() const;
    void setMigrating(bool migrating);

    static const std::string migrationSchema;

protected:
    MigrationInfo(const std::string &targetTable,
                  const std::string &sourceTable,
                  const std::string &sourceDatabasePath);

    std::string m_sourceDatabasePath; // empty for the same database migration
    std::string m_sourceTable;
    std::string m_targetTable;

    std::atomic<bool> m_isMigrating;

#pragma mark - Statement
public:
    /**
     ATTACH [sourcePath] AS WCDBMigration

     @return StatementAttach
     */
    const StatementAttach &getStatementForAttachingOldDatabase() const;

    /**
     DETACH WCDBMigration

     @return StatementDetach
     */
    static const StatementDetach &getStatementForDetachingOldDatabase();

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
    void setupStatement();
    StatementAttach m_statementForAttachingOldDatabase;
    StatementDetach m_statementForDetachingOldDatabase;
    StatementInsert m_statementForMigration;
    StatementDelete m_statementForDeleteingMigratedRow;

#pragma mark - Tamper
public:
    bool tamper(Statement &statement);
    bool tamperInsertStatementForUnstartedMigration(Statement &statement);

protected:
#pragma mark - Tamper STMT
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::DeleteSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::SelectSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::InsertSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::CreateTableSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::DropTableSTMT> &cowLang);
#pragma mark - Lang
    bool
    tamper(Lang::CopyOnWriteLazyLang<Lang::CommonTableExpression> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::Expr> &lang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprColumn> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprFunction> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprList> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprCast> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprCollate> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprPattern> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprNull> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprBetween> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprIn> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprExists> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprCase> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprCase::Pair> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ExprBase> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::JoinClause> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::JoinClause::Operand> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::JoinConstraint> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::OrderingTerm> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::QualifiedTableName> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::ResultColumn> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::SelectCore> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::SelectSTMT::Compound> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::TableOrSubquery> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTable> &cowLang);
    bool tamper(
        Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTableFunction> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryList> &cowLang);
    bool
    tamper(Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryJoinClause> &cowLang);
    bool
    tamper(Lang::CopyOnWriteLazyLang<Lang::TableOrSubquerySelect> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryBase> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT::KeyValue> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::WithClause> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::WithClause::Pair> &cowLang);

    bool tamperTableName(CopyOnWriteString &tableName);
    void tamperSchemaName(CopyOnWriteString &schemaName);

    template <typename T>
    bool tamperList(Lang::CopyOnWriteLazyLangList<T> &lang)
    {
        if (lang.empty()) {
            return false;
        }
        bool result = false;
        for (auto &element : lang.get_or_copy()) {
            if (tamper(element)) {
                result = true;
            }
        }
        return result;
    }
};

} //namespace WCDB

#endif /* MigrationInfo_hpp */
