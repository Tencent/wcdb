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

SelectCore& SelectCore::distinct()
{
    LangSelectCore& lang = getMutableLang();
    lang.distinct = true;
    return *this;
}

SelectCore& SelectCore::select(const ResultColumn& resultColumn)
{
    LangSelectCore& lang = getMutableLang();
    lang.resultColumns.append(resultColumn.getLang());
    return *this;
}
SelectCore& SelectCore::select(const std::list<ResultColumn>& resultColumns)
{
    LangSelectCore& lang = getMutableLang();
    for (const ResultColumn& resultColumn : resultColumns) {
        lang.resultColumns.append(resultColumn.getLang());
    }
    return *this;
}
SelectCore& SelectCore::from(const JoinClause& joinClause)
{
    LangSelectCore& lang = getMutableLang();
    lang.fromSwitcher = LangSelectCore::FromSwitch::JoinClause; 
    lang.joinClause.assign(joinClause.getLang());
    return *this;
}
SelectCore& SelectCore::from(const TableOrSubquery& tableOrSubquery)
{
    LangSelectCore& lang = getMutableLang();
    lang.fromSwitcher = LangSelectCore::FromSwitch::TableOrSubquery; 
    lang.tableOrSubquerys.append(tableOrSubquery.getLang());
    return *this;
}
SelectCore& SelectCore::from(const std::list<TableOrSubquery>& tableOrSubquerys)
{
    LangSelectCore& lang = getMutableLang();
    lang.fromSwitcher = LangSelectCore::FromSwitch::TableOrSubquery;
    for (const TableOrSubquery& tableOrSubquery : tableOrSubquerys) {
        lang.tableOrSubquerys.append(tableOrSubquery.getLang());
    }
    return *this;
}
SelectCore& SelectCore::where(const Expression& condition)
{
    LangSelectCore& lang = getMutableLang();
    lang.condition.assign(condition.getLang());
    return *this;
}
SelectCore& SelectCore::groupBy(const Expression& group)
{
    LangSelectCore& lang = getMutableLang();
    lang.groups.append(group.getLang());
    return *this;
}
SelectCore& SelectCore::groupBy(const std::list<Expression>& groups)
{
    LangSelectCore& lang = getMutableLang();
    for (const Expression& group : groups) {
        lang.groups.append(group.getLang());
    }
    return *this;
}
SelectCore& SelectCore::having(const Expression& having)
{
    LangSelectCore& lang = getMutableLang();
    lang.having.assign(having.getLang());
    return *this;
}
SelectCore& SelectCore::values(const Expression& value)
{
    LangSelectCore& lang = getMutableLang();
    lang.values.append(value.getLang());
    return *this;
}
SelectCore& SelectCore::values(const std::list<Expression>& values)
{
    LangSelectCore& lang = getMutableLang();
    for (const Expression& value : values) {
        lang.values.append(value.getLang());
    }
    return *this;
}
