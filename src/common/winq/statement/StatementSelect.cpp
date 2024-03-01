//
// Created by sanhuazhang on 2019/05/02
//

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

#include "WINQ.h"

namespace WCDB {

StatementSelect::StatementSelect() = default;

StatementSelect::~StatementSelect() = default;

StatementSelect& StatementSelect::with(const CommonTableExpressions& commonTableExpressions)
{
    syntax().commonTableExpressions = commonTableExpressions;
    return *this;
}

StatementSelect& StatementSelect::recursive()
{
    syntax().recursive = true;
    return *this;
}

StatementSelect& StatementSelect::select(const ResultColumns& resultColumns)
{
    Syntax::SelectCore& core = currentCore();
    core.switcher = Syntax::SelectCore::Switch::Select;
    core.resultColumns = resultColumns;
    return *this;
}

StatementSelect& StatementSelect::distinct()
{
    currentCore().distinct = true;
    return *this;
}

StatementSelect& StatementSelect::from(const TablesOrSubqueries& tablesOrSubqueries)
{
    Syntax::SelectCore& core = currentCore();
    core.tableOrSubqueries = tablesOrSubqueries;
    return *this;
}

StatementSelect& StatementSelect::from(const Join& join)
{
    Syntax::SelectCore& core = currentCore();
    core.tableOrSubqueries.clear();
    core.joinClause = join;
    return *this;
}

StatementSelect& StatementSelect::where(const Expression& condition)
{
    Syntax::SelectCore& core = currentCore();
    core.condition = condition;
    return *this;
}

StatementSelect& StatementSelect::groups(const Expressions& groups)
{
    currentCore().groups = groups;
    return *this;
}

StatementSelect& StatementSelect::group(const Expression& group)
{
    currentCore().groups.push_back(group);
    return *this;
}

StatementSelect& StatementSelect::having(const Expression& having)
{
    Syntax::SelectCore& core = currentCore();
    core.having = having;
    return *this;
}

StatementSelect& StatementSelect::window(const UnsafeStringView& window)
{
    currentCore().windows.push_back(StringView(window));
    return *this;
}

StatementSelect& StatementSelect::as(const WindowDef& windowDef)
{
    currentCore().windowDefs.push_back(windowDef);
    return *this;
}

TableOrSubquery StatementSelect::as(const UnsafeStringView& alias)
{
    return TableOrSubquery(*this).as(alias);
}

StatementSelect& StatementSelect::value(const Expression& expression)
{
    Syntax::SelectCore& core = currentCore();
    core.switcher = Syntax::SelectCore::Switch::Values;
    if (core.valuesList.empty()) {
        core.valuesList.push_back({});
    }
    core.valuesList.begin()->push_back(expression);
    return *this;
}

StatementSelect& StatementSelect::values(const Expressions& expressions)
{
    Syntax::SelectCore& core = currentCore();
    core.switcher = Syntax::SelectCore::Switch::Values;
    core.valuesList.push_back(expressions);
    return *this;
}

StatementSelect& StatementSelect::union_()
{
    syntax().compoundOperators.push_back(Syntax::CompoundOperator::Union);
    syntax().cores.push_back({});
    return *this;
}

StatementSelect& StatementSelect::unionAll()
{
    syntax().compoundOperators.push_back(Syntax::CompoundOperator::UnionAll);
    syntax().cores.push_back({});
    return *this;
}

StatementSelect& StatementSelect::intersect()
{
    syntax().compoundOperators.push_back(Syntax::CompoundOperator::Intersect);
    syntax().cores.push_back({});
    return *this;
}

StatementSelect& StatementSelect::except()
{
    syntax().compoundOperators.push_back(Syntax::CompoundOperator::Except);
    syntax().cores.push_back({});
    return *this;
}

StatementSelect& StatementSelect::order(const OrderingTerm& order)
{
    syntax().orderingTerms.push_back(order);
    return *this;
}

StatementSelect& StatementSelect::orders(const OrderingTerms& orders)
{
    syntax().orderingTerms = orders;
    return *this;
}

StatementSelect& StatementSelect::limit(const Expression& from, const Expression& to)
{
    syntax().limitParameterType = Syntax::LimitParameterType::End;
    syntax().limit = from;
    syntax().limitParameter = to;
    return *this;
}

StatementSelect& StatementSelect::limit(const Expression& limit)
{
    syntax().limitParameterType = Syntax::LimitParameterType::NotSet;
    syntax().limit = limit;
    return *this;
}

StatementSelect& StatementSelect::offset(const Expression& offset)
{
    syntax().limitParameterType = Syntax::LimitParameterType::Offset;
    syntax().limitParameter = offset;
    return *this;
}

Syntax::SelectCore& StatementSelect::currentCore()
{
    if (syntax().cores.empty()) {
        return syntax().select.getOrCreate();
    } else {
        return syntax().cores.back();
    }
}

} // namespace WCDB
