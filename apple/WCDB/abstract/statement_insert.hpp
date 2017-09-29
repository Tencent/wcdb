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

#ifndef statement_insert_hpp
#define statement_insert_hpp

#include <WCDB/conflict.hpp>
#include <WCDB/statement.hpp>

namespace WCDB {

class StatementInsert : public Statement {
public:
    StatementInsert &insert(const std::string &table,
                            Conflict conflict = Conflict::NotSet);

    template <typename T = Column>
    typename std::enable_if<std::is_base_of<Column, T>::value,
                            StatementInsert &>::type
    insert(const std::string &table,
           const std::list<const T> &columnList,
           Conflict conflict = Conflict::Replace)
    {
        m_description.append("INSERT");
        if (conflict != Conflict::NotSet) {
            m_description.append(" OR ");
            m_description.append(ConflictName(conflict));
        }
        m_description.append(" INTO " + table);
        if (!columnList.empty()) {
            m_description.append("(");
            joinDescribableList(columnList);
            m_description.append(")");
        }
        return *this;
    }

    template <typename T = Expr>
    typename std::enable_if<std::is_base_of<Expr, T>::value,
                            StatementInsert &>::type
    values(const std::list<const T> &exprList)
    {
        if (!exprList.empty()) {
            m_description.append(" VALUES(");
            joinDescribableList(exprList);
            m_description.append(")");
        }
        return *this;
    }

    virtual Statement::Type getStatementType() const override;
};

} //namespace WCDB

#endif /* statement_insert_hpp */
