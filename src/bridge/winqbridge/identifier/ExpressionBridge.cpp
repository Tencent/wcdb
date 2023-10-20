//
// Created by qiuwenchen on 2022/5/26.
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

#include "ExpressionBridge.h"
#include "BindParameter.hpp"
#include "Column.hpp"
#include "Expression.hpp"
#include "Filter.hpp"
#include "LiteralValue.hpp"
#include "ObjectBridge.hpp"
#include "RaiseFunction.hpp"
#include "ResultColumn.hpp"
#include "Schema.hpp"
#include "StatementSelect.hpp"
#include "SyntaxForwardDeclaration.h"
#include "WindowDef.hpp"
#include "WinqBridge.hpp"

CPPExpression WCDBExpressionCreateWithBool(bool value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, WCDB::LiteralValue(value));
}

CPPExpression WCDBExpressionCreateWithLong(long long value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, WCDB::LiteralValue((int64_t) value));
}

CPPExpression WCDBExpressionCreateWithDouble(double value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, WCDB::LiteralValue(value));
}

CPPExpression WCDBExpressionCreateWithString(const char* _Nullable value)
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, WCDB::LiteralValue(value));
}

CPPExpression WCDBExpressionCreateWithLiteralValue(CPPLiteralValue value)
{
    WCDBGetObjectOrReturnValue(value, WCDB::LiteralValue, cppValue, CPPExpression());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, *cppValue);
}

CPPExpression WCDBExpressionCreateWithBindParameter(CPPBindParameter value)
{
    WCDBGetObjectOrReturnValue(value, WCDB::BindParameter, cppValue, CPPExpression());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, *cppValue);
}

CPPExpression WCDBExpressionCreateWithColumn(CPPColumn column)
{
    WCDBGetObjectOrReturnValue(column, WCDB::Column, cppColumn, CPPExpression());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, *cppColumn);
}

CPPExpression WCDBExpressionCreateWithSelection(CPPStatementSelect select)
{
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, *cppSelect);
}

CPPExpression WCDBExpressionCreateWithRaiseFunction(CPPRaiseFunction function)
{
    WCDBGetObjectOrReturnValue(function, WCDB::RaiseFunction, cppFunction, CPPExpression());
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPExpression, WCDB::Expression, *cppFunction);
}

CPPExpression WCDBExpressionCreateWithFunction(const char* _Nullable funcName)
{
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPExpression, WCDB::Expression::function(funcName));
}

CPPExpression WCDBExpressionCreate(CPPCommonValue value)
{
    switch (value.type) {
    case WCDBBridgedType_Null:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDB::LiteralValue(nullptr));
    case WCDBBridgedType_Bool:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDB::LiteralValue((bool) value.intValue));
    case WCDBBridgedType_Int:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDB::LiteralValue((int64_t) value.intValue));
    case WCDBBridgedType_UInt:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDB::LiteralValue((uint64_t) value.intValue));
    case WCDBBridgedType_Double:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDB::LiteralValue(value.doubleValue));
    case WCDBBridgedType_String:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDB::LiteralValue((const char*) value.intValue));
    case WCDBBridgedType_Column:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDBGetBridgedData(WCDB::Column, value));
    case WCDBBridgedType_LiteralValue:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDBGetBridgedData(WCDB::LiteralValue, value));
    case WCDBBridgedType_BindParameter:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDBGetBridgedData(WCDB::BindParameter, value));
    case WCDBBridgedType_SelectSTMT:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDBGetBridgedData(WCDB::StatementSelect, value));
    case WCDBBridgedType_RaiseFunction:
        return WCDBCreateCPPBridgedObjectWithParameters(
        CPPExpression, WCDB::Expression, WCDBGetBridgedData(WCDB::RaiseFunction, value));
    default:
        assert(0);
        break;
    }
    return CPPExpression();
}

CPPExpression WCDBExpressionCreateWithExistStatement(CPPStatementSelect select)
{
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPExpression, WCDB::Expression::exists(*cppSelect));
}

CPPExpression WCDBExpressionCreateWithNotExistStatement(CPPStatementSelect select)
{
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPExpression, WCDB::Expression::notExists(*cppSelect));
}

void WCDBExpressionSetWithSchema(CPPExpression expression, CPPSchema schema)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppExpression->schema(*cppSchema);
}

void WCDBExpressionSetWithSchema2(CPPExpression expression, CPPCommonValue schema)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->schema(WCDBCreateSchemaFromCommonValue(schema));
}

void WCDBExpressionSetArgument(CPPExpression expression, CPPCommonValue argument)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->argument(WCDBCreateExpressionFromCommonValue(argument));
}

void WCDBExpressionSetArguments(CPPExpression expression, const CPPExpression* arguments, int num)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetCPPSyntaxListOrReturn(WCDB::Expression, cppArgs, arguments, num);
    cppExpression->arguments(cppArgs);
}

void WCDBExpressionInvoke(CPPExpression expression)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->invoke();
}

void WCDBExpressionInvokeAll(CPPExpression expression)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->invokeAll();
}

void WCDBExpressionDistinct(CPPExpression expression)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->distinct();
}

CPPExpression WCDBExpressionCast(CPPExpression expression)
{
    WCDBGetObjectOrReturnValue(expression, WCDB::Expression, cppExpression, CPPExpression());
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPExpression, WCDB::Expression::cast(*cppExpression));
}

CPPExpression WCDBExpressionCast2(CPPCommonValue expression)
{
    if (expression.type == WCDBBridgedType_String) {
        return WCDBCreateCPPBridgedObjectByCopy(
        CPPExpression,
        WCDB::Expression::cast(
        WCDB::Column(WCDB::StringView((const char*) expression.intValue))));
    } else {
        return WCDBCreateCPPBridgedObjectByCopy(
        CPPExpression,
        WCDB::Expression::cast(WCDBCreateExpressionFromCommonValue(expression)));
    }
}

void WCDBExpressionAs(CPPExpression expression, enum WCDBSyntaxColumnType type)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->as(WCDB::WinqBridge::changeColumnType(type));
}

CPPResultColumn
WCDBExpressionConfigAlias(CPPExpression expression, const char* _Nullable alias)
{
    WCDBGetObjectOrReturnValue(
    expression, WCDB::Expression, cppExpression, CPPResultColumn());
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPResultColumn, cppExpression->as(WCDB::UnsafeStringView(alias)));
}

CPPExpression WCDBExpressionCaseWithExp(CPPExpression expression)
{
    WCDBGetObjectOrReturnValue(expression, WCDB::Expression, cppExpression, CPPExpression());
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPExpression, WCDB::Expression::case_(*cppExpression));
}

CPPExpression WCDBExpressionCaseWithExp2(CPPCommonValue expression)
{
    if (expression.type == WCDBBridgedType_String) {
        return WCDBCreateCPPBridgedObjectByCopy(
        CPPExpression,
        WCDB::Expression::case_(
        WCDB::Column(WCDB::StringView((const char*) expression.intValue))));
    } else {
        return WCDBCreateCPPBridgedObjectByCopy(
        CPPExpression,
        WCDB::Expression::case_(WCDBCreateExpressionFromCommonValue(expression)));
    }
}

CPPExpression WCDBExpressionCase()
{
    return WCDBCreateCPPBridgedObjectByCopy(CPPExpression, WCDB::Expression::case_());
}

void WCDBExpressionSetWithWhenExp(CPPExpression expression, CPPExpression when)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(when, WCDB::Expression, cppWhen);
    cppExpression->when(*cppWhen);
}

void WCDBExpressionSetWithWhenExp2(CPPExpression expression, CPPCommonValue when)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->when(WCDBCreateExpressionFromCommonValue(when));
}

void WCDBExpressionSetWithThenExp(CPPExpression expression, CPPExpression then)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(then, WCDB::Expression, cppThen);
    cppExpression->then(*cppThen);
}

void WCDBExpressionSetWithThenExp2(CPPExpression expression, CPPCommonValue then)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->then(WCDBCreateExpressionFromCommonValue(then));
}

void WCDBExpressionSetWithElseExp(CPPExpression expression, CPPExpression else_)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(else_, WCDB::Expression, cppElse);
    cppExpression->else_(*cppElse);
}

void WCDBExpressionSetWithElseExp2(CPPExpression expression, CPPCommonValue else_)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->else_(WCDBCreateExpressionFromCommonValue(else_));
}

void WCDBExpressionEscapeWith(CPPExpression expression, CPPExpression operand)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(operand, WCDB::Expression, cppOperand);
    cppExpression->escape(*cppOperand);
}

void WCDBExpressionEscapeWith2(CPPExpression expression, const char* operand)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->escape(WCDB::LiteralValue(WCDB::UnsafeStringView(operand)));
}

CPPExpression WCDBExpressionCreateWithWindowFunction(const char* _Nullable funcName)
{
    return WCDBCreateCPPBridgedObjectByCopy(
    CPPExpression, WCDB::Expression::windowFunction(funcName));
}

void WCDBExpressionFilter(CPPExpression expression, CPPExpression condition)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(condition, WCDB::Expression, cppCondition);
    cppExpression->filter(*cppCondition);
}

void WCDBExpressionOverWindowDef(CPPExpression expression, CPPWindowDef def)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(def, WCDB::WindowDef, cppDef);
    cppExpression->over(*cppDef);
}
void WCDBExpressionOverWindow(CPPExpression expression, const char* _Nullable window)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->over(window);
}
