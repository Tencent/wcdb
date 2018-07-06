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

#ifndef OrderingTerm_hpp
#define OrderingTerm_hpp

#include <WCDB/Describable.hpp>

namespace WCDB {

class OrderingTerm : public DescribableWithLang<Lang::OrderingTerm> {
public:
    OrderingTerm();

    template<typename T, typename Enable = void>
    OrderingTerm(const T &t,
                 typename std::enable_if<ExpressionConvertible<T>::value>::type * = nullptr)
    {
        setupWithExpression(ExpressionConvertible<T>::as(t));
    }

    OrderingTerm(const Expression &expression);
    OrderingTerm &collate(const std::string &collateName);
    OrderingTerm &withOrder(const Order &order);

    operator std::list<OrderingTerm>() const;

protected:
    void setupWithExpression(const Expression &expression);
};

} // namespace WCDB

#endif /* OrderingTerm_hpp */
