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

#include "TableConstraintBridge.h"
#include "Column.hpp"
#include "Expression.hpp"
#include "ForeignKey.hpp"
#include "IndexedColumn.hpp"
#include "ObjectBridge.hpp"
#include "TableConstraint.hpp"
#include "WinqBridge.hpp"

CPPTableConstraint WCDBTableConstraintCreate(const char* _Nullable name)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPTableConstraint, WCDB::TableConstraint, name);
}

void WCDBTableConstraintConfigPrimaryKey(CPPTableConstraint constraint)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppConstraint);
    cppConstraint->primaryKey();
}

void WCDBTableConstraintConfigUnique(CPPTableConstraint constraint)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppConstraint);
    cppConstraint->unique();
}

void WCDBTableConstraintConfigIndexedColumn(CPPTableConstraint constraint,
                                            const CPPIndexedColumn* _Nullable columns,
                                            int colNum)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppConstraint);
    WCDBGetCPPSyntaxListOrReturn(WCDB::IndexedColumn, cppColumns, columns, colNum);
    cppConstraint->syntax().indexedColumns = cppColumns;
}

void WCDBTableConstraintConfigIndexedColumn2(CPPTableConstraint constraint, CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppTableConstraint);
    WCDB::IndexedColumns indexedColumns;
    for (int i = 0; i < columns.length; i++) {
        switch (columns.type) {
        case WCDBBridgedType_String:
            indexedColumns.emplace_back(WCDB::UnsafeStringView(
            WCDBGetCommonArrayLiteralValue(const char*, columns, i)));
            break;
        case WCDBBridgedType_Column:
            indexedColumns.emplace_back(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
            break;
        case WCDBBridgedType_Expression:
            indexedColumns.emplace_back(
            WCDBGetCommonArrayObject(WCDB::Expression, columns, i));
            break;
        case WCDBBridgedType_IndexedColumn:
            indexedColumns.push_back(
            WCDBGetCommonArrayObject(WCDB::IndexedColumn, columns, i));
            break;
        default:
            assert(0);
            break;
        }
    }
    cppTableConstraint->syntax().indexedColumns = indexedColumns;
}

void WCDBTableConstraintConfigConfliction(CPPTableConstraint constraint,
                                          enum WCDBSyntaxConflictAction conflict)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppConstraint);
    cppConstraint->conflict(WCDB::WinqBridge::changeConflict(conflict));
}

void WCDBTableConstraintConfigCheckCondition(CPPTableConstraint constraint, CPPExpression expression)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppConstraint);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppConstraint->check(*cppExpression);
}

void WCDBTableConstraintConfigForeignKey(CPPTableConstraint constraint,
                                         const CPPColumn* _Nullable columns,
                                         int colNum,
                                         CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppConstraint);
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppForeignKey);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Column, cppColumns, columns, colNum);
    cppConstraint->foreignKey(cppColumns, *cppForeignKey);
}

void WCDBTableConstraintConfigForeignKey2(CPPTableConstraint constraint,
                                          CPPCommonArray columns,
                                          CPPForeignKey foreignKey)
{
    WCDBGetObjectOrReturn(constraint, WCDB::TableConstraint, cppConstraint);
    WCDBGetObjectOrReturn(foreignKey, WCDB::ForeignKey, cppForeignKey);
    WCDB::Columns cppColumns;
    for (int i = 0; i < columns.length; i++) {
        switch (columns.type) {
        case WCDBBridgedType_String:
            cppColumns.emplace_back(WCDBGetCommonArrayLiteralValue(const char*, columns, i));
            break;
        case WCDBBridgedType_Column:
            cppColumns.push_back(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
            break;
        default:
            assert(0);
            break;
        }
    }
    cppConstraint->foreignKey(cppColumns, *cppForeignKey);
}
