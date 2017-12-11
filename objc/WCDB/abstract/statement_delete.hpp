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

#ifndef statement_delete_hpp
#define statement_delete_hpp

#include <WCDB/statement.hpp>

namespace WCDB {

class StatementDelete : public Statement {
public:
    StatementDelete &deleteFrom(const std::string &table);
    StatementDelete &where(const Expr &where);
    //StatementDeleteLimited
    template <typename T = Order>
    typename std::enable_if<std::is_base_of<Order, T>::value,
                            StatementDelete &>::type
    orderBy(const std::list<const T> &orderList)
    {
        if (!orderList.empty()) {
            m_description.append(" ORDER BY ");
            joinDescribableList(orderList);
        }
        return *this;
    }
    StatementDelete &limit(const Expr &from, const Expr &to);
    StatementDelete &limit(const Expr &expr);
    StatementDelete &offset(const Expr &expr);

    virtual Statement::Type getStatementType() const override;
};

} //namespace WCDB

#endif /* statement_delete_hpp */
