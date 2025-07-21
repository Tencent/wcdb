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

#define WCDBRustWindowDefFuncName(funcName) WCDBRust(WindowDef, funcName)
#define WCDBRustWindowDefObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(WindowDef, funcName, __VA_ARGS__)
#define WCDBRustWindowDefClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(WindowDef, funcName)
#define WCDBRustWindowDefClassMethod(funcName, ...) \
    WCDBRustClassMethod(WindowDef, funcName, __VA_ARGS__)

void* WCDBRustWindowDefClassMethodWithNoArg(createCppObj);
void WCDBRustWindowDefClassMethod(configPartitions,
                                  void* self,
                                  WCDBRustMultiTypeArrayParameter(partitions));
void WCDBRustWindowDefClassMethod(configOrders, void* self, long* ordersArray, int ordersLength);
void WCDBRustWindowDefClassMethod(configFrameSpec, void* self, void* frameSpec);
