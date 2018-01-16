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

#include <WCDB/expression.hpp>
#include <WCDB/statement_delete.hpp>
#include <WCDB/order.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

StatementDelete &StatementDelete::deleteFrom(const std::string &table)
{
    m_description.append("DELETE FROM " + table);
    return *this;
}

StatementDelete &StatementDelete::where(const Expression &condition)
{
    if (!condition.isEmpty()) {
        m_description.append(" WHERE " + condition.getDescription());
    }
    return *this;
}
    
StatementDelete &StatementDelete::orderBy(const std::list<const Order> &orderList)
{
    if (!orderList.empty()) {
        m_description.append(" ORDER BY " + stringByJoiningList(orderList));
    }
    return *this;
}
    
StatementDelete &StatementDelete::orderBy(const Order &order)
{
    m_description.append(" ORDER BY " + order.getDescription());
    return *this;
}
    
StatementDelete &StatementDelete::limit(const Expression &from,
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

StatementDelete &StatementDelete::limit(const Expression &limit)
{
    if (!limit.isEmpty()) {
        m_description.append(" LIMIT " + limit.getDescription());
    }
    return *this;
}

StatementDelete &StatementDelete::offset(const Expression &offset)
{
    if (!offset.isEmpty()) {
        m_description.append(" OFFSET " + offset.getDescription());
    }
    return *this;
}

Statement::Type StatementDelete::getStatementType() const
{
    return Statement::Type::Delete;
}

} //namespace WCDB
