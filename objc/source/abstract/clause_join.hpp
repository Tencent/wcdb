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

#ifndef clause_join_hpp
#define clause_join_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/declare.hpp>

namespace WCDB {

class JoinClause : public Describable {
public:
    enum class Type : int {
        NotSet,
        Left,
        LeftOuter,
        Inner,
        Cross,
    };

    template <typename T>
    JoinClause(const T &t,
               typename std::enable_if<TableOrSubqueryConvertible<T>::value,
                                       void>::type * = nullptr)
        : Describable(TableOrSubqueryConvertible<T>::asTableOrSubquery(t)
                          .getDescription())
    {
    }

    JoinClause &join(const Subquery &subquery,
                     JoinClause::Type type = JoinClause::Type::NotSet);
    JoinClause &naturalJoin(const Subquery &subquery,
                            JoinClause::Type type = JoinClause::Type::NotSet);

    JoinClause &on(const Expression &expression);

    JoinClause &usingColumns(const Column &column);

    JoinClause &usingColumns(const std::list<const Column> &columnList);

    template <typename T>
    typename std::enable_if<ColumnConvertible<T>::value, JoinClause &>::type
    usingColumns(const std::list<const T> &columnList)
    {
        m_description.append(" USING " + stringByJoiningList(columnList));
        return *this;
    }

protected:
    JoinClause &
    join(const Subquery &subquery, JoinClause::Type type, bool isNatural);
};

}; // namespace WCDB

#endif /* clause_join_hpp */
