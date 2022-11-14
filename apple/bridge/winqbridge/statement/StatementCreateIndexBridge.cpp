//
// Created by qiuwenchen on 2022/6/29.
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

#include "StatementCreateIndexBridge.h"
#include "Expression.hpp"
#include "IndexedColumn.hpp"
#include "Schema.hpp"
#include "StatementCreateIndex.hpp"
#include "WinqBridge.hpp"

CPPStatementCreateIndex WCDBStatementCreatIndexCreate()
{
    return WCDBCreateCPPBridgedObject(CPPStatementCreateIndex,
                                      new WCDB::StatementCreateIndex());
}

void WCDBStatementCreatIndexConfigIndexName(CPPStatementCreateIndex creatIndex,
                                            const char* _Nullable name)
{
    WCDBGetObjectOrReturn(creatIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->createIndex(name);
}

void WCDBStatementCreatIndexConfigSchema(CPPStatementCreateIndex creatIndex, CPPSchema schema)
{
    WCDBGetObjectOrReturn(creatIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppCreateIndex->schema(*cppSchema);
}

void WCDBStatementCreatIndexConfigUniqe(CPPStatementCreateIndex creatIndex)
{
    WCDBGetObjectOrReturn(creatIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->unique();
}

void WCDBStatementCreatIndexConfigIfNotExist(CPPStatementCreateIndex creatIndex)
{
    WCDBGetObjectOrReturn(creatIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->ifNotExists();
}

void WCDBStatementCreatIndexConfigTable(CPPStatementCreateIndex creatIndex,
                                        const char* _Nullable name)
{
    WCDBGetObjectOrReturn(creatIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->table(name);
}

void WCDBStatementCreatIndexConfigIndexColumns(CPPStatementCreateIndex creatIndex,
                                               const CPPIndexedColumn* _Nullable columns,
                                               int columnNum)
{
    WCDBGetObjectOrReturn(creatIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDBGetCPPSyntaxListOrReturn(WCDB::IndexedColumn, cppColumns, columns, columnNum);
    cppCreateIndex->syntax().indexedColumns = cppColumns;
}

void WCDBStatementCreatIndexConfigWhere(CPPStatementCreateIndex creatIndex, CPPExpression condition)
{
    WCDBGetObjectOrReturn(creatIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDBGetObjectOrReturn(condition, WCDB::Expression, cppCondition);
    cppCreateIndex->where(*cppCondition);
}
