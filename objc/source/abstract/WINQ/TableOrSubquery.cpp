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
#include <WCDB/WINQ.h>

namespace WCDB {

TableOrSubquery::TableOrSubquery(const StatementSelect &selectSTMT)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    lang.type = Lang::TableOrSubquery::Type::Select;
    Lang::TableOrSubquerySelect &langSelect = lang.tableOrSubquerySelect.get_or_copy();
    langSelect.selectSTMT.assign(selectSTMT.getCOWLang());
}

TableOrSubquery::TableOrSubquery(const JoinClause &joinClause)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    lang.type = Lang::TableOrSubquery::Type::JoinClause;
    Lang::TableOrSubqueryJoinClause &langJoinClause
    = lang.tableOrSubqueryJoinClause.get_or_copy();
    langJoinClause.joinClause.assign(joinClause.getCOWLang());
}

TableOrSubquery::TableOrSubquery(const std::list<TableOrSubquery> &tableOrSubquerys)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    lang.type = Lang::TableOrSubquery::Type::List;
    Lang::TableOrSubqueryList &langList = lang.tableOrSubqueryList.get_or_copy();
    for (const TableOrSubquery &tableOrSubquery : tableOrSubquerys) {
        langList.tableOrSubquerys.append(tableOrSubquery.getCOWLang());
    }
}

TableOrSubquery &TableOrSubquery::withSchema(const std::string &schemaName)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    switch (lang.type) {
    case Lang::TableOrSubquery::Type::Table:
        lang.tableOrSubqueryTable.get_or_copy().schemaName.assign(schemaName);
        break;
    case Lang::TableOrSubquery::Type::TableFunction:
        lang.tableOrSubqueryTableFunction.get_or_copy().schemaName.assign(schemaName);
        break;
    default:
        WINQRemedialError();
    }
    return *this;
}

TableOrSubquery &TableOrSubquery::as(const std::string &tableAlias)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    switch (lang.type) {
    case Lang::TableOrSubquery::Type::Table:
        lang.tableOrSubqueryTable.get_or_copy().tableAlias.assign(tableAlias);
        break;
    case Lang::TableOrSubquery::Type::TableFunction:
        lang.tableOrSubqueryTableFunction.get_or_copy().tableAlias.assign(tableAlias);
        break;
    case Lang::TableOrSubquery::Type::Select:
        lang.tableOrSubquerySelect.get_or_copy().tableAlias.assign(tableAlias);
        break;
    default:
        WINQRemedialError();
    }
    return *this;
}

TableOrSubquery &TableOrSubquery::indexedBy(const std::string &indexName)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    WINQRemedialAssert(lang.type == Lang::TableOrSubquery::Type::Table);
    Lang::TableOrSubqueryTable &langTable = lang.tableOrSubqueryTable.get_or_copy();
    langTable.indexSwitcher = Lang::TableOrSubqueryTable::IndexSwitch::Indexed;
    langTable.indexName.assign(indexName);
    return *this;
}
TableOrSubquery &TableOrSubquery::notIndexed()
{
    Lang::TableOrSubquery &lang = getMutableLang();
    WINQRemedialAssert(lang.type == Lang::TableOrSubquery::Type::Table);
    lang.tableOrSubqueryTable.get_or_copy().indexSwitcher
    = Lang::TableOrSubqueryTable::IndexSwitch::NotIndexed;
    return *this;
}

TableOrSubquery TableOrSubquery::function(const std::string &functionName)
{
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTableFunction> cowLang;
    cowLang.get_or_copy().tableFunctionName.assign(functionName);
    return cowLang;
}

TableOrSubquery
TableOrSubquery::function(const std::string &functionName, const Expression &expression)
{
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTableFunction> cowLang;
    Lang::TableOrSubqueryTableFunction &lang = cowLang.get_or_copy();
    lang.tableFunctionName.assign(functionName);
    lang.exprs.append(expression.getCOWLang());
    return cowLang;
}

TableOrSubquery TableOrSubquery::function(const std::string &functionName,
                                          const std::list<Expression> &expressions)
{
    Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTableFunction> cowLang;
    Lang::TableOrSubqueryTableFunction &lang = cowLang.get_or_copy();
    lang.tableFunctionName.assign(functionName);
    for (const Expression &expression : expressions) {
        lang.exprs.append(expression.getCOWLang());
    }
    return cowLang;
}

TableOrSubquery::TableOrSubquery(const Lang::CopyOnWriteLazyLang<Lang::TableOrSubqueryTableFunction> &tableOrSubqueryTableFunction)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    lang.type = Lang::TableOrSubquery::Type::TableFunction;
    lang.tableOrSubqueryTableFunction.assign(tableOrSubqueryTableFunction);
}

void TableOrSubquery::setTableName(const std::string &tableName)
{
    Lang::TableOrSubquery &lang = getMutableLang();
    lang.type = Lang::TableOrSubquery::Type::Table;
    Lang::TableOrSubqueryTable &langTable = lang.tableOrSubqueryTable.get_or_copy();
    langTable.tableName.assign(tableName);
}

} // namespace WCDB
