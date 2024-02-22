// Created by qiuwenchen on 2023/4/20.
//

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

#include "WCDBJNI.h"

#define WCDBJNIHandleStatementFuncName(funcName)                               \
    WCDBJNI(HandleStatement, funcName)
#define WCDBJNIHandleStatementObjectMethod(funcName, ...)                      \
    WCDBJNIObjectMethod(HandleStatement, funcName, __VA_ARGS__)
#define WCDBJNIHandleStatementObjectMethodWithNoArg(funcName)                  \
    WCDBJNIObjectMethodWithNoArg(HandleStatement, funcName)
#define WCDBJNIHandleStatementClassMethodWithNoArg(funcName)                   \
    WCDBJNIClassMethodWithNoArg(HandleStatement, funcName)
#define WCDBJNIHandleStatementClassMethod(funcName, ...)                       \
    WCDBJNIClassMethod(HandleStatement, funcName, __VA_ARGS__)

jlong WCDBJNIHandleStatementClassMethod(getError, jlong self);
jboolean WCDBJNIHandleStatementClassMethod(prepare, jlong self, jlong statement);
jboolean WCDBJNIHandleStatementClassMethod(prepareSQL, jlong self, jstring sql);
jboolean WCDBJNIHandleStatementClassMethod(checkPrepared, jlong self);
jboolean WCDBJNIHandleStatementClassMethod(step, jlong self);
void WCDBJNIHandleStatementClassMethod(reset, jlong self);
void WCDBJNIHandleStatementClassMethod(clearBindings, jlong self);
void WCDBJNIHandleStatementClassMethod(finalize, jlong self);
jboolean WCDBJNIHandleStatementClassMethod(isDone, jlong self);
void WCDBJNIHandleStatementClassMethod(bindInteger, jlong self, jlong value, jint index);
void WCDBJNIHandleStatementClassMethod(bindDouble, jlong self, jdouble value, jint index);
void WCDBJNIHandleStatementClassMethod(bindText, jlong self, jstring value, jint index);
void WCDBJNIHandleStatementClassMethod(bindBLOB, jlong self, jbyteArray value, jint index);
void WCDBJNIHandleStatementClassMethod(bindNull, jlong self, jint index);
jint WCDBJNIHandleStatementClassMethod(bindParameterIndex, jlong self, jstring parameterName);
jint WCDBJNIHandleStatementClassMethod(getColumnType, jlong self, jint index);
jlong WCDBJNIHandleStatementClassMethod(getInteger, jlong self, jint index);
jdouble WCDBJNIHandleStatementClassMethod(getDouble, jlong self, jint index);
jstring WCDBJNIHandleStatementClassMethod(getText, jlong self, jint index);
jbyteArray WCDBJNIHandleStatementClassMethod(getBLOB, jlong self, jint index);
jint WCDBJNIHandleStatementClassMethod(getColumnCount, jlong self);
jstring WCDBJNIHandleStatementClassMethod(getColumnName, jlong self, jint index);
jstring WCDBJNIHandleStatementClassMethod(getOriginalColumnName, jlong self, jint index);
jstring WCDBJNIHandleStatementClassMethod(getColumnTableName, jlong self, jint index);
jboolean WCDBJNIHandleStatementClassMethod(isReadOnly, jlong self);
