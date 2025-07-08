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

jlong WCDBJNIStatementUpdateClassMethodWithNoArg(create)
{
    return (jlong) WCDBStatementUpdateCreate().innerValue;
}

void WCDBJNIStatementUpdateClassMethod(configWith, jlong self, jlongArray expressions)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNIGetCppPointerArrayCritical(expressions);
    WCDBStatementUpdateConfigWith(
    selfStruct, (const CPPCommonTableExpression *) expressionsArray, expressionsLength);
    WCDBJNIReleaseCppPointerArrayCritical(expressions);
}

void WCDBJNIStatementUpdateClassMethod(configRecursive, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBStatementUpdateConfigRecursive(selfStruct);
}

void WCDBJNIStatementUpdateClassMethod(configTable, jlong self, WCDBJNIObjectOrStringParameter(table))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateObjectOrStringCommonValue(table, true);
    WCDBStatementUpdateConfigTable2(selfStruct, table_common);
    WCDBJNITryReleaseStringInCommonValue(table);
}

void WCDBJNIStatementUpdateClassMethod(configConfliction, jlong self, jint action)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBStatementUpdateConfigConfiction(selfStruct, action);
}

void WCDBJNIStatementUpdateClassMethod(configColumns,
                                       jlong self,
                                       WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    columns, WCDBStatementUpdateConfigColumns2(selfStruct, columns_commonArray));
}

void WCDBJNIStatementUpdateClassMethod(configValue, jlong self, WCDBJNICommonValueParameter(value))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateCommonValue(value, true);
    WCDBStatementUpdateConfigValue2(selfStruct, value_common);
    WCDBJNITryReleaseStringInCommonValue(value);
}

void WCDBJNIStatementUpdateClassMethod(configColumnsToValues,
                                       jlong self,
                                       WCDBJNIObjectOrStringArrayParameter(columns),
                                       WCDBJNIMultiTypeArrayParameter(values))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateMultiTypeArray(values);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    columns, WCDBStatementUpdateConfigColumnsToValues(selfStruct, columns_commonArray, valuesArray));
    WCDBJNIReleaseMultiTypeArray(values);
}

void WCDBJNIStatementUpdateClassMethod(configColumnsWithBindParameter,
                                       jlong self,
                                       WCDBJNIObjectOrStringArrayParameter(columns))
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNICreateObjectOrStringArrayCriticalWithAction(
    columns, WCDBStatementUpdateConfigColumnsToBindParameters(selfStruct, columns_commonArray));
}

void WCDBJNIStatementUpdateClassMethod(configCondition, jlong self, jlong condition)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNIBridgeStruct(CPPExpression, condition);
    WCDBStatementUpdateConfigCondition(selfStruct, conditionStruct);
}

void WCDBJNIStatementUpdateClassMethod(configOrders, jlong self, jlongArray orders)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    WCDBJNIGetCppPointerArrayCritical(orders);
    WCDBStatementUpdateConfigOrders(
    selfStruct, (const CPPOrderingTerm *) ordersArray, ordersLength);
    WCDBJNIReleaseCppPointerArrayCritical(orders);
}

void WCDBJNIStatementUpdateClassMethod(
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

void WCDBJNIStatementUpdateClassMethod(configLimitCount, jlong self, jint type, jlong limit)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    CPPCommonValue limit_common;
    limit_common.type = type;
    limit_common.intValue = limit;
    WCDBStatementUpdateConfigLimitCount2(selfStruct, limit_common);
}

void WCDBJNIStatementUpdateClassMethod(configOffset, jlong self, jint type, jlong offset)
{
    WCDBJNIBridgeStruct(CPPStatementUpdate, self);
    CPPCommonValue offset_common;
    offset_common.type = type;
    offset_common.intValue = offset;
    WCDBStatementUpdateConfigOffset2(selfStruct, offset_common);
}
