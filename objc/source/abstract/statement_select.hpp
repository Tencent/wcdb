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
#include <WCDB/convertible.hpp>

namespace WCDB {

class StatementSelect : public Statement {
public:
    template <typename T>
    typename std::enable_if<ColumnResultConvertible<T>::value,
    StatementSelect &>::type
    select(const std::list<const T> &columnResultList, bool distinct = false)
    {
        m_description.append("SELECT ");
        if (distinct) {
            m_description.append("DISTINCT ");
        }
        m_description.append(stringByJoiningList(columnResultList));
        return *this;
    }
    
    StatementSelect &select(const std::list<const ColumnResult> &columnResultList, bool distinct = false);
    
    StatementSelect &select(const ColumnResult &columnResult, bool distinct = false);

    template <typename T>
    typename std::enable_if<TableOrSubqueryConvertible<T>::value,
    StatementSelect &>::type
    from(const std::list<const T> &subqueryList)
    {
        m_description.append(" FROM " + stringByJoiningList(subqueryList));
        return *this;
    }

    StatementSelect &from(const std::list<const Subquery> &subqueryList);
    
    StatementSelect &from(const Subquery &subquery);
    
    StatementSelect &where(const Expression &condition);

    template <typename T>
    typename std::enable_if<OrderConvertible<T>::value,
    StatementSelect &>::type
    orderBy(const std::list<const T> &orderList)
    {
        if (!orderList.empty()) {
            m_description.append(" ORDER BY " + stringByJoiningList(orderList));
        }
        return *this;
    }

    StatementSelect &orderBy(const std::list<const Order> &orderList);
    
    StatementSelect &orderBy(const Order &order);

    StatementSelect &limit(const Expression &from, const Expression &to);
    StatementSelect &limit(const Expression &limit);
    
    //limit(from, to) is same as limit(to-from).offset(from)
    StatementSelect &offset(const Expression &offset);

    template <typename T>
    typename std::enable_if<ExpressionConvertible<T>::value,
    StatementSelect &>::type
    groupBy(const std::list<const T> &groupList)
    {
        if (!groupList.empty()) {
            m_description.append(" GROUP BY " + stringByJoiningList(groupList));
        }
        return *this;
    }

    StatementSelect &groupBy(const std::list<const Expression> &groupList);
    
    StatementSelect &groupBy(const Expression &group);

    StatementSelect &having(const Expression &having);

    virtual Statement::Type getStatementType() const override;
};

} //namespace WCDB

#endif /* statement_select_hpp */
