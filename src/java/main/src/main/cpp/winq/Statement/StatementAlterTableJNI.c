// Created by chenqiuwen on 2023/6/11.
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

#include "StatementAlterTableJNI.h"
#include "StatementAlterTableBridge.h"

jlong WCDBJNIStatementAlterTableClassMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementAlterTableCreate().innerValue;
}

void WCDBJNIStatementAlterTableObjectMethod(configTable, jlong self, jstring table)
{
    WCDBJNIBridgeStruct(CPPStatementAlterTable, self);
    WCDBJNIGetStringCritical(table);
    WCDBStatementAlterTableConfigTable(selfStruct, tableString);
    WCDBJNIReleaseStringCritical(table);
}

void WCDBJNIStatementAlterTableObjectMethod(configSchema,
                                            jlong self,
                                            WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementAlterTable, self);
    WCDBJNICreateObjectOrStringCommonValue(schema);
    WCDBStatementAlterTableConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementAlterTableObjectMethod(configRenameToTable, jlong self, jstring table)
{
    WCDBJNIBridgeStruct(CPPStatementAlterTable, self);
    WCDBJNIGetStringCritical(table);
    WCDBStatementAlterTableConfigRenameToTable(selfStruct, tableString);
    WCDBJNIReleaseStringCritical(table);
}

void WCDBJNIStatementAlterTableObjectMethod(configRenameColumn,
                                            jlong self,
                                            WCDBJNIObjectOrStringParameter(column))
{
    WCDBJNIBridgeStruct(CPPStatementAlterTable, self);
    WCDBJNICreateObjectOrStringCommonValue(column);
    WCDBStatementAlterTableConfigRenameColumn2(selfStruct, column_common);
    WCDBJNITryReleaseStringInCommonValue(column);
}

void WCDBJNIStatementAlterTableObjectMethod(configRenameToColumn,
                                            jlong self,
                                            WCDBJNIObjectOrStringParameter(column))
{
    WCDBJNIBridgeStruct(CPPStatementAlterTable, self);
    WCDBJNICreateObjectOrStringCommonValue(column);
    WCDBStatementAlterTableConfigRenameToColumn2(selfStruct, column_common);
    WCDBJNITryReleaseStringInCommonValue(column);
}

void WCDBJNIStatementAlterTableObjectMethod(configAddColumn, jlong self, jlong columnDef)
{
    WCDBJNIBridgeStruct(CPPStatementAlterTable, self);
    WCDBJNIBridgeStruct(CPPColumnDef, columnDef);
    WCDBStatementAlterTableConfigAddColumn(selfStruct, columnDefStruct);
}
