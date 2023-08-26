//
// Created by qiuwenchen on 2022/5/30.
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

#include "StatementSelectBridge.h"
#include "Column.hpp"
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
#include "Join.hpp"
#include "LiteralValue.hpp"
#include "OrderingTerm.hpp"
#include "ResultColumn.hpp"
#include "StatementSelect.hpp"
#include "TableOrSubquery.hpp"
#include "WindowDef.hpp"
#include "WinqBridge.hpp"

CPPStatementSelect WCDBStatementSelectCreate()
{
    return WCDBCreateCPPBridgedObject(CPPStatementSelect, WCDB::StatementSelect);
}

void WCDBStatementSelectConfigWith(CPPStatementSelect select,
                                   const CPPCommonTableExpression* exps,
                                   int expNum)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetCPPSyntaxListOrReturn(WCDB::CommonTableExpression, cppExps, exps, expNum);
    cppSelect->with(cppExps);
}

void WCDBStatementSelectConfigRecursive(CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->recursive();
}

void WCDBStatementSelectConfigResultColumns(CPPStatementSelect select,
                                            const CPPResultColumn* columns,
                                            int columnNum)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetCPPSyntaxListOrReturn(WCDB::ResultColumn, cppColumns, columns, columnNum);
    cppSelect->select(cppColumns);
}

void WCDBStatementSelectConfigResultColumns2(CPPStatementSelect select, CPPMultiTypeArray resultColumns)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDB::ResultColumns cppResultColumns;
    int intIndex = 0;
    int doubleIndex = 0;
    int stringIndex = 0;
    for (int i = 0; i < resultColumns.totalLength; i++) {
        switch (resultColumns.types[i]) {
        case WCDBBridgedType_Null: {
            cppResultColumns.emplace_back(WCDB::LiteralValue(nullptr));
            intIndex++;
        } break;
        case WCDBBridgedType_Bool: {
            cppResultColumns.emplace_back(
            WCDB::LiteralValue((bool) resultColumns.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_Int: {
            cppResultColumns.emplace_back(
            WCDB::LiteralValue((int64_t) resultColumns.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_UInt: {
            cppResultColumns.emplace_back(
            WCDB::LiteralValue((uint64_t) resultColumns.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_Double: {
            cppResultColumns.emplace_back(
            WCDB::LiteralValue(resultColumns.doubleValues[doubleIndex]));
            doubleIndex++;
        } break;
        case WCDBBridgedType_String: {
            cppResultColumns.emplace_back(WCDB::Column(
            WCDB::UnsafeStringView(resultColumns.stringValues[stringIndex])));
            stringIndex++;
        } break;
        case WCDBBridgedType_Column: {
            cppResultColumns.emplace_back(
            WCDBGetMultiTypeArrayObject(WCDB::Column, resultColumns, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_LiteralValue: {
            cppResultColumns.emplace_back(WCDBGetMultiTypeArrayObject(
            WCDB::LiteralValue, resultColumns, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_Expression: {
            cppResultColumns.emplace_back(WCDBGetMultiTypeArrayObject(
            WCDB::Expression, resultColumns, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_RaiseFunction: {
            cppResultColumns.emplace_back(WCDBGetMultiTypeArrayObject(
            WCDB::RaiseFunction, resultColumns, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_ResultColumn: {
            cppResultColumns.push_back(WCDBGetMultiTypeArrayObject(
            WCDB::ResultColumn, resultColumns, intIndex));
            intIndex++;
        } break;
        default:
            assert(0);
            break;
        }
    }
    cppSelect->select(cppResultColumns);
}

void WCDBStatementSelectConfigDistinct(CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->distinct();
}

void WCDBStatementSelectConfigFromTableOrSubqueries(CPPStatementSelect select,
                                                    const CPPTableOrSubquery* _Nullable tableOrSubqueries,
                                                    int queryNum)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetCPPSyntaxListOrReturn(
    WCDB::TableOrSubquery, cppQueries, tableOrSubqueries, queryNum);
    cppSelect->from(cppQueries);
}

void WCDBStatementSelectConfigFromTableOrSubqueries2(CPPStatementSelect select,
                                                     CPPMultiTypeArray tableOrSubqueries)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDB::TablesOrSubqueries cppTableOrSubqueries;
    int intIndex = 0;
    int stringIndex = 0;
    for (int i = 0; i < tableOrSubqueries.totalLength; i++) {
        switch (tableOrSubqueries.types[i]) {
        case WCDBBridgedType_String: {
            cppTableOrSubqueries.emplace_back(
            WCDB::UnsafeStringView(tableOrSubqueries.stringValues[stringIndex]));
            stringIndex++;
        } break;
        case WCDBBridgedType_TableOrSubquery: {
            cppTableOrSubqueries.push_back(WCDBGetMultiTypeArrayObject(
            WCDB::TableOrSubquery, tableOrSubqueries, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_SelectSTMT: {
            cppTableOrSubqueries.emplace_back(WCDBGetMultiTypeArrayObject(
            WCDB::StatementSelect, tableOrSubqueries, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_JoinClause: {
            cppTableOrSubqueries.emplace_back(WCDBGetMultiTypeArrayObject(
            WCDB::Join, tableOrSubqueries, intIndex));
            intIndex++;
        } break;
        default:
            assert(tableOrSubqueries.types[i] == WCDBBridgedType_Invalid);
            break;
        }
    }
    cppSelect->from(cppTableOrSubqueries);
}

void WCDBStatementSelectConfigWhere(CPPStatementSelect select, CPPExpression condition)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetObjectOrReturn(condition, WCDB::Expression, cppCondition);
    cppSelect->where(*cppCondition);
}

void WCDBStatementSelectConfigGroups(CPPStatementSelect select, const CPPExpression* exps, int expNum)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Expression, cppExps, exps, expNum);
    cppSelect->groups(cppExps);
}

void WCDBStatementSelectConfigGroups2(CPPStatementSelect select, CPPMultiTypeArray groups)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDB::Expressions cppExps;
    int intIndex = 0;
    int stringIndex = 0;
    for (int i = 0; i < groups.totalLength; i++) {
        switch (groups.types[i]) {
        case WCDBBridgedType_String: {
            cppExps.emplace_back(WCDB::Column(groups.stringValues[stringIndex]));
            stringIndex++;
        } break;
        case WCDBBridgedType_Column: {
            cppExps.emplace_back(WCDBGetMultiTypeArrayObject(WCDB::Column, groups, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_Expression: {
            cppExps.push_back(WCDBGetMultiTypeArrayObject(WCDB::Expression, groups, intIndex));
            intIndex++;
        } break;
        default:
            assert(groups.types[i] == WCDBBridgedType_Invalid);
            break;
        }
    }
    cppSelect->groups(cppExps);
}

void WCDBStatementSelectConfigHaving(CPPStatementSelect select, CPPExpression expression)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppSelect->having(*cppExpression);
}

void WCDBStatementSelectConfigWindow(CPPStatementSelect select, const char* windowName)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->window(windowName);
}

void WCDBStatementSelectConfigAs(CPPStatementSelect select, CPPWindowDef windowDef)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetObjectOrReturn(windowDef, WCDB::WindowDef, cppWindowDef);
    cppSelect->as(*cppWindowDef);
}

void WCDBStatementSelectConfigValues(CPPStatementSelect select, const CPPExpression* exps, int expNum)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Expression, cppExps, exps, expNum);
    cppSelect->values(cppExps);
}

void WCDBStatementSelectConfigUnion(CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->union_();
}

void WCDBStatementSelectConfigUnionAll(CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->unionAll();
}

void WCDBStatementSelectConfigIntersect(CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->intersect();
}

void WCDBStatementSelectConfigExcept(CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->except();
}

void WCDBStatementSelectConfigOrders(CPPStatementSelect select,
                                     const CPPOrderingTerm* orders,
                                     int orderNum)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetCPPSyntaxListOrReturn(WCDB::OrderingTerm, cppOrders, orders, orderNum);
    cppSelect->orders(cppOrders);
}

void WCDBStatementSelectConfigLimitRange(CPPStatementSelect select, CPPExpression from, CPPExpression to)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetObjectOrReturn(from, WCDB::Expression, cppFrom);
    WCDBGetObjectOrReturn(to, WCDB::Expression, cppTo);
    cppSelect->limit(*cppFrom, *cppTo);
}

void WCDBStatementSelectConfigLimitCount(CPPStatementSelect select, CPPExpression limit)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetObjectOrReturn(limit, WCDB::Expression, cppLimit);
    cppSelect->limit(*cppLimit);
}

void WCDBStatementSelectConfigLimitOffset(CPPStatementSelect select, CPPExpression offset)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetObjectOrReturn(offset, WCDB::Expression, cppOffset);
    cppSelect->offset(*cppOffset);
}

void WCDBStatementSelectConfigLimitRange2(CPPStatementSelect select,
                                          CPPCommonValue from,
                                          CPPCommonValue to)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->limit(WCDBCreateExpressionFromCommonValue(from),
                     WCDBCreateExpressionFromCommonValue(to));
}

void WCDBStatementSelectConfigLimitCount2(CPPStatementSelect select, CPPCommonValue limit)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->limit(WCDBCreateExpressionFromCommonValue(limit));
}

void WCDBStatementSelectConfigOffset2(CPPStatementSelect select, CPPCommonValue offset)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    cppSelect->offset(WCDBCreateExpressionFromCommonValue(offset));
}
