//
// Created by qiuwenchen on 2022/6/11.
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

#include "TableOrSubqueryBridge.h"
#include "Expression.hpp"
#include "Join.hpp"
#include "ObjectBridge.hpp"
#include "Schema.hpp"
#include "StatementSelect.hpp"
#include "TableOrSubquery.hpp"
#include "WinqBridge.hpp"

CPPTableOrSubquery WCDBTableOrSubqueryCreate(CPPCommonValue value)
{
    if (value.type == WCDBBridgedType_String) {
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPTableOrSubquery,
        WCDB::TableOrSubquery,
        WCDB::UnsafeStringView((const char*) value.intValue));
    } else if (value.type == WCDBBridgedType_TableOrSubquery) {
        return WCDBCreateCPPBridgedObjectByCopy(
        CPPTableOrSubquery, WCDBGetBridgedData(WCDB::TableOrSubquery, value));
    } else if (value.type == WCDBBridgedType_SelectSTMT) {
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPTableOrSubquery,
        WCDB::TableOrSubquery,
        WCDBGetBridgedData(WCDB::StatementSelect, value));
    } else if (value.type == WCDBBridgedType_JoinClause) {
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPTableOrSubquery, WCDB::TableOrSubquery, WCDBGetBridgedData(WCDB::Join, value));
    } else {
        assert(value.type == WCDBBridgedType_Null);
    }
    return CPPTableOrSubquery();
}

CPPTableOrSubquery WCDBTableOrSubqueryCreateWithTable(const char* _Nullable tableName)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPTableOrSubquery, WCDB::TableOrSubquery, WCDB::UnsafeStringView(tableName));
}

CPPTableOrSubquery WCDBTableOrSubqueryCreateWithFunction(const char* _Nullable functionName)
{
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPTableOrSubquery, WCDB::TableOrSubquery::function(functionName));
}

CPPTableOrSubquery WCDBTableOrSubqueryCreateWithJoin(CPPJoin join)
{
    WCDBGetObjectOrReturnValue(join, WCDB::Join, cppJoin, CPPTableOrSubquery());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPTableOrSubquery, WCDB::TableOrSubquery, *cppJoin);
}

CPPTableOrSubquery WCDBTableOrSubqueryCreateWithSelection(CPPStatementSelect select)
{
    WCDBGetObjectOrReturnValue(
    select, WCDB::StatementSelect, cppSelect, CPPTableOrSubquery());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPTableOrSubquery, WCDB::TableOrSubquery, *cppSelect);
}

CPPTableOrSubquery
WCDBTableOrSubqueryCreateWithTableOrSubqueries(const CPPTableOrSubquery* queries, int queryNum)
{
    WCDBGetCPPSyntaxList(WCDB::TableOrSubquery, cppQueries, queries, queryNum);
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPTableOrSubquery, WCDB::TableOrSubquery, cppQueries);
}

CPPTableOrSubquery
WCDBTableOrSubqueryCreateWithTableOrSubqueries2(CPPCommonArray tableOrSubqueries)
{
    WCDB::TablesOrSubqueries cppTableOrSubqueries;
    for (int i = 0; i < tableOrSubqueries.length; i++) {
        switch (tableOrSubqueries.type) {
        case WCDBBridgedType_String:
            cppTableOrSubqueries.emplace_back(WCDB::UnsafeStringView(
            WCDBGetCommonArrayLiteralValue(const char*, tableOrSubqueries, i)));
            break;
        case WCDBBridgedType_SelectSTMT:
            cppTableOrSubqueries.emplace_back(WCDBGetCommonArrayObject(
            WCDB::StatementSelect, tableOrSubqueries, i));
            break;
        case WCDBBridgedType_JoinClause:
            cppTableOrSubqueries.emplace_back(
            WCDBGetCommonArrayObject(WCDB::Join, tableOrSubqueries, i));
            break;
        default:
            assert(0);
            break;
        }
    }
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPTableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubqueries);
}

void WCDBTableOrSubqueryConfigSchema2(CPPTableOrSubquery tableOrSubquery, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(tableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubquery);
    cppTableOrSubquery->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBTableOrSubqueryConfigSchema(CPPTableOrSubquery tableOrSubquery, CPPSchema schema)
{
    WCDBGetObjectOrReturn(tableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubquery);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppTableOrSubquery->schema(*cppSchema);
}

void WCDBTableOrSubqueryConfigAs(CPPTableOrSubquery tableOrSubquery, const char* _Nullable aliasName)
{
    WCDBGetObjectOrReturn(tableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubquery);
    cppTableOrSubquery->as(aliasName);
}

void WCDBTableOrSubqueryConfigNotIndexed(CPPTableOrSubquery tableOrSubquery)
{
    WCDBGetObjectOrReturn(tableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubquery);
    cppTableOrSubquery->notIndexed();
}

void WCDBTableOrSubqueryConfigIndexName(CPPTableOrSubquery tableOrSubquery,
                                        const char* _Nullable indexName)
{
    WCDBGetObjectOrReturn(tableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubquery);
    cppTableOrSubquery->indexed(indexName);
}

void WCDBTableOrSubqueryConfigArguments(CPPTableOrSubquery tableOrSubquery,
                                        const CPPExpression* args,
                                        int argNum)
{
    WCDBGetObjectOrReturn(tableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubquery);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Expression, cppArgs, args, argNum);
    cppTableOrSubquery->arguments(cppArgs);
}

void WCDBTableOrSubqueryConfigArgument(CPPTableOrSubquery tableOrSubquery, CPPCommonValue expression)
{
    WCDBGetObjectOrReturn(tableOrSubquery, WCDB::TableOrSubquery, cppTableOrSubquery);
    cppTableOrSubquery->argument(WCDBCreateExpressionFromCommonValue(expression));
}
