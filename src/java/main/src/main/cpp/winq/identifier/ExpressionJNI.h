// Created by qiuwenchen on 2023/3/31.
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

#include "WCDBJNI.h"

#define WCDBJNIExpressionFuncName(funcName) WCDBJNI(Expression, funcName)
#define WCDBJNIExpressionObjectMethod(funcName, ...)                           \
    WCDBJNIObjectMethod(Expression, funcName, __VA_ARGS__)
#define WCDBJNIExpressionClassMethodWithNoArg(funcName)                        \
    WCDBJNIClassMethodWithNoArg(Expression, funcName)
#define WCDBJNIExpressionClassMethod(funcName, ...)                            \
    WCDBJNIClassMethod(Expression, funcName, __VA_ARGS__)

jlong WCDBJNIExpressionClassMethod(create, int type, jlong object);

jlong WCDBJNIExpressionClassMethod(createWithFunction, jstring func);
jlong WCDBJNIExpressionClassMethod(createWithExistStatement, jlong select);
jlong WCDBJNIExpressionClassMethod(createWithNotExistStatement, jlong select);

void WCDBJNIExpressionClassMethod(setWithSchema,
                                  jlong expression,
                                  WCDBJNIObjectOrStringParameter(schema));
void WCDBJNIExpressionClassMethod(setArgument,
                                  jlong expression,
                                  WCDBJNICommonValueParameter(argument));

void WCDBJNIExpressionClassMethod(invoke, jlong expression);
void WCDBJNIExpressionClassMethod(invokeAll, jlong expression);

void WCDBJNIExpressionClassMethod(distinct, jlong expression);

jlong WCDBJNIExpressionClassMethod(cast, WCDBJNIObjectOrStringParameter(expression));
void WCDBJNIExpressionClassMethod(as, jlong expression, jint type);

jlong WCDBJNIExpressionClassMethod(configAlias, jlong expression, jstring alias);

jlong WCDBJNIExpressionClassMethod(caseWithExp, WCDBJNIObjectOrStringParameter(expression));
void WCDBJNIExpressionClassMethod(setWithWhenExp,
                                  jlong expression,
                                  WCDBJNICommonValueParameter(when));
void WCDBJNIExpressionClassMethod(setWithThenExp,
                                  jlong expression,
                                  WCDBJNICommonValueParameter(then));
void WCDBJNIExpressionClassMethod(setWithElseExp,
                                  jlong expression,
                                  WCDBJNICommonValueParameter(else_));

void WCDBJNIExpressionClassMethod(escapeWith, jlong expression, jstring content);

jlong WCDBJNIExpressionClassMethod(createWithWindowFunction, jstring func);
void WCDBJNIExpressionClassMethod(filter, jlong expression, jlong condition);
void WCDBJNIExpressionClassMethod(overWindowDef, jlong expression, jlong def);
void WCDBJNIExpressionClassMethod(overWindow, jlong expression, jstring window);