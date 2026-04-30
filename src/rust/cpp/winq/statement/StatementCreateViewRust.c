// Created by chenqiuwen on 2023/6/11.
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

#include "StatementCreateViewRust.h"

#include "StatementCreateViewBridge.h"

void* WCDBRustStatementCreateViewClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBStatementCreateViewCreate().innerValue;
}

void WCDBRustStatementCreateViewClassMethod(configView, void* self, const char* name) {
    WCDBRustBridgeStruct(CPPStatementCreateView, self);
    // WCDBRustGetStringCritical(name);
    WCDBStatementCreateViewConfigView(selfStruct, name);
    // WCDBRustReleaseStringCritical(name);
}

void WCDBRustStatementCreateViewClassMethod(configSchema,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPStatementCreateView, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBStatementCreateViewConfigSchema2(selfStruct, schema_common);
    // WCDBRustTryReleaseStringInCommonValue(schema);
}

void WCDBRustStatementCreateViewClassMethod(configTemp, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateView, self);
    WCDBStatementCreateViewConfigTemp(selfStruct);
}

void WCDBRustStatementCreateViewClassMethod(configIfNotExist, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateView, self);
    WCDBStatementCreateViewConfigIfNotExist(selfStruct);
}

void WCDBRustStatementCreateViewClassMethod(configAs, void* self, void* select) {
    WCDBRustBridgeStruct(CPPStatementCreateView, self);
    WCDBRustBridgeStruct(CPPStatementSelect, select);
    WCDBStatementCreateViewConfigAs(selfStruct, selectStruct);
}

void WCDBRustStatementCreateViewClassMethod(configColumns,
                                            void* self,
                                            WCDBRustObjectOrStringArrayParameter(columns)) {
    WCDBRustBridgeStruct(CPPStatementCreateView, self);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        columns, WCDBStatementCreateViewConfigColumns2(selfStruct, columns_commonArray));
}
