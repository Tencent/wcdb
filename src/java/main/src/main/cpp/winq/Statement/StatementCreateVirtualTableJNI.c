// Created by chenqiuwen on 2023/11/4.
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

#include "StatementCreateVirtualTableJNI.h"
#include "StatementCreateVirtualTableBridge.h"

jlong WCDBJNIStatementCreateVirtualTableClassMethodWithNoArg(createCppObj)
{
    return (jlong) WCDBStatementCreateVirtualTableCreate().innerValue;
}

void WCDBJNIStatementCreateVirtualTableClassMethod(configTable, jlong self, jstring name)
{
    WCDBJNIGetString(name);
    WCDBJNIBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBStatementCreateVirtualTableConfigTable(selfStruct, nameString);
    WCDBJNIReleaseString(name);
}

void WCDBJNIStatementCreateVirtualTableClassMethod(configSchema,
                                                   jlong self,
                                                   WCDBJNIObjectOrStringParameter(schema))
{
    WCDBJNIBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBJNICreateObjectOrStringCommonValue(schema, true);
    WCDBStatementCreateVirtualTableConfigSchema2(selfStruct, schema_common);
    WCDBJNITryReleaseStringInCommonValue(schema);
}

void WCDBJNIStatementCreateVirtualTableClassMethod(configIfNotExist, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBStatementCreateVirtualTableConfigIfNotExist(selfStruct);
}

void WCDBJNIStatementCreateVirtualTableClassMethod(configModule, jlong self, jstring module)
{
    WCDBJNIBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBJNIGetString(module);
    WCDBStatementCreateVirtualTableConfigModule(selfStruct, moduleString);
    WCDBJNIReleaseString(module);
}

void WCDBJNIStatementCreateVirtualTableClassMethod(configArguments, jlong self, jobjectArray arguments)
{
    WCDBJNIBridgeStruct(CPPStatementCreateVirtualTable, self);
    WCDBJNIGetStringArray(arguments);
    WCDBStatementCreateVirtualTableConfigArguments(
    selfStruct, (const char *const *) argumentsCharArray, argumentsLength);
    WCDBJNIReleaseStringArray(arguments);
}
