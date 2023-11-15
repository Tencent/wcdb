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

#pragma once

#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

CPPExpression WCDBExpressionCreateWithBool(bool value);
CPPExpression WCDBExpressionCreateWithLong(long long value);
CPPExpression WCDBExpressionCreateWithDouble(double value);
CPPExpression WCDBExpressionCreateWithString(const char* _Nullable value);
CPPExpression WCDBExpressionCreateWithLiteralValue(CPPLiteralValue value);
CPPExpression WCDBExpressionCreateWithBindParameter(CPPBindParameter value);
CPPExpression WCDBExpressionCreateWithColumn(CPPColumn column);
CPPExpression WCDBExpressionCreateWithSelection(CPPStatementSelect select);
CPPExpression WCDBExpressionCreateWithRaiseFunction(CPPRaiseFunction function);
CPPExpression WCDBExpressionCreateWithFunction(const char* _Nullable funcName);
CPPExpression WCDBExpressionCreateWithExistStatement(CPPStatementSelect select);
CPPExpression WCDBExpressionCreateWithNotExistStatement(CPPStatementSelect select);

CPPExpression WCDBExpressionCreate(CPPCommonValue value);

void WCDBExpressionSetWithSchema(CPPExpression expression, CPPSchema schema);
void WCDBExpressionSetWithSchema2(CPPExpression expression, CPPCommonValue schema);
void WCDBExpressionSetArgument(CPPExpression expression, CPPCommonValue argument);
void WCDBExpressionSetArguments(CPPExpression expression,
                                const CPPExpression* _Nullable arguments,
                                int num);
void WCDBExpressionInvoke(CPPExpression expression);
void WCDBExpressionInvokeAll(CPPExpression expression);

void WCDBExpressionDistinct(CPPExpression expression);

CPPExpression WCDBExpressionCast(CPPExpression expression);
CPPExpression WCDBExpressionCast2(CPPCommonValue expression);
void WCDBExpressionAs(CPPExpression expression, enum WCDBSyntaxColumnType type);

CPPResultColumn
WCDBExpressionConfigAlias(CPPExpression expression, const char* _Nullable alias);

CPPExpression WCDBExpressionCaseWithExp(CPPExpression expression);
CPPExpression WCDBExpressionCaseWithExp2(CPPCommonValue expression);
CPPExpression WCDBExpressionCase(void);

void WCDBExpressionSetWithWhenExp(CPPExpression expression, CPPExpression when);
void WCDBExpressionSetWithWhenExp2(CPPExpression expression, CPPCommonValue when);
void WCDBExpressionSetWithThenExp(CPPExpression expression, CPPExpression then);
void WCDBExpressionSetWithThenExp2(CPPExpression expression, CPPCommonValue then);
void WCDBExpressionSetWithElseExp(CPPExpression expression, CPPExpression else_);
void WCDBExpressionSetWithElseExp2(CPPExpression expression, CPPCommonValue else_);

void WCDBExpressionEscapeWith(CPPExpression expression, CPPExpression operand);
void WCDBExpressionEscapeWith2(CPPExpression expression, const char* _Nullable operand);

CPPExpression WCDBExpressionCreateWithWindowFunction(const char* _Nullable funcName);
void WCDBExpressionFilter(CPPExpression expression, CPPExpression condition);
void WCDBExpressionOverWindowDef(CPPExpression expression, CPPWindowDef def);
void WCDBExpressionOverWindow(CPPExpression expression, const char* _Nullable window);

WCDB_EXTERN_C_END
