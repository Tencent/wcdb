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

#include "UpsertRust.h"

#include "UpsertBridge.h"

#include <stdio.h>

void* WCDBRustUpsertClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBUpsertCreate().innerValue;
}

void WCDBRustUpsertClassMethod(configIndexedColumn,
                               void* upsert,
                               WCDBRustObjectOrStringArrayParameter(columns)) {
    WCDBRustBridgeStruct(CPPUpsert, upsert);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        columns, WCDBUpsertConfigIndexdColumn2(upsertStruct, columns_commonArray));
}

void WCDBRustUpsertClassMethod(configWhere, void* upsert, void* expression) {
    WCDBRustBridgeStruct(CPPUpsert, upsert);
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBUpsertConfigWhere(upsertStruct, expressionStruct);
}

void WCDBRustUpsertClassMethod(configDoNothing, void* upsert) {
    WCDBRustBridgeStruct(CPPUpsert, upsert);
    WCDBUpsertConfigDoNothing(upsertStruct);
}

void WCDBRustUpsertClassMethod(configDoUpdate, void* upsert) {
    WCDBRustBridgeStruct(CPPUpsert, upsert);
    WCDBUpsertConfigDoUpdate(upsertStruct);
}

void WCDBRustUpsertClassMethod(configSetColumns,
                               void* upsert,
                               WCDBRustObjectOrStringArrayParameter(columns)) {
    WCDBRustBridgeStruct(CPPUpsert, upsert);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        columns, WCDBUpsertConfigSetColumns2(upsertStruct, columns_commonArray));
}

void WCDBRustUpsertClassMethod(configToValue,
                               void* upsert,
                               WCDBRustCommonValueParameter(expression)) {
    WCDBRustBridgeStruct(CPPUpsert, upsert);
    WCDBRustCreateCommonValueWithIsCritical(expression, true);
    WCDBUpsertConfigToValue2(upsertStruct, expression_common);
}