//
// Created by qiuwenchen on 2022/3/31.
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
#include "ErrorBridge.h"
#include "HandleBridge.h"
#include "ObjectBridge.h"

WCDB_EXTERN_C_BEGIN

WCDBDefineCPPBridgedType(CPPDatabase)

typedef void (*WCDBContextDestructor)(void* _Nonnull context);

CPPError WCDBDatabaseGetError(CPPDatabase database);
long WCDBDatabaseGetTag(const CPPDatabase database);
void WCDBDatabaseSetTag(CPPDatabase database, long tag);
const char* _Nullable WCDBDatabaseGetPath(CPPDatabase database);
void WCDBDatabaseGetPaths(CPPDatabase database, SwiftClosure* _Nonnull enumerator);

typedef void (*WCDBStringEnumerater)(void* _Nullable context, const char* _Nonnull string);
void WCDBDatabaseGetPaths2(CPPDatabase database,
                           void* _Nullable context,
                           WCDBStringEnumerater _Nonnull enumerater);
CPPHandle WCDBDatabaseGetHandle(CPPDatabase database);

bool WCDBDatabaseCanOpen(CPPDatabase database);
bool WCDBDatabaseIsOpened(CPPDatabase database);
bool WCDBDatabaseIsBlockaded(CPPDatabase database);
void WCDBDatabaseClose(CPPDatabase database, SwiftClosure* _Nullable callback);
typedef void (*WCDBDatabaseCloseCallback)(void* _Nonnull context);
void WCDBDatabaseClose2(CPPDatabase database,
                        void* _Nullable context,
                        WCDBDatabaseCloseCallback _Nullable callback);
void WCDBDatabaseBlockade(CPPDatabase database);
void WCDBDatabaseUnblockade(CPPDatabase database);
void WCDBDatabasePurge(CPPDatabase database);

void WCDBDatabaseConfigCipher(CPPDatabase database,
                              const unsigned char* _Nullable cipherKey,
                              int keyLength,
                              int pageSize,
                              int cipherVersion);
void WCDBDatabaseConfig(CPPDatabase database,
                        const char* _Nonnull name,
                        SwiftClosure* _Nonnull invocation,
                        SwiftClosure* _Nullable uninvocation,
                        int priority);

typedef bool (*WCDBConfigCallback)(void* _Nonnull context, CPPHandle handle);

void WCDBDatabaseConfig2(CPPDatabase database,
                         const char* _Nonnull name,
                         WCDBConfigCallback _Nonnull invocation,
                         void* _Nonnull invocationContext,
                         WCDBConfigCallback _Nullable unInvocation,
                         void* _Nullable unInvocationContext,
                         int priority,
                         WCDBContextDestructor _Nonnull destructor);

void WCDBDatabaseGlobalTracePerformance(SwiftClosure* _Nullable tracer);
void WCDBDatabaseTracePerformance(CPPDatabase database, SwiftClosure* _Nullable tracer);

typedef void (*WCDBPerformanceTracer)(void* _Nullable context,
                                      const char* _Nonnull path,
                                      unsigned long long handleId,
                                      const char* _Nonnull sql,
                                      double cost);
void WCDBDatabaseGlobalTracePerformance2(WCDBPerformanceTracer _Nullable tracer,
                                         void* _Nullable context,
                                         WCDBContextDestructor _Nullable destructor);
void WCDBDatabaseTracePerformance2(CPPDatabase database,
                                   WCDBPerformanceTracer _Nullable tracer,
                                   void* _Nullable context,
                                   WCDBContextDestructor _Nullable destructor);

void WCDBDatabaseGlobalTraceSQL(SwiftClosure* _Nullable tracer);
void WCDBDatabaseTraceSQL(CPPDatabase database, SwiftClosure* _Nullable tracer);

typedef void (*WCDBSQLTracer)(void* _Nullable context,
                              const char* _Nonnull path,
                              unsigned long long handleId,
                              const char* _Nonnull sql);
void WCDBDatabaseGlobalTraceSQL2(WCDBSQLTracer _Nullable tracer,
                                 void* _Nullable context,
                                 WCDBContextDestructor _Nullable destructor);
void WCDBDatabaseTraceSQL2(CPPDatabase database,
                           WCDBSQLTracer _Nullable tracer,
                           void* _Nullable context,
                           WCDBContextDestructor _Nullable destructor);

void WCDBDatabaseGlobalTraceError(SwiftClosure* _Nullable tracer);
void WCDBDatabaseTraceError(CPPDatabase database, SwiftClosure* _Nullable tracer);

typedef void (*WCDBErrorTracer)(void* _Nullable context, CPPError error);
void WCDBDatabaseGlobalTraceError2(WCDBErrorTracer _Nullable tracer,
                                   void* _Nullable context,
                                   WCDBContextDestructor _Nullable destructor);
void WCDBDatabaseTraceError2(CPPDatabase database,
                             WCDBErrorTracer _Nullable tracer,
                             void* _Nullable context,
                             WCDBContextDestructor _Nullable destructor);

void WCDBDatabaseGlobalTraceOperation(SwiftClosure* _Nullable tracer);

typedef void (*WCDBOperationTracer)(void* _Nullable context, CPPDatabase database, long operation);
void WCDBDatabaseGlobalTraceOperation2(WCDBOperationTracer _Nullable tracer,
                                       void* _Nullable context,
                                       WCDBContextDestructor _Nullable destructor);

bool WCDBDatabaseRemoveFile(CPPDatabase database);
bool WCDBDatabaseMoveFile(CPPDatabase database, const char* _Nonnull destination);

OptionalUInt64 WCDBDatabaseGetFileSize(CPPDatabase database);

void WCDBDatabaseSetNotificationWhenCorrupted(CPPDatabase database,
                                              SwiftClosure* _Nullable onCorrupted);

typedef void (*WCDBCorruptioNotification)(void* _Nullable context, CPPDatabase database);
void WCDBDatabaseSetNotificationWhenCorrupted2(CPPDatabase database,
                                               WCDBCorruptioNotification _Nullable notification,
                                               void* _Nullable context,
                                               WCDBContextDestructor _Nullable destructor);

bool WCDBDatabaseCheckIfCorrupted(CPPDatabase database);
bool WCDBDatabaseCheckIsAlreadyCorrupted(CPPDatabase database);
void WCDBDatabaseEnableAutoBackup(CPPDatabase database, bool enable);
bool WCDBDatabaseBackup(CPPDatabase database);
void WCDBDatabaseFilterBackup(CPPDatabase database, SwiftClosure* _Nullable tableShouldBeBackedUp);

typedef bool (*WCDBBackupFilter)(void* _Nullable context, const char* _Nonnull tableName);
void WCDBDatabaseFilterBackup2(CPPDatabase database,
                               WCDBBackupFilter _Nullable filter,
                               void* _Nullable context,
                               WCDBContextDestructor _Nullable destructor);

bool WCDBDatabaseDeposit(CPPDatabase database);
bool WCDBDatabaseRemoveDepositedFiles(CPPDatabase database);
bool WCDBDatabaseContainDepositedFiles(CPPDatabase database);
double WCDBDatabaseRetrieve(CPPDatabase database, SwiftClosure* _Nullable onProgressUpdated);

typedef bool (*WCDBRetrieveProgressMonitor)(void* _Nullable context,
                                            double percentage,
                                            double increment);
double WCDBDatabaseRetrieve2(CPPDatabase database,
                             WCDBRetrieveProgressMonitor _Nullable monitor,
                             void* _Nullable context,
                             WCDBContextDestructor _Nullable destructor);

bool WCDBDatabasePassiveCheckpoint(CPPDatabase database);
bool WCDBDatabaseTruncateCheckpoint(CPPDatabase database);

void WCDBDatabaseFilterMigration(CPPDatabase database, SwiftClosure* _Nullable filter);

typedef void (*WCDBMigrationInfoSetter)(void* _Nonnull context,
                                        const char* _Nonnull sourceTable,
                                        const char* _Nonnull sourceDatabase);
typedef void (*WCDBMigrationFilter)(void* _Nullable context,
                                    const char* _Nonnull database,
                                    const char* _Nonnull table,
                                    void* _Nonnull info,
                                    WCDBMigrationInfoSetter _Nonnull setter);
void WCDBDatabaseFilterMigration2(CPPDatabase database,
                                  WCDBMigrationFilter _Nullable filter,
                                  void* _Nullable context,
                                  WCDBContextDestructor _Nullable destructor);

bool WCDBDatabaseStepMigration(CPPDatabase database);
void WCDBDatabaseEnableAutoMigration(CPPDatabase database, bool flag);
void WCDBDatabaseSetNotificationWhenMigrated(CPPDatabase database,
                                             SwiftClosure* _Nullable onMigrated);

typedef void (*WCDBMigrationNotification)(void* _Nullable context,
                                          CPPDatabase database,
                                          const char* _Nullable databasePath,
                                          const char* _Nullable table,
                                          const char* _Nullable sourceDatabase,
                                          const char* _Nullable sourceTable);
void WCDBDatabaseSetNotificationWhenMigrated2(CPPDatabase database,
                                              WCDBMigrationNotification _Nullable notification,
                                              void* _Nullable context,
                                              WCDBContextDestructor _Nullable destructor);

bool WCDBDatabaseIsMigrated(CPPDatabase database);

WCDB_EXTERN_C_END
