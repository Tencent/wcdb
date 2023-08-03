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

#include "StatementCreateTableJNI.h"
#include "StatementCreateTableBridge.h"

jlong WCDBJNIStatementCreateTableObjectMethodWithNoArg(create)
{
    return (jlong) WCDBStatementCreateTableCreate().innerValue;
}

void WCDBJNIStatementCreateTableObjectMethod(configTableName, jlong self, jstring tableName)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBJNIGetStringCritical(tableName);
    WCDBStatementCreateTableConfigTable(selfStruct, tableNameString);
    WCDBJNIReleaseStringCritical(tableName);
}

void WCDBJNIStatementCreateTableObjectMethod(configSchema,
                                             jlong self,
                                             WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBJNICreateObjectOrStringCommonValue(schema, true);
    WCDBStatementCreateTableConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementCreateTableObjectMethod(configTemp, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBStatementCreateTableConfigTemp(selfStruct);
}

void WCDBJNIStatementCreateTableObjectMethod(configIfNotExist, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBStatementCreateTableConfigIfNotExist(selfStruct);
}

void WCDBJNIStatementCreateTableObjectMethod(configAs, jlong self, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    WCDBStatementCreateTableConfigAs(selfStruct, selectStruct);
}

void WCDBJNIStatementCreateTableObjectMethod(configColumn, jlong self, jlong column)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBJNIBridgeStruct(CPPColumnDef, column);
    WCDBStatementCreateTableConfigColumn(selfStruct, columnStruct);
}

void WCDBJNIStatementCreateTableObjectMethod(configColumns, jlong self, jlongArray columns)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBJNIGetCppPointerArrayCritical(columns);
    WCDBStatementCreateTableConfigColumns(
    selfStruct, (const CPPColumnDef*) columnsArray, columnsLength);
    WCDBJNIReleaseCppPointerArrayCritical(columns);
}

void WCDBJNIStatementCreateTableObjectMethod(configConstraints, jlong self, jlongArray constraints)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBJNIGetCppPointerArrayCritical(constraints);
    WCDBStatementCreateTableConfigTableConstraints(
    selfStruct, (const CPPTableConstraint*) constraintsArray, constraintsLength);
    WCDBJNIReleaseCppPointerArrayCritical(constraints)
}

void WCDBJNIStatementCreateTableObjectMethod(configWithoutRowid, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTable, self);
    WCDBStatementCreateTableConfigWithoutRowId(selfStruct);
}
