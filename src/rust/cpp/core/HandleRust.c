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

#include "HandleRust.h"
#include "HandleBridge.h"
#include "assert.h"

//jlong WCDBRustHandleClassMethod(getError, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return (jlong) WCDBHandleGetError(selfStruct).innerValue;
//}
//
//jlong WCDBRustHandleClassMethod(getOrCreatePreparedStatement, jlong self, jlong statement)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return (jlong) WCDBHandleGetOrCreatePreparedStatement(selfStruct, (CPPObject *) statement)
//    .innerValue;
//}
//
//jlong WCDBRustHandleClassMethod(getOrCreatePreparedStatementWithSQL, jlong self, jstring sql)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    WCDBRustGetString(sql);
//    jlong ret = (jlong) WCDBHandleGetOrCreatePreparedSQL(selfStruct, sqlString).innerValue;
//    WCDBRustReleaseString(sql);
//    return ret;
//}

void* WCDBRustHandleClassMethod(getMainStatement, void* self)
{
    WCDBRustBridgeStruct(CPPHandle, self);
    return (void*) WCDBHandleGetMainStatement(selfStruct).innerValue;
}

//void WCDBRustHandleClassMethod(finalizeAllStatements, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    WCDBHandleFinalizeStatements(selfStruct);
//}
//
//jboolean WCDBRustHandleClassMethod(execute, jlong self, jlong statement)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleExecute(selfStruct, (CPPObject *) statement);
//}
//
//jboolean WCDBRustHandleClassMethod(executeSQL, jlong self, jstring sql)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    WCDBRustGetString(sql);
//    jlong ret = (jlong) WCDBHandleExecuteSQL(selfStruct, sqlString);
//    WCDBRustReleaseString(sql);
//    return ret;
//}
//
//jint WCDBRustHandleClassMethod(tableExist, jlong self, jstring table)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    WCDBRustGetString(table);
//    OptionalBool ret = WCDBHandleExistTable(selfStruct, tableString);
//    WCDBRustReleaseString(table);
//    return ret.hasValue ? (jint) ret.value : 2;
//}
//
//jint WCDBRustHandleClassMethod(getChanges, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleGetChange(selfStruct);
//}
//
//jint WCDBRustHandleClassMethod(getTotalChanges, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleGetTotalChange(selfStruct);
//}
//
//jlong WCDBRustHandleClassMethod(getLastInsertRowid, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleGetLastInsertedRowID(selfStruct);
//}
//
//jboolean WCDBRustHandleClassMethod(isInTransaction, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleIsInTransaction(selfStruct);
//}
//
//jboolean WCDBRustHandleClassMethod(beginTransaction, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleBeginTransaction(selfStruct);
//}
//
//jboolean WCDBRustHandleClassMethod(commitTransaction, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleCommitTransaction(selfStruct);
//}
//
//void WCDBRustHandleClassMethod(rollbackTransaction, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    return WCDBHandleRollbackTransaction(selfStruct);
//}

typedef struct TransactionContext {
    RustTransactionCallback rust_callback;
    void* closure_raw;
    void* database_raw;
} TransactionContext;

bool WCDBRustHandleTransactionCallBack(TransactionContext* context, CPPHandle handle)
{
    return context->rust_callback(context->closure_raw, context->database_raw, handle.innerValue);
}

bool WCDBRustHandleObjectMethod(runTransaction, void* self, RustTransactionCallback rust_callback, void* closure_raw, void* database_raw)
{
    WCDBRustBridgeStruct(CPPHandle, self);
    TransactionContext context;
    context.rust_callback = rust_callback;
    context.closure_raw = closure_raw;
    context.database_raw = database_raw;
    return WCDBHandleRunTransaction(
    selfStruct, &context, (TransactionCallback) WCDBRustHandleTransactionCallBack);
}

//bool WCDBRustHandlePausableTransactionCallBack(TransactionContext *context,
//                                              CPPHandle handle,
//                                              bool *stop,
//                                              bool isNewTransaction)
//{
//    JNIEnv *env = context->env;
//
//    static jmethodID g_methodId = NULL;
//    if (g_methodId == NULL) {
//        g_methodId = (*env)->GetMethodID(
//        env, WCDBRustGetHandleClass(), "onPausableTransaction", "(JLcom/tencent/wcdb/core/PausableTransaction;Z)I");
//        if (g_methodId == NULL) {
//            assert(0);
//            return false;
//        }
//    }
//    jint ret = (*env)->CallIntMethod(
//    env, context->handle, g_methodId, (jlong) handle.innerValue, context->transaction, isNewTransaction);
//    if ((*env)->ExceptionCheck(env)) {
//        ret = 2;
//    }
//    if (ret == 2) {
//        return false;
//    } else {
//        *stop = ret == 1;
//        return true;
//    }
//}
//
//jboolean WCDBRustHandleObjectMethod(runPausableTransaction, jlong self, jobject transaction)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    TransactionContext context;
//    context.env = env;
//    context.handle = obj;
//    context.transaction = transaction;
//    return WCDBHandleRunPausableTransaction(
//    selfStruct, &context, (PausableTransaction) WCDBRustHandlePausableTransactionCallBack);
//}
//
//jlong WCDBRustHandleClassMethodWithNoArg(createCancellationSignal)
//{
//    jlong ret = (jlong) WCDBCancellationSignalCreate().innerValue;
//    return ret;
//}
//
//void WCDBRustHandleClassMethod(cancelSignal, jlong signal)
//{
//    WCDBRustBridgeStruct(CPPCancellationSignal, signal);
//    WCDBCancellationSignalCancel(signalStruct);
//}
//
//void WCDBRustHandleClassMethod(attachCancellationSignal, jlong self, jlong signal)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    WCDBRustBridgeStruct(CPPCancellationSignal, signal);
//    WCDBHandleAttachCancellationSignal(selfStruct, signalStruct);
//}
//
//void WCDBRustHandleClassMethod(detachCancellationSignal, jlong self)
//{
//    WCDBRustBridgeStruct(CPPHandle, self);
//    WCDBHandleDettachCancellationSignal(selfStruct);
//}
