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

#include "JoinRust.h"

#include "JoinBridge.h"

void* WCDBRustJoinClassMethod(createCppObj, WCDBRustObjectOrStringParameter(query)) {
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    void* ret = (void*)WCDBJoinCreateWithTableOrSubquery2(query_common).innerValue;
    return ret;
}

void WCDBRustJoinClassMethod(configWith, void* join, WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWith2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithJoin, void* join, WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithLeftOuterJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithLeftOuterJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithLeftJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithLeftJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithInnerJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithInnerJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithCrossJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithCrossJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithNaturalJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithNaturalJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithNaturalLeftOuterJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithNaturalLeftOuterJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithNaturalLeftJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithNaturalLeftJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithNaturalInnerJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithNaturalInnerJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configWithNaturalCrossJoin,
                             void* join,
                             WCDBRustObjectOrStringParameter(query)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringCommonValue(query, true);
    WCDBJoinWithNaturalCrossJoin2(joinStruct, query_common);
}

void WCDBRustJoinClassMethod(configOn, void* join, void* expression) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustBridgeStruct(CPPExpression, expression);
    WCDBJoinConfigOn(joinStruct, expressionStruct);
}

void WCDBRustJoinClassMethod(configUsingColumn,
                             void* join,
                             WCDBRustObjectOrStringArrayParameter(columns)) {
    WCDBRustBridgeStruct(CPPJoin, join);
    WCDBRustCreateObjectOrStringArrayCriticalWithAction(
        columns, WCDBJoinConfigUsingColumn2(joinStruct, columns_commonArray));
}