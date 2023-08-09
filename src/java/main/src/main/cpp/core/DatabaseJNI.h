// Created by chenqiuwen on 2023/5/3.
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

#define WCDBJNIDatabaseFuncName(funcName) WCDBJNI(Database, funcName)
#define WCDBJNIDatabaseObjectMethod(funcName, ...)                             \
    WCDBJNIObjectMethod(Database, funcName, __VA_ARGS__)
#define WCDBJNIDatabaseObjectMethodWithNoArg(funcName)                         \
    WCDBJNIObjectMethodWithNoArg(Database, funcName)
#define WCDBJNIDatabaseClassMethodWithNoArg(funcName)                          \
    WCDBJNIClassMethodWithNoArg(Database, funcName)
#define WCDBJNIDatabaseClassMethod(funcName, ...)                              \
    WCDBJNIClassMethod(Database, funcName, __VA_ARGS__)

#define WCDBJNIDatabaseSignature "Lcom/tencent/wcdb/core/Database"

jlong WCDBJNIDatabaseObjectMethod(getError, jlong self);
jlong WCDBJNIDatabaseObjectMethod(getTag, jlong self);
void WCDBJNIDatabaseObjectMethod(setTag, jlong self, jlong tag);
jstring WCDBJNIDatabaseObjectMethod(getPath, jlong self);
jobject WCDBJNIDatabaseObjectMethod(getPaths, jlong self);
jlong WCDBJNIDatabaseObjectMethod(getHandle, jlong self);

jboolean WCDBJNIDatabaseObjectMethod(canOpen, jlong self);
jboolean WCDBJNIDatabaseObjectMethod(isOpened, jlong self);
jboolean WCDBJNIDatabaseObjectMethod(isBlockaded, jlong self);
void WCDBJNIDatabaseObjectMethod(close, jlong self, jobject callback);
void WCDBJNIDatabaseObjectMethod(blockade, jlong self);
void WCDBJNIDatabaseObjectMethod(unblockade, jlong self);
void WCDBJNIDatabaseObjectMethod(purge, jlong self);

void WCDBJNIDatabaseObjectMethod(configCipher, jlong self, jbyteArray cipherKey, jint pageSize, jint cipherVersion);
void WCDBJNIDatabaseObjectMethod(
config, jlong self, jstring name, jobject invocation, jobject unInvocation, jint priority);

void WCDBJNIDatabaseClassMethod(globalTracePerformance, jobject tracer);
void WCDBJNIDatabaseObjectMethod(tracePerformance, jlong self, jobject tracer);

void WCDBJNIDatabaseClassMethod(globalTraceSQL, jobject tracer);
void WCDBJNIDatabaseObjectMethod(traceSQL, jlong self, jobject tracer);

void WCDBJNIDatabaseClassMethod(globalTraceError, jobject tracer);
void WCDBJNIDatabaseObjectMethod(traceError, jlong self, jobject tracer);

void WCDBJNIDatabaseClassMethod(globalTraceOperation, jobject tracer);
void WCDBJNIDatabaseClassMethod(enumerateInfo, jobject javaInfo, jlong cppInfo);

jboolean WCDBJNIDatabaseObjectMethod(removeFiles, jlong self);
jboolean WCDBJNIDatabaseObjectMethod(moveFile, jlong self, jstring destination);

jlong WCDBJNIDatabaseObjectMethod(getFileSize, jlong self);

void WCDBJNIDatabaseObjectMethod(setNotificationWhenCorrupted, jlong self, jobject notification);
jboolean WCDBJNIDatabaseObjectMethod(checkIfCorrupted, jlong self);
jboolean WCDBJNIDatabaseObjectMethod(checkIfIsAlreadyCorrupted, jlong self);
void WCDBJNIDatabaseObjectMethod(enableAutoBackup, jlong self, jboolean enable);
jboolean WCDBJNIDatabaseObjectMethod(backup, jlong self);
void WCDBJNIDatabaseObjectMethod(filterBackup, jlong self, jobject tableShouldBeBackup);
jboolean WCDBJNIDatabaseObjectMethod(deposit, jlong self);
jboolean WCDBJNIDatabaseObjectMethod(removeDepositedFiles, jlong self);
jboolean WCDBJNIDatabaseObjectMethod(containDepositedFiles, jlong self);
jdouble WCDBJNIDatabaseObjectMethod(retrieve, jlong self, jobject onProgressUpdate);

jboolean WCDBJNIDatabaseObjectMethod(passiveCheckpoint, jlong self);
jboolean WCDBJNIDatabaseObjectMethod(truncateCheckpoint, jlong self);

void WCDBJNIDatabaseObjectMethod(addMigrationSource,
                                 jlong self,
                                 jstring sourcePath,
                                 jbyteArray cipherKey,
                                 jobject filter);
void WCDBJNIDatabaseClassMethod(setMigrationInfo,
                                jlong infoSetter,
                                jlong info,
                                jstring sourceTable,
                                jlong filterCondition);
jboolean WCDBJNIDatabaseObjectMethod(stepMigration, jlong self);
void WCDBJNIDatabaseObjectMethod(enableAutoMigration, jlong self, jboolean flag);
void WCDBJNIDatabaseObjectMethod(setNotificationWhenMigrated, jlong self, jobject onMigrated);
jboolean WCDBJNIDatabaseObjectMethod(isMigrated, jlong self);
