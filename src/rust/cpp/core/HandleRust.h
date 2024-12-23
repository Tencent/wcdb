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
#define WCDBRustHandleObjectMethod(funcName, ...)                               \
    WCDBRustObjectMethod(Handle, funcName, __VA_ARGS__)
#define WCDBRustHandleObjectMethodWithNoArg(funcName)                           \
    WCDBRustObjectMethodWithNoArg(Handle, funcName)
#define WCDBRustHandleClassMethodWithNoArg(funcName)                            \
    WCDBRustClassMethodWithNoArg(Handle, funcName)
#define WCDBRustHandleClassMethod(funcName, ...)                                \
    WCDBRustClassMethod(Handle, funcName, __VA_ARGS__)

//jlong WCDBRustHandleClassMethod(getError, jlong self);
//jlong WCDBRustHandleClassMethod(getOrCreatePreparedStatement, jlong self, jlong statement);
//jlong WCDBRustHandleClassMethod(getOrCreatePreparedStatementWithSQL, jlong self, jstring sql);
//jlong WCDBRustHandleClassMethod(getMainStatement, jlong self);
//void WCDBRustHandleClassMethod(finalizeAllStatements, jlong self);
//jboolean WCDBRustHandleClassMethod(execute, jlong self, jlong statement);
//jboolean WCDBRustHandleClassMethod(executeSQL, jlong self, jstring sql);
//jint WCDBRustHandleClassMethod(tableExist, jlong self, jstring table);
//
//jint WCDBRustHandleClassMethod(getChanges, jlong self);
//jint WCDBRustHandleClassMethod(getTotalChanges, jlong self);
//jlong WCDBRustHandleClassMethod(getLastInsertRowid, jlong self);
//
//jboolean WCDBRustHandleClassMethod(isInTransaction, jlong self);
//jboolean WCDBRustHandleClassMethod(beginTransaction, jlong self);
//jboolean WCDBRustHandleClassMethod(commitTransaction, jlong self);
//void WCDBRustHandleClassMethod(rollbackTransaction, jlong self);

typedef bool (*RustTransactionCallback)(void* closure_raw, void* database_raw, void* cpp_handle);
bool WCDBRustHandleObjectMethod(runTransaction, void* self, RustTransactionCallback rust_callback, void* closure_raw, void* database_raw);
//jboolean WCDBRustHandleObjectMethod(runPausableTransaction, jlong self, jobject transaction);
//
//jlong WCDBRustHandleClassMethodWithNoArg(createCancellationSignal);
//void WCDBRustHandleClassMethod(cancelSignal, jlong signal);
//
//void WCDBRustHandleClassMethod(attachCancellationSignal, jlong self, jlong signal);
//void WCDBRustHandleClassMethod(detachCancellationSignal, jlong self);
