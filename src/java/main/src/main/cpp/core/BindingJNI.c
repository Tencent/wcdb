// Created by chenqiuwen on 2023/4/19.
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

#include "BindingJNI.h"
#include "BindingBridge.h"

jlong WCDBJNIBindingObjectMethodWithNoArg(create)
{
    return (jlong) WCDBBindingCreate().innerValue;
}

void WCDBJNIBindingObjectMethod(addColumnDef, jlong self, jlong columnDef)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    WCDBJNIBridgeStruct(CPPColumnDef, columnDef);
    WCDBBindingAddColumnDef(selfStruct, columnDefStruct);
}

void WCDBJNIBindingObjectMethod(addIndex, jlong self, jstring indexSubfix, jlong createIndex)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    WCDBJNIBridgeStruct(CPPStatementCreateIndex, createIndex);
    WCDBJNIGetString(indexSubfix);
    WCDBBindingAddIndex(selfStruct, indexSubfixString, createIndexStruct);
    WCDBJNIReleaseString(indexSubfix);
}

void WCDBJNIBindingObjectMethod(addTableConstraint, jlong self, jlong constraint)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    WCDBJNIBridgeStruct(CPPTableConstraint, constraint);
    WCDBBindingAddTableConstraint(selfStruct, constraintStruct);
}

void WCDBJNIBindingObjectMethod(configVirtualModule, jlong self, jstring moduleName)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    WCDBJNIGetString(moduleName);
    WCDBBindingConfigVirtualModule(selfStruct, moduleNameString);
    WCDBJNIReleaseString(moduleName);
}

void WCDBJNIBindingObjectMethod(configVirtualModuleArgument, jlong self, jstring argument)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    WCDBJNIGetString(argument);
    WCDBBindingConfigVirtualModuleArgument(selfStruct, argumentString);
    WCDBJNIReleaseString(argument);
}

jboolean WCDBJNIBindingObjectMethod(createTable, jlong self, jstring tableName, jlong handle)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    WCDBJNIBridgeStruct(CPPHandle, handle);
    WCDBJNIGetString(tableName);
    jboolean ret = WCDBBindingCreateTable(selfStruct, tableNameString, handleStruct);
    WCDBJNIReleaseString(tableName);
    return ret;
}

jboolean
WCDBJNIBindingObjectMethod(createVirtualTable, jlong self, jstring tableName, jlong handle)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    WCDBJNIBridgeStruct(CPPHandle, handle);
    WCDBJNIGetString(tableName);
    jboolean ret = WCDBBindingCreateVirtualTable(selfStruct, tableNameString, handleStruct);
    WCDBJNIReleaseString(tableName);
    return ret;
}

jlong WCDBJNIBindingObjectMethod(getBaseBinding, jlong self)
{
    WCDBJNIBridgeStruct(CPPBinding, self);
    return (jlong) WCDBBindingGetBaseBinding(selfStruct);
}
