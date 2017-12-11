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

#ifndef statement_update_hpp
#define statement_update_hpp

#include <WCDB/conflict.hpp>
#include <WCDB/statement.hpp>

namespace WCDB {

class StatementUpdate : public Statement {
public:
    StatementUpdate &update(const std::string &table,
                            Conflict conflict = Conflict::NotSet);
    template <typename T, typename U>
    typename std::enable_if<std::is_base_of<Column, T>::value &&
                                std::is_base_of<Expr, U>::value,
                            StatementUpdate &>::type
    set(const std::list<const std::pair<const T, const U>> &valueList)
    {
        m_description.append(" SET ");
        bool flag = false;
        for (const auto &value : valueList) {
            if (flag) {
                m_description.append(",");
            } else {
                flag = true;
            }
            m_description.append(value.first.getDescription() + "=" +
                                 value.second.getDescription());
        }
        return *this;
    }

    StatementUpdate &where(const Expr &where);
    //StatementUpdateLimited
    template <typename T = Order>
    typename std::enable_if<std::is_base_of<Order, T>::value,
                            StatementUpdate &>::type
    orderBy(const std::list<const T> &orderList)
    {
        if (!orderList.empty()) {
            m_description.append(" ORDER BY ");
            joinDescribableList(orderList);
        }
        return *this;
    }
    StatementUpdate &limit(const Expr &from, const Expr &to);
    StatementUpdate &limit(const Expr &limit);
    StatementUpdate &offset(const Expr &offset);

    virtual Statement::Type getStatementType() const override;

protected:
    bool m_set;
};

} //namespace WCDB

#endif /* statement_update_hpp */
