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

#include "StatementInsertRust.h"

#include "StatementInsertBridge.h"

void* WCDBRustStatementInsertClassMethodWithNoArg(create) {
    return (void*)WCDBStatementInsertCreate().innerValue;
}

// void WCDBRustStatementInsertClassMethod(configWith, jlong self, jlongArray expressions)
//{
//     WCDBRustBridgeStruct(CPPStatementInsert, self);
//     WCDBRustGetCppPointerArrayCritical(expressions);
//     WCDBStatementInsertConfigWith(
//     selfStruct, (const CPPCommonTableExpression*) expressionsArray, expressionsLength);
//     WCDBRustReleaseCppPointerArrayCritical(expressions);
// }
//
// void WCDBRustStatementInsertClassMethod(configRecursive, jlong self)
//{
//     WCDBRustBridgeStruct(CPPStatementInsert, self);
//     WCDBStatementInsertConfigRecursive(selfStruct);
// }

void WCDBRustStatementInsertClassMethod(configTableName, void* self, const char* tableName) {
    WCDBRustBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigTable(selfStruct, tableName);
}

// void WCDBRustStatementInsertClassMethod(configSchema,
//                                        jlong self,
//                                        WCDBRustObjectOrStringParameter(schema))
//{
//     WCDBRustBridgeStruct(CPPStatementInsert, self);
//     WCDBRustCreateObjectOrStringCommonValue(schema, true);
//     WCDBStatementInsertConfigSchema2(selfStruct, schema_common);
//     WCDBRustTryReleaseStringInCommonValue(schema);
// }
//
void WCDBRustStatementInsertClassMethod(configConflictAction, void* self, int action) {
    WCDBRustBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigConfiction(selfStruct, action);
}
//
// void WCDBRustStatementInsertClassMethod(configAs, jlong self, jstring alias)
//{
//    WCDBRustBridgeStruct(CPPStatementInsert, self);
//    WCDBRustGetStringCritical(alias);
//    WCDBStatementInsertConfigAlias(selfStruct, aliasString);
//    WCDBRustReleaseStringCritical(alias);
//}

void WCDBRustStatementInsertClassMethod(configColumns,
                                        void* self,
                                        WCDBRustObjectOrStringArrayParameter(columns)) {
    WCDBRustBridgeStruct(CPPStatementInsert, self);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        columns, WCDBStatementInsertConfigColumns2(selfStruct, columns_commonArray));
}

// void WCDBRustStatementInsertClassMethod(configValues, jlong self,
// WCDBRustMultiTypeArrayParameter(value))
//{
//     WCDBRustBridgeStruct(CPPStatementInsert, self);
//     WCDBRustCreateMultiTypeArray(value);
//     WCDBStatementInsertConfigValuesWithMultiTypeArray(selfStruct, valueArray);
//     WCDBRustReleaseMultiTypeArray(value);
// }

void WCDBRustStatementInsertClassMethod(configValuesWithBindParameters, void* self, int count) {
    WCDBRustBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigValuesWithBindParameters(selfStruct, count);
}

// void WCDBRustStatementInsertClassMethod(configSelect, jlong self, jlong select)
//{
//     WCDBRustBridgeStruct(CPPStatementInsert, self);
//     WCDBRustBridgeStruct(CPPStatementSelect, select);
//     WCDBStatementInsertConfigSelect(selfStruct, selectStruct);
// }
//
void WCDBRustStatementInsertClassMethod(configDefaultValues, void* self) {
    WCDBRustBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigDefaultValues(selfStruct);
}

// void WCDBRustStatementInsertClassMethod(configUpsert, jlong self, jlong upsert)
//{
//     WCDBRustBridgeStruct(CPPStatementInsert, self);
//     WCDBRustBridgeStruct(CPPUpsert, upsert);
//     WCDBStatementInsertConfigUpsert(selfStruct, upsertStruct);
// }
