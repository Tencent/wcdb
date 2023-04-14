// Created by chenqiuwen on 2023/4/1.
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

#define WCDBJNIExpressionOperableFuncName(funcName)                            \
    WCDBJNI(ExpressionOperable, funcName)
#define WCDBJNIExpressionOperableObjectMethod(funcName, ...)                   \
    WCDBJNIObjectMethod(ExpressionOperable, funcName, __VA_ARGS__)
#define WCDBJNIExpressionOperableClassMethodWithNoArg(funcName)                \
    WCDBJNIClassMethodWithNoArg(ExpressionOperable, funcName)
#define WCDBJNIExpressionOperableClassMethod(funcName, ...)                    \
    WCDBJNIClassMethod(ExpressionOperable, funcName, __VA_ARGS__)

jlong WCDBJNIExpressionOperableObjectMethod(nullOperate, jint operandType, jlong operand, jboolean isNot);

jlong WCDBJNIExpressionOperableObjectMethod(binaryOperate,
                                            jint leftType,
                                            jlong left,
                                            WCDBJNICommonValueParameter(right),
                                            jint operatorType,
                                            jboolean isNot);

jlong WCDBJNIExpressionOperableObjectMethod(betweenOperate,
                                            jint operandType,
                                            jlong operand,
                                            WCDBJNICommonValueParameter(left),
                                            WCDBJNICommonValueParameter(right),
                                            jboolean isNot);

jlong WCDBJNIExpressionOperableObjectMethod(inOperate,
                                            jint operandType,
                                            jlong operand,
                                            WCDBJNICommonArrayParameter(values),
                                            jboolean isNot);

jlong WCDBJNIExpressionOperableObjectMethod(
inTableOperate, jint operandType, jlong operand, jstring table, jboolean isNot);

jlong WCDBJNIExpressionOperableObjectMethod(
inFunctionOperate, jint operandType, jlong operand, jstring func, jboolean isNot);

jlong WCDBJNIExpressionOperableObjectMethod(
inSelectionOperate, jint operandType, jlong operand, jlong select, jboolean isNot);

jlong WCDBJNIExpressionOperableObjectMethod(collateOperate,
                                            jint operandType,
                                            jlong operand,
                                            jstring collation);