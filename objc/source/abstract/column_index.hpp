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

#ifndef column_index_hpp
#define column_index_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/declare.hpp>

namespace WCDB {

class ColumnIndex : public Describable {
public:
    template <typename T>
    ColumnIndex(const T &t,
                OrderTerm term = OrderTerm::NotSet,
                typename std::enable_if<ColumnConvertible<T>::value, void>::type
                    * = nullptr)
        : Describable(ColumnConvertible<T>::asColumn(t).getDescription())
    {
        if (term != OrderTerm::NotSet) {
            m_description.append(" ");
            m_description.append(OrderTermName(term));
        }
    }

    template <typename T>
    ColumnIndex(const T &t,
                OrderTerm term = OrderTerm::NotSet,
                typename std::enable_if<ExpressionConvertible<T>::value,
                                        void>::type * = nullptr)
        : Describable(
              ExpressionConvertible<T>::asExpression(t).getDescription())
    {
        if (term != OrderTerm::NotSet) {
            m_description.append(" ");
            m_description.append(OrderTermName(term));
        }
    }

    ColumnIndex(const Column &column, OrderTerm term = OrderTerm::NotSet);

    operator std::list<const ColumnIndex>() const;
};

} //namespace WCDB

#endif /* column_index_hpp */
