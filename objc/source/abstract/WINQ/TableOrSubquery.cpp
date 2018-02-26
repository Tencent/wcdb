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

#include <WCDB/WINQ.h>

namespace WCDB {

TableOrSubquery::TableOrSubquery(const StatementSelect &selectSTMT)
{
    lang::TableOrSubquery &lang = getMutableLang();
    lang.switcher = lang::TableOrSubquery::Switch::Select;
    lang::TableOrSubquerySelect &langSelect =
        lang.tableOrSubquerySelect.get_or_copy();
    langSelect.selectSTMT.assign(selectSTMT.getLang());
}

TableOrSubquery::TableOrSubquery(const JoinClause &joinClause)
{
    lang::TableOrSubquery &lang = getMutableLang();
    lang.switcher = lang::TableOrSubquery::Switch::JoinClause;
    lang::TableOrSubqueryJoinClause &langJoinClause =
        lang.tableOrSubqueryJoinClause.get_or_copy();
    langJoinClause.joinClause.assign(joinClause.getLang());
}

TableOrSubquery::TableOrSubquery(
    const std::list<TableOrSubquery> &tableOrSubquerys)
{
    lang::TableOrSubquery &lang = getMutableLang();
    lang.switcher = lang::TableOrSubquery::Switch::List;
    lang::TableOrSubqueryList &langList =
        lang.tableOrSubqueryList.get_or_copy();
    for (const TableOrSubquery &tableOrSubquery : tableOrSubquerys) {
        langList.tableOrSubquerys.append(tableOrSubquery.getLang());
    }
}

TableOrSubquery &TableOrSubquery::withSchema(const std::string &schemaName)
{
    lang::TableOrSubquery &lang = getMutableLang();
    switch (lang.switcher) {
        case lang::TableOrSubquery::Switch::Table:
            lang.tableOrSubqueryTable.get_or_copy().schemaName.assign(
                schemaName);
            break;
        case lang::TableOrSubquery::Switch::TableFunction:
            lang.tableOrSubqueryTableFunction.get_or_copy().schemaName.assign(
                schemaName);
            break;
        default:
            assert(false);
            ;
            break;
    }
    return *this;
}

TableOrSubquery &TableOrSubquery::as(const std::string &tableAlias)
{
    lang::TableOrSubquery &lang = getMutableLang();
    switch (lang.switcher) {
        case lang::TableOrSubquery::Switch::Table:
            lang.tableOrSubqueryTable.get_or_copy().tableAlias.assign(
                tableAlias);
            break;
        case lang::TableOrSubquery::Switch::TableFunction:
            lang.tableOrSubqueryTableFunction.get_or_copy().tableAlias.assign(
                tableAlias);
            break;
        case lang::TableOrSubquery::Switch::Select:
            lang.tableOrSubquerySelect.get_or_copy().tableAlias.assign(
                tableAlias);
            break;
        default:
            assert(false);
            break;
    }
    return *this;
}

TableOrSubquery &TableOrSubquery::indexedBy(const std::string &indexName)
{
    lang::TableOrSubquery &lang = getMutableLang();
    assert(lang.switcher == lang::TableOrSubquery::Switch::Table);
    lang::TableOrSubqueryTable &langTable =
        lang.tableOrSubqueryTable.get_or_copy();
    langTable.indexSwitcher = lang::TableOrSubqueryTable::IndexSwitch::Indexed;
    langTable.indexName.assign(indexName);
    return *this;
}
TableOrSubquery &TableOrSubquery::notIndexed()
{
    lang::TableOrSubquery &lang = getMutableLang();
    assert(lang.switcher == lang::TableOrSubquery::Switch::Table);
    lang.tableOrSubqueryTable.get_or_copy().indexSwitcher =
        lang::TableOrSubqueryTable::IndexSwitch::NotIndexed;
    return *this;
}

TableOrSubquery TableOrSubquery::Function(const std::string &functionName)
{
    lang::copy_on_write_lazy_lang<lang::TableOrSubqueryTableFunction> cowLang;
    cowLang.get_or_copy().tableFunctionName.assign(functionName);
    return cowLang;
}

TableOrSubquery TableOrSubquery::Function(const std::string &functionName,
                                          const Expression &expression)
{
    lang::copy_on_write_lazy_lang<lang::TableOrSubqueryTableFunction> cowLang;
    lang::TableOrSubqueryTableFunction &lang = cowLang.get_or_copy();
    lang.tableFunctionName.assign(functionName);
    lang.exprs.append(expression.getLang());
    return cowLang;
}

TableOrSubquery
TableOrSubquery::Function(const std::string &functionName,
                          const std::list<Expression> &expressions)
{
    lang::copy_on_write_lazy_lang<lang::TableOrSubqueryTableFunction> cowLang;
    lang::TableOrSubqueryTableFunction &lang = cowLang.get_or_copy();
    lang.tableFunctionName.assign(functionName);
    for (const Expression &expression : expressions) {
        lang.exprs.append(expression.getLang());
    }
    return cowLang;
}

TableOrSubquery::TableOrSubquery(
    const lang::copy_on_write_lazy_lang<lang::TableOrSubqueryTableFunction>
        &tableOrSubqueryTableFunction)
{
    lang::TableOrSubquery &lang = getMutableLang();
    lang.switcher = lang::TableOrSubquery::Switch::TableFunction;
    lang.tableOrSubqueryTableFunction.assign(tableOrSubqueryTableFunction);
}

void TableOrSubquery::setTableName(const std::string &tableName)
{
    lang::TableOrSubquery &lang = getMutableLang();
    lang.switcher = lang::TableOrSubquery::Switch::Table;
    lang::TableOrSubqueryTable &langTable =
        lang.tableOrSubqueryTable.get_or_copy();
    langTable.tableName.assign(tableName);
}

} // namespace WCDB
