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

#include "StatementSelectRust.h"
#include "StatementSelectBridge.h"

void* WCDBRustStatementSelectClassMethodWithNoArg(create)
{
    return (void*) WCDBStatementSelectCreate().innerValue;
}

//void WCDBRustStatementSelectClassMethod(configWith, jlong self, jlongArray expressions)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBRustGetCppPointerArrayCritical(expressions);
//    WCDBStatementSelectConfigWith(
//    selfStruct, (const CPPCommonTableExpression*) expressionsArray, expressionsLength);
//    WCDBRustReleaseCppPointerArrayCritical(expressions);
//}
//
//void WCDBRustStatementSelectClassMethod(configRecursive, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBStatementSelectConfigRecursive(selfStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configResultColumns,
//                                       jlong self,
//                                       WCDBRustMultiTypeArrayParameter(resultColumns))
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBRustCreateMultiTypeArray(resultColumns);
//    WCDBStatementSelectConfigResultColumns2(selfStruct, resultColumnsArray);
//    WCDBRustReleaseMultiTypeArray(resultColumns);
//}
//
//void WCDBRustStatementSelectClassMethod(configDistiction, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBStatementSelectConfigDistinct(selfStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configTableOrSubqueries,
//                                       jlong self,
//                                       WCDBRustMultiTypeArrayParameter(tableOrSubqueries))
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBRustCreateMultiTypeArray(tableOrSubqueries);
//    WCDBStatementSelectConfigFromTableOrSubqueries2(selfStruct, tableOrSubqueriesArray);
//    WCDBRustReleaseMultiTypeArray(tableOrSubqueries);
//}
//
//void WCDBRustStatementSelectClassMethod(configCondition, jlong self, jlong condition)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBRustBridgeStruct(CPPExpression, condition);
//    WCDBStatementSelectConfigWhere(selfStruct, conditionStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configGroups,
//                                       jlong self,
//                                       WCDBRustMultiTypeArrayParameter(groups))
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBRustCreateMultiTypeArray(groups);
//    WCDBStatementSelectConfigGroups2(selfStruct, groupsArray);
//    WCDBRustReleaseMultiTypeArray(groups);
//}
//
//void WCDBRustStatementSelectClassMethod(configHaving, jlong self, jlong expression)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBRustBridgeStruct(CPPExpression, expression);
//    WCDBStatementSelectConfigHaving(selfStruct, expressionStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configUnion, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBStatementSelectConfigUnion(selfStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configUnionAll, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBStatementSelectConfigUnionAll(selfStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configIntersect, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBStatementSelectConfigIntersect(selfStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configExcept, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBStatementSelectConfigExcept(selfStruct);
//}
//
//void WCDBRustStatementSelectClassMethod(configOrders, jlong self, jlongArray orders)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    WCDBRustGetCppPointerArrayCritical(orders);
//    WCDBStatementSelectConfigOrders(
//    selfStruct, (const CPPOrderingTerm*) ordersArray, ordersLength);
//    WCDBRustReleaseCppPointerArrayCritical(orders);
//}
//
//void WCDBRustStatementSelectClassMethod(
//configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    CPPCommonValue from_common;
//    from_common.type = fromType;
//    from_common.intValue = from;
//    CPPCommonValue to_common;
//    to_common.type = toType;
//    to_common.intValue = to;
//    WCDBStatementSelectConfigLimitRange2(selfStruct, from_common, to_common);
//}
//
//void WCDBRustStatementSelectClassMethod(configLimitCount, jlong self, jint type, jlong limit)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    CPPCommonValue limit_common;
//    limit_common.type = type;
//    limit_common.intValue = limit;
//    WCDBStatementSelectConfigLimitCount2(selfStruct, limit_common);
//}
//
//void WCDBRustStatementSelectClassMethod(configOffset, jlong self, jint type, jlong offset)
//{
//    WCDBRustBridgeStruct(CPPStatementSelect, self);
//    CPPCommonValue offset_common;
//    offset_common.type = type;
//    offset_common.intValue = offset;
//    WCDBStatementSelectConfigOffset2(selfStruct, offset_common);
//}
