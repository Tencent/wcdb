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

#include "WCDBRust.h"

#define WCDBRustExpressionOperableFuncName(funcName)                            \
    WCDBRust(ExpressionOperable, funcName)
#define WCDBRustExpressionOperableObjectMethod(funcName, ...)                   \
    WCDBRustObjectMethod(ExpressionOperable, funcName, __VA_ARGS__)
#define WCDBRustExpressionOperableClassMethodWithNoArg(funcName)                \
    WCDBRustClassMethodWithNoArg(ExpressionOperable, funcName)
#define WCDBRustExpressionOperableClassMethod(funcName, ...)                    \
    WCDBRustClassMethod(ExpressionOperable, funcName, __VA_ARGS__)

//jlong WCDBRustExpressionOperableClassMethod(nullOperate, jint operandType, jlong operand, jboolean isNot);
//
void *WCDBRustExpressionOperableClassMethod(binaryOperate,
                                            int leftType,
                                            long left,
                                            WCDBRustCommonValueParameter(right),
                                            int operatorType,
                                            bool isNot);
//
//jlong WCDBRustExpressionOperableClassMethod(betweenOperate,
//                                           jint operandType,
//                                           jlong operand,
//                                           WCDBRustCommonValueParameter(left),
//                                           WCDBRustCommonValueParameter(right),
//                                           jboolean isNot);
//
//jlong WCDBRustExpressionOperableClassMethod(inOperate,
//                                           jint operandType,
//                                           jlong operand,
//                                           WCDBRustCommonArrayParameter(values),
//                                           jboolean isNot);
//
//jlong WCDBRustExpressionOperableClassMethod(
//inTableOperate, jint operandType, jlong operand, jstring table, jboolean isNot);
//
//jlong WCDBRustExpressionOperableClassMethod(
//inFunctionOperate, jint operandType, jlong operand, jstring func, jboolean isNot);
//
//jlong WCDBRustExpressionOperableClassMethod(
//inSelectionOperate, jint operandType, jlong operand, jlong select, jboolean isNot);
//
//jlong WCDBRustExpressionOperableClassMethod(collateOperate, jint operandType, jlong operand, jstring collation);