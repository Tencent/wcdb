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

#pragma once

#include "WCDBRust.h"

#define WCDBRustStatementSelectFuncName(funcName) WCDBRust(StatementSelect, funcName)
#define WCDBRustStatementSelectObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(StatementSelect, funcName, __VA_ARGS__)
#define WCDBRustStatementSelectObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(StatementSelect, funcName)
#define WCDBRustStatementSelectClassMethodWithNoArg(funcName) \
    WCDBRustClassMethodWithNoArg(StatementSelect, funcName)
#define WCDBRustStatementSelectClassMethod(funcName, ...) \
    WCDBRustClassMethod(StatementSelect, funcName, __VA_ARGS__)

void* WCDBRustStatementSelectClassMethodWithNoArg(create);

// void WCDBRustStatementSelectClassMethod(configWith, jlong self, jlongArray expressions);
// void WCDBRustStatementSelectClassMethod(configRecursive, jlong self);
//
void WCDBRustStatementSelectClassMethod(configResultColumns,
                                        void* self,
                                        WCDBRustMultiTypeArrayParameter(resultColumns));

// void WCDBRustStatementSelectClassMethod(configDistiction, jlong self);
void WCDBRustStatementSelectClassMethod(configTableOrSubqueries,
                                        void* self,
                                        WCDBRustMultiTypeArrayParameter(tableOrSubqueries));

void WCDBRustStatementSelectClassMethod(configCondition, void* self, void* condition);
// void WCDBRustStatementSelectClassMethod(configGroups,
//                                        jlong self,
//                                        WCDBRustMultiTypeArrayParameter(groups));
// void WCDBRustStatementSelectClassMethod(configHaving, jlong self, jlong expression);
// void WCDBRustStatementSelectClassMethod(configUnion, jlong self);
// void WCDBRustStatementSelectClassMethod(configUnionAll, jlong self);
// void WCDBRustStatementSelectClassMethod(configIntersect, jlong self);
// void WCDBRustStatementSelectClassMethod(configExcept, jlong self);
// void WCDBRustStatementSelectClassMethod(configOrders, jlong self, jlongArray orders);
// void WCDBRustStatementSelectClassMethod(
// configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to);
// void WCDBRustStatementSelectClassMethod(configLimitCount, jlong self, jint type, jlong limit);
// void WCDBRustStatementSelectClassMethod(configOffset, jlong self, jint type, jlong offset);
