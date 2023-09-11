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

#include "JoinBridge.h"
#include "Column.hpp"
#include "Expression.hpp"
#include "Join.hpp"
#include "ObjectBridge.hpp"
#include "TableOrSubquery.hpp"
#include "WinqBridge.hpp"

CPPJoin WCDBJoinCreateWithTableOrSubquery(CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturnValue(query, WCDB::TableOrSubquery, cppQuery, CPPJoin());
    CPPJoin ret = WCDBCreateCPPBridgedObject(CPPJoin, WCDB::Join);
    WCDBGetObjectOrReturnValue(ret, WCDB::Join, join, CPPJoin());
    join->table(*cppQuery);
    return ret;
}

CPPJoin WCDBJoinCreateWithTableOrSubquery2(CPPCommonValue query)
{
    CPPJoin ret = WCDBCreateCPPBridgedObject(CPPJoin, WCDB::Join);
    WCDBGetObjectOrReturnValue(ret, WCDB::Join, join, CPPJoin());
    join->table(WCDBCreateTableOrSubqueryFromCommonValue(query));
    return ret;
}

void WCDBJoinWith(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->with(*cppQuery);
}

void WCDBJoinWith2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->with(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->join(*cppQuery);
}

void WCDBJoinWithJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->join(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithLeftOuterJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->leftOuterJoin(*cppQuery);
}

void WCDBJoinWithLeftOuterJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->leftOuterJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithLeftJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->leftJoin(*cppQuery);
}

void WCDBJoinWithLeftJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->leftJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithInnerJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->innerJoin(*cppQuery);
}

void WCDBJoinWithInnerJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->innerJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithCrossJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->crossJoin(*cppQuery);
}

void WCDBJoinWithCrossJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->crossJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithNaturalJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->naturalJoin(*cppQuery);
}

void WCDBJoinWithNaturalJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->naturalJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithNaturalLeftOuterJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->naturalLeftOuterJoin(*cppQuery);
}

void WCDBJoinWithNaturalLeftOuterJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->naturalLeftOuterJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithNaturalLeftJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->naturalLeftJoin(*cppQuery);
}

void WCDBJoinWithNaturalLeftJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->naturalLeftJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithNaturalInnerJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->naturalInnerJoin(*cppQuery);
}

void WCDBJoinWithNaturalInnerJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->naturalInnerJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinWithNaturalCrossJoin(CPPJoin join, CPPTableOrSubquery query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(query, WCDB::TableOrSubquery, cppQuery);
    cppJoin->naturalCrossJoin(*cppQuery);
}

void WCDBJoinWithNaturalCrossJoin2(CPPJoin join, CPPCommonValue query)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    cppJoin->naturalCrossJoin(WCDBCreateTableOrSubqueryFromCommonValue(query));
}

void WCDBJoinConfigOn(CPPJoin join, CPPExpression expression)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppJoin->on(*cppExpression);
}

void WCDBJoinConfigUsingColumn(CPPJoin join, const CPPColumn* columns, int columnNum)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Column, cppColumns, columns, columnNum);
    cppJoin->usingColumns(cppColumns);
}

void WCDBJoinConfigUsingColumn2(CPPJoin join, CPPCommonArray columns)
{
    WCDBGetObjectOrReturn(join, WCDB::Join, cppJoin);
    WCDB::Columns cppColumns;
    for (int i = 0; i < columns.length; i++) {
        switch (columns.type) {
        case WCDBBridgedType_String:
            cppColumns.emplace_back(WCDB::UnsafeStringView(
            WCDBGetCommonArrayLiteralValue(const char*, columns, i)));
            break;
        case WCDBBridgedType_Column:
            cppColumns.push_back(WCDBGetCommonArrayObject(WCDB::Column, columns, i));
            break;
        default:
            assert(0);
            break;
        }
    }
    cppJoin->usingColumns(cppColumns);
}
