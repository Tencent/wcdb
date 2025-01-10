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

#include "DatabaseBridge.h"
#include "WCDBRust.h"

//#define WCDBRustDatabaseFuncName(funcName) WCDBRust(Database, funcName)
//#define WCDBRustDatabaseObjectMethod(funcName, ...)                             \
//    WCDBRustObjectMethod(Database, funcName, __VA_ARGS__)
//#define WCDBRustDatabaseObjectMethodWithNoArg(funcName)                         \
//    WCDBRustObjectMethodWithNoArg(Database, funcName)
//#define WCDBRustDatabaseClassMethodWithNoArg(funcName)                          \
//    WCDBRustClassMethodWithNoArg(Database, funcName)
#define WCDBRustDatabaseClassMethod(funcName, ...)                                \
    WCDBRustClassMethod(Database, funcName, __VA_ARGS__)

//#define WCDBRustDatabaseSignature "Lcom/tencent/wcdb/core/Database"
//
void* WCDBRustDatabaseClassMethod(getError, void* self);
//jlong WCDBRustDatabaseClassMethod(getTag, jlong self);
//void WCDBRustDatabaseClassMethod(setTag, jlong self, jlong tag);
const char* WCDBRustDatabaseClassMethod(getPath, void* self);
//jobject WCDBRustDatabaseClassMethod(getPaths, jlong self);
void* WCDBRustDatabaseClassMethod(getHandle, void* self, bool writeHint);

//jboolean WCDBRustDatabaseClassMethod(canOpen, jlong self);
//jboolean WCDBRustDatabaseClassMethod(isOpened, jlong self);
//jboolean WCDBRustDatabaseClassMethod(isBlockaded, jlong self);
void WCDBRustDatabaseClassMethod(close, void* self, void* context, WCDBDatabaseCloseCallback callback);
//void WCDBRustDatabaseClassMethod(blockade, jlong self);
//void WCDBRustDatabaseClassMethod(unblockade, jlong self);
//void WCDBRustDatabaseClassMethod(purge, jlong self);
//
//void WCDBRustDatabaseClassMethod(configCipher, jlong self, jbyteArray cipherKey, jint pageSize, jint cipherVersion);
//void WCDBRustDatabaseClassMethod(
//config, jlong self, jstring name, jobject invocation, jobject unInvocation, jint priority);
//
//void WCDBRustDatabaseClassMethod(enableLiteMode, jlong self, jboolean enable);
//
//void WCDBRustDatabaseClassMethod(globalTracePerformance, jobject tracer);
//void WCDBRustDatabaseClassMethod(tracePerformance, jlong self, jobject tracer);
//
//void WCDBRustDatabaseClassMethod(globalTraceSQL, jobject tracer);
//void WCDBRustDatabaseClassMethod(traceSQL, jlong self, jobject tracer);
//void WCDBRustDatabaseClassMethod(setFullSQLTraceEnable, jlong self, jboolean enable);
//
//void WCDBRustDatabaseClassMethod(globalTraceError, jobject tracer);
//void WCDBRustDatabaseClassMethod(traceError, jlong self, jobject tracer);
//
//void WCDBRustDatabaseClassMethod(globalTraceOperation, jobject tracer);
//void WCDBRustDatabaseClassMethod(enumerateInfo, jobject javaInfo, jlong cppInfo);
//
//void WCDBRustDatabaseClassMethod(globalTraceDatabaseBusy, jobject tracer, jdouble timeOut);
//
//jboolean WCDBRustDatabaseClassMethod(removeFiles, jlong self);
//jboolean WCDBRustDatabaseClassMethod(moveFile, jlong self, jstring destination);
//
//jlong WCDBRustDatabaseClassMethod(getFileSize, jlong self);
//
//void WCDBRustDatabaseClassMethod(addTokenizer, jlong self, jstring tokenizer);
//void WCDBRustDatabaseClassMethod(addAuxiliaryFunction, jlong self, jstring auxiliaryFunction);
//
//void WCDBRustDatabaseClassMethod(setNotificationWhenCorrupted, jlong self, jobject notification);
//jboolean WCDBRustDatabaseClassMethod(checkIfCorrupted, jlong self);
//jboolean WCDBRustDatabaseClassMethod(checkIfIsAlreadyCorrupted, jlong self);
//void WCDBRustDatabaseClassMethod(enableAutoBackup, jlong self, jboolean enable);
//jboolean WCDBRustDatabaseClassMethod(backup, jlong self);
//void WCDBRustDatabaseClassMethod(filterBackup, jlong self, jobject tableShouldBeBackup);
//jboolean WCDBRustDatabaseClassMethod(deposit, jlong self);
//jboolean WCDBRustDatabaseClassMethod(removeDepositedFiles, jlong self);
//jboolean WCDBRustDatabaseClassMethod(containDepositedFiles, jlong self);
//jdouble WCDBRustDatabaseClassMethod(retrieve, jlong self, jobject onProgressUpdate);
//jdouble WCDBRustDatabaseClassMethod(vacuum, jlong self, jobject onProgressUpdate);
//void WCDBRustDatabaseClassMethod(enableAutoVacuum, jlong self, jboolean incremental);
//jboolean WCDBRustDatabaseClassMethod(incrementalVacuum, jlong self, jint pageCount);
//
//jboolean WCDBRustDatabaseClassMethod(passiveCheckpoint, jlong self);
//jboolean WCDBRustDatabaseClassMethod(truncateCheckpoint, jlong self);
//void WCDBRustDatabaseClassMethod(setAutoCheckpointEnable, jlong self, jboolean enable);
//
//void WCDBRustDatabaseClassMethod(addMigrationSource,
//                                jlong self,
//                                jstring sourcePath,
//                                jbyteArray cipherKey,
//                                jobject filter);
//void WCDBRustDatabaseClassMethod(setMigrationInfo,
//                                jlong infoSetter,
//                                jlong info,
//                                jstring sourceTable,
//                                jlong filterCondition);
//jboolean WCDBRustDatabaseClassMethod(stepMigration, jlong self);
//void WCDBRustDatabaseClassMethod(enableAutoMigration, jlong self, jboolean flag);
//void WCDBRustDatabaseClassMethod(setNotificationWhenMigrated, jlong self, jobject onMigrated);
//jboolean WCDBRustDatabaseClassMethod(isMigrated, jlong self);
//
//jbyteArray
//WCDBRustDatabaseClassMethod(trainDictWithStrings, jobjectArray stringArray, jbyte dictId);
//jbyteArray
//WCDBRustDatabaseClassMethod(trainDictWithDatas, jobjectArray dataArray, jbyte dictId);
//jboolean WCDBRustDatabaseClassMethod(registerDict, jbyteArray dict, jbyte dictId);
//void WCDBRustDatabaseClassMethod(addZSTDNormalCompress, jlong info, jlong column);
//void WCDBRustDatabaseClassMethod(addZSTDDictCompress, jlong info, jlong column, jbyte dictId);
//void WCDBRustDatabaseClassMethod(addZSTDMultiDictCompress,
//                                jlong info,
//                                jlong column,
//                                jlong matchColumn,
//                                jlongArray values,
//                                jbyteArray dictIds);
//void WCDBRustDatabaseClassMethod(enableReplaceCompression, jlong info);
//void WCDBRustDatabaseClassMethod(setCompression, jlong self, jobject filter);
//void WCDBRustDatabaseClassMethod(disableCompressNewData, jlong self, jboolean disable);
//jboolean WCDBRustDatabaseClassMethod(stepCompression, jlong self);
//void WCDBRustDatabaseClassMethod(enableAutoCompression, jlong self, jboolean enable);
//void WCDBRustDatabaseClassMethod(setNotificationWhenCompressed, jlong self, jobject onCompressed);
//jboolean WCDBRustDatabaseClassMethod(isCompressed, jlong self);
//jdouble WCDBRustDatabaseClassMethod(rollbackCompression, jlong self, jobject onProgressUpdate);
//
//jint WCDBRustDatabaseClassMethod(getNumberOfAliveHandle, jlong self);
