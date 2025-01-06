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

#define WCDBRustStatementDeleteFuncName(funcName)                               \
    WCDBRust(StatementDelete, funcName)
#define WCDBRustStatementDeleteObjectMethod(funcName, ...)                      \
    WCDBRustObjectMethod(StatementDelete, funcName, __VA_ARGS__)
#define WCDBRustStatementDeleteObjectMethodWithNoArg(funcName)                  \
    WCDBRustObjectMethodWithNoArg(StatementDelete, funcName)
#define WCDBRustStatementDeleteClassMethodWithNoArg(funcName)                   \
    WCDBRustClassMethodWithNoArg(StatementDelete, funcName)
#define WCDBRustStatementDeleteClassMethod(funcName, ...)                       \
    WCDBRustClassMethod(StatementDelete, funcName, __VA_ARGS__)

void* WCDBRustStatementDeleteClassMethodWithNoArg(create);

//void WCDBRustStatementDeleteClassMethod(configWith, jlong self, jlongArray expressions);
//void WCDBRustStatementDeleteClassMethod(configRecursive, jlong self);
//
void WCDBRustStatementDeleteClassMethod(configTable,
                                       void* self,
                                       WCDBRustObjectOrStringParameter(table));
void WCDBRustStatementDeleteClassMethod(configCondition, void* self, void* condition);
void WCDBRustStatementDeleteClassMethod(configOrders, void* self, void** orders, size_t len);
//void WCDBRustStatementDeleteClassMethod(
//configLimitRange, jlong self, jint fromType, jlong from, jint toType, jlong to);
void WCDBRustStatementDeleteClassMethod(configLimitCount, void* self, int type, long limit);
//void WCDBRustStatementDeleteClassMethod(configOffset, jlong self, jint type, jlong offset);
