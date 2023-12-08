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

#include "WINQ.h"

namespace WCDB {

Join::Join() = default;

Join::~Join() = default;

Join& Join::table(const TableOrSubquery& tableOrSubquery)
{
    syntax().tableOrSubqueries.push_back(tableOrSubquery);
    return *this;
}

Join& Join::with(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::With, tableOrSubquery);
    return *this;
}

Join& Join::join(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::Join, tableOrSubquery);
    return *this;
}

Join& Join::leftOuterJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::LeftOuterJoin, tableOrSubquery);
    return *this;
}

Join& Join::leftJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::LeftJoin, tableOrSubquery);
    return *this;
}

Join& Join::innerJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::InnerJoin, tableOrSubquery);
    return *this;
}

Join& Join::crossJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::CrossJoin, tableOrSubquery);
    return *this;
}

Join& Join::naturalJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::NaturalJoin, tableOrSubquery);
    return *this;
}

Join& Join::naturalLeftOuterJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::NaturalLeftOuterJoin, tableOrSubquery);
    return *this;
}

Join& Join::naturalLeftJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::NaturalLeftJoin, tableOrSubquery);
    return *this;
}

Join& Join::naturalInnerJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::NaturalInnerJoin, tableOrSubquery);
    return *this;
}

Join& Join::naturalCrossJoin(const TableOrSubquery& tableOrSubquery)
{
    join(Syntax::JoinOperator::NaturalCrossJoin, tableOrSubquery);
    return *this;
}

Join& Join::on(const Expression& expression)
{
    return constraint(JoinConstraint().on(expression));
}

Join& Join::usingColumn(const Column& column)
{
    return constraint(JoinConstraint().column(column));
}

Join& Join::usingColumns(const Columns& columns)
{
    return constraint(JoinConstraint().columns(columns));
}

Join& Join::constraint(const JoinConstraint& constraint)
{
    syntax().joinConstraints.back() = constraint;
    return *this;
}

Join& Join::join(const Syntax::JoinOperator& joinOperator, const TableOrSubquery& tableOrSubquery)
{
    syntax().joinOperators.push_back(joinOperator);
    syntax().tableOrSubqueries.push_back(tableOrSubquery);
    syntax().joinConstraints.push_back(nullptr);
    return *this;
}

TableOrSubquery Join::as(const UnsafeStringView& alias)
{
    return TableOrSubquery(*this).as(alias);
}

} // namespace WCDB
