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

jlong WCDBJNIStatementSelectObjectMethodWithNoArg(create)
{
    return (jlong) WCDBStatementSelectCreate().innerValue;
}

void WCDBJNIStatementSelectObjectMethod(configWith, jlong self, jlongArray expressions)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIGetCppPointerArray(expressions);
    WCDBStatementSelectConfigWith(
    selfStruct, (const CPPCommonTableExpression*) expressionsArray, expressionsLength);
    WCDBJNIReleaseCppPointerArray(expressions);
}

void WCDBJNIStatementSelectObjectMethod(configRecursive, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigRecursive(selfStruct);
}

void WCDBJNIStatementSelectObjectMethod(configResultColumns,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(resultColumns))
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNICreateMultiTypeArray(resultColumns);
    WCDBStatementSelectConfigResultColumns2(selfStruct, resultColumnsArray);
    WCDBJNIReleaseMultiTypeArray(resultColumns);
}

void WCDBJNIStatementSelectObjectMethod(configDistiction, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigDistinct(selfStruct);
}

void WCDBJNIStatementSelectObjectMethod(configTableOrSubqueries,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(tableOrSubqueries))
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNICreateMultiTypeArray(tableOrSubqueries);
    WCDBStatementSelectConfigFromTableOrSubqueries2(selfStruct, tableOrSubqueriesArray);
    WCDBJNIReleaseMultiTypeArray(tableOrSubqueries);
}

void WCDBJNIStatementSelectObjectMethod(configCondition, jlong self, jlong condition)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIBridgeStruct(CPPExpression, condition);
    WCDBStatementSelectConfigWhere(selfStruct, conditionStruct);
}

void WCDBJNIStatementSelectObjectMethod(configGroups,
                                        jlong self,
                                        WCDBJNIMultiTypeArrayParameter(groups))
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNICreateMultiTypeArray(groups);
    WCDBStatementSelectConfigGroups2(selfStruct, groupsArray);
    WCDBJNIReleaseMultiTypeArray(groups);
}

void WCDBJNIStatementSelectObjectMethod(configHaving, jlong self, jlong expression)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIBridgeStruct(CPPExpression, expression);
    WCDBStatementSelectConfigHaving(selfStruct, expressionStruct);
}

void WCDBJNIStatementSelectObjectMethod(configUnion, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigUnion(selfStruct);
}

void WCDBJNIStatementSelectObjectMethod(configUnionAll, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigUnionAll(selfStruct);
}

void WCDBJNIStatementSelectObjectMethod(configIntersect, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigIntersect(selfStruct);
}

void WCDBJNIStatementSelectObjectMethod(configExcept, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBStatementSelectConfigExcept(selfStruct);
}

void WCDBJNIStatementSelectObjectMethod(configOrders, jlong self, jlongArray orders)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    WCDBJNIGetCppPointerArray(orders);
    WCDBStatementSelectConfigOrders(
    selfStruct, (const CPPOrderingTerm*) ordersArray, ordersLength);
    WCDBJNIReleaseCppPointerArray(orders);
}

void WCDBJNIStatementSelectObjectMethod(
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

void WCDBJNIStatementSelectObjectMethod(configLimitCount, jlong self, jint type, jlong limit)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    CPPCommonValue limit_common;
    limit_common.type = type;
    limit_common.intValue = limit;
    WCDBStatementSelectConfigLimitCount2(selfStruct, limit_common);
}

void WCDBJNIStatementSelectObjectMethod(configOffset, jlong self, jint type, jlong offset)
{
    WCDBJNIBridgeStruct(CPPStatementSelect, self);
    CPPCommonValue offset_common;
    offset_common.type = type;
    offset_common.intValue = offset;
    WCDBStatementSelectConfigOffset2(selfStruct, offset_common);
}
