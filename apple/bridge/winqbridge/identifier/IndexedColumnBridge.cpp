//
// Created by qiuwenchen on 2022/6/10.
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

#include <WCDB/Column.hpp>
#include <WCDB/Expression.hpp>
#include <WCDB/IndexedColumn.hpp>
#include <WCDB/IndexedColumnBridge.h>
#include <WCDB/ObjectBridge.hpp>
#include <WCDB/WinqBridge.hpp>

CPPIndexedColumn WCDBIndexedColumnCreateWithColumn(CPPColumn column)
{
    WCDBGetObjectOrReturnValue(column, WCDB::Column, cppColumn, CPPIndexedColumn());
    return WCDBCreateCPPBridgedObject(CPPIndexedColumn, new WCDB::IndexedColumn(*cppColumn));
}

CPPIndexedColumn WCDBIndexedColumnCreateWithExpression(CPPExpression expression)
{
    WCDBGetObjectOrReturnValue(
    expression, WCDB::Expression, cppExpression, CPPIndexedColumn());
    return WCDBCreateCPPBridgedObject(CPPIndexedColumn,
                                      new WCDB::IndexedColumn(*cppExpression));
}

void WCDBIndexedColumnConfigCollation(CPPIndexedColumn indexedColumn, const char* _Nullable collation)
{
    WCDBGetObjectOrReturn(indexedColumn, WCDB::IndexedColumn, cppIndexedColumn);
    cppIndexedColumn->collate(collation);
}

void WCDBIndexedColumnConfigOrder(CPPIndexedColumn indexedColumn, enum WCDBSyntaxOrder order)
{
    WCDBGetObjectOrReturn(indexedColumn, WCDB::IndexedColumn, cppIndexedColumn);
    cppIndexedColumn->order(WCDB::WinqBridge::changeOrder(order));
}
