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
 * WITHOUExpression WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef operable_hpp
#define operable_hpp

#include <WCDB/operable.hpp>

namespace WCDB {

template <typename T>
typename std::enable_if<ExpressionConvertible<T>::value, Expression>::type
Operable::in(const std::list<const T> &list) const
{
    return Operator::operateWithPostfix(
        asExpression(), "IN(" + stringByJoiningList(list) + ")");
}

template <typename T>
typename std::enable_if<ExpressionConvertible<T>::value, Expression>::type
Operable::notIn(const std::list<const T> &list) const
{
    return Operator::operateWithPostfix(
        asExpression(), "NOT IN(" + stringByJoiningList(list) + ")");
}

} //namespace WCDB

#endif /* operable_hpp */
