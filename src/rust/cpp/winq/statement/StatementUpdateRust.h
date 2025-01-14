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

#define WCDBRustStatementUpdateFuncName(funcName)                               \
    WCDBRust(StatementUpdate, funcName)
#define WCDBRustStatementUpdateObjectMethod(funcName, ...)                      \
    WCDBRustObjectMethod(StatementUpdate, funcName, __VA_ARGS__)
#define WCDBRustStatementUpdateObjectMethodWithNoArg(funcName)                  \
    WCDBRustObjectMethodWithNoArg(StatementUpdate, funcName)
#define WCDBRustStatementUpdateClassMethodWithNoArg(funcName)                   \
    WCDBRustClassMethodWithNoArg(StatementUpdate, funcName)
#define WCDBRustStatementUpdateClassMethod(funcName, ...)                       \
    WCDBRustClassMethod(StatementUpdate, funcName, __VA_ARGS__)

void *WCDBRustStatementUpdateClassMethodWithNoArg(create);

//void WCDBRustStatementUpdateClassMethod(configWith, jlong self, jlongArray expressions);
//void WCDBRustStatementUpdateClassMethod(configRecursive, jlong self);
//
void WCDBRustStatementUpdateClassMethod(configTable,
                                        void *self,
                                        WCDBRustObjectOrStringParameter(table));

//void WCDBRustStatementUpdateClassMethod(configConfliction, jlong self, jint action);
//void WCDBRustStatementUpdateClassMethod(configColumns,
//                                       jlong self,
//                                       WCDBRustObjectOrStringArrayParameter(columns));
//void WCDBRustStatementUpdateClassMethod(configValue, jlong self, WCDBRustCommonValueParameter(value));
//void WCDBRustStatementUpdateClassMethod(configColumnsToValues,
//                                       jlong self,
//                                       WCDBRustObjectOrStringArrayParameter(columns),
//                                       WCDBRustMultiTypeArrayParameter(values));
void WCDBRustStatementUpdateClassMethod(configColumnsToBindParameters,
                                        void *self,
                                        WCDBRustObjectOrStringArrayParameter(columns));
//void WCDBRustStatementUpdateClassMethod(configCondition, jlong self, jlong condition);
//void WCDBRustStatementUpdateClassMethod(configOrders, jlong self, jlongArray orders);
//void WCDBRustStatementUpdateClassMethod(
//configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to);
//void WCDBRustStatementUpdateClassMethod(configLimitCount, jlong self, jint type, jlong limit);
//void WCDBRustStatementUpdateClassMethod(configOffset, jlong self, jint type, jlong offset);
