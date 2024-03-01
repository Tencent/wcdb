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
#include "BindParameter.hpp"
#include "Column.hpp"
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
#include "LiteralValue.hpp"
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

void WCDBStatementUpdateConfigTable2(CPPStatementUpdate update, CPPCommonValue table)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    if (table.type == WCDBBridgedType_String) {
        cppUpdate->update(WCDB::UnsafeStringView((const char*) table.intValue));
    } else if (table.type == WCDBBridgedType_QualifiedTableName) {
        cppUpdate->update(WCDBGetBridgedData(WCDB::QualifiedTable, table));
    }
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

void WCDBStatementUpdateConfigColumns2(CPPStatementUpdate update, CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDB::Columns cppColumns;
    if (columns.type == WCDBBridgedType_String) {
        for (int i = 0; i < columns.length; i++) {
            cppColumns.emplace_back(WCDBGetCommonArrayLiteralValue(const char*, columns, i));
        }
    } else if (columns.type == WCDBBridgedType_Column) {
        for (int i = 0; i < columns.length; i++) {
            cppColumns.push_back(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
        }
    } else {
        assert(0);
    }
    cppUpdate->set(cppColumns);
}

void WCDBStatementUpdateConfigValue(CPPStatementUpdate update, CPPExpression expression)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppUpdate->to(*cppExpression);
}

void WCDBStatementUpdateConfigValue2(CPPStatementUpdate update, CPPCommonValue expression)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    cppUpdate->to(WCDBCreateExpressionFromCommonValue(expression));
}

void WCDBStatementUpdateConfigColumnsToValues(CPPStatementUpdate update,
                                              CPPCommonArray columns,
                                              CPPMultiTypeArray values)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    int totalLength
    = columns.length > values.totalLength ? values.totalLength : columns.length;
    int intIndex = 0;
    int doubleIndex = 0;
    int stringIndex = 0;
    for (int i = 0; i < totalLength; i++) {
        if (columns.type == WCDBBridgedType_String) {
            cppUpdate->set(
            WCDB::Column(WCDBGetCommonArrayLiteralValue(const char*, columns, i)));
        } else if (columns.type == WCDBBridgedType_Column) {
            cppUpdate->set(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
        } else {
            assert(0);
        }
        switch (values.types[i]) {
        case WCDBBridgedType_Null: {
            cppUpdate->to(WCDB::LiteralValue(nullptr));
            intIndex++;
        } break;
        case WCDBBridgedType_Bool: {
            cppUpdate->to(WCDB::LiteralValue((bool) values.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_Int: {
            cppUpdate->to(WCDB::LiteralValue((int64_t) values.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_UInt: {
            cppUpdate->to(WCDB::LiteralValue((uint64_t) values.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_Double: {
            cppUpdate->to(WCDB::LiteralValue(values.doubleValues[doubleIndex]));
            doubleIndex++;
        } break;
        case WCDBBridgedType_String: {
            cppUpdate->to(WCDB::LiteralValue(
            WCDB::UnsafeStringView(values.stringValues[stringIndex])));
            stringIndex++;
        } break;
        case WCDBBridgedType_Column: {
            cppUpdate->to(WCDBGetMultiTypeArrayObject(WCDB::Column, values, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_LiteralValue: {
            cppUpdate->to(WCDBGetMultiTypeArrayObject(WCDB::LiteralValue, values, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_BindParameter: {
            cppUpdate->to(WCDBGetMultiTypeArrayObject(WCDB::BindParameter, values, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_Expression: {
            cppUpdate->to(WCDBGetMultiTypeArrayObject(WCDB::Expression, values, intIndex));
            intIndex++;
        } break;
        default:
            break;
        }
    }
}

void WCDBStatementUpdateConfigColumnsToBindParameters(CPPStatementUpdate update,
                                                      CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    if (columns.type == WCDBBridgedType_String) {
        for (int i = 0; i < columns.length; i++) {
            cppUpdate->set(WCDBGetCommonArrayLiteralValue(const char*, columns, i));
            cppUpdate->to(WCDB::BindParameter(i + 1));
        }
    } else if (columns.type == WCDBBridgedType_Column) {
        for (int i = 0; i < columns.length; i++) {
            cppUpdate->set(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
            cppUpdate->to(WCDB::BindParameter(i + 1));
        }
    } else {
        assert(0);
    }
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

void WCDBStatementUpdateConfigLimitRange(CPPStatementUpdate update, CPPExpression from, CPPExpression to)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppUpdate);
    WCDBGetObjectOrReturn(from, WCDB::Expression, cppFrom);
    WCDBGetObjectOrReturn(to, WCDB::Expression, cppTo);
    cppUpdate->limit(*cppFrom, *cppTo);
}

void WCDBStatementUpdateConfigLimitCount(CPPStatementUpdate update, CPPExpression limit)
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

void WCDBStatementUpdateConfigLimitRange2(CPPStatementUpdate update,
                                          CPPCommonValue from,
                                          CPPCommonValue to)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppSelect);
    cppSelect->limit(WCDBCreateExpressionFromCommonValue(from),
                     WCDBCreateExpressionFromCommonValue(to));
}

void WCDBStatementUpdateConfigLimitCount2(CPPStatementUpdate update, CPPCommonValue limit)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppSelect);
    cppSelect->limit(WCDBCreateExpressionFromCommonValue(limit));
}

void WCDBStatementUpdateConfigOffset2(CPPStatementUpdate update, CPPCommonValue offset)
{
    WCDBGetObjectOrReturn(update, WCDB::StatementUpdate, cppSelect);
    cppSelect->offset(WCDBCreateExpressionFromCommonValue(offset));
}
