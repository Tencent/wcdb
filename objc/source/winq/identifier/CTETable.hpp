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

#ifndef _WCDB_CTETABLE_HPP
#define _WCDB_CTETABLE_HPP

#include <WCDB/SQL.hpp>

namespace WCDB {

class CTETable final : public TypedSyntax<Syntax::CTETableName, SQL> {
public:
    using TypedSyntax<Syntax::CTETableName, SQL>::TypedSyntax;

    template<typename T, typename Enable = typename std::enable_if<CTETableConvertible<T>::value>::type>
    CTETable(const T& t) : CTETable(CTETableConvertible<T>::asCTETable(t))
    {
    }

    CTETable(const UnsafeString& name);
    CTETable& column(const Column& column);
};

} // namespace WCDB

#endif /* _WCDB_CTETABLE_HPP */
