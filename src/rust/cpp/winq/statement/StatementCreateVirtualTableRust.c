// Created by chenqiuwen on 2023/11/4.
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

#include "StatementCreateVirtualTableRust.h"

#include "StatementCreateVirtualTableBridge.h"

void* WCDBRustStatementCreateVirtualTableClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBStatementCreateVirtualTableCreate().innerValue;
}

void WCDBRustStatementCreateVirtualTableClassMethod(configTable, void* self, const char* name) {
    // WCDBRustGetString(name);
    WCDBRustBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBStatementCreateVirtualTableConfigTable(selfStruct, name);
    // WCDBRustReleaseString(name);
}

void WCDBRustStatementCreateVirtualTableClassMethod(configSchema,
                                                    void* self,
                                                    WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBStatementCreateVirtualTableConfigSchema2(selfStruct, schema_common);
    // WCDBRustTryReleaseStringInCommonValue(schema);
}

void WCDBRustStatementCreateVirtualTableClassMethod(configIfNotExist, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBStatementCreateVirtualTableConfigIfNotExist(selfStruct);
}

void WCDBRustStatementCreateVirtualTableClassMethod(configModule, void* self, const char* module) {
    WCDBRustBridgeStruct(CPPStatementCreateVirtualTable, self);
    // WCDBRustGetString(module);
    WCDBStatementCreateVirtualTableConfigModule(selfStruct, module);
    // WCDBRustReleaseString(module);
}

void WCDBRustStatementCreateVirtualTableClassMethod(configArguments,
                                                    void* self,
                                                    const char** arguments,
                                                    int argumentsLength) {
    WCDBRustBridgeStruct(CPPStatementCreateVirtualTable, self);
    // WCDBRustGetStringArray(arguments);
    WCDBStatementCreateVirtualTableConfigArguments(selfStruct, (const char* const*)arguments,
                                                   argumentsLength);
    // WCDBRustReleaseStringArray(arguments);
}
