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

#include "StatementDropTableRust.h"

#include "StatementDropTableBridge.h"

void* WCDBRustStatementDropTableClassMethodWithNoArg(create) {
    return (void*)WCDBStatementDropTableCreate().innerValue;
}

void WCDBRustStatementDropTableClassMethod(configSchema,
                                           void* self,
                                           WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPStatementDropTable, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBStatementDropTableConfigSchema2(selfStruct, schema_common);
}

void WCDBRustStatementDropTableClassMethod(configTableName, void* self, const char* tableName) {
    WCDBRustBridgeStruct(CPPStatementDropTable, self);
    WCDBStatementDropTableConfigTable(selfStruct, tableName);
}

void WCDBRustStatementDropTableClassMethod(configIfExist, void* self) {
    WCDBRustBridgeStruct(CPPStatementDropTable, self);
    WCDBStatementDropTableConfigIfExists(selfStruct);
}