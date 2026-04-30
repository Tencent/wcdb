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

#include "WCDBRust.h"

#define WCDBRustExpressionFuncName(funcName) WCDBRust(Expression, funcName)
#define WCDBRustExpressionObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(Expression, funcName, __VA_ARGS__)
#define WCDBRustExpressionClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(Expression, funcName)
#define WCDBRustExpressionClassMethod(funcName, ...) \
    WCDBRustClassMethod(Expression, funcName, __VA_ARGS__)

void* WCDBRustExpressionClassMethod(create, int type, void* object);

void* WCDBRustExpressionClassMethod(createWithFunction, const char* func);
void* WCDBRustExpressionClassMethod(createWithExistStatement, void* select);
void* WCDBRustExpressionClassMethod(createWithNotExistStatement, void* select);

void WCDBRustExpressionClassMethod(setWithSchema,
                                   void* expression,
                                   WCDBRustObjectOrStringParameter(schema));
void WCDBRustExpressionClassMethod(argument,
                                   void* expression,
                                   WCDBRustCommonValueParameter(argument));

void WCDBRustExpressionClassMethod(invoke, void* expression);
void WCDBRustExpressionClassMethod(invokeAll, void* expression);

void WCDBRustExpressionClassMethod(distinct, void* expression);

void* WCDBRustExpressionClassMethod(cast, WCDBRustObjectOrStringParameter(expression));
void WCDBRustExpressionClassMethod(as, void* expression, int type);

void* WCDBRustExpressionClassMethod(configAlias, void* expression, const char* alias);

void* WCDBRustExpressionClassMethod(caseWithExp, WCDBRustObjectOrStringParameter(expression));
void WCDBRustExpressionClassMethod(setWithWhenExp,
                                   void* expression,
                                   WCDBRustCommonValueParameter(when));
void WCDBRustExpressionClassMethod(setWithThenExp,
                                   void* expression,
                                   WCDBRustCommonValueParameter(then));
void WCDBRustExpressionClassMethod(setWithElseExp,
                                   void* expression,
                                   WCDBRustCommonValueParameter(else_));

void WCDBRustExpressionClassMethod(escapeWith, void* expression, const char* content);

void* WCDBRustExpressionClassMethod(createWithWindowFunction, const char* func);
void WCDBRustExpressionClassMethod(filter, void* expression, void* condition);
void WCDBRustExpressionClassMethod(overWindowDef, void* expression, void* def);
void WCDBRustExpressionClassMethod(overWindow, void* expression, const char* window);
