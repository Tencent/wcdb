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

#include "StatementInsertBridge.h"
#include "BindParameter.hpp"
#include "Column.hpp"
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
#include "LiteralValue.hpp"
#include "Schema.hpp"
#include "StatementInsert.hpp"
#include "StatementSelect.hpp"
#include "Upsert.hpp"
#include "WinqBridge.hpp"

CPPStatementInsert WCDBStatementInsertCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementInsert, WCDB::StatementInsert);
}

void WCDBStatementInsertConfigWith(CPPStatementInsert insert,
                                   const CPPCommonTableExpression* _Nullable exps,
                                   int expNum)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDBGetCPPSyntaxListOrReturn(WCDB::CommonTableExpression, cppExps, exps, expNum);
    cppInsert->with(cppExps);
}

void WCDBStatementInsertConfigRecursive(CPPStatementInsert insert)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppInsert->recursive();
}

void WCDBStatementInsertConfigTable(CPPStatementInsert insert, const char* _Nullable name)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppInsert->insertIntoTable(name);
}

void WCDBStatementInsertConfigSchema(CPPStatementInsert insert, CPPSchema schema)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppInsert->schema(*cppSchema);
}

void WCDBStatementInsertConfigSchema2(CPPStatementInsert insert, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppInsert->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementInsertConfigConfiction(CPPStatementInsert insert,
                                         enum WCDBSyntaxConflictAction action)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppInsert->syntax().conflictAction = WCDB::WinqBridge::changeConflictAction(action);
}

void WCDBStatementInsertConfigAlias(CPPStatementInsert insert, const char* _Nullable name)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppInsert->as(name);
}

void WCDBStatementInsertConfigColumns(CPPStatementInsert insert,
                                      const CPPColumn* _Nullable columns,
                                      int colNum)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Column, cppColumns, columns, colNum);
    cppInsert->columns(cppColumns);
}

void WCDBStatementInsertConfigColumns2(CPPStatementInsert insert, CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDB::Columns cppColumns;
    for (int i = 0; i < columns.length; i++) {
        switch (columns.type) {
        case WCDBBridgedType_String:
            cppColumns.emplace_back(
            WCDB::Column(WCDBGetCommonArrayLiteralValue(const char*, columns, i)));
            break;
        case WCDBBridgedType_Column:
            cppColumns.emplace_back(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
            break;
        default:
            assert(0);
            break;
        }
    }
    cppInsert->columns(cppColumns);
}

void WCDBStatementInsertConfigValuesWithMultiTypeArray(CPPStatementInsert insert,
                                                       CPPMultiTypeArray values)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    int intIndex = 0;
    int doubleIndex = 0;
    int stringIndex = 0;
    for (int i = 0; i < values.totalLength; i++) {
        switch (values.types[i]) {
        case WCDBBridgedType_Null: {
            cppInsert->value(WCDB::LiteralValue(nullptr));
            intIndex++;
        } break;
        case WCDBBridgedType_Bool: {
            cppInsert->value(WCDB::LiteralValue((bool) values.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_Int: {
            cppInsert->value(WCDB::LiteralValue((int64_t) values.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_UInt: {
            cppInsert->value(WCDB::LiteralValue((uint64_t) values.intValues[intIndex]));
            intIndex++;
        } break;
        case WCDBBridgedType_Double: {
            cppInsert->value(WCDB::LiteralValue(values.doubleValues[doubleIndex]));
            doubleIndex++;
        } break;
        case WCDBBridgedType_String: {
            cppInsert->value(WCDB::LiteralValue(
            WCDB::UnsafeStringView(values.stringValues[stringIndex])));
            stringIndex++;
        } break;
        case WCDBBridgedType_Column: {
            cppInsert->value(WCDBGetMultiTypeArrayObject(WCDB::Column, values, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_LiteralValue: {
            cppInsert->value(WCDBGetMultiTypeArrayObject(WCDB::LiteralValue, values, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_BindParameter: {
            cppInsert->value(WCDBGetMultiTypeArrayObject(WCDB::BindParameter, values, intIndex));
            intIndex++;
        } break;
        case WCDBBridgedType_Expression: {
            cppInsert->value(WCDBGetMultiTypeArrayObject(WCDB::Expression, values, intIndex));
            intIndex++;
        } break;
        default:
            break;
        }
    }
}

void WCDBStatementInsertConfigValuesWithBindParameters(CPPStatementInsert insert, int parametersCount)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppInsert->values(WCDB::BindParameter::bindParameters(parametersCount));
}

void WCDBStatementInsertConfigValues(CPPStatementInsert insert,
                                     const CPPExpression* _Nullable exps,
                                     int expNum)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Expression, cppExps, exps, expNum);
    cppInsert->values(cppExps);
}

void WCDBStatementInsertConfigSelect(CPPStatementInsert insert, CPPStatementSelect select)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDBGetObjectOrReturn(select, WCDB::StatementSelect, cppSelect)
    cppInsert->values(*cppSelect);
}

void WCDBStatementInsertConfigDefaultValues(CPPStatementInsert insert)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    cppInsert->defaultValues();
}

void WCDBStatementInsertConfigUpsert(CPPStatementInsert insert, CPPUpsert upsert)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    cppInsert->upsert(*cppUpsert);
}
