// Created by qiuwenchen on 2023/6/8.
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

#include "ForeignKeyJNI.h"
#include "ForeignKeyBridge.h"

jlong WCDBJNIForeignKeyClassMethodWithNoArg(createCppObject)
{
    return (jlong) WCDBForeignKeyCreate().innerValue;
}

void WCDBJNIForeignKeyObjectMethod(configReferencesTable, jlong self, jstring table)
{
    WCDBJNIGetStringCritical(table);
    WCDBJNIBridgeStruct(CPPForeignKey, self);
    WCDBForeignKeyConfigReferencesTable(selfStruct, tableString);
    WCDBJNIReleaseStringCritical(table);
}

void WCDBJNIForeignKeyObjectMethod(configColumns,
                                   jlong self,
                                   WCDBJNIObjectOrStringArrayParameter(column))
{
    WCDBJNIBridgeStruct(CPPForeignKey, self);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    column, WCDBForeignKeyAddColumn2(selfStruct, column_commonArray);)
}

void WCDBJNIForeignKeyObjectMethod(configOnDeleteAction, jlong self, jint action)
{
    WCDBJNIBridgeStruct(CPPForeignKey, self);
    switch (action) {
    case 0:
        WCDBForeignKeyConfigOnDeleteSetNull(selfStruct);
        break;
    case 1:
        WCDBForeignKeyConfigOnDeleteSetDefault(selfStruct);
        break;
    case 2:
        WCDBForeignKeyConfigOnDeleteCascade(selfStruct);
        break;
    case 3:
        WCDBForeignKeyConfigOnDeleteRestrict(selfStruct);
        break;
    case 4:
        WCDBForeignKeyConfigOnDeleteNoAction(selfStruct);
        break;
    }
}

void WCDBJNIForeignKeyObjectMethod(configOnUpdateAction, jlong self, jint action)
{
    WCDBJNIBridgeStruct(CPPForeignKey, self);
    switch (action) {
    case 0:
        WCDBForeignKeyConfigOnUpdateSetNull(selfStruct);
        break;
    case 1:
        WCDBForeignKeyConfigOnUpdateSetDefault(selfStruct);
        break;
    case 2:
        WCDBForeignKeyConfigOnUpdateCascade(selfStruct);
        break;
    case 3:
        WCDBForeignKeyConfigOnUpdateRestrict(selfStruct);
        break;
    case 4:
        WCDBForeignKeyConfigOnUpdateNoAction(selfStruct);
        break;
    }
}

void WCDBJNIForeignKeyObjectMethod(configMatch, jlong self, jint match)
{
    WCDBJNIBridgeStruct(CPPForeignKey, self);
    WCDBForeignKeyConfigMatch(selfStruct, match);
}

void WCDBJNIForeignKeyObjectMethod(configDeferrable, jlong self, jint type)
{
    WCDBJNIBridgeStruct(CPPForeignKey, self);
    switch (type) {
    case 0:
        WCDBForeignKeyConfigDeferrable(selfStruct);
        break;
    case 1:
        WCDBForeignKeyConfigDeferrableInitiallyDeferred(selfStruct);
        break;
    case 2:
        WCDBForeignKeyConfigDeferrableInitiallyImmediate(selfStruct);
        break;
    }
}

void WCDBJNIForeignKeyObjectMethod(configNotDeferrable, jlong self, jint type)
{
    WCDBJNIBridgeStruct(CPPForeignKey, self);
    switch (type) {
    case 0:
        WCDBForeignKeyConfigNotDeferrable(selfStruct);
        break;
    case 1:
        WCDBForeignKeyConfigNotDeferrableInitiallyDeferred(selfStruct);
        break;
    case 2:
        WCDBForeignKeyConfigNotDeferrableInitiallyImmediate(selfStruct);
        break;
    }
}