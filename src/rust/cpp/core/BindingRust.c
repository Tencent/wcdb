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

#include "BindingRust.h"

#include "BindingBridge.h"

#include <printf.h>

void* WCDBRustBindingClassMethodWithNoArg(create) {
    return (void*)WCDBBindingCreate().innerValue;
}

void WCDBRustBindingClassMethod(addColumnDef, void* self, void* columnDef) {
    WCDBRustBridgeStruct(CPPBinding, self);
    WCDBRustBridgeStruct(CPPColumnDef, columnDef);
    WCDBBindingAddColumnDef(selfStruct, columnDefStruct);
}

void WCDBRustBindingClassMethod(enableAutoIncrementForExistingTable, void* self) {
    WCDBRustBridgeStruct(CPPBinding, self);
    WCDBBindingEnableAutoIncrementForExistingTable(selfStruct);
}

void WCDBRustBindingClassMethod(addIndex,
                                void* self,
                                const char* indexNameOrSuffix,
                                bool isFullName,
                                void* createIndex) {
    WCDBRustBridgeStruct(CPPBinding, self);
    WCDBRustBridgeStruct(CPPStatementCreateIndex, createIndex);
    WCDBBindingAddIndex(selfStruct, indexNameOrSuffix, isFullName, createIndexStruct);
}

void WCDBRustBindingClassMethod(addTableConstraint, void* self, void* constraint) {
    WCDBRustBridgeStruct(CPPBinding, self);
    WCDBRustBridgeStruct(CPPTableConstraint, constraint);
    WCDBBindingAddTableConstraint(selfStruct, constraintStruct);
}
//
// void WCDBRustBindingClassMethod(configVirtualModule, jlong self, jstring moduleName)
//{
//     WCDBRustBridgeStruct(CPPBinding, self);
//     WCDBRustGetStringCritical(moduleName);
//     WCDBBindingConfigVirtualModule(selfStruct, moduleNameString);
//     WCDBRustReleaseStringCritical(moduleName);
// }
//
// void WCDBRustBindingClassMethod(configVirtualModuleArgument, jlong self, jstring argument)
//{
//     WCDBRustBridgeStruct(CPPBinding, self);
//     WCDBRustGetStringCritical(argument);
//     WCDBBindingConfigVirtualModuleArgument(selfStruct, argumentString);
//     WCDBRustReleaseStringCritical(argument);
// }
//
// void WCDBRustBindingClassMethod(configWithoutRowId, jlong self)
//{
//     WCDBRustBridgeStruct(CPPBinding, self);
//     WCDBBindingConfigWithoutRowId(selfStruct);
// }

bool WCDBRustBinding_createTable(void* self, const char* tableName, void* handle) {
    WCDBRustBridgeStruct(CPPBinding, self);
    WCDBRustBridgeStruct(CPPHandle, handle);
    bool ret = WCDBBindingCreateTable(selfStruct, tableName, handleStruct);
    return ret;
}

// jboolean WCDBRustBindingClassMethod(createVirtualTable, jlong self, jstring tableName, jlong
// handle)
//{
//     WCDBRustBridgeStruct(CPPBinding, self);
//     WCDBRustBridgeStruct(CPPHandle, handle);
//     WCDBRustGetString(tableName);
//     jboolean ret = WCDBBindingCreateVirtualTable(selfStruct, tableNameString, handleStruct);
//     WCDBRustReleaseString(tableName);
//     return ret;
// }

void* WCDBRustBindingClassMethod(getBaseBinding, void* self) {
    WCDBRustBridgeStruct(CPPBinding, self);
    return (void*)WCDBBindingGetBaseBinding(selfStruct);
}
