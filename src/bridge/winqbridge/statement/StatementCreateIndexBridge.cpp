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
#include "Column.hpp"
#include "Expression.hpp"
#include "IndexedColumn.hpp"
#include "Schema.hpp"
#include "StatementCreateIndex.hpp"
#include "WinqBridge.hpp"

CPPStatementCreateIndex WCDBStatementCreateIndexCreate()
{
    return WCDBCreateCPPBridgedObject(CPPStatementCreateIndex, WCDB::StatementCreateIndex);
}

void WCDBStatementCreateIndexConfigIndexName(CPPStatementCreateIndex createIndex,
                                             const char* _Nullable name)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->createIndex(name);
}

void WCDBStatementCreateIndexConfigSchema(CPPStatementCreateIndex createIndex, CPPSchema schema)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppCreateIndex->schema(*cppSchema);
}

void WCDBStatementCreateIndexConfigSchema2(CPPStatementCreateIndex createIndex,
                                           CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBStatementCreateIndexConfigUniqe(CPPStatementCreateIndex createIndex)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->unique();
}

void WCDBStatementCreateIndexConfigIfNotExist(CPPStatementCreateIndex createIndex)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->ifNotExists();
}

void WCDBStatementCreateIndexConfigTable(CPPStatementCreateIndex createIndex,
                                         const char* _Nullable name)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    cppCreateIndex->table(name);
}

void WCDBStatementCreateIndexConfigIndexColumns(CPPStatementCreateIndex createIndex,
                                                const CPPIndexedColumn* _Nullable columns,
                                                int columnNum)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDBGetCPPSyntaxListOrReturn(WCDB::IndexedColumn, cppColumns, columns, columnNum);
    cppCreateIndex->syntax().indexedColumns = cppColumns;
}

void WCDBStatementCreateIndexConfigIndexColumns2(CPPStatementCreateIndex createIndex,
                                                 CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDB::IndexedColumns indexedColumns;
    for (int i = 0; i < columns.length; i++) {
        switch (columns.type) {
        case WCDBBridgedType_String:
            indexedColumns.emplace_back(
            WCDB::Column(WCDBGetCommonArrayLiteralValue(const char*, columns, i)));
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
    cppCreateIndex->syntax().indexedColumns = indexedColumns;
}

void WCDBStatementCreateIndexConfigWhere(CPPStatementCreateIndex createIndex, CPPExpression condition)
{
    WCDBGetObjectOrReturn(createIndex, WCDB::StatementCreateIndex, cppCreateIndex);
    WCDBGetObjectOrReturn(condition, WCDB::Expression, cppCondition);
    cppCreateIndex->where(*cppCondition);
}
