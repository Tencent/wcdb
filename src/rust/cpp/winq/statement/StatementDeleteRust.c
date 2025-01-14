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

#include "StatementDeleteRust.h"
#include "StatementDeleteBridge.h"

void *WCDBRustStatementDeleteClassMethodWithNoArg(create) {
    return (void *) WCDBStatementDeleteCreate().innerValue;
}

//void WCDBRustStatementDeleteClassMethod(configWith, jlong self, jlongArray expressions)
//{
//    WCDBRustBridgeStruct(CPPStatementDelete, self);
//    WCDBRustGetCppPointerArrayCritical(expressions);
//    WCDBStatementDeleteConfigWith(
//    selfStruct, (const CPPCommonTableExpression *) expressionsArray, expressionsLength);
//    WCDBRustReleaseCppPointerArrayCritical(expressions);
//}
//
//void WCDBRustStatementDeleteClassMethod(configRecursive, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementDelete, self);
//    WCDBStatementDeleteConfigRecursive(selfStruct);
//}

void WCDBRustStatementDeleteClassMethod(configTable, void *self, WCDBRustObjectOrStringParameter(table)) {
    WCDBRustBridgeStruct(CPPStatementDelete, self);
    WCDBRustCreateObjectOrStringCommonValue(table, true);
    WCDBStatementDeleteConfigDeleteFrom2(selfStruct, table_common);
}

void WCDBRustStatementDeleteClassMethod(configCondition, void *self, void *condition) {
    WCDBRustBridgeStruct(CPPStatementDelete, self);
    WCDBRustBridgeStruct(CPPExpression, condition);
    WCDBStatementDeleteConfigWhere(selfStruct, conditionStruct);
}

void WCDBRustStatementDeleteClassMethod(configOrders, void *self, void **orders, size_t len) {
    WCDBRustBridgeStruct(CPPStatementDelete, self);
//    WCDBRustGetCppPointerArrayCritical(orders, len);
    WCDBStatementDeleteConfigOrder(
            selfStruct, (const CPPOrderingTerm *) orders, len);
//    WCDBRustReleaseCppPointerArrayCritical(orders);
}

//void WCDBRustStatementDeleteClassMethod(
//configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to)
//{
//    WCDBRustBridgeStruct(CPPStatementDelete, self);
//    CPPCommonValue from_common;
//    from_common.type = fromType;
//    from_common.intValue = from;
//    CPPCommonValue to_common;
//    to_common.type = toType;
//    to_common.intValue = to;
//    WCDBStatementDeleteConfigLimitRange2(selfStruct, from_common, to_common);
//}

void WCDBRustStatementDeleteClassMethod(configLimitCount, void *self, int type, long limit) {
    WCDBRustBridgeStruct(CPPStatementDelete, self);
    CPPCommonValue limit_common;
    limit_common.type = type;
    limit_common.intValue = limit;
    WCDBStatementDeleteConfigLimitCount2(selfStruct, limit_common);
}

//void WCDBRustStatementDeleteClassMethod(configOffset, jlong self, jint type, jlong offset)
//{
//    WCDBRustBridgeStruct(CPPStatementDelete, self);
//    CPPCommonValue offset_common;
//    offset_common.type = type;
//    offset_common.intValue = offset;
//    WCDBStatementDeleteConfigOffset2(selfStruct, offset_common);
//}
