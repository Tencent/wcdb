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
void WCDBRustFrameSpecClassMethod(configRows, void* self);
void WCDBRustFrameSpecClassMethod(configUnboundedPreceding, void* self);
void WCDBRustFrameSpecClassMethod(configPreceding,
                                  void* self,
                                  WCDBRustObjectOrIntegerParameter(expression));
void WCDBRustFrameSpecClassMethod(configCurrentRow, void* self);
void WCDBRustFrameSpecClassMethod(configBetweenUnboundedPreceding, void* self);
void WCDBRustFrameSpecClassMethod(configBetweenPreceding,
                                  void* self,
                                  WCDBRustObjectOrIntegerParameter(expression));
void WCDBRustFrameSpecClassMethod(configBetweenCurrentRow, void* self);
void WCDBRustFrameSpecClassMethod(configBetweenFollowing,
                                  void* self,
                                  WCDBRustObjectOrIntegerParameter(expression));
void WCDBRustFrameSpecClassMethod(configAndPreceding,
                                  void* self,
                                  WCDBRustObjectOrIntegerParameter(expression));
void WCDBRustFrameSpecClassMethod(configAndCurrentRow, void* self);
void WCDBRustFrameSpecClassMethod(configAndFollowing,
                                  void* self,
                                  WCDBRustObjectOrIntegerParameter(expression));
void WCDBRustFrameSpecClassMethod(configAndUnboundedFollowing, void* self);
