// Created by qiuwenchen on 2023/6/12.
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

#include "StatementReindexJNI.h"
#include "StatementReindexBridge.h"

jlong WCDBJNIStatementReindexObjectMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementReIndexCreate().innerValue;
}

void WCDBJNIStatementReindexObjectMethod(configCollation, jlong self, jstring collation)
{
    WCDBJNIBridgeStruct(CPPStatementReIndex, self);
    WCDBJNIGetString(collation);
    WCDBStatementReIndexConfigCollation(selfStruct, collationString);
    WCDBJNIReleaseString(collation);
}

void WCDBJNIStatementReindexObjectMethod(configTable, jlong self, jstring name)
{
    WCDBJNIBridgeStruct(CPPStatementReIndex, self);
    WCDBJNIGetString(name);
    WCDBStatementReIndexConfigTable(selfStruct, nameString);
    WCDBJNIReleaseString(name);
}

void WCDBJNIStatementReindexObjectMethod(configIndex, jlong self, jstring name)
{
    WCDBJNIBridgeStruct(CPPStatementReIndex, self);
    WCDBJNIGetString(name);
    WCDBStatementReIndexConfigIndex(selfStruct, nameString);
    WCDBJNIReleaseString(name);
}

void WCDBJNIStatementReindexObjectMethod(configSchema,
                                         jlong self,
                                         WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementReIndex, self);
    WCDBJNICreateObjectOrStringCommonValue(schema);
    WCDBStatementReIndexConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}
