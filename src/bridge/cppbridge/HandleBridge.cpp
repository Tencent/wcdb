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

#include "HandleBridge.h"
#include "InnerHandle.hpp"
#include "ObjectBridge.hpp"
#include "RecyclableHandle.hpp"

CPPError WCDBHandleGetError(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, CPPError());
    const WCDB::Error& error = cppHandle->getError();
    return WCDBCreateUnmanagedCPPObject(CPPError, &error);
}

bool WCDBHandleCheckValid(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    WCDB_UNUSED(cppHandle);
    return true;
}

WCDB::HandleStatement* WCDB::GetMainHandleStatement(WCDB::InnerHandle* handle)
{
    return static_cast<WCDB::HandleStatement*>(handle->m_mainStatement);
}

CPPHandleStatement WCDBHandleGetMainStatement(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, CPPHandleStatement());
    WCDB::HandleStatement* stmt = WCDB::GetMainHandleStatement(cppHandle);
    return WCDBCreateUnmanagedCPPObject(CPPHandleStatement, stmt);
}

CPPHandleStatement
WCDBHandleGetOrCreatePreparedStatement(CPPHandle handle, CPPObject* statement)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, CPPHandleStatement());
    WCDB::Statement* cppStatement
    = (WCDB::Statement*) WCDB::ObjectBridge::extractOriginalCPPObject(statement);
    WCDB::HandleStatement* stmt = cppHandle->getOrCreatePreparedStatement(*cppStatement);
    return WCDBCreateUnmanagedCPPObject(CPPHandleStatement, stmt);
}

CPPHandleStatement
WCDBHandleGetOrCreatePreparedSQL(CPPHandle handle, const char* _Nonnull sql)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, CPPHandleStatement());
    WCDB::HandleStatement* stmt
    = cppHandle->getOrCreatePreparedStatement(WCDB::UnsafeStringView(sql));
    return WCDBCreateUnmanagedCPPObject(CPPHandleStatement, stmt);
}

void WCDBHandleFinalizeStatements(CPPHandle handle)
{
    WCDBGetObjectOrReturn(handle, WCDB::InnerHandle, cppHandle);
    cppHandle->finalizeStatements();
}

bool WCDBHandleExecute(CPPHandle handle, CPPObject* statement)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    WCDB::Statement* cppStatement
    = (WCDB::Statement*) WCDB::ObjectBridge::extractOriginalCPPObject(statement);
    return cppHandle->execute(*cppStatement);
}

bool WCDBHandleExecuteSQL(CPPHandle handle, const char* _Nonnull sql)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppHandle->execute(WCDB::UnsafeStringView(sql));
}

OptionalBool WCDBHandleExistTable(CPPHandle handle, const char* _Nonnull tableName)
{
    OptionalBool ret = { false, false };
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, ret);
    auto exist = cppHandle->tableExists(WCDB::UnsafeStringView(tableName));
    if (exist.succeed()) {
        ret.hasValue = exist.succeed();
        ret.value = exist.value();
    }
    return ret;
}

int WCDBHandleGetChange(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, 0);
    return cppHandle->getChanges();
}

int WCDBHandleGetTotalChange(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, 0);
    return cppHandle->getTotalChange();
}

signed long long WCDBHandleGetLastInsertedRowID(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, 0);
    return cppHandle->getLastInsertedRowID();
}

bool WCDBHandleIsReadOnly(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, true);
    return cppHandle->isReadonly();
}

bool WCDBHandleIsInTransaction(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppHandle->isInTransaction();
}

bool WCDBHandleBeginTransaction(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppHandle->beginTransaction();
}

bool WCDBHandleCommitTransaction(CPPHandle handle)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppHandle->commitOrRollbackTransaction();
}

void WCDBHandleRollbackTransaction(CPPHandle handle)
{
    WCDBGetObjectOrReturn(handle, WCDB::InnerHandle, cppHandle);
    cppHandle->rollbackTransaction();
}

bool WCDBHandleRunTransaction(CPPHandle handle, void* _Nonnull context, TransactionCallback _Nonnull transaction)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppHandle->runTransaction([transaction, context](WCDB::InnerHandle* innerHandle) {
        CPPHandle bridgeHandle = WCDBCreateUnmanagedCPPObject(CPPHandle, innerHandle);
        return transaction(context, bridgeHandle);
    });
}

bool WCDBHandleRunPausableTransaction(CPPHandle handle,
                                      void* _Nonnull context,
                                      PausableTransaction _Nonnull pausableTransaction)
{
    WCDBGetObjectOrReturnValue(handle, WCDB::InnerHandle, cppHandle, false);
    return cppHandle->runPausableTransactionWithOneLoop(
    [pausableTransaction, context](WCDB::InnerHandle* innerHandle, bool& stop, bool isNewTransaction) {
        CPPHandle bridgeHandle = WCDBCreateUnmanagedCPPObject(CPPHandle, innerHandle);
        return pausableTransaction(context, bridgeHandle, &stop, isNewTransaction);
    });
}

CPPCancellationSignal WCDBCancellationSignalCreate()
{
    return WCDBCreateCPPBridgedObjectWithParameters(
    CPPCancellationSignal, WCDB::InnerHandle::CancellationSignal, new bool(false));
}

void WCDBCancellationSignalCancel(CPPCancellationSignal signal)
{
    WCDBGetObjectOrReturn(signal, WCDB::InnerHandle::CancellationSignal, cppSignal);
    **cppSignal = true;
}

void WCDBHandleAttachCancellationSignal(CPPHandle handle, CPPCancellationSignal signal)
{
    WCDBGetObjectOrReturn(handle, WCDB::InnerHandle, cppHandle);
    WCDBGetObjectOrReturn(signal, WCDB::InnerHandle::CancellationSignal, cppSignal);
    cppHandle->attachCancellationSignal(*cppSignal);
}

void WCDBHandleDettachCancellationSignal(CPPHandle handle)
{
    WCDBGetObjectOrReturn(handle, WCDB::InnerHandle, cppHandle);
    cppHandle->detachCancellationSignal();
}
