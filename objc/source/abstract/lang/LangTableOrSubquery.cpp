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
    assert(!tableName.empty());
    description.append(tableName.get());
    if (!tableAlias.empty()) {
        description.append(" AS " + tableAlias.get());
    }
    if (indexSwitcher != IndexSwitch::NotSet) {
        switch (indexSwitcher) {
            case IndexSwitch::Indexed:
                assert(!indexName.empty());
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
    assert(!tableFunctionName.empty());
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

CopyOnWriteString TableOrSubqueryJoinClause::SQL() const
{
    assert(!joinClause.empty());
    return "(" + joinClause.description().get() + ")";
}

CopyOnWriteString TableOrSubquerySelect::SQL() const
{
    std::string description;
    assert(!selectSTMT.empty());
    description.append("(" + selectSTMT.description().get() + ")");
    if (!tableAlias.empty()) {
        description.append(" AS " + tableAlias.get());
    }
    return description;
}

CopyOnWriteString TableOrSubqueryList::SQL() const
{
    assert(!tableOrSubquerys.empty());
    return "(" + tableOrSubquerys.description().get() + ")";
}

TableOrSubquery::TableOrSubquery() : switcher(Switch::NotSet)
{
}

CopyOnWriteString TableOrSubquery::SQL() const
{
    switch (switcher) {
        case Switch::Table:
            assert(!tableOrSubqueryTable.empty());
            return tableOrSubqueryTable.description();
        case Switch::TableFunction:
            assert(!tableOrSubqueryTableFunction.empty());
            return tableOrSubqueryTableFunction.description();
        case Switch::JoinClause:
            assert(!tableOrSubqueryJoinClause.empty());
            return tableOrSubqueryJoinClause.description();
        case Switch::Select:
            assert(!tableOrSubquerySelect.empty());
            return tableOrSubquerySelect.description();
        case Switch::List:
            assert(!tableOrSubqueryList.empty());
            return tableOrSubqueryList.description();
        default:
            assert(false);
            break;
    }
}

} // namespace Lang

} // namespace WCDB
