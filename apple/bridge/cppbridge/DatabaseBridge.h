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
#include <WCDB/ErrorBridge.h>
#include <WCDB/HandleBridge.h>
#include <WCDB/ObjectBridge.h>

WCDB_EXTERN_C_BEGIN

WCDBDefineCPPBridgedType(CPPDatabase)

CPPError WCDBDatabaseGetError(CPPDatabase database);
long WCDBDatabaseGetTag(const CPPDatabase database);
void WCDBDatabaseSetTag(CPPDatabase database, long tag);
const char* _Nullable WCDBDatabaseGetPath(CPPDatabase database);
void WCDBDatabaseGetPaths(CPPDatabase database, SwiftClosure* _Nonnull enumerator);
CPPHandle WCDBDatabaseGetHandle(CPPDatabase database);

OptionalBool WCDBDatabaseExistTable(CPPDatabase database, const char* _Nonnull tableName);

bool WCDBDatabaseCanOpen(CPPDatabase database);
bool WCDBDatabaseIsOpened(CPPDatabase database);
bool WCDBDatabaseIsBlockaded(CPPDatabase database);
void WCDBDatabaseClose(CPPDatabase database, SwiftClosure* _Nullable callback);
void WCDBDatabaseBlockaded(CPPDatabase database);
void WCDBDatabaseUnblockaded(CPPDatabase database);
void WCDBDatabasePurge(CPPDatabase database);

void WCDBDatabaseConfigCipher(CPPDatabase database,
                              const unsigned char* _Nullable cipherKey,
                              int keyLength,
                              int pageSize);
void WCDBDatabaseConfig(CPPDatabase database,
                        const char* _Nonnull name,
                        SwiftClosure* _Nonnull invocation,
                        SwiftClosure* _Nullable uninvocation,
                        int priority);

void WCDBDatabaseGlobalTracePerformance(SwiftClosure* _Nullable tracer);
void WCDBDatabaseTracePerformance(CPPDatabase database, SwiftClosure* _Nullable tracer);
void WCDBDatabaseGlobalTraceSQL(SwiftClosure* _Nullable tracer);
void WCDBDatabaseTraceSQL(CPPDatabase database, SwiftClosure* _Nullable tracer);
void WCDBDatabaseGlobalTraceError(SwiftClosure* _Nullable tracer);

bool WCDBDatabaseRemoveFile(CPPDatabase database);
bool WCDBDatabaseMoveFile(CPPDatabase database, const char* _Nonnull destination);

OptionalUInt64 WCDBDatabaseGetFileSize(CPPDatabase database);

void WCDBDatabaseSetNotificationWhenCorrupted(CPPDatabase database,
                                              SwiftClosure* _Nullable onCorrupted);
bool WCDBDatabaseCheckIfCorrupted(CPPDatabase database);
bool WCDBDatabaseCheckIsAlreadyCorrupted(CPPDatabase database);
void WCDBDatabaseEnableAutoBackup(CPPDatabase database, bool enable);
bool WCDBDatabaseBackup(CPPDatabase database);
void WCDBDatabaseFilterBackup(CPPDatabase database, SwiftClosure* _Nullable tableShouldBeBackedUp);
bool WCDBDatabaseDeposit(CPPDatabase database);
bool WCDBDatabaseRemoveDepositedFiles(CPPDatabase database);
bool WCDBDatabaseContainDepositedFiles(CPPDatabase database);
double WCDBDatabaseRetrieve(CPPDatabase database, SwiftClosure* _Nullable onProgressUpdated);

bool WCDBDatabasePassiveCheckpoint(CPPDatabase database);
bool WCDBDatabaseTruncateCheckpoint(CPPDatabase database);

WCDB_EXTERN_C_END
