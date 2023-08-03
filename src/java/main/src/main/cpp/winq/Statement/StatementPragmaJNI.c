// Created by chenqiuwen on 2023/4/12.
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

#include "StatementPragmaJNI.h"
#include "StatementPragmaBridge.h"

jlong WCDBJNIStatementPragmaObjectMethodWithNoArg(create)
{
    return (jlong) WCDBStatementPragmaCreate().innerValue;
}
void WCDBJNIStatementPragmaObjectMethod(configSchema, jlong self, jstring schemaName)
{
    WCDBJNIBridgeStruct(CPPStatementPragma, self);
    WCDBJNIGetStringCritical(schemaName);
    WCDBStatementPragmaConfigSchema2(selfStruct, schemaNameString);
    WCDBJNIReleaseStringCritical(schemaName);
}

void WCDBJNIStatementPragmaObjectMethod(configPragma, jlong self, jlong pragma)
{
    WCDBJNIBridgeStruct(CPPStatementPragma, self);
    WCDBJNIBridgeStruct(CPPPragma, pragma);
    WCDBStatementPragmaConfigPragma(selfStruct, pragmaStruct);
}

void WCDBJNIStatementPragmaObjectMethod(configToValue, jlong self, WCDBJNICommonValueParameter(value))
{
    WCDBJNIBridgeStruct(CPPStatementPragma, self);
    WCDBJNICreateCommonValue(value, true);
    WCDBStatementPragmaConfigToValue2(selfStruct, value_common);
    WCDBJNITryReleaseStringInCommonValue(value);
}

void WCDBJNIStatementPragmaObjectMethod(configWithValue,
                                        jlong self,
                                        WCDBJNICommonValueParameter(value))
{
    WCDBJNIBridgeStruct(CPPStatementPragma, self);
    WCDBJNICreateCommonValue(value, true);
    WCDBStatementPragmaConfigWithValue2(selfStruct, value_common);
    WCDBJNITryReleaseStringInCommonValue(value);
}
