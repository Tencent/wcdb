// Created by qiuwenchen on 2023/4/13.
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

#include "StatementSelectJNI.h"
#include "StatementSelectBridge.h"

jlong WCDBJNIStatementSelectClassMethodWithNoArg(create)
{
    return (jlong) WCDBStatementSelectCreate().innerValue;
}

void WCDBJNIStatementSelectClassMethod(configWith, jlong self, jlongArray expressions)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIGetCppPointerArrayCritical(expressions);
    WCDBStatementSelectConfigWith(
    selfStruct, (const CPPCommonTableExpression *) expressionsArray, expressionsLength);
    WCDBJNIReleaseCppPointerArrayCritical(expressions);
}

void WCDBJNIStatementSelectClassMethod(configRecursive, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigRecursive(selfStruct);
}

void WCDBJNIStatementSelectClassMethod(configResultColumns,
                                       jlong self,
                                       WCDBJNIMultiTypeArrayParameter(resultColumns))
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNICreateMultiTypeArray(resultColumns);
    WCDBStatementSelectConfigResultColumns2(selfStruct, resultColumnsArray);
    WCDBJNIReleaseMultiTypeArray(resultColumns);
}

void WCDBJNIStatementSelectClassMethod(configDistiction, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigDistinct(selfStruct);
}

void WCDBJNIStatementSelectClassMethod(configTableOrSubqueries,
                                       jlong self,
                                       WCDBJNIMultiTypeArrayParameter(tableOrSubqueries))
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNICreateMultiTypeArray(tableOrSubqueries);
    WCDBStatementSelectConfigFromTableOrSubqueries2(selfStruct, tableOrSubqueriesArray);
    WCDBJNIReleaseMultiTypeArray(tableOrSubqueries);
}

void WCDBJNIStatementSelectClassMethod(configCondition, jlong self, jlong condition)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIBridgeStruct(CPPExpression, condition);
    WCDBStatementSelectConfigWhere(selfStruct, conditionStruct);
}

void WCDBJNIStatementSelectClassMethod(configGroups,
                                       jlong self,
                                       WCDBJNIMultiTypeArrayParameter(groups))
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNICreateMultiTypeArray(groups);
    WCDBStatementSelectConfigGroups2(selfStruct, groupsArray);
    WCDBJNIReleaseMultiTypeArray(groups);
}

void WCDBJNIStatementSelectClassMethod(configHaving, jlong self, jlong expression)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBStatementSelectConfigHaving(selfStruct, expressionStruct);
}

void WCDBJNIStatementSelectClassMethod(configUnion, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigUnion(selfStruct);
}

void WCDBJNIStatementSelectClassMethod(configUnionAll, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigUnionAll(selfStruct);
}

void WCDBJNIStatementSelectClassMethod(configIntersect, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigIntersect(selfStruct);
}

void WCDBJNIStatementSelectClassMethod(configExcept, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigExcept(selfStruct);
}

void WCDBJNIStatementSelectClassMethod(configOrders, jlong self, jlongArray orders)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIGetCppPointerArrayCritical(orders);
    WCDBStatementSelectConfigOrders(
    selfStruct, (const CPPOrderingTerm *) ordersArray, ordersLength);
    WCDBJNIReleaseCppPointerArrayCritical(orders);
}

void WCDBJNIStatementSelectClassMethod(
configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    CPPCommonValue from_common;
    from_common.type = fromType;
    from_common.intValue = from;
    CPPCommonValue to_common;
    to_common.type = toType;
    to_common.intValue = to;
    WCDBStatementSelectConfigLimitRange2(selfStruct, from_common, to_common);
}

void WCDBJNIStatementSelectClassMethod(configLimitCount, jlong self, jint type, jlong limit)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    CPPCommonValue limit_common;
    limit_common.type = type;
    limit_common.intValue = limit;
    WCDBStatementSelectConfigLimitCount2(selfStruct, limit_common);
}

void WCDBJNIStatementSelectClassMethod(configOffset, jlong self, jint type, jlong offset)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    CPPCommonValue offset_common;
    offset_common.type = type;
    offset_common.intValue = offset;
    WCDBStatementSelectConfigOffset2(selfStruct, offset_common);
}
