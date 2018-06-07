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

#include <WCDB/Assertion.hpp>
#include <WCDB/Lang.h>

namespace WCDB {

namespace Lang {

SelectSTMT::SelectSTMT() : recursive(false), offset(false)
{
}

void SelectSTMT::resolveTableOrSubquery(
    CopyOnWriteLazyLang<TableOrSubquery> &tableOrSubquery) const
{
    if (tableOrSubquery.empty()) {
        return;
    }
    switch (tableOrSubquery.get().type) {
        case TableOrSubqueryBase::Type::Table:
            resolveTable(tableOrSubquery.get_or_copy().tableOrSubqueryTable);
            break;
        case TableOrSubqueryBase::Type::TableOrSubquery:
            resolveTableOrSubquery(tableOrSubquery);
            break;
        case TableOrSubqueryBase::Type::List:
            resolveTableOrSubqueryList(
                tableOrSubquery.get_or_copy().tableOrSubqueryList);
        default:
            break;
    }
}

void SelectSTMT::resolveTableOrSubqueryBases(
    CopyOnWriteLazyLangList<TableOrSubqueryBase> &tableOrSubquerys) const
{
    if (tableOrSubquerys.empty()) {
        return;
    }
    for (auto &tableOrSubquery : tableOrSubquerys.get_or_copy()) {
        if (tableOrSubquery.empty()) {
            continue;
        }
        switch (tableOrSubquery.get().getType()) {
            case TableOrSubqueryBase::Type::Table: {
                CopyOnWriteLazyLang<TableOrSubqueryTable> table =
                    tableOrSubquery;
                resolveTable(table);
                tableOrSubquery.assign(table);
            } break;
            case TableOrSubqueryBase::Type::TableOrSubquery: {
                CopyOnWriteLazyLang<TableOrSubquery> table = tableOrSubquery;
                resolveTableOrSubquery(table);
                tableOrSubquery.assign(table);
            } break;
            case TableOrSubqueryBase::Type::List: {
                CopyOnWriteLazyLang<TableOrSubqueryList> table =
                    tableOrSubquery;
                resolveTableOrSubqueryList(table);
                tableOrSubquery.assign(table);
            } break;
            default:
                break;
        }
    }
}

void SelectSTMT::resolveTableOrSubqueryList(
    CopyOnWriteLazyLang<TableOrSubqueryList> &tableOrSubqueryList) const
{
    if (tableOrSubqueryList.empty()) {
        return;
    }
    resolveTableOrSubqueryBases(
        tableOrSubqueryList.get_or_copy().tableOrSubquerys);
}

void SelectSTMT::resolveTable(
    CopyOnWriteLazyLang<TableOrSubqueryTable> &table) const
{
    if (table.empty() || !table.get().schemaName.isNull() ||
        table.get().tableName.empty()) {
        return;
    }
    for (const auto &commonTableExpression : commonTableExpressions.get()) {
        if (commonTableExpression.empty()) {
            continue;
        }
        const CopyOnWriteString &tableName =
            commonTableExpression.get().tableName;
        if (tableName.equal(table.get().tableName.get())) {
            table.get_or_copy().schemaName.assign(anySchema());
        }
    }
}

void SelectSTMT::resolveTableOrSubquerys(
    CopyOnWriteLazyLangList<TableOrSubquery> &tableOrSubquerys) const
{
    if (tableOrSubquerys.empty()) {
        return;
    }
    for (auto &tableOrSubquery : tableOrSubquerys.get_or_copy()) {
        resolveTableOrSubquery(tableOrSubquery);
    }
}

CopyOnWriteString SelectSTMT::SQL() const
{
    std::string description;
    if (!commonTableExpressions.empty()) {
        description.append("WITH ");
        if (recursive) {
            description.append("RECURSIVE ");
        }
        description.append(commonTableExpressions.description().get() + " ");
    }

    LangRemedialAssert(!selectCore.empty());

    auto core = selectCore;
    if (!commonTableExpressions.empty()) {
        resolveTableOrSubquerys(core.get_or_copy().tableOrSubquerys);
    }
    description.append(core.description().get());

    if (!compoundCores.empty()) {
        CopyOnWriteLazyLangList<Compound> compounds = compoundCores;
        if (!commonTableExpressions.empty()) {
            for (auto &compound : compounds.get_or_copy()) {
                if (compound.empty()) {
                    continue;
                }
                auto core = compound.get_or_copy().selectCore;
                if (core.empty()) {
                    continue;
                }
                resolveTableOrSubquerys(core.get_or_copy().tableOrSubquerys);
            }
        }
        description.append(" " + compounds.description().get());
    }

    if (!orderingTerms.empty()) {
        description.append(" ORDER BY " + orderingTerms.description().get());
    }

    if (!limit.empty()) {
        description.append(" LIMIT " + limit.description().get());
        if (!limitParameter.empty()) {
            if (offset) {
                description.append(" OFFSET ");
            } else {
                description.append(", ");
            }
            description.append(limitParameter.description().get());
        }
    }

    return description;
}

SelectSTMT::Compound::Compound() : compoundOperator(Operator::NotSet)
{
}

CopyOnWriteString SelectSTMT::Compound::SQL() const
{
    std::string description;
    description.append(SelectSTMT::Compound::OperatorName(compoundOperator));
    description.append(" ");
    description.append(selectCore.description().get());
    return description;
}

constexpr const char *
SelectSTMT::Compound::OperatorName(const Operator &compoundOperator)
{
    switch (compoundOperator) {
        case Operator::Union:
            return "UNION";
        case Operator::UnionAll:
            return "UNION ALL";
        case Operator::Intersect:
            return "INTERSECT";
        case Operator::Except:
            return "EXCEPT";
        default:
            return "";
    }
}

STMT::Type SelectSTMT::getSTMTType() const
{
    return STMT::Type::Select;
}

STMT::Type SelectSTMT::getType()
{
    return STMT::Type::Select;
}

//TODO refactor: remove redundant calculatedDescription
template <>
CopyOnWriteString
CopyOnWriteLazyLangList<SelectSTMT::Compound>::calculatedDescription() const
{
    std::string description;
    bool space = false;
    for (const auto &element : this->get()) {
        if (space) {
            description.append(" ");
        } else {
            space = true;
        }
        LangRemedialAssert(!element.empty());
        description.append(element.description().get());
    }
    return description;
}

} // namespace Lang

} // namespace WCDB
