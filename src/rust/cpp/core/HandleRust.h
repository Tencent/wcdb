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

#define WCDBRustHandleFuncName(funcName) WCDBRust(Handle, funcName)
#define WCDBRustHandleObjectMethod(funcName, ...) \
    WCDBRustObjectMethod(Handle, funcName, __VA_ARGS__)
#define WCDBRustHandleObjectMethodWithNoArg(funcName) \
    WCDBRustObjectMethodWithNoArg(Handle, funcName)
#define WCDBRustHandleClassMethodWithNoArg(funcName) WCDBRustClassMethodWithNoArg(Handle, funcName)
#define WCDBRustHandleClassMethod(funcName, ...) WCDBRustClassMethod(Handle, funcName, __VA_ARGS__)

void* WCDBRustHandleClassMethod(getError, void* self);

// jlong WCDBRustHandleClassMethod(getOrCreatePreparedStatement, void* self, jlong statement);
// jlong WCDBRustHandleClassMethod(getOrCreatePreparedStatementWithSQL, void* self, jstring sql);
void* WCDBRustHandleClassMethod(getMainStatement, void* self);
// void WCDBRustHandleClassMethod(finalizeAllStatements, void* self);
bool WCDBRustHandleClassMethod(execute, void* self, void* statement);

bool WCDBRustHandleClassMethod(executeSQL, void* self, const char* sql);
// jint WCDBRustHandleClassMethod(tableExist, void* self, jstring table);
//
int WCDBRustHandleClassMethod(getChanges, void* self);

// jint WCDBRustHandleClassMethod(getTotalChanges, void* self);
long long WCDBRustHandleClassMethod(getLastInsertRowid, void* self);
//
// jboolean WCDBRustHandleClassMethod(isInTransaction, void* self);
// jboolean WCDBRustHandleClassMethod(beginTransaction, void* self);
// jboolean WCDBRustHandleClassMethod(commitTransaction, void* self);
// void WCDBRustHandleClassMethod(rollbackTransaction, void* self);

typedef bool (*RustTransactionCallback)(void* closure_raw, void* database_raw, void* cpp_handle);

bool WCDBRustHandleObjectMethod(runTransaction,
                                void* self,
                                RustTransactionCallback rust_callback,
                                void* closure_raw,
                                void* database_raw);
// jboolean WCDBRustHandleObjectMethod(runPausableTransaction, void* self, jobject transaction);
//
// jlong WCDBRustHandleClassMethodWithNoArg(createCancellationSignal);
// void WCDBRustHandleClassMethod(cancelSignal, jlong signal);
//
// void WCDBRustHandleClassMethod(attachCancellationSignal, void* self, jlong signal);
// void WCDBRustHandleClassMethod(detachCancellationSignal, void* self);
