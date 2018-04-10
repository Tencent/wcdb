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

SelectCore &SelectCore::distinct()
{
    Lang::SelectCore &lang = getMutableLang();
    lang.distinct = true;
    return *this;
}

SelectCore &SelectCore::select(const ResultColumn &resultColumn)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.switcher = Lang::SelectCore::Switch::Select;
    lang.resultColumns.append(resultColumn.getCOWLang());
    return *this;
}
SelectCore &SelectCore::select(const std::list<ResultColumn> &resultColumns)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.switcher = Lang::SelectCore::Switch::Select;
    for (const ResultColumn &resultColumn : resultColumns) {
        lang.resultColumns.append(resultColumn.getCOWLang());
    }
    return *this;
}
SelectCore &SelectCore::from(const JoinClause &joinClause)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.fromSwitcher = Lang::SelectCore::FromSwitch::JoinClause;
    lang.joinClause.assign(joinClause.getCOWLang());
    return *this;
}
SelectCore &SelectCore::from(const TableOrSubquery &tableOrSubquery)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.fromSwitcher = Lang::SelectCore::FromSwitch::TableOrSubquery;
    lang.tableOrSubquerys.append(tableOrSubquery.getCOWLang());
    return *this;
}
SelectCore &SelectCore::from(const std::list<TableOrSubquery> &tableOrSubquerys)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.fromSwitcher = Lang::SelectCore::FromSwitch::TableOrSubquery;
    for (const TableOrSubquery &tableOrSubquery : tableOrSubquerys) {
        lang.tableOrSubquerys.append(tableOrSubquery.getCOWLang());
    }
    return *this;
}
SelectCore &SelectCore::where(const Expression &condition)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.condition.assign(condition.getCOWLang());
    return *this;
}
SelectCore &SelectCore::groupBy(const Expression &group)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.groups.append(group.getCOWLang());
    return *this;
}
SelectCore &SelectCore::groupBy(const std::list<Expression> &groups)
{
    Lang::SelectCore &lang = getMutableLang();
    for (const Expression &group : groups) {
        lang.groups.append(group.getCOWLang());
    }
    return *this;
}
SelectCore &SelectCore::having(const Expression &having)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.having.assign(having.getCOWLang());
    return *this;
}
SelectCore &SelectCore::values(const Expression &value)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.switcher = Lang::SelectCore::Switch::Values;
    lang.values.append(value.getCOWLang());
    return *this;
}
SelectCore &SelectCore::values(const std::list<Expression> &values)
{
    Lang::SelectCore &lang = getMutableLang();
    lang.switcher = Lang::SelectCore::Switch::Values;
    for (const Expression &value : values) {
        lang.values.append(value.getCOWLang());
    }
    return *this;
}

} // namespace WCDB
