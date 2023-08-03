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

#include "StatementDropIndexJNI.h"
#include "StatementDropIndexBridge.h"

jlong WCDBJNIStatementDropIndexObjectMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementDropIndexCreate().innerValue;
}

void WCDBJNIStatementDropIndexObjectMethod(configSchema,
                                           jlong self,
                                           WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementDropIndex, self);
    WCDBJNICreateObjectOrStringCommonValue(schema);
    WCDBStatementDropIndexConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementDropIndexObjectMethod(configIndex, jlong self, jstring indexName)
{
    WCDBJNIBridgeStruct(CPPStatementDropIndex, self);
    WCDBJNIGetStringCritical(indexName);
    WCDBStatementDropIndexConfigIndex(selfStruct, indexNameString);
    WCDBJNIReleaseStringCritical(indexName);
}

void WCDBJNIStatementDropIndexObjectMethod(configIfExist, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementDropIndex, self);
    WCDBStatementDropIndexConfigIfExists(selfStruct);
}
