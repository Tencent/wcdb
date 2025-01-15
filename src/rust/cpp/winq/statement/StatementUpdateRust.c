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

#include "StatementUpdateRust.h"
#include "StatementUpdateBridge.h"

void *WCDBRustStatementUpdateClassMethodWithNoArg(create) {
    return (void *) WCDBStatementUpdateCreate().innerValue;
}

//void WCDBRustStatementUpdateClassMethod(configWith, jlong self, jlongArray expressions)
//{
//    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    WCDBRustGetCppPointerArrayCritical(expressions);
//    WCDBStatementUpdateConfigWith(
//    selfStruct, (const CPPCommonTableExpression*) expressionsArray, expressionsLength);
//    WCDBRustReleaseCppPointerArrayCritical(expressions);
//}
//
//void WCDBRustStatementUpdateClassMethod(configRecursive, jlong self)
//{
//    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    WCDBStatementUpdateConfigRecursive(selfStruct);
//}
//
void WCDBRustStatementUpdateClassMethod(configTable, void *self, WCDBRustObjectOrStringParameter(table)) {
    WCDBRustBridgeStruct(CPPStatementUpdate, self);
    WCDBRustCreateObjectOrStringCommonValue(table, true);
    WCDBStatementUpdateConfigTable2(selfStruct, table_common);
}

//
//void WCDBRustStatementUpdateClassMethod(configConfliction, jlong self, jint action)
//{
//    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    WCDBStatementUpdateConfigConfiction(selfStruct, action);
//}
//
//void WCDBRustStatementUpdateClassMethod(configColumns,
//                                       jlong self,
//                                       WCDBRustObjectOrStringArrayParameter(columns))
//{
//    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
//    columns, WCDBStatementUpdateConfigColumns2(selfStruct, columns_commonArray));
//}
//
//void WCDBRustStatementUpdateClassMethod(configValue, jlong self, WCDBRustCommonValueParameter(value))
//{
//    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    WCDBRustCreateCommonValue(value, true);
//    WCDBStatementUpdateConfigValue2(selfStruct, value_common);
//    WCDBRustTryReleaseStringInCommonValue(value);
//}
//
//void WCDBRustStatementUpdateClassMethod(configColumnsToValues,
//                                       jlong self,
//                                       WCDBRustObjectOrStringArrayParameter(columns),
//                                       WCDBRustMultiTypeArrayParameter(values))
//{
//    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    WCDBRustCreateMultiTypeArray(values);
//    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
//    columns, WCDBStatementUpdateConfigColumnsToValues(selfStruct, columns_commonArray, valuesArray));
//    WCDBRustReleaseMultiTypeArray(values);
//}
//
void WCDBRustStatementUpdateClassMethod(configColumnsToBindParameters,
                                        void *self,
                                        WCDBRustObjectOrStringArrayParameter(columns)) {
    WCDBRustBridgeStruct(CPPStatementUpdate, self);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
            columns, WCDBStatementUpdateConfigColumnsToBindParameters(selfStruct, columns_commonArray));
}

void WCDBRustStatementUpdateClassMethod(configCondition, void* self, void* condition)
{
    WCDBRustBridgeStruct(CPPStatementUpdate, self);
    WCDBRustBridgeStruct(CPPExpression, condition);
    WCDBStatementUpdateConfigCondition(selfStruct, conditionStruct);
}

void WCDBRustStatementUpdateClassMethod(configOrders, void* self, void** orders, size_t len)
{
    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    WCDBRustGetCppPointerArrayCritical(orders);
    WCDBStatementUpdateConfigOrders(
    selfStruct, (const CPPOrderingTerm*) orders, (int)len);
//    WCDBRustReleaseCppPointerArrayCritical(orders);
}

//void WCDBRustStatementUpdateClassMethod(
//configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to)
//{
//    WCDBRustBridgeStruct(CPPStatementUpdate, self);
//    CPPCommonValue from_common;
//    from_common.type = fromType;
//    from_common.intValue = from;
//    CPPCommonValue to_common;
//    to_common.type = toType;
//    to_common.intValue = to;
//    WCDBStatementUpdateConfigLimitRange2(selfStruct, from_common, to_common);
//}

void WCDBRustStatementUpdateClassMethod(configLimitCount, void* self, int type, long limit)
{
    WCDBRustBridgeStruct(CPPStatementUpdate, self);
    CPPCommonValue limit_common;
    limit_common.type = type;
    limit_common.intValue = limit;
    WCDBStatementUpdateConfigLimitCount2(selfStruct, limit_common);
}

void WCDBRustStatementUpdateClassMethod(configOffset, void* self, int type, long offset)
{
    WCDBRustBridgeStruct(CPPStatementUpdate, self);
    CPPCommonValue offset_common;
    offset_common.type = type;
    offset_common.intValue = offset;
    WCDBStatementUpdateConfigOffset2(selfStruct, offset_common);
}
