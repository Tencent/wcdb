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

jlong WCDBJNIDatabaseClassMethod(getError, jlong self);
jlong WCDBJNIDatabaseClassMethod(getTag, jlong self);
void WCDBJNIDatabaseClassMethod(setTag, jlong self, jlong tag);
jstring WCDBJNIDatabaseClassMethod(getPath, jlong self);
jobject WCDBJNIDatabaseClassMethod(getPaths, jlong self);
jlong WCDBJNIDatabaseClassMethod(getHandle, jlong self, jboolean writeHint);

jboolean WCDBJNIDatabaseClassMethod(canOpen, jlong self);
jboolean WCDBJNIDatabaseClassMethod(isOpened, jlong self);
jboolean WCDBJNIDatabaseClassMethod(isBlockaded, jlong self);
void WCDBJNIDatabaseClassMethod(close, jlong self, jobject callback);
void WCDBJNIDatabaseClassMethod(blockade, jlong self);
void WCDBJNIDatabaseClassMethod(unblockade, jlong self);
void WCDBJNIDatabaseClassMethod(purge, jlong self);

void WCDBJNIDatabaseClassMethod(configCipher, jlong self, jbyteArray cipherKey, jint pageSize, jint cipherVersion);
void WCDBJNIDatabaseClassMethod(
config, jlong self, jstring name, jobject invocation, jobject unInvocation, jint priority);

void WCDBJNIDatabaseClassMethod(enableLiteMode, jlong self, jboolean enable);

void WCDBJNIDatabaseClassMethod(globalTracePerformance, jobject tracer);
void WCDBJNIDatabaseClassMethod(tracePerformance, jlong self, jobject tracer);

void WCDBJNIDatabaseClassMethod(globalTraceSQL, jobject tracer);
void WCDBJNIDatabaseClassMethod(traceSQL, jlong self, jobject tracer);
void WCDBJNIDatabaseClassMethod(setFullSQLTraceEnable, jlong self, jboolean enable);

void WCDBJNIDatabaseClassMethod(globalTraceError, jobject tracer);
void WCDBJNIDatabaseClassMethod(traceError, jlong self, jobject tracer);

void WCDBJNIDatabaseClassMethod(globalTraceOperation, jobject tracer);
void WCDBJNIDatabaseClassMethod(enumerateInfo, jobject javaInfo, jlong cppInfo);

void WCDBJNIDatabaseClassMethod(globalTraceDatabaseBusy, jobject tracer, jdouble timeOut);

jboolean WCDBJNIDatabaseClassMethod(removeFiles, jlong self);
jboolean WCDBJNIDatabaseClassMethod(moveFile, jlong self, jstring destination);

jlong WCDBJNIDatabaseClassMethod(getFileSize, jlong self);

void WCDBJNIDatabaseClassMethod(addTokenizer, jlong self, jstring tokenizer);
void WCDBJNIDatabaseClassMethod(addAuxiliaryFunction, jlong self, jstring auxiliaryFunction);

void WCDBJNIDatabaseClassMethod(setNotificationWhenCorrupted, jlong self, jobject notification);
jboolean WCDBJNIDatabaseClassMethod(checkIfCorrupted, jlong self);
jboolean WCDBJNIDatabaseClassMethod(checkIfIsAlreadyCorrupted, jlong self);
void WCDBJNIDatabaseClassMethod(enableAutoBackup, jlong self, jboolean enable);
jboolean WCDBJNIDatabaseClassMethod(backup, jlong self);
void WCDBJNIDatabaseClassMethod(filterBackup, jlong self, jobject tableShouldBeBackup);
jboolean WCDBJNIDatabaseClassMethod(deposit, jlong self);
jboolean WCDBJNIDatabaseClassMethod(removeDepositedFiles, jlong self);
jboolean WCDBJNIDatabaseClassMethod(containDepositedFiles, jlong self);
jdouble WCDBJNIDatabaseClassMethod(retrieve, jlong self, jobject onProgressUpdate);
jdouble WCDBJNIDatabaseClassMethod(vacuum, jlong self, jobject onProgressUpdate);
void WCDBJNIDatabaseClassMethod(enableAutoVacuum, jlong self, jboolean incremental);
jboolean WCDBJNIDatabaseClassMethod(incrementalVacuum, jlong self, jint pageCount);

jboolean WCDBJNIDatabaseClassMethod(passiveCheckpoint, jlong self);
jboolean WCDBJNIDatabaseClassMethod(truncateCheckpoint, jlong self);
void WCDBJNIDatabaseClassMethod(setAutoCheckpointEnable, jlong self, jboolean enable);

void WCDBJNIDatabaseClassMethod(addMigrationSource,
                                jlong self,
                                jstring sourcePath,
                                jbyteArray cipherKey,
                                jobject filter);
void WCDBJNIDatabaseClassMethod(setMigrationInfo,
                                jlong infoSetter,
                                jlong info,
                                jstring sourceTable,
                                jlong filterCondition);
jboolean WCDBJNIDatabaseClassMethod(stepMigration, jlong self);
void WCDBJNIDatabaseClassMethod(enableAutoMigration, jlong self, jboolean flag);
void WCDBJNIDatabaseClassMethod(setNotificationWhenMigrated, jlong self, jobject onMigrated);
jboolean WCDBJNIDatabaseClassMethod(isMigrated, jlong self);

jbyteArray
WCDBJNIDatabaseClassMethod(trainDictWithStrings, jobjectArray stringArray, jbyte dictId);
jbyteArray
WCDBJNIDatabaseClassMethod(trainDictWithDatas, jobjectArray dataArray, jbyte dictId);
jboolean WCDBJNIDatabaseClassMethod(registerDict, jbyteArray dict, jbyte dictId);
void WCDBJNIDatabaseClassMethod(addZSTDNormalCompress, jlong info, jlong column);
void WCDBJNIDatabaseClassMethod(addZSTDDictCompress, jlong info, jlong column, jbyte dictId);
void WCDBJNIDatabaseClassMethod(addZSTDMultiDictCompress,
                                jlong info,
                                jlong column,
                                jlong matchColumn,
                                jlongArray values,
                                jbyteArray dictIds);
void WCDBJNIDatabaseClassMethod(enableReplaceCompression, jlong info);
void WCDBJNIDatabaseClassMethod(setCompression, jlong self, jobject filter);
void WCDBJNIDatabaseClassMethod(disableCompressNewData, jlong self, jboolean disable);
jboolean WCDBJNIDatabaseClassMethod(stepCompression, jlong self);
void WCDBJNIDatabaseClassMethod(enableAutoCompression, jlong self, jboolean enable);
void WCDBJNIDatabaseClassMethod(setNotificationWhenCompressed, jlong self, jobject onCompressed);
jboolean WCDBJNIDatabaseClassMethod(isCompressed, jlong self);
jdouble WCDBJNIDatabaseClassMethod(rollbackCompression, jlong self, jobject onProgressUpdate);

jint WCDBJNIDatabaseClassMethod(getNumberOfAliveHandle, jlong self);
