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

#ifndef MigrationInfos_hpp
#define MigrationInfos_hpp

#include <WCDB/Lock.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <set>

namespace WCDB {

class MigrationInfos {
#pragma mark - Initialize
public:
    static std::shared_ptr<MigrationInfos>
    infos(const std::list<std::shared_ptr<MigrationInfo>> &infos);

protected:
    MigrationInfos(const std::list<std::shared_ptr<MigrationInfo>> &infos);
    MigrationInfos() = delete;
    MigrationInfos(const MigrationInfos &) = delete;
    MigrationInfos &operator=(const MigrationInfos &) = delete;

#pragma mark - Basic
public:
    bool isSameDatabaseMigration() const;
    const std::map<std::string, std::pair<std::string, int>> &
    getSchemasForAttaching() const;
    const std::map<std::string, std::shared_ptr<MigrationInfo>> &
    getInfos() const;
    MigrationInfo *getMigratingInfo();
    void markAsMigrationStarted();
    void markAsMigrationStarted(const std::string &table);
    void markAsMigrating(const std::string &table);
    void markAsMigrated(bool &schemasChanged);
    bool didMigratingStart() const;

    SharedLock &getSharedLock();

protected:
    SharedLock m_lock;
    std::atomic<bool> m_migratingStarted;
    std::shared_ptr<MigrationInfo> m_migratingInfo;
    //schema -> {path, reference}
    std::map<std::string, std::pair<std::string, int>> m_schemas;
    //target table -> infos
    std::map<std::string, std::shared_ptr<MigrationInfo>> m_infos;

#pragma mark - Tamper
public:
    bool tamper(Statement &statement);

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
    bool tamperTableAndSchemaName(CopyOnWriteString &tableName,
                                  CopyOnWriteString &schemaName);

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

#endif /* MigrationInfos_hpp */
