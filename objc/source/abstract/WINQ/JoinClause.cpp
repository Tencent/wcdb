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

#include <WCDB/Assertion.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

JoinClause::JoinClause(const TableOrSubquery &tableOrSubquery)
{
    Lang::JoinClause &lang = getMutableLang();
    lang.tableOrSubquery.assign(tableOrSubquery.getCOWLang());
}

JoinClause &JoinClause::with(const TableOrSubquery &tableOrSubquery,
                             const JoinConstraint &joinConstraint)
{
    Lang::JoinClause &lang = getMutableLang();

    Lang::CopyOnWriteLazyLang<Lang::JoinClause::Operand> cowJoinOperand;

    {
        Lang::JoinClause::Operand &joinOperand = cowJoinOperand.get_or_copy();

        Lang::JoinOperator joinOperator;
        joinOperator.join = false;

        joinOperand.joinOperator.assign(joinOperator);
        joinOperand.tableOrSubquery.assign(tableOrSubquery.getCOWLang());
        joinOperand.joinConstraint.assign(joinConstraint.getCOWLang());
    }

    lang.joinOperands.append(cowJoinOperand);
    return *this;
}

JoinClause &JoinClause::join(const TableOrSubquery &tableOrSubquery,
                             const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::NotSet, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::leftJoin(const TableOrSubquery &tableOrSubquery,
                                 const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::Left, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::leftOuterJoin(const TableOrSubquery &tableOrSubquery,
                                      const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::LeftOuter,
                      tableOrSubquery, joinConstraint);
    return *this;
}

JoinClause &JoinClause::innerJoin(const TableOrSubquery &tableOrSubquery,
                                  const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::Inner, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::crossJoin(const TableOrSubquery &tableOrSubquery,
                                  const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::Cross, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::naturalJoin(const TableOrSubquery &tableOrSubquery,
                                    const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::NotSet, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::naturalLeftJoin(const TableOrSubquery &tableOrSubquery,
                                        const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::Left, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &
JoinClause::naturalLeftOuterJoin(const TableOrSubquery &tableOrSubquery,
                                 const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::LeftOuter,
                      tableOrSubquery, joinConstraint);
    return *this;
}

JoinClause &JoinClause::naturalInnerJoin(const TableOrSubquery &tableOrSubquery,
                                         const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::Inner, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::naturalCrossJoin(const TableOrSubquery &tableOrSubquery,
                                         const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::Cross, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::with(const TableOrSubquery &tableOrSubquery)
{
    Lang::JoinClause &lang = getMutableLang();

    Lang::CopyOnWriteLazyLang<Lang::JoinClause::Operand> cowJoinOperand;

    {
        Lang::JoinClause::Operand &joinOperand = cowJoinOperand.get_or_copy();

        Lang::JoinOperator joinOperator;
        joinOperator.join = false;

        joinOperand.joinOperator.assign(joinOperator);
        joinOperand.tableOrSubquery.assign(tableOrSubquery.getCOWLang());
    }

    lang.joinOperands.append(cowJoinOperand);
    return *this;
}

JoinClause &JoinClause::join(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::NotSet, tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::leftJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::Left, tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::leftOuterJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::LeftOuter,
                      tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::innerJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::Inner, tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::crossJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(false, Lang::JoinOperator::Type::Cross, tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::naturalJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::NotSet, tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::naturalLeftJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::Left, tableOrSubquery);
    return *this;
}

JoinClause &
JoinClause::naturalLeftOuterJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::LeftOuter,
                      tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::naturalInnerJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::Inner, tableOrSubquery);
    return *this;
}

JoinClause &JoinClause::naturalCrossJoin(const TableOrSubquery &tableOrSubquery)
{
    appendJoinOperand(true, Lang::JoinOperator::Type::Cross, tableOrSubquery);
    return *this;
}

void JoinClause::appendJoinOperand(bool natural,
                                   const Lang::JoinOperator::Type &type,
                                   const TableOrSubquery &tableOrSubquery,
                                   const JoinConstraint &joinConstraint)
{
    Lang::JoinClause &lang = getMutableLang();

    Lang::CopyOnWriteLazyLang<Lang::JoinClause::Operand> cowJoinOperand;

    {
        Lang::JoinClause::Operand &joinOperand = cowJoinOperand.get_or_copy();

        Lang::JoinOperator joinOperator;
        joinOperator.join = true;
        joinOperator.natural = natural;
        joinOperator.type = type;

        joinOperand.joinOperator.assign(joinOperator);
        joinOperand.tableOrSubquery.assign(tableOrSubquery.getCOWLang());
        joinOperand.joinConstraint.assign(joinConstraint.getCOWLang());
    }

    lang.joinOperands.append(cowJoinOperand);
}

void JoinClause::appendJoinOperand(bool natural,
                                   const Lang::JoinOperator::Type &type,
                                   const TableOrSubquery &tableOrSubquery)
{
    Lang::JoinClause &lang = getMutableLang();

    Lang::CopyOnWriteLazyLang<Lang::JoinClause::Operand> cowJoinOperand;

    {
        Lang::JoinClause::Operand &joinOperand = cowJoinOperand.get_or_copy();

        Lang::JoinOperator joinOperator;
        joinOperator.join = true;
        joinOperator.natural = natural;
        joinOperator.type = type;

        joinOperand.joinOperator.assign(joinOperator);
        joinOperand.tableOrSubquery.assign(tableOrSubquery.getCOWLang());
    }

    lang.joinOperands.append(cowJoinOperand);
}

} // namespace WCDB
