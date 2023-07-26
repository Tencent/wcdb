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

jlong WCDBJNIStatementCreateTriggerObjectMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementCreateTriggerCreate().innerValue;
}

void WCDBJNIStatementCreateTriggerObjectMethod(configTrigger, jlong self, jstring name)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIGetString(name);
    WCDBStatementCreateTriggerConfigTrigger(selfStruct, nameString);
    WCDBJNIReleaseString(name);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configSchema,
                                               jlong self,
                                               WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNICreateObjectOrStringCommonValue(schema);
    WCDBStatementCreateTriggerConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configTemp, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigTemp(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configIfNotExist, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigIfNotExist(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configBefore, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigBefore(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configAfter, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigAfter(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configInsteadOf, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigInsteadOf(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configDelete, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigDelete(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configInsert, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigInsert(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configUpdate, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigUpdate(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configColumns,
                                               jlong self,
                                               WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNICreateObjectOrStringArrayWithAction(
    columns, WCDBStatementCreateTriggerConfigColumns2(selfStruct, columns_commonArray));
}

void WCDBJNIStatementCreateTriggerObjectMethod(configTable, jlong self, jstring table)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIGetString(table);
    WCDBStatementCreateTriggerConfigTable(selfStruct, tableString);
    WCDBJNIReleaseString(table);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configForEachRow, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBStatementCreateTriggerConfigForEachRow(selfStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(configWhen, jlong self, jlong expression)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBStatementCreateTriggerConfigWhen(selfStruct, expressionStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(executeInsert, jlong self, jlong insert)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementInsert, insert);
    WCDBStatementCreateTriggerExecuteInsert(selfStruct, insertStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(executeUpdate, jlong self, jlong update)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementUpdate, update);
    WCDBStatementCreateTriggerExecuteUpdate(selfStruct, updateStruct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(executeDelete, jlong self, jlong delete_)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementDelete, delete_);
    WCDBStatementCreateTriggerExecuteDelete(selfStruct, delete_Struct);
}

void WCDBJNIStatementCreateTriggerObjectMethod(executeSelect, jlong self, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementCreateTrigger, self);
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    WCDBStatementCreateTriggerExecuteSelect(selfStruct, selectStruct);
}
