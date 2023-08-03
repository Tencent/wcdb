// Created by qiuwenchen on 2023/4/12.
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

#include "StatementInsertJNI.h"
#include "StatementInsertBridge.h"

jlong WCDBJNIStatementInsertObjectMethodWithNoArg(create)
{
    return (jlong) WCDBStatementInsertCreate().innerValue;
}

void WCDBJNIStatementInsertObjectMethod(configWith, jlong self, jlongArray expressions)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNIGetCppPointerArrayCritical(expressions);
    WCDBStatementInsertConfigWith(
    selfStruct, (const CPPCommonTableExpression*) expressionsArray, expressionsLength);
    WCDBJNIReleaseCppPointerArrayCritical(expressions);
}

void WCDBJNIStatementInsertObjectMethod(configRecursive, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigRecursive(selfStruct);
}

void WCDBJNIStatementInsertObjectMethod(configTableName, jlong self, jstring tableName)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNIGetStringCritical(tableName);
    WCDBStatementInsertConfigTable(selfStruct, tableNameString);
    WCDBJNIReleaseStringCritical(tableName);
}

void WCDBJNIStatementInsertObjectMethod(configSchema,
                                        jlong self,
                                        WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNICreateObjectOrStringCommonValue(schema, true);
    WCDBStatementInsertConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementInsertObjectMethod(configConfliction, jlong self, jint action)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigConfiction(selfStruct, action);
}

void WCDBJNIStatementInsertObjectMethod(configAs, jlong self, jstring alias)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNIGetStringCritical(alias);
    WCDBStatementInsertConfigAlias(selfStruct, aliasString);
    WCDBJNIReleaseStringCritical(alias);
}

void WCDBJNIStatementInsertObjectMethod(configColumns,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    columns, WCDBStatementInsertConfigColumns2(selfStruct, columns_commonArray));
}

void WCDBJNIStatementInsertObjectMethod(configValues,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(value))
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNICreateMultiTypeArray(value);
    WCDBStatementInsertConfigValuesWithMultiTypeArray(selfStruct, valueArray);
    WCDBJNIReleaseMultiTypeArray(value);
}

void WCDBJNIStatementInsertObjectMethod(configValuesWithBindParameters, jlong self, jint count)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigValuesWithBindParameters(selfStruct, count);
}

void WCDBJNIStatementInsertObjectMethod(configSelect, jlong self, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    WCDBStatementInsertConfigSelect(selfStruct, selectStruct);
}

void WCDBJNIStatementInsertObjectMethod(configDefaultValues, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBStatementInsertConfigDefaultValues(selfStruct);
}

void WCDBJNIStatementInsertObjectMethod(configUpsert, jlong self, jlong upsert)
{
    WCDBJNIBridgeStruct(CPPStatementInsert, self);
    WCDBJNIBridgeStruct(CPPUpsert, upsert);
    WCDBStatementInsertConfigUpsert(selfStruct, upsertStruct);
}
