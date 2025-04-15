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

#define WCDBRustExpressionOperableFuncName(funcName) WCDBRust(ExpressionOperable, funcName)
#define WCDBRustExpressionOperableObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(ExpressionOperable, funcName, __VA_ARGS__)
#define WCDBRustExpressionOperableClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(ExpressionOperable, funcName)
#define WCDBRustExpressionOperableClassMethod(funcName, ...) \
    WCDBRustClassMethod(ExpressionOperable, funcName, __VA_ARGS__)

void* WCDBRustExpressionOperableClassMethod(nullOperate,
                                            int operandType,
                                            void* operand,
                                            bool isNot);

void* WCDBRustExpressionOperableClassMethod(binaryOperate,
                                            int leftType,
                                            long left,
                                            WCDBRustCommonValueParameter(right),
                                            int operatorType,
                                            bool isNot);

void* WCDBRustExpressionOperableClassMethod(betweenOperate,
                                            int operandType,
                                            long operand,
                                            WCDBRustCommonValueParameter(left),
                                            WCDBRustCommonValueParameter(right),
                                            bool isNot);

void* WCDBRustExpressionOperableClassMethod(inOperate,
                                            int operandType,
                                            long operand,
                                            WCDBRustCommonArrayParameter(values),
                                            bool isNot);

void* WCDBRustExpressionOperableClassMethod(inTableOperate,
                                            int operandType,
                                            void* operand,
                                            const char* table,
                                            bool isNot);
//
// jlong WCDBRustExpressionOperableClassMethod(
// inFunctionOperate, jint operandType, jlong operand, jstring func, jboolean isNot);
//
// jlong WCDBRustExpressionOperableClassMethod(
// inSelectionOperate, jint operandType, jlong operand, jlong select, jboolean isNot);
//

void* WCDBRustExpressionOperableClassMethod(collateOperate,
                                            int operandType,
                                            long operand,
                                            const char* collation);
