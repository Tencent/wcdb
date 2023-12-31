//
// Created by qiuwenchen on 2022/4/1.
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
#include "ObjectBridge.h"

#include "ErrorBridge.h"
#include "HandleStatementBridge.h"
#include "WinqBridge.h"

WCDB_EXTERN_C_BEGIN

WCDBDefineCPPBridgedType(CPPHandle) WCDBDefineCPPBridgedType(CPPCancellationSignal)

CPPError WCDBHandleGetError(CPPHandle handle);
bool WCDBHandleCheckValid(CPPHandle handle);

CPPHandleStatement WCDBHandleGetMainStatement(CPPHandle handle);
CPPHandleStatement
WCDBHandleGetOrCreatePreparedStatement(CPPHandle handle, CPPObject* _Nonnull statement);
CPPHandleStatement
WCDBHandleGetOrCreatePreparedSQL(CPPHandle handle, const char* _Nonnull sql);
void WCDBHandleFinalizeStatements(CPPHandle handle);
bool WCDBHandleExecute(CPPHandle handle, CPPObject* _Nonnull statement);
bool WCDBHandleExecuteSQL(CPPHandle handle, const char* _Nonnull sql);
OptionalBool WCDBHandleExistTable(CPPHandle handle, const char* _Nonnull tableName);
int WCDBHandleGetChange(CPPHandle handle);
int WCDBHandleGetTotalChange(CPPHandle handle);
signed long long WCDBHandleGetLastInsertedRowID(CPPHandle handle);

bool WCDBHandleIsInTransaction(CPPHandle handle);
bool WCDBHandleBeginTransaction(CPPHandle handle);
bool WCDBHandleCommitTransaction(CPPHandle handle);
void WCDBHandleRollbackTransaction(CPPHandle handle);

typedef bool (*TransactionCallback)(void* _Nonnull context, CPPHandle handle);
bool WCDBHandleRunTransaction(CPPHandle handle,
                              void* _Nonnull context,
                              TransactionCallback _Nonnull transaction);
typedef bool (*PausableTransaction)(void* _Nonnull context, CPPHandle, bool* _Nonnull, bool);
bool WCDBHandleRunPausableTransaction(CPPHandle handle,
                                      void* _Nonnull context,
                                      PausableTransaction _Nonnull pausableTransaction);

CPPCancellationSignal WCDBCancellationSignalCreate();
void WCDBCancellationSignalCancel(CPPCancellationSignal signal);

void WCDBHandleAttachCancellationSignal(CPPHandle handle, CPPCancellationSignal signal);
void WCDBHandleDettachCancellationSignal(CPPHandle handle);

WCDB_EXTERN_C_END
