// Created by qiuwenchen on 2023/4/28.
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

#define WCDBJNIHandleFuncName(funcName) WCDBJNI(Handle, funcName)
#define WCDBJNIHandleObjectMethod(funcName, ...)                               \
    WCDBJNIObjectMethod(Handle, funcName, __VA_ARGS__)
#define WCDBJNIHandleObjectMethodWithNoArg(funcName)                           \
    WCDBJNIObjectMethodWithNoArg(Handle, funcName)
#define WCDBJNIHandleClassMethodWithNoArg(funcName)                            \
    WCDBJNIClassMethodWithNoArg(Handle, funcName)
#define WCDBJNIHandleClassMethod(funcName, ...)                                \
    WCDBJNIClassMethod(Handle, funcName, __VA_ARGS__)

jlong WCDBJNIHandleClassMethod(getError, jlong self);
jlong WCDBJNIHandleClassMethod(getOrCreatePreparedStatement, jlong self, jlong statement);
jlong WCDBJNIHandleClassMethod(getOrCreatePreparedStatementWithSQL, jlong self, jstring sql);
jlong WCDBJNIHandleClassMethod(getMainStatement, jlong self);
void WCDBJNIHandleClassMethod(finalizeAllStatements, jlong self);
jboolean WCDBJNIHandleClassMethod(execute, jlong self, jlong statement);
jboolean WCDBJNIHandleClassMethod(executeSQL, jlong self, jstring sql);
jint WCDBJNIHandleClassMethod(tableExist, jlong self, jstring table);

jint WCDBJNIHandleClassMethod(getChanges, jlong self);
jint WCDBJNIHandleClassMethod(getTotalChanges, jlong self);
jlong WCDBJNIHandleClassMethod(getLastInsertRowid, jlong self);

jboolean WCDBJNIHandleClassMethod(isInTransaction, jlong self);
jboolean WCDBJNIHandleClassMethod(beginTransaction, jlong self);
jboolean WCDBJNIHandleClassMethod(commitTransaction, jlong self);
void WCDBJNIHandleClassMethod(rollbackTransaction, jlong self);

jboolean WCDBJNIHandleObjectMethod(runTransaction, jlong self, jobject transaction);
jboolean WCDBJNIHandleObjectMethod(runPausableTransaction, jlong self, jobject transaction);

jlong WCDBJNIHandleClassMethodWithNoArg(createCancellationSignal);
void WCDBJNIHandleClassMethod(cancelSignal, jlong signal);

void WCDBJNIHandleClassMethod(attachCancellationSignal, jlong self, jlong signal);
void WCDBJNIHandleClassMethod(detachCancellationSignal, jlong self);
