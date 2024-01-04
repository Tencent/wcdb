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

#include "SQL.hpp"

namespace WCDB {

class TableOrSubquery;

class WCDB_API Join final : public SpecifiedSyntax<Syntax::JoinClause, SQL> {
public:
    using SpecifiedSyntax<Syntax::JoinClause, SQL>::SpecifiedSyntax;
    Join();
    ~Join() override;

    Join& table(const TableOrSubquery& tableOrSubquery);

    Join& with(const TableOrSubquery& tableOrSubquery);
    Join& join(const TableOrSubquery& tableOrSubquery);
    Join& leftOuterJoin(const TableOrSubquery& tableOrSubquery);
    Join& leftJoin(const TableOrSubquery& tableOrSubquery);
    Join& innerJoin(const TableOrSubquery& tableOrSubquery);
    Join& crossJoin(const TableOrSubquery& tableOrSubquery);
    Join& naturalJoin(const TableOrSubquery& tableOrSubquery);
    Join& naturalLeftOuterJoin(const TableOrSubquery& tableOrSubquery);
    Join& naturalLeftJoin(const TableOrSubquery& tableOrSubquery);
    Join& naturalInnerJoin(const TableOrSubquery& tableOrSubquery);
    Join& naturalCrossJoin(const TableOrSubquery& tableOrSubquery);

    Join& on(const Expression& expression);
    Join& usingColumn(const Column& column);
    Join& usingColumns(const Columns& columns);

    TableOrSubquery as(const UnsafeStringView& alias);

private:
    Join& constraint(const JoinConstraint& constraint);
    Join& join(const Syntax::JoinOperator& joinOperator, const TableOrSubquery& tableOrSubquery);
};

} // namespace WCDB
