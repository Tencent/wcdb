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

#pragma once

#include "StatementCreateViewJNI.h"
#include "StatementCreateViewBridge.h"

jlong WCDBJNIStatementCreateViewObjectMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementCreateViewCreate().innerValue;
}

void WCDBJNIStatementCreateViewObjectMethod(configView, jlong self, jstring name)
{
    WCDBJNIBridgeStruct(CPPStatementCreateView, self);
    WCDBJNIGetString(name);
    WCDBStatementCreateViewConfigView(selfStruct, nameString);
    WCDBJNIReleaseString(name);
}

void WCDBJNIStatementCreateViewObjectMethod(configSchema,
                                            jlong self,
                                            WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementCreateView, self);
    WCDBJNICreateObjectOrStringCommonValue(schema);
    WCDBStatementCreateViewConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementCreateViewObjectMethod(configTemp, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateView, self);
    WCDBStatementCreateViewConfigTemp(selfStruct);
}

void WCDBJNIStatementCreateViewObjectMethod(configIfNotExist, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateView, self);
    WCDBStatementCreateViewConfigIfNotExist(selfStruct);
}

void WCDBJNIStatementCreateViewObjectMethod(configAs, jlong self, jlong select)
{
    WCDBJNIBridgeStruct(CPPStatementCreateView, self);
    WCDBJNIBridgeStruct(CPPStatementSelect, select);
    WCDBStatementCreateViewConfigAs(selfStruct, selectStruct);
}

void WCDBJNIStatementCreateViewObjectMethod(configColumns,
                                            jlong self,
                                            WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementCreateView, self);
    WCDBJNICreateObjectOrStringArrayWithAction(
    columns, WCDBStatementCreateViewConfigColumns2(selfStruct, columns_commonArray));
}
