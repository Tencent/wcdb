//
// Created by qiuwenchen on 2022/7/3.
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

#include "StatementDeleteBridge.h"
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
#include "OrderingTerm.hpp"
#include "QualifiedTable.hpp"
#include "StatementDelete.hpp"
#include "WinqBridge.hpp"

CPPStatementDelete WCDBStatementDeleteCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementDelete, WCDB::StatementDelete);
}

void WCDBStatementDeleteConfigWith(CPPStatementDelete deleteStatement,
                                   const CPPCommonTableExpression* _Nullable expressions,
                                   int expNum)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    WCDBGetCPPSyntaxListOrReturn(
    WCDB::CommonTableExpression, cppExpressions, expressions, expNum);
    cppDelete->with(cppExpressions);
}

void WCDBStatementDeleteConfigRecursive(CPPStatementDelete deleteStatement)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    cppDelete->recursive();
}

void WCDBStatementDeleteConfigDeleteFrom(CPPStatementDelete deleteStatement, CPPQualifiedTable table)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    WCDBGetObjectOrReturn(table, WCDB::QualifiedTable, cppTable);
    cppDelete->deleteFrom(*cppTable);
}

void WCDBStatementDeleteConfigDeleteFrom2(CPPStatementDelete deleteStatement, CPPCommonValue table)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    if (table.type == WCDBBridgedType_String) {
        cppDelete->deleteFrom(WCDB::UnsafeStringView((const char*) table.intValue));
    } else if (table.type == WCDBBridgedType_QualifiedTableName) {
        cppDelete->deleteFrom(WCDBGetBridgedData(WCDB::QualifiedTable, table));
    } else {
        assert(table.type == WCDBBridgedType_Null);
    }
}

void WCDBStatementDeleteConfigWhere(CPPStatementDelete deleteStatement, CPPExpression expression)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppDelete->where(*cppExpression);
}

void WCDBStatementDeleteConfigOrder(CPPStatementDelete deleteStatement,
                                    const CPPOrderingTerm* _Nullable orders,
                                    int orderNum)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    WCDBGetCPPSyntaxListOrReturn(WCDB::OrderingTerm, cppOrders, orders, orderNum);
    cppDelete->orders(cppOrders);
}

void WCDBStatementDeleteConfigLimitRange(CPPStatementDelete deleteStatement,
                                         CPPExpression from,
                                         CPPExpression to)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    WCDBGetObjectOrReturn(from, WCDB::Expression, cppFrom);
    WCDBGetObjectOrReturn(to, WCDB::Expression, cppTo);
    cppDelete->limit(*cppFrom, *cppTo);
}

void WCDBStatementDeleteConfigLimitCount(CPPStatementDelete deleteStatement, CPPExpression limit)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    WCDBGetObjectOrReturn(limit, WCDB::Expression, cppLimit);
    cppDelete->limit(*cppLimit);
}

void WCDBStatementDeleteConfigOffset(CPPStatementDelete deleteStatement, CPPExpression offset)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    WCDBGetObjectOrReturn(offset, WCDB::Expression, cppOffset);
    cppDelete->offset(*cppOffset);
}

void WCDBStatementDeleteConfigLimitRange2(CPPStatementDelete deleteStatement,
                                          CPPCommonValue from,
                                          CPPCommonValue to)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    cppDelete->limit(WCDBCreateExpressionFromCommonValue(from),
                     WCDBCreateExpressionFromCommonValue(to));
}

void WCDBStatementDeleteConfigLimitCount2(CPPStatementDelete deleteStatement, CPPCommonValue limit)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    cppDelete->limit(WCDBCreateExpressionFromCommonValue(limit));
}

void WCDBStatementDeleteConfigOffset2(CPPStatementDelete deleteStatement, CPPCommonValue offset)
{
    WCDBGetObjectOrReturn(deleteStatement, WCDB::StatementDelete, cppDelete);
    cppDelete->offset(WCDBCreateExpressionFromCommonValue(offset));
}
