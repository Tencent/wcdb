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
#include "StatementSelect.hpp"
#include "SyntaxForwardDeclaration.h"
#include "WindowDef.hpp"
#include "WinqBridge.hpp"

CPPExpression WCDBExpressionCreateWithBool(bool value)
{
    return WCDBCreateCPPBridgedObject(
    CPPExpression, new WCDB::Expression(WCDB::LiteralValue(value)));
}

CPPExpression WCDBExpressionCreateWithLong(long long value)
{
    return WCDBCreateCPPBridgedObject(
    CPPExpression, new WCDB::Expression(WCDB::LiteralValue((int64_t) value)));
}

CPPExpression WCDBExpressionCreateWithDouble(double value)
{
    return WCDBCreateCPPBridgedObject(
    CPPExpression, new WCDB::Expression(WCDB::LiteralValue(value)));
}

CPPExpression WCDBExpressionCreateWithString(const char* _Nullable value)
{
    return WCDBCreateCPPBridgedObject(
    CPPExpression, new WCDB::Expression(WCDB::LiteralValue(value)));
}

CPPExpression WCDBExpressionCreateWithLiteralValue(CPPLiteralValue value)
{
    WCDBGetObjectOrReturnValue(value, WCDB::LiteralValue, cppValue, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, new WCDB::Expression(*cppValue));
}

CPPExpression WCDBExpressionCreateWithBindParameter(CPPBindParameter value)
{
    WCDBGetObjectOrReturnValue(value, WCDB::BindParameter, cppValue, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, new WCDB::Expression(*cppValue));
}

CPPExpression WCDBExpressionCreateWithColumn(CPPColumn column)
{
    WCDBGetObjectOrReturnValue(column, WCDB::Column, cppColumn, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, new WCDB::Expression(*cppColumn));
}

CPPExpression WCDBExpressionCreateWithSelection(CPPStatementSelect select)
{
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, new WCDB::Expression(*cppSelect));
}

CPPExpression WCDBExpressionCreateWithRaiseFunction(CPPRaiseFunction function)
{
    WCDBGetObjectOrReturnValue(function, WCDB::RaiseFunction, cppFunction, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, new WCDB::Expression(*cppFunction));
}

CPPExpression WCDBExpressionCreateWithFunction(const char* _Nullable funcName)
{
    return WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression::function(funcName));
}

CPPExpression WCDBExpressionCreateWithExistStatement(CPPStatementSelect select)
{
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression::exists(*cppSelect));
}

CPPExpression WCDBExpressionCreateWithNotExistStatement(CPPStatementSelect select)
{
    WCDBGetObjectOrReturnValue(select, WCDB::StatementSelect, cppSelect, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression::notExists(*cppSelect));
}

void WCDBExpressionSetWithSchema(CPPExpression expression, CPPSchema schema)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(schema, WCDB::Schema, cppSchema);
    cppExpression->schema(*cppSchema);
}

void WCDBExpressionSetArgument(CPPExpression expression, CPPExpression argument)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(argument, WCDB::Expression, cppArgument);
    cppExpression->argument(*cppArgument);
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
    return WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression::cast(*cppExpression));
}

void WCDBExpressionAs(CPPExpression expression, enum WCDBSyntaxColumnType type)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    cppExpression->as(WCDB::WinqBridge::changeColumnType(type));
}

CPPExpression WCDBExpressionCaseWithExp(CPPExpression expression)
{
    WCDBGetObjectOrReturnValue(expression, WCDB::Expression, cppExpression, CPPExpression());
    return WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression::case_(*cppExpression));
}

CPPExpression WCDBExpressionCase()
{
    return WCDBCreateCPPBridgedObject(CPPExpression, WCDB::Expression::case_());
}

void WCDBExpressionSetWithWhenExp(CPPExpression expression, CPPExpression when)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(when, WCDB::Expression, cppWhen);
    cppExpression->when(*cppWhen);
}

void WCDBExpressionSetWithThenExp(CPPExpression expression, CPPExpression then)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(then, WCDB::Expression, cppThen);
    cppExpression->then(*cppThen);
}

void WCDBExpressionSetWithElseExp(CPPExpression expression, CPPExpression else_)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(else_, WCDB::Expression, cppElse);
    cppExpression->else_(*cppElse);
}

void WCDBExpressionEscapeWith(CPPExpression expression, CPPExpression operand)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(operand, WCDB::Expression, cppOperand);
    cppExpression->escape(*cppOperand);
}

CPPExpression WCDBExpressionCreateWithWindowFunction(const char* _Nullable funcName)
{
    return WCDBCreateCPPBridgedObject(
    CPPExpression, WCDB::Expression::windowFunction(funcName));
}

void WCDBExpressionFilter(CPPExpression expression, CPPFilter filter)
{
    WCDBGetObjectOrReturn(expression, WCDB::Expression, cppExpression);
    WCDBGetObjectOrReturn(filter, WCDB::Filter, cppFilter);
    cppExpression->filter(*cppFilter);
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
