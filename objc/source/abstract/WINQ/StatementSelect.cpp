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

StatementSelect &
StatementSelect::with(const CommonTableExpression &commonTableExpression)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.recursive = false;
    lang.commonTableExpressions.append(commonTableExpression.getLang());
    return *this;
}

StatementSelect &StatementSelect::with(
    const std::list<CommonTableExpression> &commonTableExpressions)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.recursive = false;
    for (const CommonTableExpression &commonTableExpression :
         commonTableExpressions) {
        lang.commonTableExpressions.append(commonTableExpression.getLang());
    }
    return *this;
}

StatementSelect &StatementSelect::withRecursive(
    const CommonTableExpression &commonTableExpression)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.recursive = true;
    lang.commonTableExpressions.append(commonTableExpression.getLang());
    return *this;
}

StatementSelect &StatementSelect::withRecursive(
    const std::list<CommonTableExpression> &commonTableExpressions)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.recursive = true;
    for (const CommonTableExpression &commonTableExpression :
         commonTableExpressions) {
        lang.commonTableExpressions.append(commonTableExpression.getLang());
    }
    return *this;
}

StatementSelect &StatementSelect::distinct()
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.selectCore.get_or_copy().distinct = true;
    return *this;
}

StatementSelect &StatementSelect::select(const ResultColumn &resultColumn)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    selectCore.switcher = Lang::SelectCore::Switch::Select;
    selectCore.resultColumns.append(resultColumn.getLang());
    return *this;
}

StatementSelect &
StatementSelect::select(const std::list<ResultColumn> &resultColumns)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    selectCore.switcher = Lang::SelectCore::Switch::Select;
    for (const ResultColumn &resultColumn : resultColumns) {
        selectCore.resultColumns.append(resultColumn.getLang());
    }
    return *this;
}

StatementSelect &StatementSelect::from(const TableOrSubquery &tableOrSubquery)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    selectCore.fromSwitcher = Lang::SelectCore::FromSwitch::TableOrSubquery;
    selectCore.tableOrSubquerys.append(tableOrSubquery.getLang());
    return *this;
}

StatementSelect &StatementSelect::from(const JoinClause &joinClause)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    selectCore.fromSwitcher = Lang::SelectCore::FromSwitch::JoinClause;
    selectCore.joinClause.assign(joinClause.getLang());
    return *this;
}

StatementSelect &
StatementSelect::from(const std::list<TableOrSubquery> &tableOrSubquerys)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    selectCore.fromSwitcher = Lang::SelectCore::FromSwitch::TableOrSubquery;
    for (const TableOrSubquery &tableOrSubquery : tableOrSubquerys) {
        selectCore.tableOrSubquerys.append(tableOrSubquery.getLang());
    }
    return *this;
}

StatementSelect &StatementSelect::where(const Expression &condition)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.selectCore.get_or_copy().condition.assign(condition.getLang());
    return *this;
}

StatementSelect &StatementSelect::groupBy(const Expression &group)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.selectCore.get_or_copy().groups.append(group.getLang());
    return *this;
}

StatementSelect &StatementSelect::groupBy(const std::list<Expression> &groups)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    for (const Expression &group : groups) {
        selectCore.groups.append(group.getLang());
    }
    return *this;
}

StatementSelect &StatementSelect::having(const Expression &having)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.selectCore.get_or_copy().having.assign(having.getLang());
    return *this;
}

StatementSelect &StatementSelect::values(const Expression &value)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    selectCore.switcher = Lang::SelectCore::Switch::Values;
    selectCore.values.append(value.getLang());
    return *this;
}

StatementSelect &StatementSelect::values(const std::list<Expression> &values)
{
    Lang::SelectSTMT &lang = getMutableLang();
    Lang::SelectCore &selectCore = lang.selectCore.get_or_copy();
    selectCore.switcher = Lang::SelectCore::Switch::Values;
    for (const Expression &value : values) {
        selectCore.values.append(value.getLang());
    }
    return *this;
}

StatementSelect &StatementSelect::union_(const SelectCore &selectCore)
{
    compound(Lang::SelectSTMT::Compound::Operator::Union, selectCore);
    return *this;
}

StatementSelect &StatementSelect::unionAll(const SelectCore &selectCore)
{
    compound(Lang::SelectSTMT::Compound::Operator::UnionAll, selectCore);
    return *this;
}

StatementSelect &StatementSelect::intersect(const SelectCore &selectCore)
{
    compound(Lang::SelectSTMT::Compound::Operator::Intersect, selectCore);
    return *this;
}

StatementSelect &StatementSelect::except(const SelectCore &selectCore)
{
    compound(Lang::SelectSTMT::Compound::Operator::Except, selectCore);
    return *this;
}

StatementSelect &StatementSelect::orderBy(const OrderingTerm &orderingTerm)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.orderingTerms.append(orderingTerm.getLang());
    return *this;
}

StatementSelect &
StatementSelect::orderBy(const std::list<OrderingTerm> &orderingTerms)
{
    Lang::SelectSTMT &lang = getMutableLang();
    for (const OrderingTerm &orderingTerm : orderingTerms) {
        lang.orderingTerms.append(orderingTerm.getLang());
    }
    return *this;
}

StatementSelect &StatementSelect::limit(const Expression &from,
                                        const Expression &to)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.offset = false;
    lang.limit.assign(from.getLang());
    lang.limitParameter.assign(to.getLang());
    return *this;
}

StatementSelect &StatementSelect::limit(const Expression &limit)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.offset = true;
    lang.limit.assign(limit.getLang());
    return *this;
}

StatementSelect &StatementSelect::offset(const Expression &offset)
{
    Lang::SelectSTMT &lang = getMutableLang();
    lang.limitParameter.assign(offset.getLang());
    return *this;
}

void StatementSelect::compound(
    const Lang::SelectSTMT::Compound::Operator &compoundOperator,
    const SelectCore &selectCore)
{
    Lang::CopyOnWriteLazyLang<Lang::SelectSTMT::Compound> compoundCore;
    Lang::SelectSTMT::Compound &compound = compoundCore.get_or_copy();
    compound.compoundOperator = compoundOperator;
    compound.selectCore.assign(selectCore.getLang());

    Lang::SelectSTMT &lang = getMutableLang();
    lang.compoundCores.append(compoundCore);
}

Statement::Type StatementSelect::getType() const
{
    return Statement::Type::Select;
}

Expression StatementSelect::getRedirectSource() const
{
    return *this;
}

} // namespace WCDB
