// Created by qiuwenchen on 2023/4/11.
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

#include "StatementDeleteJNI.h"
#include "StatementDeleteBridge.h"

jlong WCDBJNIStatementDeleteObjectMethodWithNoArg(create)
{
    return (jlong) WCDBStatementDeleteCreate().innerValue;
}

void WCDBJNIStatementDeleteObjectMethod(configWith, jlong self, jlongArray expressions)
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    WCDBJNIGetCppPointerArrayCritical(expressions);
    WCDBStatementDeleteConfigWith(
    selfStruct, (const CPPCommonTableExpression *) expressionsArray, expressionsLength);
    WCDBJNIReleaseCppPointerArrayCritical(expressions);
}

void WCDBJNIStatementDeleteObjectMethod(configRecursive, jlong self)
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    WCDBStatementDeleteConfigRecursive(selfStruct);
}

void WCDBJNIStatementDeleteObjectMethod(configTable, jlong self, WCDBJNIObjectOrStringParameter(table))
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    WCDBJNICreateObjectOrStringCommonValue(table, true);
    WCDBStatementDeleteConfigDeleteFrom2(selfStruct, table_common);
    WCDBJNITryReleaseStringInCommonValue(table);
}

void WCDBJNIStatementDeleteObjectMethod(configCondition, jlong self, jlong condition)
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    WCDBJNIBridgeStruct(CPPExpression, condition);
    WCDBStatementDeleteConfigWhere(selfStruct, conditionStruct);
}

void WCDBJNIStatementDeleteObjectMethod(configOrders, jlong self, jlongArray orders)
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    WCDBJNIGetCppPointerArrayCritical(orders);
    WCDBStatementDeleteConfigOrder(
    selfStruct, (const CPPOrderingTerm *) ordersArray, ordersLength);
    WCDBJNIReleaseCppPointerArrayCritical(orders);
}

void WCDBJNIStatementDeleteObjectMethod(
configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to)
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    CPPCommonValue from_common;
    from_common.type = fromType;
    from_common.intValue = from;
    CPPCommonValue to_common;
    to_common.type = toType;
    to_common.intValue = to;
    WCDBStatementDeleteConfigLimitRange2(selfStruct, from_common, to_common);
}

void WCDBJNIStatementDeleteObjectMethod(configLimitCount, jlong self, jint type, jlong limit)
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    CPPCommonValue limit_common;
    limit_common.type = type;
    limit_common.intValue = limit;
    WCDBStatementDeleteConfigLimitCount2(selfStruct, limit_common);
}

void WCDBJNIStatementDeleteObjectMethod(configOffset, jlong self, jint type, jlong offset)
{
    WCDBJNIBridgeStruct(CPPStatementDelete, self);
    CPPCommonValue offset_common;
    offset_common.type = type;
    offset_common.intValue = offset;
    WCDBStatementDeleteConfigOffset2(selfStruct, offset_common);
}
