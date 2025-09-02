// Created by qiuwenchen on 2023/4/11.
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

#include "StatementCreateTableRust.h"

#include "StatementCreateTableBridge.h"

void* WCDBRustStatementCreateTableClassMethodWithNoArg(create) {
    return (void*)WCDBStatementCreateTableCreate().innerValue;
}

void WCDBRustStatementCreateTableClassMethod(configTableName, void* self, const char* tableName) {
    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
    WCDBStatementCreateTableConfigTable(selfStruct, tableName);
}
//
// void WCDBRustStatementCreateTableClassMethod(configSchema,
//                                            jlong self,
//                                            WCDBRustObjectOrStringParameter(schema))
//{
//    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
//    WCDBRustCreateObjectOrStringCommonValue(schema, true);
//    WCDBStatementCreateTableConfigSchema2(selfStruct, schema_common);
//    WCDBRustTryReleaseStringInCommonValue(schema);
//}
//
// void WCDBRustStatementCreateTableClassMethod(configTemp, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
//    WCDBStatementCreateTableConfigTemp(selfStruct);
//}
//
// void WCDBRustStatementCreateTableClassMethod(configIfNotExist, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
//    WCDBStatementCreateTableConfigIfNotExist(selfStruct);
//}
//
// void WCDBRustStatementCreateTableClassMethod(configAs, jlong self, jlong select)
//{
//    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
//    WCDBRustBridgeStruct(CPPStatementSelect, select);
//    WCDBStatementCreateTableConfigAs(selfStruct, selectStruct);
//}
//
// void WCDBRustStatementCreateTableClassMethod(configColumn, jlong self, jlong column)
//{
//    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
//    WCDBRustBridgeStruct(CPPColumnDef, column);
//    WCDBStatementCreateTableConfigColumn(selfStruct, columnStruct);
//}
//
void WCDBRustStatementCreateTableClassMethod(configColumns,
                                             void* self,
                                             const void** columns,
                                             int len) {
    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
    WCDBStatementCreateTableConfigColumns(selfStruct, (const CPPColumnDef*)columns, len);
}
//
// void WCDBRustStatementCreateTableClassMethod(configConstraints, jlong self, jlongArray
// constraints)
//{
//    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
//    WCDBRustGetCppPointerArrayCritical(constraints);
//    WCDBStatementCreateTableConfigTableConstraints(
//    selfStruct, (const CPPTableConstraint*) constraintsArray, constraintsLength);
//    WCDBRustReleaseCppPointerArrayCritical(constraints)
//}
//
// void WCDBRustStatementCreateTableClassMethod(configWithoutRowid, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementCreateTable, self);
//    WCDBStatementCreateTableConfigWithoutRowId(selfStruct);
//}
