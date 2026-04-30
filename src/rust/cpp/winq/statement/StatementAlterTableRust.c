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

#include "StatementAlterTableRust.h"

#include "StatementAlterTableBridge.h"

void* WCDBRustStatementAlterTableClassMethodWithNoArg(createCppObj) {
    return (void*)WCDBStatementAlterTableCreate().innerValue;
}

void WCDBRustStatementAlterTableClassMethod(configTable, void* self, const char* table) {
    WCDBRustBridgeStruct(CPPStatementAlterTable, self);
    WCDBStatementAlterTableConfigTable(selfStruct, table);
}

void WCDBRustStatementAlterTableClassMethod(configSchema,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(schema)) {
    WCDBRustBridgeStruct(CPPStatementAlterTable, self);
    WCDBRustCreateObjectOrStringCommonValue(schema, true);
    WCDBStatementAlterTableConfigSchema2(selfStruct, schema_common);
}

void WCDBRustStatementAlterTableClassMethod(configRenameToTable, void* self, const char* table) {
    WCDBRustBridgeStruct(CPPStatementAlterTable, self);
    WCDBStatementAlterTableConfigRenameToTable(selfStruct, table);
}

void WCDBRustStatementAlterTableClassMethod(configRenameColumn,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(column)) {
    WCDBRustBridgeStruct(CPPStatementAlterTable, self);
    WCDBRustCreateObjectOrStringCommonValue(column, true);
    WCDBStatementAlterTableConfigRenameColumn2(selfStruct, column_common);
}

void WCDBRustStatementAlterTableClassMethod(configRenameToColumn,
                                            void* self,
                                            WCDBRustObjectOrStringParameter(column)) {
    WCDBRustBridgeStruct(CPPStatementAlterTable, self);
    WCDBRustCreateObjectOrStringCommonValue(column, true);
    WCDBStatementAlterTableConfigRenameToColumn2(selfStruct, column_common);
}

void WCDBRustStatementAlterTableClassMethod(configAddColumn, void* self, void* columnDef) {
    WCDBRustBridgeStruct(CPPStatementAlterTable, self);
    WCDBRustBridgeStruct(CPPColumnDef, columnDef);
    WCDBStatementAlterTableConfigAddColumn(selfStruct, columnDefStruct);
}
