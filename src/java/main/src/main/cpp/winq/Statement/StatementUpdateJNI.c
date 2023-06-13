// Created by chenqiuwen on 2023/4/13.
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

#include "StatementUpdateJNI.h"
#include "StatementUpdateBridge.h"

jlong WCDBJNIStatementUpdateObjectMethodWithNoArg(create)
{
    return (jlong) WCDBStatementUpdateCreate().innerValue;
}

void WCDBJNIStatementUpdateObjectMethod(configWith, jlong self, jlongArray expressions)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNIGetCppPointerArray(expressions);
    WCDBStatementUpdateConfigWith(
    selfStruct, (const CPPCommonTableExpression*) expressionsArray, expressionsLength);
    WCDBJNIReleaseCppPointerArray(expressions);
}

void WCDBJNIStatementUpdateObjectMethod(configRecursive, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBStatementUpdateConfigRecursive(selfStruct);
}

void WCDBJNIStatementUpdateObjectMethod(configTable, jlong self, WCDBJNIObjectOrStringParameter(table))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateObjectOrStringCommonValue(table);
    WCDBStatementUpdateConfigTable2(selfStruct, table_common);
    WCDBJNITryReleaseStringInCommonValue(table);
}

void WCDBJNIStatementUpdateObjectMethod(configConfliction, jlong self, jint action)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBStatementUpdateConfigConfiction(selfStruct, action);
}

void WCDBJNIStatementUpdateObjectMethod(configColumns,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateObjectOrStringArrayWithAction(
    columns, WCDBStatementUpdateConfigColumns2(selfStruct, columns_commonArray));
}

void WCDBJNIStatementUpdateObjectMethod(configValue, jlong self, WCDBJNICommonValueParameter(value))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateCommonValue(value);
    WCDBStatementUpdateConfigValue2(selfStruct, value_common);
    WCDBJNITryReleaseStringInCommonValue(value);
}

void WCDBJNIStatementUpdateObjectMethod(configColumnsToValues,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns),
                                        WCDBJNIMultiTypeArrayParameter(values))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateMultiTypeArray(values);
    WCDBJNICreateObjectOrStringArrayWithAction(
    columns, WCDBStatementUpdateConfigColumnsToValues(selfStruct, columns_commonArray, valuesArray));
    WCDBJNIReleaseMultiTypeArray(values);
}

void WCDBJNIStatementUpdateObjectMethod(configColumnsWithBindParameter,
                                        jlong self,
                                        WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateObjectOrStringArrayWithAction(
    columns, WCDBStatementUpdateConfigColumnsToBindParameters(selfStruct, columns_commonArray));
}

void WCDBJNIStatementUpdateObjectMethod(configCondition, jlong self, jlong condition)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNIBridgeStruct(CPPExpression, condition);
    WCDBStatementUpdateConfigCondition(selfStruct, conditionStruct);
}

void WCDBJNIStatementUpdateObjectMethod(configOrders, jlong self, jlongArray orders)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNIGetCppPointerArray(orders);
    WCDBStatementUpdateConfigOrders(
    selfStruct, (const CPPOrderingTerm*) ordersArray, ordersLength);
    WCDBJNIReleaseCppPointerArray(orders);
}

void WCDBJNIStatementUpdateObjectMethod(
configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    CPPCommonValue from_common;
    from_common.type = fromType;
    from_common.intValue = from;
    CPPCommonValue to_common;
    to_common.type = toType;
    to_common.intValue = to;
    WCDBStatementUpdateConfigLimitRange2(selfStruct, from_common, to_common);
}

void WCDBJNIStatementUpdateObjectMethod(configLimitCount, jlong self, jint type, jlong limit)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    CPPCommonValue limit_common;
    limit_common.type = type;
    limit_common.intValue = limit;
    WCDBStatementUpdateConfigLimitCount2(selfStruct, limit_common);
}

void WCDBJNIStatementUpdateObjectMethod(configOffset, jlong self, jint type, jlong offset)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    CPPCommonValue offset_common;
    offset_common.type = type;
    offset_common.intValue = offset;
    WCDBStatementUpdateConfigOffset2(selfStruct, offset_common);
}
