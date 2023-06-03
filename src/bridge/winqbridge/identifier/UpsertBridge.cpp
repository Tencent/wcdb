//
// Created by qiuwenchen on 2022/6/12.
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

#include "UpsertBridge.h"
#include "Column.hpp"
#include "Expression.hpp"
#include "IndexedColumn.hpp"
#include "ObjectBridge.hpp"
#include "Upsert.hpp"
#include "WinqBridge.hpp"

CPPUpsert WCDBUpsertCreate()
{
    return WCDBCreateCPPBridgedObject(CPPUpsert, WCDB::Upsert);
}

void WCDBUpsertConfigIndexdColumn(CPPUpsert upsert,
                                  const CPPIndexedColumn* _Nullable columns,
                                  int colnum)
{
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    WCDBGetCPPSyntaxListOrReturn(WCDB::IndexedColumn, cppColumns, columns, colnum);
    cppUpsert->syntax().indexedColumns = cppColumns;
}

void WCDBUpsertConfigWhere(CPPUpsert upsert, CPPExpression expression)
{
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppUpsert->where(*cppExpression);
}

void WCDBUpsertConfigDoNothing(CPPUpsert upsert)
{
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    cppUpsert->doNothing();
}

void WCDBUpsertConfigDoUpdate(CPPUpsert upsert)
{
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    cppUpsert->doUpdate();
}

void WCDBUpsertConfigSetColumns(CPPUpsert upsert, const CPPColumn* _Nullable columns, int colNum)
{
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Column, cppColumns, columns, colNum);
    cppUpsert->set(cppColumns);
}

void WCDBUpsertConfigToValue(CPPUpsert upsert, CPPExpression expression)
{
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppUpsert->to(*cppExpression);
}
