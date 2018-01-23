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

#include <WCDB/column_result.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/order.hpp>
#include <WCDB/statement_select.hpp>
#include <WCDB/subquery.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

StatementSelect &StatementSelect::select(const ColumnResult &columnResult,
                                         bool distinct)
{
    m_description.append("SELECT ");
    if (distinct) {
        m_description.append("DISTINCT ");
    }
    m_description.append(columnResult.getDescription());
    return *this;
}

StatementSelect &
StatementSelect::select(const std::list<const ColumnResult> &columnResultList,
                        bool distinct)
{
    m_description.append("SELECT ");
    if (distinct) {
        m_description.append("DISTINCT ");
    }
    m_description.append(stringByJoiningList(columnResultList));
    return *this;
}

StatementSelect &
StatementSelect::from(const std::list<const Subquery> &subqueryList)
{
    m_description.append(" FROM " + stringByJoiningList(subqueryList));
    return *this;
}

StatementSelect &StatementSelect::from(const Subquery &subquery)
{
    m_description.append(" FROM " + subquery.getDescription());
    return *this;
}

StatementSelect &StatementSelect::where(const Expression &condition)
{
    if (!condition.isEmpty()) {
        m_description.append(" WHERE " + condition.getDescription());
    }
    return *this;
}

StatementSelect &
StatementSelect::orderBy(const std::list<const Order> &orderList)
{
    if (!orderList.empty()) {
        m_description.append(" ORDER BY " + stringByJoiningList(orderList));
    }
    return *this;
}

StatementSelect &StatementSelect::orderBy(const Order &order)
{
    m_description.append(" ORDER BY " + order.getDescription());
    return *this;
}

StatementSelect &StatementSelect::limit(const Expression &from,
                                        const Expression &to)
{
    if (!from.isEmpty()) {
        m_description.append(" LIMIT " + from.getDescription());
        if (!to.isEmpty()) {
            m_description.append(", " + to.getDescription());
        }
    }
    return *this;
}

StatementSelect &StatementSelect::limit(const Expression &limit)
{
    if (!limit.isEmpty()) {
        m_description.append(" LIMIT " + limit.getDescription());
    }
    return *this;
}

StatementSelect &StatementSelect::offset(const Expression &offset)
{
    if (!offset.isEmpty()) {
        m_description.append(" OFFSET " + offset.getDescription());
    }
    return *this;
}

StatementSelect &
StatementSelect::groupBy(const std::list<const Expression> &groupList)
{
    if (!groupList.empty()) {
        m_description.append(" GROUP BY " + stringByJoiningList(groupList));
    }
    return *this;
}

StatementSelect &StatementSelect::groupBy(const Expression &group)
{
    m_description.append(" GROUP BY " + group.getDescription());
    return *this;
}

StatementSelect &StatementSelect::having(const Expression &having)
{
    if (!having.isEmpty()) {
        m_description.append(" HAVING " + having.getDescription());
    }
    return *this;
}

Statement::Type StatementSelect::getStatementType() const
{
    return Statement::Type::Select;
}

} //namespace WCDB
