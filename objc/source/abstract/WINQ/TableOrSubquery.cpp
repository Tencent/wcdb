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

TableOrSubquery::TableOrSubquery(const std::string& tableName)
{
    LangTableOrSubquery& lang = getMutableLang();
    lang.switcher = LangTableOrSubquery::Switch::Table;
    LangTableOrSubqueryTable& langTable = lang.tableOrSubqueryTable.get_or_copy();
    langTable.tableName.assign(tableName);
}

TableOrSubquery::TableOrSubquery(const StatementSelect& selectSTMT)
{
    LangTableOrSubquery& lang = getMutableLang();
    lang.switcher = LangTableOrSubquery::Switch::Select;
    LangTableOrSubquerySelect& langSelect = lang.tableOrSubquerySelect.get_or_copy();
    langSelect.selectSTMT.assign(selectSTMT.getLang());
}

TableOrSubquery::TableOrSubquery(const JoinClause& joinClause)
{
    LangTableOrSubquery& lang = getMutableLang();
    lang.switcher = LangTableOrSubquery::Switch::JoinClause;
    LangTableOrSubqueryJoinClause& langJoinClause = lang.tableOrSubqueryJoinClause.get_or_copy();
    langJoinClause.joinClause.assign(joinClause.getLang());
}

TableOrSubquery::TableOrSubquery(const std::list<TableOrSubquery>& tableOrSubquerys)
{
    LangTableOrSubquery& lang = getMutableLang();
    lang.switcher = LangTableOrSubquery::Switch::List;
    LangTableOrSubqueryList& langList = lang.tableOrSubqueryList.get_or_copy();
    for (const TableOrSubquery& tableOrSubquery : tableOrSubquerys) {
        langList.tableOrSubquerys.append(tableOrSubquery.getLang());
    }
}

TableOrSubquery& TableOrSubquery::withSchema(const std::string& schemaName)
{
    LangTableOrSubquery& lang = getMutableLang();
    switch (lang.switcher) {
        case LangTableOrSubquery::Switch::Table:
            lang.tableOrSubqueryTable.get_or_copy().schemaName.assign(schemaName);
            break;
        case LangTableOrSubquery::Switch::TableFunction:
            lang.tableOrSubqueryTableFunction.get_or_copy().schemaName.assign(schemaName);
            break;            
        default:
            abort();
            break;
    }
    return *this;
}

TableOrSubquery& TableOrSubquery::as(const std::string& tableAlias)
{
    LangTableOrSubquery& lang = getMutableLang();
    switch (lang.switcher) {
        case LangTableOrSubquery::Switch::Table:
            lang.tableOrSubqueryTable.get_or_copy().tableAlias.assign(tableAlias);
            break;            
        case LangTableOrSubquery::Switch::TableFunction:            
            lang.tableOrSubqueryTableFunction.get_or_copy().tableAlias.assign(tableAlias);
            break;
        case LangTableOrSubquery::Switch::Select:            
            lang.tableOrSubquerySelect.get_or_copy().tableAlias.assign(tableAlias);
            break;
        default:
            abort();
            break;
    }
    return *this;
}

TableOrSubquery& TableOrSubquery::indexedBy(const std::string& indexName)
{
    LangTableOrSubquery& lang = getMutableLang();
    assert(lang.switcher == LangTableOrSubquery::Switch::Table);
    LangTableOrSubqueryTable& langTable = lang.tableOrSubqueryTable.get_or_copy();
    langTable.indexSwitcher = LangTableOrSubqueryTable::IndexSwitch::Indexed;
    langTable.indexName.assign(indexName);
    return *this;
}
TableOrSubquery& TableOrSubquery::notIndexed()
{
    LangTableOrSubquery& lang = getMutableLang();
    assert(lang.switcher == LangTableOrSubquery::Switch::Table);
    lang.tableOrSubqueryTable.get_or_copy().indexSwitcher = LangTableOrSubqueryTable::IndexSwitch::NotIndexed;
    return *this;
}

TableOrSubquery TableOrSubquery::Function(const std::string& functionName)
{
    copy_on_write_lazy_lang<LangTableOrSubqueryTableFunction> cowLang;
    cowLang.get_or_copy().tableFunctionName.assign(functionName);
    return cowLang;
}

TableOrSubquery TableOrSubquery::Function(const std::string& functionName, const Expression& expression)
{
    copy_on_write_lazy_lang<LangTableOrSubqueryTableFunction> cowLang;
    LangTableOrSubqueryTableFunction& lang = cowLang.get_or_copy();
    lang.tableFunctionName.assign(functionName);
    lang.exprs.append(expression.getLang());
    return cowLang;
}

TableOrSubquery TableOrSubquery::Function(const std::string& functionName, const std::list<Expression>& expressions)
{
    copy_on_write_lazy_lang<LangTableOrSubqueryTableFunction> cowLang;
    LangTableOrSubqueryTableFunction& lang = cowLang.get_or_copy();
    lang.tableFunctionName.assign(functionName);
    for (const Expression& expression : expressions) {
        lang.exprs.append(expression.getLang());
    }
    return cowLang;
}

TableOrSubquery::TableOrSubquery(const copy_on_write_lazy_lang<LangTableOrSubqueryTableFunction>& tableOrSubqueryTableFunction)
{
    LangTableOrSubquery& lang = getMutableLang();
    lang.switcher = LangTableOrSubquery::Switch::TableFunction;
    lang.tableOrSubqueryTableFunction.assign(tableOrSubqueryTableFunction);
}
