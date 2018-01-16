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

#include <WCDB/order.hpp>
#include <WCDB/column.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/statement_update.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

StatementUpdate &StatementUpdate::update(const std::string &table,
                                         Conflict conflict)
{
    m_description.append("UPDATE ");
    if (conflict != Conflict::NotSet) {
        m_description.append("OR ");
        m_description.append(ConflictName(conflict));
        m_description.append(" ");
    }
    m_description.append(table);
    return *this;
}
    
StatementUpdate &StatementUpdate::set(const std::list<const std::pair<const Column, const Expression>> &valueList)
{
    m_description.append(" SET ");
    bool flag = false;
    for (const auto &value : valueList) {
        if (flag) {
            m_description.append(", ");
        } else {
            flag = true;
        }
        m_description.append(value.first.getDescription() + "=" + value.second.getDescription());
    }
    return *this;
}
    
StatementUpdate &StatementUpdate::set(const std::pair<const Column, const Expression> &value)
{
    m_description.append(" SET " + value.first.getDescription() + "=" + value.second.getDescription());
    return *this;
}

StatementUpdate &StatementUpdate::where(const Expression &condition)
{
    if (!condition.isEmpty()) {
        m_description.append(" WHERE " + condition.getDescription());
    }
    return *this;
}
    
StatementUpdate &StatementUpdate::orderBy(const std::list<const Order> &orderList)
{
    if (!orderList.empty()) {
        m_description.append(" ORDER BY " + stringByJoiningList(orderList));
    }
    return *this;
}
    
StatementUpdate &StatementUpdate::orderBy(const Order &order)
{
    m_description.append(" ORDER BY " + order.getDescription());
    return *this;
}

StatementUpdate &StatementUpdate::limit(const Expression &from,
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

StatementUpdate &StatementUpdate::limit(const Expression &limit)
{
    if (!limit.isEmpty()) {
        m_description.append(" LIMIT " + limit.getDescription());
    }
    return *this;
}

StatementUpdate &StatementUpdate::offset(const Expression &offset)
{
    if (!offset.isEmpty()) {
        m_description.append(" OFFSET " + offset.getDescription());
    }
    return *this;
}

Statement::Type StatementUpdate::getStatementType() const
{
    return Statement::Type::Update;
}

} //namespace WCDB
