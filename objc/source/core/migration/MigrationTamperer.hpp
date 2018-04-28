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

#ifndef MigrationTamperer_hpp
#define MigrationTamperer_hpp

#include <WCDB/MigrationInfos.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationTamperer {
public:
    MigrationTamperer(MigrationInfos *infos);

    MigrationTamperer() = delete;
    MigrationTamperer(const MigrationTamperer &) = delete;
    MigrationTamperer &operator=(const MigrationTamperer &) = delete;

#pragma mark - Tamper
public:
    //return true to indicate source statement is tampered
    bool tamper(const Statement &statement);

    const Statement &getTamperedSourceStatement() const;

    bool isTampered() const;
    const Statement &getTamperedStatement() const;

    const std::shared_ptr<MigrationInfo> &getAssociatedInfo() const;

protected:
    bool doTamper(Statement &statement);

    Statement m_tamperedSourceStatement;

    Statement m_tamperedStatement;
    bool m_isTampered;

    bool m_isSelectTampering;
    bool m_isSourceTampering;

    MigrationInfos *m_migrationInfos;
    const std::map<std::string, std::shared_ptr<MigrationInfo>> &m_infosMap;
    std::shared_ptr<MigrationInfo> m_associatedInfo;
    SharedLockGuard m_lockGuard;

#pragma mark - Tamper STMT
protected:
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::DeleteSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::SelectSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::InsertSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::CreateTableSTMT> &cowLang);
    bool tamper(Lang::CopyOnWriteLazyLang<Lang::DropTableSTMT> &cowLang);
#pragma mark - Lang
protected:
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

#pragma GCC visibility pop

#endif /* MigrationTamperer_hpp */
