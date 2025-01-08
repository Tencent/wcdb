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

void* WCDBRustHandleStatementClassMethod(getError, void* self);
bool WCDBRustHandleStatementClassMethod(prepare, void* self, void* statement);
//bool WCDBRustHandleStatementClassMethod(prepareSQL, void* self, jstring sql);
//bool WCDBRustHandleStatementClassMethod(checkPrepared, void* self);
bool WCDBRustHandleStatementClassMethod(step, void* self);
void WCDBRustHandleStatementClassMethod(reset, void* self);
//void WCDBRustHandleStatementClassMethod(clearBindings, void* self);
void WCDBRustHandleStatementClassMethod(finalize, void* self);
bool WCDBRustHandleStatementClassMethod(isDone, void* self);
void WCDBRustHandleStatementClassMethod(bindInteger, void* self, long long value, int index);
void WCDBRustHandleStatementClassMethod(bindDouble, void* self, double value, int index);
void WCDBRustHandleStatementClassMethod(bindText, void* self, const char* value, int index);
//void WCDBRustHandleStatementClassMethod(bindBLOB, void* self, jbyteArray value, jint index);
void WCDBRustHandleStatementClassMethod(bindNull, void* self, int index);
//jint WCDBRustHandleStatementClassMethod(bindParameterIndex, void* self, jstring parameterName);
//jint WCDBRustHandleStatementClassMethod(getColumnType, void* self, jint index);
long long WCDBRustHandleStatementClassMethod(getInteger, void* self, int index);
double WCDBRustHandleStatementClassMethod(getDouble, void* self, int index);
const char* WCDBRustHandleStatementClassMethod(getText, void* self, int index);
//jbyteArray WCDBRustHandleStatementClassMethod(getBLOB, void* self, jint index);
//jint WCDBRustHandleStatementClassMethod(getColumnCount, void* self);
//jstring WCDBRustHandleStatementClassMethod(getColumnName, void* self, jint index);
//jstring WCDBRustHandleStatementClassMethod(getOriginalColumnName, void* self, jint index);
//jstring WCDBRustHandleStatementClassMethod(getColumnTableName, void* self, jint index);
//bool WCDBRustHandleStatementClassMethod(isReadOnly, void* self);
