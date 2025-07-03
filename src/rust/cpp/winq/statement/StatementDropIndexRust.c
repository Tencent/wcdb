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

#include "StatementDropIndexRust.h"

#include "StatementDropIndexBridge.h"

void* WCDBRustStatementDropIndexClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBStatementDropIndexCreate().innerValue;
}

void WCDBRustStatementDropIndexClassMethod(configSchema,
                                           void* self,
                                           WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPStatementDropIndex, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBStatementDropIndexConfigSchema2(selfStruct, schema_common);
}

void WCDBRustStatementDropIndexClassMethod(configIndex, void* self, const char* indexName) {
    WCDBRustBridgeStruct(CPPStatementDropIndex, self);
    WCDBStatementDropIndexConfigIndex(selfStruct, indexName);
}

void WCDBRustStatementDropIndexClassMethod(configIfExist, void* self) {
    WCDBRustBridgeStruct(CPPStatementDropIndex, self);
    WCDBStatementDropIndexConfigIfExists(selfStruct);
}
