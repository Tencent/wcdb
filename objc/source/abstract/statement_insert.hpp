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

#include <WCDB/convertible.hpp>
#include <WCDB/statement.hpp>

namespace WCDB {

class StatementInsert : public Statement {
public:
    StatementInsert &insert(const std::string &table,
                            Conflict conflict = Conflict::NotSet);

    template <typename T>
    typename std::enable_if<ColumnConvertible<T>::value,
                            StatementInsert &>::type
    insert(const std::string &table,
           const std::list<const T> &columnList,
           Conflict conflict = Conflict::NotSet)
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

    StatementInsert &insert(const std::string &table,
                            const std::list<const Column> &columnList,
                            Conflict conflict = Conflict::NotSet);

    StatementInsert &insert(const std::string &table,
                            const Column &column,
                            Conflict conflict = Conflict::NotSet);

    StatementInsert &values(const std::list<const Expression> &expressionList);

    StatementInsert &values(const Expression &expression);

    template <typename T>
    typename std::enable_if<ExpressionConvertible<T>::value,
                            StatementInsert &>::type
    values(const std::list<const T> &expressionList)
    {
        if (!expressionList.empty()) {
            m_description.append(" VALUES(" +
                                 stringByJoiningList(expressionList) + ")");
        }
        return *this;
    }

    virtual Statement::Type getStatementType() const override;
};

} //namespace WCDB

#endif /* statement_insert_hpp */
