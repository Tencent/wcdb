//
// Created by qiuwenchen on 2022/7/15.
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

#include "StatementUpdateBridge.h"
#include "Column.hpp"
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
#include "OrderingTerm.hpp"
#include "QualifiedTable.hpp"
#include "StatementUpdate.hpp"
#include "WinqBridge.hpp"

CPPStatementUpdate WCDBStatementUpdateCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementUpdate, WCDB::StatementUpdate);
}

void WCDBStatementUpdateConfigWith(CPPStatementUpdate update,
                                   const CPPCommonTableExpression* _Nullable exps,
                                   int expNum)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetCPPSyntaxListOrReturn(WCDB::CommonTableExpression, cppExps, exps, expNum);
    cppUpdate->with(cppExps);
}

void WCDBStatementUpdateConfigRecursive(CPPStatementUpdate update)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    cppUpdate->recursive();
}

void WCDBStatementUpdateConfigTable(CPPStatementUpdate update, CPPQualifiedTable table)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(table, WCDB::QualifiedTable, cppTable);
    cppUpdate->update(*cppTable);
}

void WCDBStatementUpdateConfigConfiction(CPPStatementUpdate update,
                                         enum WCDBSyntaxConflictAction action)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    cppUpdate->syntax().conflictAction = WCDB::WinqBridge::changeConflictAction(action);
}

void WCDBStatementUpdateConfigColumns(CPPStatementUpdate update,
                                      const CPPColumn* _Nullable columns,
                                      int colNum)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Column, cppColumns, columns, colNum);
    cppUpdate->set(cppColumns);
}

void WCDBStatementUpdateConfigValue(CPPStatementUpdate update, CPPExpression expression)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppUpdate->to(*cppExpression);
}

void WCDBStatementUpdateConfigCondition(CPPStatementUpdate update, CPPExpression expression)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppUpdate->where(*cppExpression);
}

void WCDBStatementUpdateConfigOrders(CPPStatementUpdate update,
                                     const CPPOrderingTerm* orders,
                                     int orderNum)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetCPPSyntaxListOrReturn(WCDB::OrderingTerm, cppOrders, orders, orderNum);
    cppUpdate->orders(cppOrders);
}

void WCDBStatementUpdateConfigLimitFromTo(CPPStatementUpdate update, CPPExpression from, CPPExpression to)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(from, WCDB::Expression, cppFrom);
    WCDBGetObjectOrReturn(to, WCDB::Expression, cppTo);
    cppUpdate->limit(*cppFrom, *cppTo);
}

void WCDBStatementUpdateConfigLimitFrom(CPPStatementUpdate update, CPPExpression limit)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(limit, WCDB::Expression, cppLimit);
    cppUpdate->limit(*cppLimit);
}

void WCDBStatementUpdateConfigLimitOffset(CPPStatementUpdate update, CPPExpression offset)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(offset, WCDB::Expression, cppOffset);
    cppUpdate->offset(*cppOffset);
}
