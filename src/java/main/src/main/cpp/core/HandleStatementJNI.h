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

jlong WCDBJNIHandleStatementObjectMethod(getError, jlong self);
jboolean WCDBJNIHandleStatementObjectMethod(prepare, jlong self, jlong statement);
jboolean WCDBJNIHandleStatementObjectMethod(prepareSQL, jlong self, jstring sql);
jboolean WCDBJNIHandleStatementObjectMethod(checkPrepared, jlong self);
jboolean WCDBJNIHandleStatementObjectMethod(step, jlong self);
void WCDBJNIHandleStatementObjectMethod(reset, jlong self);
void WCDBJNIHandleStatementObjectMethod(finalize, jlong self);
jboolean WCDBJNIHandleStatementObjectMethod(isDone, jlong self);
void WCDBJNIHandleStatementObjectMethod(bindInteger, jlong self, jlong value, jint index);
void WCDBJNIHandleStatementObjectMethod(bindDouble, jlong self, jdouble value, jint index);
void WCDBJNIHandleStatementObjectMethod(bindText, jlong self, jstring value, jint index);
void WCDBJNIHandleStatementObjectMethod(bindBLOB, jlong self, jbyteArray value, jint index);
void WCDBJNIHandleStatementObjectMethod(bindNull, jlong self, jint index);
jint WCDBJNIHandleStatementObjectMethod(bindParameterIndex, jlong self, jstring parameterName);
jint WCDBJNIHandleStatementObjectMethod(getColumnType, jlong self, jint index);
jlong WCDBJNIHandleStatementObjectMethod(getInteger, jlong self, jint index);
jdouble WCDBJNIHandleStatementObjectMethod(getDouble, jlong self, jint index);
jstring WCDBJNIHandleStatementObjectMethod(getText, jlong self, jint index);
jbyteArray WCDBJNIHandleStatementObjectMethod(getBLOB, jlong self, jint index);
jint WCDBJNIHandleStatementObjectMethod(getColumnCount, jlong self);
jstring WCDBJNIHandleStatementObjectMethod(getColumnName, jlong self, jint index);
jstring WCDBJNIHandleStatementObjectMethod(getOriginalColumnName, jlong self, jint index);
jstring WCDBJNIHandleStatementObjectMethod(getColumnTableName, jlong self, jint index);
jboolean WCDBJNIHandleStatementObjectMethod(isReadOnly, jlong self);
