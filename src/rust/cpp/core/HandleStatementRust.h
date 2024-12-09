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

#define WCDBRustHandleStatementFuncName(funcName)                               \
    WCDBRust(HandleStatement, funcName)
#define WCDBRustHandleStatementObjectMethod(funcName, ...)                      \
    WCDBRustObjectMethod(HandleStatement, funcName, __VA_ARGS__)
#define WCDBRustHandleStatementObjectMethodWithNoArg(funcName)                  \
    WCDBRustObjectMethodWithNoArg(HandleStatement, funcName)
#define WCDBRustHandleStatementClassMethodWithNoArg(funcName)                   \
    WCDBRustClassMethodWithNoArg(HandleStatement, funcName)
#define WCDBRustHandleStatementClassMethod(funcName, ...)                       \
    WCDBRustClassMethod(HandleStatement, funcName, __VA_ARGS__)

//jlong WCDBRustHandleStatementClassMethod(getError, jlong self);
//jboolean WCDBRustHandleStatementClassMethod(prepare, jlong self, jlong statement);
//jboolean WCDBRustHandleStatementClassMethod(prepareSQL, jlong self, jstring sql);
//jboolean WCDBRustHandleStatementClassMethod(checkPrepared, jlong self);
//jboolean WCDBRustHandleStatementClassMethod(step, jlong self);
//void WCDBRustHandleStatementClassMethod(reset, jlong self);
//void WCDBRustHandleStatementClassMethod(clearBindings, jlong self);
//void WCDBRustHandleStatementClassMethod(finalize, jlong self);
//jboolean WCDBRustHandleStatementClassMethod(isDone, jlong self);
//void WCDBRustHandleStatementClassMethod(bindInteger, jlong self, jlong value, jint index);
//void WCDBRustHandleStatementClassMethod(bindDouble, jlong self, jdouble value, jint index);
//void WCDBRustHandleStatementClassMethod(bindText, jlong self, jstring value, jint index);
//void WCDBRustHandleStatementClassMethod(bindBLOB, jlong self, jbyteArray value, jint index);
//void WCDBRustHandleStatementClassMethod(bindNull, jlong self, jint index);
//jint WCDBRustHandleStatementClassMethod(bindParameterIndex, jlong self, jstring parameterName);
//jint WCDBRustHandleStatementClassMethod(getColumnType, jlong self, jint index);
long long WCDBRustHandleStatementClassMethod(getInteger, void* self, int index);
//jdouble WCDBRustHandleStatementClassMethod(getDouble, jlong self, jint index);
//jstring WCDBRustHandleStatementClassMethod(getText, jlong self, jint index);
//jbyteArray WCDBRustHandleStatementClassMethod(getBLOB, jlong self, jint index);
//jint WCDBRustHandleStatementClassMethod(getColumnCount, jlong self);
//jstring WCDBRustHandleStatementClassMethod(getColumnName, jlong self, jint index);
//jstring WCDBRustHandleStatementClassMethod(getOriginalColumnName, jlong self, jint index);
//jstring WCDBRustHandleStatementClassMethod(getColumnTableName, jlong self, jint index);
//jboolean WCDBRustHandleStatementClassMethod(isReadOnly, jlong self);
