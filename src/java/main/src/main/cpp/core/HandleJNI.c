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

#include "HandleJNI.h"
#include "HandleBridge.h"
#include "assert.h"

static jclass g_handleClass = NULL;

jlong WCDBJNIHandleObjectMethod(getError, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return (jlong) WCDBHandleGetError(selfStruct).innerValue;
}

jlong WCDBJNIHandleObjectMethod(getOrCreatePreparedStatement, jlong self, jlong statement)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return (jlong) WCDBHandleGetOrCreatePreparedStatement(selfStruct, (CPPObject *) statement)
    .innerValue;
}

jlong WCDBJNIHandleObjectMethod(getOrCreatePreparedStatementWithSQL, jlong self, jstring sql)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    WCDBJNIGetString(sql);
    jlong ret = (jlong) WCDBHandleGetOrCreatePreparedSQL(selfStruct, sqlString).innerValue;
    WCDBJNIReleaseString(sql);
    return ret;
}

jlong WCDBJNIHandleObjectMethod(getMainStatement, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return (jlong) WCDBHandleGetMainStatement(selfStruct).innerValue;
}

void WCDBJNIHandleObjectMethod(finalizeAllStatements, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    WCDBHandleFinalizeStatements(selfStruct);
}

jboolean WCDBJNIHandleObjectMethod(execute, jlong self, jlong statement)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleExecute(selfStruct, (CPPObject *) statement);
}

jboolean WCDBJNIHandleObjectMethod(executeSQL, jlong self, jstring sql)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    WCDBJNIGetString(sql);
    jlong ret = (jlong) WCDBHandleExecuteSQL(selfStruct, sqlString);
    WCDBJNIReleaseString(sql);
    return ret;
}

jint WCDBJNIHandleObjectMethod(tableExist, jlong self, jstring table)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    WCDBJNIGetString(table);
    OptionalBool ret = WCDBHandleExistTable(selfStruct, tableString);
    WCDBJNIReleaseString(table);
    return ret.hasValue ? (jint) ret.value : 2;
}

jint WCDBJNIHandleObjectMethod(getChanges, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleGetChange(selfStruct);
}

jint WCDBJNIHandleObjectMethod(getTotalChanges, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleGetTotalChange(selfStruct);
}

jlong WCDBJNIHandleObjectMethod(getLastInsertRowid, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleGetLastInsertedRowID(selfStruct);
}

jboolean WCDBJNIHandleObjectMethod(isInTransaction, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleGetLastInsertedRowID(selfStruct);
}

jboolean WCDBJNIHandleObjectMethod(beginTransaction, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleBeginTransaction(selfStruct);
}

jboolean WCDBJNIHandleObjectMethod(commitTransaction, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleCommitTransaction(selfStruct);
}

void WCDBJNIHandleObjectMethod(rollbackTransaction, jlong self)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    return WCDBHandleRollbackTransaction(selfStruct);
}

typedef struct TransactionContext {
    JNIEnv *env;
    jobject handle;
    jobject transaction;
} TransactionContext;

bool WCDBJNIHanldeTransactionCallBack(TransactionContext *context, CPPHandle handle)
{
    JNIEnv *env = context->env;
    if (g_handleClass == NULL) {
        g_handleClass = (*env)->FindClass(env, "com/tencent/wcdb/core/Handle");
        WCDBJNICreateGlobalRel(g_handleClass);
        if (g_handleClass == NULL) {
            assert(0);
            return false;
        }
    }

    static jmethodID g_methodId = NULL;
    if (g_methodId == NULL) {
        g_methodId = (*env)->GetMethodID(
        env, g_handleClass, "onTransaction", "(JLcom/tencent/wcdb/core/Transaction;)Z");
        if (g_methodId == NULL) {
            assert(0);
            return false;
        }
    }
    return (*env)->CallBooleanMethod(
    env, context->handle, g_methodId, (jlong) handle.innerValue, context->transaction);
}

jboolean WCDBJNIHandleObjectMethod(runTransaction, jlong self, jobject transaction)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    TransactionContext context;
    context.env = env;
    context.handle = obj;
    context.transaction = transaction;
    return WCDBHandleRunTransaction2(selfStruct, &context, WCDBJNIHanldeTransactionCallBack);
}

bool WCDBJNIHanldePausableTransactionCallBack(TransactionContext *context,
                                              CPPHandle handle,
                                              bool *stop,
                                              bool isNewTransaction)
{
    JNIEnv *env = context->env;
    if (g_handleClass == NULL) {
        g_handleClass = (*env)->FindClass(env, "com/tencent/wcdb/core/Handle");
        WCDBJNICreateGlobalRel(g_handleClass);
        if (g_handleClass == NULL) {
            assert(0);
            return false;
        }
    }

    static jmethodID g_methodId = NULL;
    if (g_methodId == NULL) {
        g_methodId = (*env)->GetMethodID(
        env, g_handleClass, "onPausableTransaction", "(JLcom/tencent/wcdb/core/PausableTransaction;Z)I");
        if (g_methodId == NULL) {
            assert(0);
            return false;
        }
    }
    jint ret = (*env)->CallIntMethod(
    env, context->handle, g_methodId, (jlong) handle.innerValue, context->transaction, isNewTransaction);
    if (ret == 2) {
        return false;
    } else {
        *stop = ret == 1;
        return true;
    }
}

jboolean WCDBJNIHandleObjectMethod(runPausableTransaction, jlong self, jobject transaction)
{
    WCDBJNIBridgeStruct(CPPHandle, self);
    TransactionContext context;
    context.env = env;
    context.handle = obj;
    context.transaction = transaction;
    return WCDBHandleRunPausableTransaction2(
    selfStruct, &context, WCDBJNIHanldePausableTransactionCallBack);
}
