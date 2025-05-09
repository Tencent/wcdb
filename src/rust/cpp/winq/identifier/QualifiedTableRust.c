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

#include "QualifiedTableRust.h"

#include "QualifiedTableBridge.h"

void* WCDBRustQualifiedTableClassMethod(create, const char* tableName) {
    void* ret = (void*)WCDBQualifiedTableCreateWithTable(tableName).innerValue;
    return ret;
}

void WCDBRustQualifiedTableClassMethod(configSchema,
                                       void* self,
                                       WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPQualifiedTable, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBQualifiedTableConfigSchema2(selfStruct, schema_common);
}

void WCDBRustQualifiedTableClassMethod(configAlias, void* self, const char* aliasString) {
    WCDBRustBridgeStruct(CPPQualifiedTable, self);
    WCDBQualifiedTableConfigAliasName(selfStruct, aliasString);
}

void WCDBRustQualifiedTableClassMethod(configIndex, void* self, const char* indexString) {
    WCDBRustBridgeStruct(CPPQualifiedTable, self);
    WCDBQualifiedTableConfigIndexName(selfStruct, indexString);
}

void WCDBRustQualifiedTableClassMethod(configNotIndexed, void* self) {
    WCDBRustBridgeStruct(CPPQualifiedTable, self);
    WCDBQualifiedTableConfigNoIndexed(selfStruct);
}