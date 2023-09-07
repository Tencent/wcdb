//
// Created by qiuwenchen on 2022/5/29.
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

#include "ColumnConstraintBridge.h"
#include "Column.hpp"
#include "ColumnConstraint.hpp"
#include "Expression.hpp"
#include "ExpressionBridge.h"
#include "ForeignKey.hpp"
#include "LiteralValue.hpp"
#include "ObjectBridge.hpp"
#include "RaiseFunction.hpp"
#include "StatementSelect.hpp"
#include "WinqBridge.hpp"

CPPColumnConstraint WCDBColumnConstraintCreate(const char* _Nullable name)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPColumnConstraint, WCDB::ColumnConstraint, name);
}

void WCDBColumnConstraintConfigPrimaryKey(CPPColumnConstraint constraint)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnConstraint->primaryKey();
}

void WCDBColumnConstraintConfigOrder(CPPColumnConstraint constraint, WCDBSyntaxOrder order)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    switch (order) {
    case WCDBSyntaxOrder_Asc:
        cppColumnConstraint->order(WCDB::Order::ASC);
        break;
    case WCDBSyntaxOrder_Desc:
        cppColumnConstraint->order(WCDB::Order::DESC);
        break;
    default:
        break;
    }
}

void WCDBColumnConstraintConfigCoflictAction(CPPColumnConstraint constraint,
                                             WCDBSyntaxConflictAction conflictAction)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnConstraint->conflict(WCDB::WinqBridge::changeConflict(conflictAction));
}

void WCDBColumnConstraintConfigAutoIncrement(CPPColumnConstraint constraint)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnConstraint->autoIncrement();
}

void WCDBColumnConstraintConfigNotNull(CPPColumnConstraint constraint)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnConstraint->notNull();
}
void WCDBColumnConstraintConfigUnique(CPPColumnConstraint constraint)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnConstraint->unique();
}
void WCDBColumnConstraintConfigCheck(CPPColumnConstraint constraint, CPPExpression expression)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppColumnConstraint->check(*cppExpression);
}

void WCDBColumnConstraintConfigDefaultValue(CPPColumnConstraint constraint, CPPExpression expression)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppColumnConstraint->default_(*cppExpression);
}

void WCDBColumnConstraintConfigDefaultValue2(CPPColumnConstraint constraint, CPPCommonValue expression)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnConstraint->default_(WCDBCreateExpressionFromCommonValue(expression));
}

void WCDBColumnConstraintConfigCollation(CPPColumnConstraint constraint, const char* collation)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    cppColumnConstraint->collate(collation);
}

void WCDBColumnConstraintConfigForeignKey(CPPColumnConstraint constraint, CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppColumnConstraint);
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppObj);
    cppColumnConstraint->foreignKey(*cppObj);
}

void WCDBColumnConstraintConfigUnIndexed(CPPColumnConstraint constraint)
{
    WCDBGetObjectOrReturn(constraint, WCDB::ColumnConstraint, cppConstraint);
    cppConstraint->unIndexed();
}
