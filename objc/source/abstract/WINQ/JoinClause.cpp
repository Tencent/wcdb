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

#include <WCDB/WINQ.h>

namespace WCDB {

JoinClause::JoinClause(const TableOrSubquery &tableOrSubquery)
{
    lang::JoinClause &lang = getMutableLang();
    lang.tableOrSubquery.assign(tableOrSubquery.getLang());
}

JoinClause &JoinClause::leftJoin(const TableOrSubquery &tableOrSubquery,
                                 const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, lang::JoinOperator::Type::Left, tableOrSubquery,
                      joinConstraint);
    return *this;
}
JoinClause &JoinClause::leftOuterJoin(const TableOrSubquery &tableOrSubquery,
                                      const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, lang::JoinOperator::Type::LeftOuter,
                      tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause &JoinClause::innerJoin(const TableOrSubquery &tableOrSubquery,
                                  const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, lang::JoinOperator::Type::Inner, tableOrSubquery,
                      joinConstraint);
    return *this;
}
JoinClause &JoinClause::crossJoin(const TableOrSubquery &tableOrSubquery,
                                  const JoinConstraint &joinConstraint)
{
    appendJoinOperand(false, lang::JoinOperator::Type::Cross, tableOrSubquery,
                      joinConstraint);
    return *this;
}

JoinClause &JoinClause::naturalLeftJoin(const TableOrSubquery &tableOrSubquery,
                                        const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, lang::JoinOperator::Type::Left, tableOrSubquery,
                      joinConstraint);
    return *this;
}
JoinClause &
JoinClause::naturalLeftOuterJoin(const TableOrSubquery &tableOrSubquery,
                                 const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, lang::JoinOperator::Type::LeftOuter,
                      tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause &JoinClause::naturalInnerJoin(const TableOrSubquery &tableOrSubquery,
                                         const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, lang::JoinOperator::Type::Inner, tableOrSubquery,
                      joinConstraint);
    return *this;
}
JoinClause &JoinClause::naturalCrossJoin(const TableOrSubquery &tableOrSubquery,
                                         const JoinConstraint &joinConstraint)
{
    appendJoinOperand(true, lang::JoinOperator::Type::Cross, tableOrSubquery,
                      joinConstraint);
    return *this;
}

void JoinClause::appendJoinOperand(bool natural,
                                   const lang::JoinOperator::Type &type,
                                   const TableOrSubquery &tableOrSubquery,
                                   const JoinConstraint &joinConstraint)
{
    lang::JoinClause &lang = getMutableLang();

    lang::copy_on_write_lazy_lang<lang::JoinClause::Operand> cowJoinOperand;

    {
        lang::JoinClause::Operand &joinOperand = cowJoinOperand.get_or_copy();

        lang::JoinOperator joinOperator;
        joinOperator.join = true;
        joinOperator.natural = natural;
        joinOperator.type = type;

        joinOperand.joinOperator.assign(joinOperator);
        joinOperand.tableOrSubquery.assign(tableOrSubquery.getLang());
        joinOperand.joinConstraint.assign(joinConstraint.getLang());
    }

    lang.joinOperands.append(cowJoinOperand);
}

} // namespace WCDB
