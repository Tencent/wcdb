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

#include "StatementCreateindexRust.h"

#include "StatementCreateIndexBridge.h"

void* WCDBRustStatementCreateIndexClassMethodWithNoArg(create) {
    return (void*)WCDBStatementCreateIndexCreate().innerValue;
}

// void WCDBRustStatementCreateIndexClassMethod(configIndex, jlong self, jstring name)
//{
//     WCDBRustBridgeStruct(CPPStatementCreateIndex, self);
//     WCDBRustGetStringCritical(name);
//     WCDBStatementCreateIndexConfigIndexName(selfStruct, nameString);
//     WCDBRustReleaseStringCritical(name);
// }
//
// void WCDBRustStatementCreateIndexClassMethod(configSchema,
//                                             jlong self,
//                                             WCDBRustObjectOrStringParameter(schema))
//{
//     WCDBRustBridgeStruct(CPPStatementCreateIndex, self);
//     WCDBRustCreateObjectOrStringCommonValue(schema, true);
//     WCDBStatementCreateIndexConfigSchema2(selfStruct, schema_common);
//     WCDBRustTryReleaseStringInCommonValue(schema);
// }
//
// void WCDBRustStatementCreateIndexClassMethod(configUnique, jlong self)
//{
//     WCDBRustBridgeStruct(CPPStatementCreateIndex, self);
//     WCDBStatementCreateIndexConfigUniqe(selfStruct);
// }
//
void WCDBRustStatementCreateIndexClassMethod(configIfNotExist, void* self) {
    WCDBRustBridgeStruct(CPPStatementCreateIndex, self);
    WCDBStatementCreateIndexConfigIfNotExist(selfStruct);
}

// void WCDBRustStatementCreateIndexClassMethod(configTable, jlong self, jstring tableName)
//{
//     WCDBRustBridgeStruct(CPPStatementCreateIndex, self);
//     WCDBRustGetStringCritical(tableName);
//     WCDBStatementCreateIndexConfigTable(selfStruct, tableNameString);
//     WCDBRustReleaseStringCritical(tableName);
// }
//
void WCDBRustStatementCreateIndexClassMethod(configIndexedColumns,
                                             void* self,
                                             WCDBRustObjectOrStringArrayParameter(indexColumns)) {
    WCDBRustBridgeStruct(CPPStatementCreateIndex, self);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        indexColumns,
        WCDBStatementCreateIndexConfigIndexColumns2(selfStruct, indexColumns_commonArray));
}
//
// void WCDBRustStatementCreateIndexClassMethod(configWhere, jlong self, jlong condition)
//{
//    WCDBRustBridgeStruct(CPPStatementCreateIndex, self);
//    WCDBRustBridgeStruct(CPPExpression, condition);
//    WCDBStatementCreateIndexConfigWhere(selfStruct, conditionStruct);
//}