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

#define WCDBRustBindingFuncName(funcName) WCDBRust(Binding, funcName)
#define WCDBRustBindingObjectMethod(funcName, ...)                              \
    WCDBRustObjectMethod(Binding, funcName, __VA_ARGS__)
#define WCDBRustBindingObjectMethodWithNoArg(funcName)                          \
    WCDBRustObjectMethodWithNoArg(Binding, funcName)
#define WCDBRustBindingClassMethodWithNoArg(funcName)                           \
    WCDBRustClassMethodWithNoArg(Binding, funcName)
#define WCDBRustBindingClassMethod(funcName, ...)                               \
    WCDBRustClassMethod(Binding, funcName, __VA_ARGS__)

void* WCDBRustBindingClassMethodWithNoArg(create);
void WCDBRustBindingClassMethod(addColumnDef, void* self, void* columnDef);
//void WCDBRustBindingClassMethod(enableAutoIncrementForExistingTable, jlong self);
//void WCDBRustBindingClassMethod(
//addIndex, jlong self, jstring indexNameOrSuffix, jboolean isFullName, jlong createIndex);
//void WCDBRustBindingClassMethod(addTableConstraint, jlong self, jlong constraint);
//void WCDBRustBindingClassMethod(configVirtualModule, jlong self, jstring moduleName);
//void WCDBRustBindingClassMethod(configVirtualModuleArgument, jlong self, jstring argument);
//void WCDBRustBindingClassMethod(configWithoutRowId, jlong self);
bool WCDBRustBindingClassMethod(createTable, void* self, const char* tableName, void* handle);
//jboolean
//WCDBRustBindingClassMethod(createVirtualTable, jlong self, jstring tableName, jlong handle);
void* WCDBRustBindingClassMethod(getBaseBinding, void* self);
