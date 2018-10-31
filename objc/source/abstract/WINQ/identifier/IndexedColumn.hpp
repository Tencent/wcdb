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

#ifndef IndexedColumn_hpp
#define IndexedColumn_hpp

#include <WCDB/SQL.hpp>

namespace WCDB {

class IndexedColumn : public TypedSyntax<Syntax::IndexedColumn, SQL> {
public:
    using TypedSyntax<Syntax::IndexedColumn, SQL>::TypedSyntax;

    template<typename T, typename Enable = typename std::enable_if<IndexedColumnConvertible<T>::value>::type>
    IndexedColumn(const T& t)
    : IndexedColumn(IndexedColumnConvertible<T>::asIndexedColumn(t))
    {
    }

    IndexedColumn(const Column& column);
    IndexedColumn(const Expression& expression);

    IndexedColumn& collate(const String& collation);
    IndexedColumn& order(const Order& order);
};

} // namespace WCDB

#endif /* IndexedColumn_hpp */
