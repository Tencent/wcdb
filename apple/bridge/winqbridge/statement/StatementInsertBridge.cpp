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
#include "Column.hpp"
#include "CommonTableExpression.hpp"
#include "Expression.hpp"
#include "Schema.hpp"
#include "StatementInsert.hpp"
#include "StatementSelect.hpp"
#include "Upsert.hpp"
#include "WinqBridge.hpp"

CPPStatementInsert WCDBStatementInsertCreate(void)
{
    return WCDBCreateCPPBridgedObject(CPPStatementInsert, new WCDB::StatementInsert());
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

void WCDBStatementInsertConfigDefaultUpsert(CPPStatementInsert insert, CPPUpsert upsert)
{
    WCDBGetObjectOrReturn(insert, WCDB::StatementInsert, cppInsert);
    WCDBGetObjectOrReturn(upsert, WCDB::Upsert, cppUpsert);
    cppInsert->upsert(*cppUpsert);
}
