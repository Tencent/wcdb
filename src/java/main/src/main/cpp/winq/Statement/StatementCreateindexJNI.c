// Created by chenqiuwen on 2023/4/9.
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

#include "StatementCreateindexJNI.h"
#include "StatementCreateIndexBridge.h"

jlong WCDBJNIStatementCreateIndexObjectMethodWithNoArg(create)
{
    return (jlong) WCDBStatementCreateIndexCreate().innerValue;
}

void WCDBJNIStatementCreateIndexObjectMethod(configIndex, jlong self, jstring name)
{
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, self);
    WCDBJNIGetStringCritical(name);
    WCDBStatementCreateIndexConfigIndexName(selfStruct, nameString);
    WCDBJNIReleaseStringCritical(name);
}

void WCDBJNIStatementCreateIndexObjectMethod(configSchema,
                                             jlong self,
                                             WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, self);
    WCDBJNICreateObjectOrStringCommonValue(schema, true);
    WCDBStatementCreateIndexConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementCreateIndexObjectMethod(configUnique, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, self);
    WCDBStatementCreateIndexConfigUniqe(selfStruct);
}

void WCDBJNIStatementCreateIndexObjectMethod(configIfNotExist, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, self);
    WCDBStatementCreateIndexConfigIfNotExist(selfStruct);
}

void WCDBJNIStatementCreateIndexObjectMethod(configTable, jlong self, jstring tableName)
{
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, self);
    WCDBJNIGetStringCritical(tableName);
    WCDBStatementCreateIndexConfigTable(selfStruct, tableNameString);
    WCDBJNIReleaseStringCritical(tableName);
}

void WCDBJNIStatementCreateIndexObjectMethod(configIndexedColumns,
                                             jlong self,
                                             WCDBJNIObjectOrStringArrayParameter(indexColumns))
{
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, self);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    indexColumns, WCDBStatementCreateIndexConfigIndexColumns2(selfStruct, indexColumns_commonArray));
}

void WCDBJNIStatementCreateIndexObjectMethod(configWhere, jlong self, jlong condition)
{
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, self);
    WCDBJNIBridgeStruct(CPPExpression, condition);
    WCDBStatementCreateIndexConfigWhere(selfStruct, conditionStruct);
}