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
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
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

void WCDBStatementSelectConfigLimitFromTo(CPPStatementSelect select, CPPExpression from, CPPExpression to)
{
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect);
    WCDBGetObjectOrReturn(from, WCDB::Expression, cppFrom);
    WCDBGetObjectOrReturn(to, WCDB::Expression, cppTo);
    cppSelect->limit(*cppFrom, *cppTo);
}

void WCDBStatementSelectConfigLimitFrom(CPPStatementSelect select, CPPExpression limit)
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
