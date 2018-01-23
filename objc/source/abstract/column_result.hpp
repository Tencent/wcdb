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

#ifndef column_result_hpp
#define column_result_hpp

#include <WCDB/convertible.hpp>
#include <WCDB/declare.hpp>

namespace WCDB {

class ColumnResult : public Describable {
public:
    template <typename T>
    ColumnResult(const T &t,
                 typename std::enable_if<ExpressionConvertible<T>::value,
                                         void>::type * = nullptr)
        : Describable(
              ExpressionConvertible<T>::asExpression(t).getDescription())
    {
    }

    ColumnResult &as(const std::string &name);

    operator std::list<const ColumnResult>() const;
};

} //namespace WCDB

#endif /* column_result_hpp */
