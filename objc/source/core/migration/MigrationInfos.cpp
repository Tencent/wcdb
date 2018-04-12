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

#include <WCDB/MigrationInfos.hpp>

namespace WCDB {

#pragma mark - Initialize
std::shared_ptr<MigrationInfos>
MigrationInfos::infos(const std::list<std::shared_ptr<MigrationInfo>> &infos)
{
    return std::shared_ptr<MigrationInfos>(new MigrationInfos(infos));
}

MigrationInfos::MigrationInfos(
    const std::list<std::shared_ptr<MigrationInfo>> &infos)
    : m_migratingStarted(false)
{
    assert(!infos.empty());
    for (const auto &info : infos) {
        if (!info->isSameDatabaseMigration()) {
            auto iter = m_schemas.find(info->schema);
            if (iter == m_schemas.end()) {
                iter =
                    m_schemas
                        .insert({info->schema, {info->sourceDatabasePath, 0}})
                        .first;
            }
            ++iter->second.second;
        }
        assert(m_infos.find(info->targetTable) == m_infos.end());
        m_infos.insert({info->targetTable, info});
    }
}

#pragma mark - Basic
bool MigrationInfos::isSameDatabaseMigration() const
{
    return m_schemas.empty();
}

const std::map<std::string, std::pair<std::string, int>> &
MigrationInfos::getSchemasForAttaching() const
{
    return m_schemas;
}

const std::map<std::string, std::shared_ptr<MigrationInfo>> &
MigrationInfos::getInfos() const
{
    return m_infos;
}

MigrationInfo *MigrationInfos::getMigratingInfo()
{
    return m_migratingInfo.get();
}

bool MigrationInfos::didMigratingStart() const
{
    return m_migratingStarted.load();
}

void MigrationInfos::markAsMigrationStarted()
{
    m_migratingStarted.store(true);
}

void MigrationInfos::markAsMigrationStarted(const std::string &table)
{
    m_migratingStarted.store(true);
    assert(!table.empty());
    auto iter = m_infos.find(table);
    assert(iter != m_infos.end());
    m_migratingInfo = iter->second;
}

//TODO assertion for locking status
void MigrationInfos::markAsMigrating(const std::string &table)
{
    m_migratingStarted.store(true);
    assert(!table.empty());
    auto iter = m_infos.find(table);
    if (iter != m_infos.end()) {
        m_migratingInfo = iter->second;
    }
}

void MigrationInfos::markAsMigrated(bool &schemasChanged)
{
    assert(m_migratingInfo != nullptr);
    auto iter = m_infos.find(m_migratingInfo->targetTable);
    assert(iter != m_infos.end());
    auto schemaIter = m_schemas.find(iter->second->schema);
    assert(schemaIter != m_schemas.end());
    if (--schemaIter->second.second == 0) {
        m_schemas.erase(schemaIter);
        schemasChanged = true;
    } else {
        schemasChanged = false;
    }
    m_infos.erase(iter);
    m_migratingInfo = nullptr;
}

SharedLock &MigrationInfos::getSharedLock()
{
    return m_lock;
}

#pragma mark - Tamper
#define TAMPER_PREPARE(cowLang)                                                \
    if (cowLang.empty()) {                                                     \
        return false;                                                          \
    };                                                                         \
    auto &lang = cowLang.get_or_copy();

bool MigrationInfos::tamper(Statement &statement)
{
    if (m_infos.empty()) {
        return false;
    }
    Lang::CopyOnWriteLazyLang<Lang::Lang> &cowLang = statement.getCOWLang();
    if (!cowLang.empty()) {
        switch (statement.getStatementType()) {
            case Statement::Type::Update: {
                Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT> stmt(cowLang);
                if (tamper(stmt)) {
                    statement.getCOWLang().assign(stmt);
                    return true;
                }
            } break;
            case Statement::Type::Insert: {
                Lang::CopyOnWriteLazyLang<Lang::InsertSTMT> stmt(cowLang);
                if (tamper(stmt)) {
                    statement.getCOWLang().assign(stmt);
                    return true;
                }
            } break;
            case Statement::Type::Select: {
                Lang::CopyOnWriteLazyLang<Lang::SelectSTMT> stmt(cowLang);
                if (tamper(stmt)) {
                    statement.getCOWLang().assign(stmt);
                    return true;
                }
            } break;
            case Statement::Type::Delete: {
                Lang::CopyOnWriteLazyLang<Lang::DeleteSTMT> stmt(cowLang);
                if (tamper(stmt)) {
                    statement.getCOWLang().assign(stmt);
                    return true;
                }
            } break;
            case Statement::Type::CreateTable: {
                Lang::CopyOnWriteLazyLang<Lang::CreateTableSTMT> stmt(cowLang);
                if (tamper(stmt)) {
                    statement.getCOWLang().assign(stmt);
                    return true;
                }
            } break;
            case Statement::Type::DropTable: {
                Lang::CopyOnWriteLazyLang<Lang::DropTableSTMT> stmt(cowLang);
                if (tamper(stmt)) {
                    statement.getCOWLang().assign(stmt);
                    return true;
                }
            } break;
            default:
                break;
        }
    }
    return false;
}

#pragma mark - Tamper STMT
bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::DropTableSTMT> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperTableAndSchemaName(lang.name, lang.schemaName);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::SelectSTMT> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    Lang::CopyOnWriteLazyLang<Lang::SelectCore> copy = lang.selectCore;

    bool result = tamperList(lang.commonTableExpressions);
    result = tamper(lang.selectCore) || result;
    result = tamperList(lang.compoundCores) || result;
    result = tamperList(lang.orderingTerms) || result;
    result = tamper(lang.limit) || result;
    result = tamper(lang.limitParameter) || result;
    if (result) {
        Lang::CopyOnWriteLazyLang<Lang::SelectSTMT::Compound> compoundCore;
        Lang::SelectSTMT::Compound &compound = compoundCore.get_or_copy();
        compound.compoundOperator = Lang::SelectSTMT::Compound::Operator::Union;
        compound.selectCore.assign(copy.get());

        lang.compoundCores.append(compoundCore);
    }
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.withClause);
    result = tamper(lang.qualifiedTableName);
    result = tamperList(lang.keyValues) || result;
    result = tamper(lang.condition) || result;
    result = tamperList(lang.orderingTerms) || result;
    result = tamper(lang.limit) || result;
    result = tamper(lang.limitParameter) || result;
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::DeleteSTMT> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.withClause);
    result = tamper(lang.qualifiedTableName);
    result = tamper(lang.condition) || result;
    result = tamperList(lang.orderingTerms) || result;
    result = tamper(lang.limit) || result;
    result = tamper(lang.limitParameter) || result;
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::InsertSTMT> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.withClause);
    result =
        tamperTableAndSchemaName(lang.tableName, lang.schemaName) || result;
    switch (lang.switcher) {
        case Lang::InsertSTMT::Switch::Select:
            result = tamper(lang.selectSTMT) || result;
            break;
        case Lang::InsertSTMT::Switch::Values:
            result = tamperList(lang.exprs) || result;
            break;
        default:
            break;
    }
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::CreateTableSTMT> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    switch (lang.switcher) {
        case Lang::CreateTableSTMT::Switch::Select:
            return tamper(lang.selectSTMT);
        default:
            break;
    }
    return false;
}

#pragma mark - Lang
bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::CommonTableExpression> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.selectSTMT);
}

bool MigrationInfos::tamper(Lang::CopyOnWriteLazyLang<Lang::Expr> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    switch (lang.type) {
        case Lang::ExprBase::Type::Column:
            return tamper(lang.exprColumn);
        case Lang::ExprBase::Type::UnaryOperator:
            return tamper(lang.exprUnaryOperator);
        case Lang::ExprBase::Type::BinaryOperator:
            return tamper(lang.exprBinaryOperator);
        case Lang::ExprBase::Type::Function:
            return tamper(lang.exprFunction);
        case Lang::ExprBase::Type::List:
            return tamper(lang.exprList);
        case Lang::ExprBase::Type::Cast:
            return tamper(lang.exprCast);
        case Lang::ExprBase::Type::Collate:
            return tamper(lang.exprCollate);
        case Lang::ExprBase::Type::Pattern:
            return tamper(lang.exprPattern);
        case Lang::ExprBase::Type::Null:
            return tamper(lang.exprNull);
        case Lang::ExprBase::Type::In:
            return tamper(lang.exprIn);
        case Lang::ExprBase::Type::Exists:
            return tamper(lang.exprExists);
        case Lang::ExprBase::Type::Case:
            return tamper(lang.exprCase);
        default:
            break;
    }
    return false;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprColumn> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperTableAndSchemaName(lang.tableName, lang.schemaName);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.expr);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.left);
    result = tamper(lang.right) || result;
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprFunction> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    switch (lang.type) {
        case Lang::ExprFunction::Type::Expr:
        case Lang::ExprFunction::Type::DistinctExpr:
            return tamperList(lang.exprs);
        default:
            break;
    }
    return false;
}

bool MigrationInfos::tamper(Lang::CopyOnWriteLazyLang<Lang::ExprList> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperList(lang.exprs);
}

bool MigrationInfos::tamper(Lang::CopyOnWriteLazyLang<Lang::ExprCast> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.expr);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprCollate> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.expr);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprPattern> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.left);
    result = tamper(lang.right) || result;
    result = tamper(lang.escape) || result;
    return result;
}

bool MigrationInfos::tamper(Lang::CopyOnWriteLazyLang<Lang::ExprNull> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.expr);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprBetween> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.expr);
    result = tamper(lang.left) || result;
    result = tamper(lang.right) || result;
    return result;
}

bool MigrationInfos::tamper(Lang::CopyOnWriteLazyLang<Lang::ExprIn> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.expr);
    switch (lang.switcher) {
        case Lang::ExprIn::Switch::Select:
            result = tamper(lang.selectSTMT) || result;
            break;
        case Lang::ExprIn::Switch::Expr:
        case Lang::ExprIn::Switch::Function:
            result = tamperList(lang.exprs) || result;
            break;
        case Lang::ExprIn::Switch::Table:
            result = tamperTableAndSchemaName(lang.tableNameOrFunction,
                                              lang.schemaName) ||
                     result;
            break;
        default:
            break;
    }
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprExists> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.selectSTMT);
}

bool MigrationInfos::tamper(Lang::CopyOnWriteLazyLang<Lang::ExprCase> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.exprCase);
    result = tamperList(lang.pairs) || result;
    result = tamper(lang.exprElse) || result;
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ExprCase::Pair> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.when);
    result = tamper(lang.then) || result;
    return result;
}

bool MigrationInfos::tamper(Lang::CopyOnWriteLazyLang<Lang::ExprBase> &cowLang)
{
    bool result = false;
    switch (cowLang.get().getType()) {
        case Lang::ExprBase::Type::Column: {
            Lang::CopyOnWriteLazyLang<Lang::ExprColumn> t;
            t.assign((const Lang::ExprColumn &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::UnaryOperator: {
            Lang::CopyOnWriteLazyLang<Lang::ExprUnaryOperation> t;
            t.assign((const Lang::ExprUnaryOperation &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::BinaryOperator: {
            Lang::CopyOnWriteLazyLang<Lang::ExprBinaryOperation> t;
            t.assign((const Lang::ExprBinaryOperation &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::List: {
            Lang::CopyOnWriteLazyLang<Lang::ExprList> t;
            t.assign((const Lang::ExprList &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Cast: {
            Lang::CopyOnWriteLazyLang<Lang::ExprCast> t;
            t.assign((const Lang::ExprCast &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Collate: {
            Lang::CopyOnWriteLazyLang<Lang::ExprCollate> t;
            t.assign((const Lang::ExprCollate &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Pattern: {
            Lang::CopyOnWriteLazyLang<Lang::ExprPattern> t;
            t.assign((const Lang::ExprPattern &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Null: {
            Lang::CopyOnWriteLazyLang<Lang::ExprNull> t;
            t.assign((const Lang::ExprNull &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Between: {
            Lang::CopyOnWriteLazyLang<Lang::ExprBetween> t;
            t.assign((const Lang::ExprBetween &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::In: {
            Lang::CopyOnWriteLazyLang<Lang::ExprIn> t;
            t.assign((const Lang::ExprIn &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Exists: {
            Lang::CopyOnWriteLazyLang<Lang::ExprExists> t;
            t.assign((const Lang::ExprExists &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Case: {
            Lang::CopyOnWriteLazyLang<Lang::ExprCase> t;
            t.assign((const Lang::ExprCase &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::ExprBase::Type::Expr: {
            Lang::CopyOnWriteLazyLang<Lang::Expr> t;
            t.assign((const Lang::Expr &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        default:
            break;
    }
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::JoinClause> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = tamper(lang.tableOrSubquery);
    result = tamperList(lang.joinOperands) || result;
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::JoinClause::Operand> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.tableOrSubquery);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::JoinConstraint> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    switch (lang.type) {
        case Lang::JoinConstraint::Type::On:
            return tamper(lang.expr);
        default:
            break;
    }
    return false;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::OrderingTerm> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.expr);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::QualifiedTableName> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperTableAndSchemaName(lang.tableName, lang.schemaName);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::ResultColumn> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    switch (lang.type) {
        case Lang::ResultColumn::Type::Star:
            return tamperTableName(lang.tableName);
        case Lang::ResultColumn::Type::Expr:
            return tamper(lang.expr);
        default:
            break;
    }
    return false;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::SelectCore> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    bool result = false;
    switch (lang.switcher) {
        case Lang::SelectCore::Switch::Select:
            result = tamperList(lang.resultColumns) || result;
            switch (lang.fromSwitcher) {
                case Lang::SelectCore::FromSwitch::TableOrSubquery:
                    result = tamperList(lang.tableOrSubquerys) || result;
                    break;
                case Lang::SelectCore::FromSwitch::JoinClause:
                    result = tamper(lang.joinClause) || result;
                    break;
                default:
                    break;
            }
            result = tamper(lang.condition) || result;
            result = tamperList(lang.groups) || result;
            result = tamper(lang.having) || result;
            break;
        case Lang::SelectCore::Switch::Values:
            result = tamperList(lang.values) || result;
            break;
        default:
            break;
    }
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::SelectSTMT::Compound> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.selectCore);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubquery> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    switch (lang.type) {
        case Lang::TableOrSubqueryBase::Type::Table:
            return tamper(lang.tableOrSubqueryTable);
        case Lang::TableOrSubqueryBase::Type::TableFunction:
            return tamper(lang.tableOrSubqueryTableFunction);
        case Lang::TableOrSubqueryBase::Type::List:
            return tamper(lang.tableOrSubqueryList);
        case Lang::TableOrSubqueryBase::Type::JoinClause:
            return tamper(lang.tableOrSubqueryJoinClause);
        case Lang::TableOrSubqueryBase::Type::Select:
            return tamper(lang.tableOrSubquerySelect);
        default:
            break;
    }
    return false;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTable> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperTableAndSchemaName(lang.tableName, lang.schemaName);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTableFunction> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperList(lang.exprs);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryList> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperList(lang.tableOrSubquerys);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryJoinClause> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.joinClause);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubquerySelect> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.selectSTMT);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryBase> &cowLang)
{
    bool result = false;
    switch (cowLang.get().getType()) {
        case Lang::TableOrSubqueryBase::Type::Table: {
            Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTable> t;
            t.assign((const Lang::TableOrSubqueryTable &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::TableOrSubqueryBase::Type::TableFunction: {
            Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTableFunction> t;
            t.assign(
                (const Lang::TableOrSubqueryTableFunction &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::TableOrSubqueryBase::Type::List: {
            Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryList> t;
            t.assign((const Lang::TableOrSubqueryList &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::TableOrSubqueryBase::Type::JoinClause: {
            Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryJoinClause> t;
            t.assign((const Lang::TableOrSubqueryJoinClause &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::TableOrSubqueryBase::Type::Select: {
            Lang::CopyOnWriteLazyLang<Lang::TableOrSubquerySelect> t;
            t.assign((const Lang::TableOrSubquerySelect &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        case Lang::TableOrSubqueryBase::Type::TableOrSubquery: {
            Lang::CopyOnWriteLazyLang<Lang::TableOrSubquery> t;
            t.assign((const Lang::TableOrSubquery &) cowLang.get());
            result = tamper(t);
            cowLang.assign(t);
        } break;
        default:
            break;
    }
    return result;
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::UpdateSTMT::KeyValue> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.value);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::WithClause> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamperList(lang.pairs);
}

bool MigrationInfos::tamper(
    Lang::CopyOnWriteLazyLang<Lang::WithClause::Pair> &cowLang)
{
    TAMPER_PREPARE(cowLang);
    return tamper(lang.selectSTMT);
}

bool MigrationInfos::tamperTableName(CopyOnWriteString &tableName)
{
    auto iter = m_infos.find(tableName.get());
    if (iter != m_infos.end()) {
        tableName.assign(iter->second->sourceTable);
        return true;
    }
    return false;
}

bool MigrationInfos::tamperTableAndSchemaName(CopyOnWriteString &tableName,
                                              CopyOnWriteString &schemaName)
{
    if (schemaName.empty()) {
        auto iter = m_infos.find(tableName.get());
        if (iter != m_infos.end()) {
            tableName.assign(iter->second->sourceTable);
            if (iter->second->isSameDatabaseMigration()) {
                schemaName.assign(iter->second->schema);
            }
            return true;
        }
    }
    return false;
}

} //namespace WCDB
