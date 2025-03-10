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

#include "ColumnRust.h"

#include "ColumnBridge.h"

void* WCDBRustColumnClassMethodWithNoArg(createAll) {
    return (void*)WCDBColumnCreateAll().innerValue;
}

// jlong WCDBRustColumnClassMethodWithNoArg(createRowId)
//{
//     return (jlong) WCDBColumnCreateRowId().innerValue;
// }

void* WCDBRustColumn_createWithName(const char* name, void* binding) {
    return (void*)WCDBColumnCreateWithName2(name, (const void*)binding).innerValue;
}

// jlong WCDBRustColumnClassMethod(copy, jlong column)
//{
//     WCDBRustBridgeStruct(CPPColumn, column);
//     return (jlong) WCDBColumnCopy(columnStruct).innerValue;
// }
//
// void WCDBRustColumnClassMethod(inTable, jlong column, jstring table)
//{
//     WCDBRustGetStringCritical(table);
//     WCDBRustBridgeStruct(CPPColumn, column);
//     WCDBColumnInTable(columnStruct, tableString);
//     WCDBRustReleaseStringCritical(table);
// }
//
// void WCDBRustColumnClassMethod(ofSchema, jlong column, WCDBRustObjectOrStringParameter(schema))
//{
//     WCDBRustBridgeStruct(CPPColumn, column);
//     WCDBRustCreateObjectOrStringCommonValue(schema, true);
//     WCDBColumnOfSchema2(columnStruct, schema_common);
//     WCDBRustTryReleaseStringInCommonValue(schema);
// }
//
// jlong WCDBRustColumnClassMethod(configAlias, jlong column, jstring alias)
//{
//     WCDBRustBridgeStruct(CPPColumn, column);
//     WCDBRustGetString(alias);
//     jlong ret = (jlong) WCDBColumnConfigAlias(columnStruct, aliasString).innerValue;
//     WCDBRustReleaseString(alias);
//     return ret;
// }
