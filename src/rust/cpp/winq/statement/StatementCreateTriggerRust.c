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

#include "StatementCreateTriggerRust.h"

#include "StatementCreateTriggerBridge.h"

void* WCDBRustStatementCreateTriggerClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBStatementCreateTriggerCreate().innerValue;
}

void WCDBRustStatementCreateTriggerClassMethod(configTrigger, void* self, const char* name) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    // WCDBRustGetStringCritical(name);
    WCDBStatementCreateTriggerConfigTrigger(selfStruct, name);
    // WCDBRustReleaseStringCritical(name);
}

void WCDBRustStatementCreateTriggerClassMethod(configSchema,
                                               void* self,
                                               WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBStatementCreateTriggerConfigSchema2(selfStruct, schema_common);
    // WCDBRustTryReleaseStringInCommonValue(schema);
}

void WCDBRustStatementCreateTriggerClassMethod(configTemp, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigTemp(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configIfNotExist, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigIfNotExist(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configBefore, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigBefore(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configAfter, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigAfter(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configInsteadOf, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigInsteadOf(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configDelete, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigDelete(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configInsert, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigInsert(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configUpdate, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigUpdate(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configColumns,
                                               void* self,
                                               WCDBRustObjectOrStringArrayParameter(columns)) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        columns, WCDBStatementCreateTriggerConfigColumns2(selfStruct, columns_commonArray));
}

void WCDBRustStatementCreateTriggerClassMethod(configTable, void* self, const char* table) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    // WCDBRustGetStringCritical(table);
    WCDBStatementCreateTriggerConfigTable(selfStruct, table);
    // WCDBRustReleaseStringCritical(table);
}

void WCDBRustStatementCreateTriggerClassMethod(configForEachRow, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigForEachRow(selfStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(configWhen, void* self, void* expression) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBStatementCreateTriggerConfigWhen(selfStruct, expressionStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(executeInsert, void* self, void* insert) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBRustBridgeStruct(CPPStatementInsert, insert);
    WCDBStatementCreateTriggerExecuteInsert(selfStruct, insertStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(executeUpdate, void* self, void* update) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBRustBridgeStruct(CPPStatementUpdate, update);
    WCDBStatementCreateTriggerExecuteUpdate(selfStruct, updateStruct);
}

void WCDBRustStatementCreateTriggerClassMethod(executeDelete, void* self, void* delete_) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBRustBridgeStruct(CPPStatementDelete, delete_);
    WCDBStatementCreateTriggerExecuteDelete(selfStruct, delete_Struct);
}

void WCDBRustStatementCreateTriggerClassMethod(executeSelect, void* self, void* select) {
    WCDBRustBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBRustBridgeStruct(CPPStatementSelect, select);
    WCDBStatementCreateTriggerExecuteSelect(selfStruct, selectStruct);
}
