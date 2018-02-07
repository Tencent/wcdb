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

JoinClause::JoinClause(const TableOrSubquery& tableOrSubquery)
{
    LangJoinClause& lang = getMutableLang();
    lang.tableOrSubquery.assign(tableOrSubquery.getLang());
}

JoinClause& JoinClause::leftJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(false, LangJoinOperator::Type::Left, tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause& JoinClause::leftOuterJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(false, LangJoinOperator::Type::LeftOuter, tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause& JoinClause::innerJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(false, LangJoinOperator::Type::Inner, tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause& JoinClause::crossJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(false, LangJoinOperator::Type::Cross, tableOrSubquery, joinConstraint);
    return *this;
}

JoinClause& JoinClause::naturalLeftJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(true, LangJoinOperator::Type::Left, tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause& JoinClause::naturalLeftOuterJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(true, LangJoinOperator::Type::LeftOuter, tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause& JoinClause::naturalInnerJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(true, LangJoinOperator::Type::Inner, tableOrSubquery, joinConstraint);
    return *this;
}
JoinClause& JoinClause::naturalCrossJoin(const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    appendJoinOperand(true, LangJoinOperator::Type::Cross, tableOrSubquery, joinConstraint);
    return *this;
}

void JoinClause::appendJoinOperand(bool natural, const LangJoinOperator::Type& type, const TableOrSubquery& tableOrSubquery, const JoinConstraint& joinConstraint)
{
    LangJoinClause& lang = getMutableLang();
    
    copy_on_write_lazy_lang<LangJoinClause::Operand> cowJoinOperand;
    
    {
        LangJoinClause::Operand& joinOperand = cowJoinOperand.get_or_copy();
        
        LangJoinOperator joinOperator;
        joinOperator.join = true;
        joinOperator.natural = natural;
        joinOperator.type = type;
        
        joinOperand.joinOperator.assign(joinOperator);
        joinOperand.tableOrSubquery.assign(tableOrSubquery.getLang());
        joinOperand.joinConstraint.assign(joinConstraint.getLang());
    }
    
    lang.joinOperands.append(cowJoinOperand);
}
