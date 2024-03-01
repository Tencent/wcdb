//
// Created by sanhuazhang on 2019/05/02
//

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

#pragma once

#include "Column.hpp"
#include "SQL.hpp"

namespace WCDB {

class WCDB_API ColumnDef final : public SpecifiedSyntax<Syntax::ColumnDef, SQL> {
public:
    using SpecifiedSyntax<Syntax::ColumnDef, SQL>::SpecifiedSyntax;

    template<typename T, typename Enable = typename std::enable_if<ColumnConvertible<T>::value>::type>
    ColumnDef(const T& t) : ColumnDef(ColumnConvertible<T>::asColumn(t))
    {
    }

    template<typename T, typename Enable = typename std::enable_if<ColumnConvertible<T>::value>::type>
    ColumnDef(const T& t, const ColumnType& type)
    : ColumnDef(ColumnConvertible<T>::asColumn(t), type)
    {
    }

    ColumnDef();
    ColumnDef(const Column& column);
    ColumnDef(const Column& column, const ColumnType& type);
    ~ColumnDef() override;

    ColumnDef& constraint(const ColumnConstraint& constraint);
};

} // namespace WCDB
