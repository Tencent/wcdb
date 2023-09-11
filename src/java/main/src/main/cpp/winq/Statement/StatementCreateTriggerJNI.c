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

#include "StatementCreateTriggerJNI.h"
#include "StatementCreateTriggerBridge.h"

jlong WCDBJNIStatementCreateTriggerClassMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementCreateTriggerCreate().innerValue;
}

void WCDBJNIStatementCreateTriggerClassMethod(configTrigger, jlong self, jstring name)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIGetStringCritical(name);
    WCDBStatementCreateTriggerConfigTrigger(selfStruct, nameString);
    WCDBJNIReleaseStringCritical(name);
}

void WCDBJNIStatementCreateTriggerClassMethod(configSchema,
                                              jlong self,
                                              WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNICreateObjectOrStringCommonValue(schema, true);
    WCDBStatementCreateTriggerConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementCreateTriggerClassMethod(configTemp, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigTemp(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configIfNotExist, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigIfNotExist(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configBefore, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigBefore(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configAfter, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigAfter(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configInsteadOf, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigInsteadOf(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configDelete, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigDelete(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configInsert, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigInsert(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configUpdate, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigUpdate(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configColumns,
                                              jlong self,
                                              WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    columns, WCDBStatementCreateTriggerConfigColumns2(selfStruct, columns_commonArray));
}

void WCDBJNIStatementCreateTriggerClassMethod(configTable, jlong self, jstring table)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIGetStringCritical(table);
    WCDBStatementCreateTriggerConfigTable(selfStruct, tableString);
    WCDBJNIReleaseStringCritical(table);
}

void WCDBJNIStatementCreateTriggerClassMethod(configForEachRow, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigForEachRow(selfStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(configWhen, jlong self, jlong expression)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBStatementCreateTriggerConfigWhen(selfStruct, expressionStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(executeInsert, jlong self, jlong insert)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementInsert, insert);
    WCDBStatementCreateTriggerExecuteInsert(selfStruct, insertStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(executeUpdate, jlong self, jlong update)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementUpdate, update);
    WCDBStatementCreateTriggerExecuteUpdate(selfStruct, updateStruct);
}

void WCDBJNIStatementCreateTriggerClassMethod(executeDelete, jlong self, jlong delete_)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementDelete, delete_);
    WCDBStatementCreateTriggerExecuteDelete(selfStruct, delete_Struct);
}

void WCDBJNIStatementCreateTriggerClassMethod(executeSelect, jlong self, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    WCDBStatementCreateTriggerExecuteSelect(selfStruct, selectStruct);
}
