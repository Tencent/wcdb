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

// #define WCDBRustDatabaseFuncName(funcName) WCDBRust(Database, funcName)
// #def ine WCDBRustDatabaseObjectMethod(funcName, ...)                             \
//    WCDBRustObjectMethod(Database, funcName, __VA_ARGS__)
// #def ine WCDBRustDatabaseObjectMethodWithNoArg(funcName)                         \
//    WCDBRustObjectMethodWithNoArg(Database, funcName)
// #def ine WCDBRustDatabaseClassMethodWithNoArg(funcName)                          \
//    WCDBRustClassMethodWithNoArg(Database, funcName)
#define WCDBRustDatabaseClassMethod(funcName, ...) \
    WCDBRustClassMethod(Database, funcName, __VA_ARGS__)

// #define WCDBRustDatabaseSignature "Lcom/tencent/wcdb/core/Database"
//
void* WCDBRustDatabaseClassMethod(getError, void* self);

void* WCDBRustDatabaseClassMethod(getTag, void* self);
void WCDBRustDatabaseClassMethod(setTag, void* self, long tag);
const char* WCDBRustDatabaseClassMethod(getPath, void* self);

// jobject WCDBRustDatabaseClassMethod(getPaths, jlong self);
void* WCDBRustDatabaseClassMethod(getHandle, void* self, bool writeHint);

bool WCDBRustDatabaseClassMethod(canOpen, void* self);

bool WCDBRustDatabaseClassMethod(isOpened, void* self);
bool WCDBRustDatabaseClassMethod(isBlockaded, void* self);
void WCDBRustDatabaseClassMethod(close,
                                 void* self,
                                 void* context,
                                 WCDBDatabaseCloseCallback callback);

void WCDBRustDatabaseClassMethod(blockade, void* self);
void WCDBRustDatabaseClassMethod(unblockade, void* self);
// void WCDBRustDatabaseClassMethod(purge, jlong self);
//
// void WCDBRustDatabaseClassMethod(configCipher, jlong self, jbyteArray cipherKey, jint pageSize,
// jint cipherVersion); void WCDBRustDatabaseClassMethod( config, jlong self, jstring name, jobject
// invocation, jobject unInvocation, jint priority);
//
// void WCDBRustDatabaseClassMethod(enableLiteMode, jlong self, jboolean enable);
//
typedef void (*RustGlobalTracePerformanceCallback)(long,
                                                   const char*,
                                                   unsigned long long,
                                                   const char*,
                                                   const CPPPerformanceInfo*);

void WCDBRustDatabaseClassMethod(globalTracePerformance,
                                 RustGlobalTracePerformanceCallback rust_callback);

// void WCDBRustDatabaseClassMethod(tracePerformance, jlong self, jobject tracer);
//
typedef void (
    *RustGlobalTraceSQLCallback)(long, const char*, unsigned long long, const char*, const char*);

void WCDBRustDatabaseClassMethod(globalTraceSQL, RustGlobalTraceSQLCallback rust_callback);

typedef void (
    *RustTraceSQLCallback)(long, const char*, unsigned long long, const char*, const char*);

void WCDBRustDatabaseClassMethod(traceSQL, void* self, RustTraceSQLCallback rust_callback);
// void WCDBRustDatabaseClassMethod(setFullSQLTraceEnable, jlong self, jboolean enable);
//

typedef void (*RustGlobalTraceTraceExceptionCallback)(void* exception);

void WCDBRustDatabaseClassMethod(globalTraceException,
                                 RustGlobalTraceTraceExceptionCallback rust_callback);
void WCDBRustDatabaseClassMethod(traceError,
                                 void* self,
                                 RustGlobalTraceTraceExceptionCallback rust_callback);

// void WCDBRustDatabaseClassMethod(globalTraceOperation, jobject tracer);
// void WCDBRustDatabaseClassMethod(enumerateInfo, jobject javaInfo, jlong cppInfo);
//
// void WCDBRustDatabaseClassMethod(globalTraceDatabaseBusy, jobject tracer, jdouble timeOut);
//
bool WCDBRustDatabaseClassMethod(removeFiles, void* self);
// jboolean WCDBRustDatabaseClassMethod(moveFile, jlong self, jstring destination);
//
// jlong WCDBRustDatabaseClassMethod(getFileSize, jlong self);
//
// void WCDBRustDatabaseClassMethod(addTokenizer, jlong self, jstring tokenizer);
// void WCDBRustDatabaseClassMethod(addAuxiliaryFunction, jlong self, jstring auxiliaryFunction);

typedef void (*RustGlobalCorruptionNotificationCallback)(void* self);
void WCDBRustDatabaseClassMethod(setNotificationWhenCorrupted,
                                 void* self,
                                 RustGlobalCorruptionNotificationCallback* notification);

bool WCDBRustDatabaseClassMethod(checkIfCorrupted, void* self);
bool WCDBRustDatabaseClassMethod(checkIfIsAlreadyCorrupted, void* self);
void WCDBRustDatabaseClassMethod(enableAutoBackup, void* self, bool enable);
bool WCDBRustDatabaseClassMethod(backup, void* self);

typedef bool (*RustTableShouldBeBackupCallback)(const char* table);
void WCDBRustDatabaseClassMethod(filterBackup,
                                 void* self,
                                 RustTableShouldBeBackupCallback* tableShouldBeBackup);

bool WCDBRustDatabaseClassMethod(deposit, void* self);
bool WCDBRustDatabaseClassMethod(removeDepositedFiles, void* self);
bool WCDBRustDatabaseClassMethod(containDepositedFiles, void* self);
typedef bool (*RustProgressMonitorCallback)(double percentage, double increment);
double WCDBRustDatabaseClassMethod(retrieve,
                                   void* self,
                                   RustProgressMonitorCallback* onProgressUpdate);
bool WCDBRustDatabaseClassMethod(vacuum, void* self, RustProgressMonitorCallback* onProgressUpdate);
void WCDBRustDatabaseClassMethod(enableAutoVacuum, void* self, bool incremental);
bool WCDBRustDatabaseClassMethod(incrementalVacuum, void* self, int pageCount);
//
// jboolean WCDBRustDatabaseClassMethod(passiveCheckpoint, jlong self);
bool WCDBRustDatabaseClassMethod(truncateCheckpoint, void* self);
void WCDBRustDatabaseClassMethod(setAutoCheckpointEnable, void* self, bool enable);
//
// void WCDBRustDatabaseClassMethod(addMigrationSource,
//                                 jlong self,
//                                 jstring sourcePath,
//                                 jbyteArray cipherKey,
//                                 jobject filter);
// void WCDBRustDatabaseClassMethod(setMigrationInfo,
//                                 jlong infoSetter,
//                                 jlong info,
//                                 jstring sourceTable,
//                                 jlong filterCondition);
// jboolean WCDBRustDatabaseClassMethod(stepMigration, jlong self);
// void WCDBRustDatabaseClassMethod(enableAutoMigration, jlong self, jboolean flag);
// void WCDBRustDatabaseClassMethod(setNotificationWhenMigrated, jlong self, jobject onMigrated);
// jboolean WCDBRustDatabaseClassMethod(isMigrated, jlong self);
//
// jbyteArray
// WCDBRustDatabaseClassMethod(trainDictWithStrings, jobjectArray stringArray, jbyte dictId);
// jbyteArray
// WCDBRustDatabaseClassMethod(trainDictWithDatas, jobjectArray dataArray, jbyte dictId);
// jboolean WCDBRustDatabaseClassMethod(registerDict, jbyteArray dict, jbyte dictId);
// void WCDBRustDatabaseClassMethod(addZSTDNormalCompress, jlong info, jlong column);
// void WCDBRustDatabaseClassMethod(addZSTDDictCompress, jlong info, jlong column, jbyte dictId);
// void WCDBRustDatabaseClassMethod(addZSTDMultiDictCompress,
//                                 jlong info,
//                                 jlong column,
//                                 jlong matchColumn,
//                                 jlongArray values,
//                                 jbyteArray dictIds);
// void WCDBRustDatabaseClassMethod(enableReplaceCompression, jlong info);
// void WCDBRustDatabaseClassMethod(setCompression, jlong self, jobject filter);
// void WCDBRustDatabaseClassMethod(disableCompressNewData, jlong self, jboolean disable);
// jboolean WCDBRustDatabaseClassMethod(stepCompression, jlong self);
// void WCDBRustDatabaseClassMethod(enableAutoCompression, jlong self, jboolean enable);
// void WCDBRustDatabaseClassMethod(setNotificationWhenCompressed, jlong self, jobject
// onCompressed); jboolean WCDBRustDatabaseClassMethod(isCompressed, jlong self); jdouble
// WCDBRustDatabaseClassMethod(rollbackCompression, jlong self, jobject onProgressUpdate);
//
// jint WCDBRustDatabaseClassMethod(getNumberOfAliveHandle, jlong self);
