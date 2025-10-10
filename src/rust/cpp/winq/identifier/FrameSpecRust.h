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

#define WCDBRustFrameSpecFuncName(funcName) WCDBRust(FrameSpec, funcName)
#define WCDBRustFrameSpecObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(FrameSpec, funcName, __VA_ARGS__)
#define WCDBRustFrameSpecClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(FrameSpec, funcName)
#define WCDBRustFrameSpecClassMethod(funcName, ...) \
    WCDBRustClassMethod(FrameSpec, funcName, __VA_ARGS__)

void* WCDBRustFrameSpecClassMethodWithNoArg(createCppObj);
void WCDBRustFrameSpecClassMethod(configRange, void* self);
// void WCDBRustFrameSpecClassMethod(configRows, jlong self);
void WCDBRustFrameSpecClassMethod(configUnboundedPreceding, void* self);
// void WCDBRustFrameSpecClassMethod(configPreceding,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression));
// void WCDBRustFrameSpecClassMethod(configCurrentRow, jlong self);
// void WCDBRustFrameSpecClassMethod(configBetweenUnboundedPreceding, jlong self);
// void WCDBRustFrameSpecClassMethod(configBetweenPreceding,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression));
// void WCDBRustFrameSpecClassMethod(configBetweenCurrentRow, jlong self);
// void WCDBRustFrameSpecClassMethod(configBetweenFollowing,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression));
// void WCDBRustFrameSpecClassMethod(configAndPreceding,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression));
// void WCDBRustFrameSpecClassMethod(configAndCurrentRow, jlong self);
// void WCDBRustFrameSpecClassMethod(configAndFollowing,
//                                  jlong self,
//                                  WCDBRustObjectOrIntegerParameter(expression));
// void WCDBRustFrameSpecClassMethod(configAndUnboundedFollowing, jlong self);
