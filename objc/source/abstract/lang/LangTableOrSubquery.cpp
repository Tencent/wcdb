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

namespace WCDB {

namespace Lang {

CopyOnWriteString TableOrSubqueryTable::SQL() const
{
    std::string description;
    if (!schemaName.empty()) {
        description.append(schemaName.get() + ".");
    }
    LangDebugAssert(!tableName.empty());
    description.append(tableName.get());
    if (!tableAlias.empty()) {
        description.append(" AS " + tableAlias.get());
    }
    if (indexSwitcher != IndexSwitch::NotSet) {
        switch (indexSwitcher) {
            case IndexSwitch::Indexed:
                LangDebugAssert(!indexName.empty());
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
    if (!schemaName.empty()) {
        description.append(schemaName.get() + ".");
    }
    LangDebugAssert(!tableFunctionName.empty());
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
    LangDebugAssert(!joinClause.empty());
    return "(" + joinClause.description().get() + ")";
}

TableOrSubqueryBase::Type TableOrSubqueryJoinClause::getType() const
{
    return TableOrSubqueryBase::Type::JoinClause;
}

CopyOnWriteString TableOrSubquerySelect::SQL() const
{
    std::string description;
    LangDebugAssert(!selectSTMT.empty());
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
    LangDebugAssert(!tableOrSubquerys.empty());
    return "(" + tableOrSubquerys.description().get() + ")";
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
    switch (type) {
        case Type::Table:
            LangDebugAssert(!tableOrSubqueryTable.empty());
            return tableOrSubqueryTable.description();
        case Type::TableFunction:
            LangDebugAssert(!tableOrSubqueryTableFunction.empty());
            return tableOrSubqueryTableFunction.description();
        case Type::JoinClause:
            LangDebugAssert(!tableOrSubqueryJoinClause.empty());
            return tableOrSubqueryJoinClause.description();
        case Type::Select:
            LangDebugAssert(!tableOrSubquerySelect.empty());
            return tableOrSubquerySelect.description();
        case Type::List:
            LangDebugAssert(!tableOrSubqueryList.empty());
            return tableOrSubqueryList.description();
        default:
            WCTDebugFatalError("", return "";);
    }
}

TableOrSubqueryBase::Type TableOrSubquery::getType() const
{
    return TableOrSubqueryBase::Type::TableOrSubquery;
}

} // namespace Lang

} // namespace WCDB
