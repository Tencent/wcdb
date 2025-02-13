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

void* WCDBRustExpressionClassMethod(create, int type, long long object);

void* WCDBRustExpressionClassMethod(createWithFunction, const char* func);
// jlong WCDBRustExpressionClassMethod(createWithExistStatement, jlong select);
// jlong WCDBRustExpressionClassMethod(createWithNotExistStatement, jlong select);
//
// void WCDBRustExpressionClassMethod(setWithSchema,
//                                   jlong expression,
//                                   WCDBRustObjectOrStringParameter(schema));
void WCDBRustExpressionClassMethod(argument,
                                   void* expression,
                                   WCDBRustCommonValueParameter(argument));
//
// void WCDBRustExpressionClassMethod(invoke, jlong expression);
// void WCDBRustExpressionClassMethod(invokeAll, jlong expression);
//
void WCDBRustExpressionClassMethod(distinct, void* expression);
//
// jlong WCDBRustExpressionClassMethod(cast, WCDBRustObjectOrStringParameter(expression));
// void WCDBRustExpressionClassMethod(as, jlong expression, jint type);
//
// jlong WCDBRustExpressionClassMethod(configAlias, jlong expression, jstring alias);
//
// jlong WCDBRustExpressionClassMethod(caseWithExp, WCDBRustObjectOrStringParameter(expression));
// void WCDBRustExpressionClassMethod(setWithWhenExp,
//                                  jlong expression,
//                                  WCDBRustCommonValueParameter(when));
// void WCDBRustExpressionClassMethod(setWithThenExp,
//                                  jlong expression,
//                                  WCDBRustCommonValueParameter(then));
// void WCDBRustExpressionClassMethod(setWithElseExp,
//                                  jlong expression,
//                                  WCDBRustCommonValueParameter(else_));
//
void WCDBRustExpressionClassMethod(escapeWith, void* expression, const char* content);
//
// jlong WCDBRustExpressionClassMethod(createWithWindowFunction, jstring func);
// void WCDBRustExpressionClassMethod(filter, jlong expression, jlong condition);
// void WCDBRustExpressionClassMethod(overWindowDef, jlong expression, jlong def);
// void WCDBRustExpressionClassMethod(overWindow, jlong expression, jstring window);
