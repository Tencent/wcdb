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

#include <WCDB/Lang.h>
#include <WCDB/String.hpp>

namespace WCDB {

namespace Lang {

CopyOnWriteString TableOrSubqueryTable::SQL() const
{
    std::string description;
    description.append(schemaName.empty() ? mainSchema() : schemaName.get());
    description.append(".");
    LangRemedialAssert(!tableName.empty());
    description.append(tableName.get());
    if (!tableAlias.empty()) {
        description.append(" AS " + tableAlias.get());
    }
    if (indexSwitcher != IndexSwitch::NotSet) {
        switch (indexSwitcher) {
            case IndexSwitch::Indexed:
                LangRemedialAssert(!indexName.empty());
                description.append(" INDEXED BY " + indexName.get());
                break;
            case IndexSwitch::NotIndexed:
                description.append(" NOT INDEXED");
                break;
            default:
                break;
        }
    }
    return description;
}

TableOrSubqueryBase::Type TableOrSubqueryTable::getType() const
{
    return TableOrSubqueryBase::Type::Table;
}

TableOrSubqueryTable::TableOrSubqueryTable()
    : indexSwitcher(IndexSwitch::NotSet)
{
}

CopyOnWriteString TableOrSubqueryTableFunction::SQL() const
{
    std::string description;
    description.append(schemaName.empty() ? mainSchema() : schemaName.get());
    description.append(".");
    LangRemedialAssert(!tableFunctionName.empty());
    description.append(tableFunctionName.get());
    description.append("(");
    if (!exprs.empty()) {
        description.append(exprs.description().get());
    }
    description.append(")");
    if (!tableAlias.empty()) {
        description.append(" AS " + tableAlias.get());
    }
    return description;
}

TableOrSubqueryBase::Type TableOrSubqueryTableFunction::getType() const
{
    return TableOrSubqueryBase::Type::TableFunction;
}

CopyOnWriteString TableOrSubqueryJoinClause::SQL() const
{
    std::string description("(");
    LangRemedialAssert(!joinClause.empty());
    description.append(joinClause.description().get());
    description.append(")");
    return description;
}

TableOrSubqueryBase::Type TableOrSubqueryJoinClause::getType() const
{
    return TableOrSubqueryBase::Type::JoinClause;
}

CopyOnWriteString TableOrSubquerySelect::SQL() const
{
    std::string description;
    LangRemedialAssert(!selectSTMT.empty());
    description.append("(" + selectSTMT.description().get() + ")");
    if (!tableAlias.empty()) {
        description.append(" AS " + tableAlias.get());
    }
    return description;
}

TableOrSubqueryBase::Type TableOrSubquerySelect::getType() const
{
    return TableOrSubqueryBase::Type::Select;
}

CopyOnWriteString TableOrSubqueryList::SQL() const
{
    std::string description("(");
    LangRemedialAssert(!tableOrSubquerys.empty());
    description.append(tableOrSubquerys.description().get());
    description.append(")");
    return description;
}

TableOrSubqueryBase::Type TableOrSubqueryList::getType() const
{
    return TableOrSubqueryBase::Type::List;
}

TableOrSubquery::TableOrSubquery() : type(Type::NotSet)
{
}

CopyOnWriteString TableOrSubquery::SQL() const
{
    std::string description = String::empty();
    switch (type) {
        case Type::Table:
            LangRemedialAssert(!tableOrSubqueryTable.empty());
            return tableOrSubqueryTable.description();
        case Type::TableFunction:
            LangRemedialAssert(!tableOrSubqueryTableFunction.empty());
            return tableOrSubqueryTableFunction.description();
        case Type::JoinClause:
            LangRemedialAssert(!tableOrSubqueryJoinClause.empty());
            return tableOrSubqueryJoinClause.description();
        case Type::Select:
            LangRemedialAssert(!tableOrSubquerySelect.empty());
            return tableOrSubquerySelect.description();
        case Type::List:
            LangRemedialAssert(!tableOrSubqueryList.empty());
            return tableOrSubqueryList.description();
        default:
            LangRemedialFatalError();
    }
    return description;
}

TableOrSubqueryBase::Type TableOrSubquery::getType() const
{
    return TableOrSubqueryBase::Type::TableOrSubquery;
}

} // namespace Lang

} // namespace WCDB
