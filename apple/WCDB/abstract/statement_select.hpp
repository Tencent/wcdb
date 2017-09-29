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

#ifndef statement_select_hpp
#define statement_select_hpp

#include <WCDB/statement.hpp>

namespace WCDB {

class StatementSelect : public Statement {
public:
    template <typename T = ColumnResult>
    typename std::enable_if<std::is_base_of<ColumnResult, T>::value,
                            StatementSelect &>::type
    select(const std::list<const T> &columnResultList, bool distinct = false)
    {
        m_description.append("SELECT ");
        if (distinct) {
            m_description.append("DISTINCT ");
        }
        joinDescribableList(columnResultList);
        return *this;
    }

    template <typename T = Subquery>
    typename std::enable_if<std::is_base_of<Subquery, T>::value,
                            StatementSelect &>::type
    from(const std::list<const T> &subqueryList)
    {
        m_description.append(" FROM ");
        joinDescribableList(subqueryList);
        return *this;
    }

    StatementSelect &from(const JoinClause &joinClause);
    StatementSelect &from(const std::string &tableName);
    StatementSelect &where(const Expr &where);

    template <typename T = Order>
    typename std::enable_if<std::is_base_of<Order, T>::value,
                            StatementSelect &>::type
    orderBy(const std::list<const T> &orderList)
    {
        if (!orderList.empty()) {
            m_description.append(" ORDER BY ");
            joinDescribableList(orderList);
        }
        return *this;
    }

    StatementSelect &limit(const Expr &from, const Expr &to);
    StatementSelect &limit(const Expr &limit);
    StatementSelect &
    offset(const Expr &
               offset); //limit(from, to) is same as limit(to-from).offset(from)

    template <typename T = Expr>
    typename std::enable_if<std::is_base_of<Expr, T>::value,
                            StatementSelect &>::type
    groupBy(const std::list<const T> &groupList)
    {
        if (!groupList.empty()) {
            m_description.append(" GROUP BY ");
            joinDescribableList(groupList);
        }
        return *this;
    }

    StatementSelect &having(const Expr &having);

    virtual Statement::Type getStatementType() const override;

    static StatementSelect Fts3Tokenizer;
};

} //namespace WCDB

#endif /* statement_select_hpp */
