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

#include <WCDB/column.hpp>
#include <WCDB/expression.hpp>
#include <WCDB/literal_value.hpp>
#include <WCDB/statement_insert.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

StatementInsert &StatementInsert::insert(const std::string &table,
                                         Conflict conflict)
{
    m_description.append("INSERT");
    if (conflict != Conflict::NotSet) {
        m_description.append(" OR ");
        m_description.append(ConflictName(conflict));
    }
    m_description.append(" INTO " + table);
    return *this;
}

StatementInsert &
StatementInsert::insert(const std::string &table,
                        const std::list<const Column> &columnList,
                        Conflict conflict)
{
    m_description.append("INSERT");
    if (conflict != Conflict::NotSet) {
        m_description.append(" OR ");
        m_description.append(ConflictName(conflict));
    }
    m_description.append(" INTO " + table);
    if (!columnList.empty()) {
        m_description.append("(" + stringByJoiningList(columnList) + ")");
    }
    return *this;
}

StatementInsert &StatementInsert::insert(const std::string &table,
                                         const Column &column,
                                         Conflict conflict)
{
    m_description.append("INSERT");
    if (conflict != Conflict::NotSet) {
        m_description.append(" OR ");
        m_description.append(ConflictName(conflict));
    }
    m_description.append(" INTO " + table + "(" + column.getDescription() +
                         ")");
    return *this;
}

StatementInsert &
StatementInsert::values(const std::list<const Expression> &expressionList)
{
    if (!expressionList.empty()) {
        m_description.append(" VALUES(" + stringByJoiningList(expressionList) +
                             ")");
    }
    return *this;
}

StatementInsert &StatementInsert::values(const Expression &expression)
{
    m_description.append(" VALUES(" + expression.getDescription() + ")");
    return *this;
}

Statement::Type StatementInsert::getStatementType() const
{
    return Statement::Type::Insert;
}

} //namespace WCDB
