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

#include "JNIHelp.h"
#include "Logger.h"
#include "ModuleLoader.h"
#include "SQLiteConnection.h"
#include <backup/mm_backup.h>
#include <jni.h>
#include <stdlib.h>
#include <sys/time.h>

namespace wcdb {

static void print_xlog(int prio, const char *msg)
{
    android_LogPriority level;
    if (prio == MMBAK_LOG_DEBUG)
        level = ANDROID_LOG_DEBUG;
    else if (prio == MMBAK_LOG_INFO)
        level = ANDROID_LOG_INFO;
    else
        level = ANDROID_LOG_ERROR;

    wcdb_log_write(level, "WCDB.BackupKit", msg);
}

static jlong JNICALL BackupKit_nativeInit(
    JNIEnv *env, jclass cls, jstring outPath, jbyteArray key, jint flags)
{
    const char *path_ = env->GetStringUTFChars(outPath, NULL);
    const unsigned char *key_;
    int key_len_;

    if (key) {
        key_len_ = env->GetArrayLength(key);
        key_ = (unsigned char *) env->GetByteArrayElements(key, NULL);
    } else {
        key_len_ = 0;
        key_ = NULL;
    }

    mm_backup_ctx *ctx =
        mm_backup_init(key_, key_len_, path_, (uint32_t) flags, print_xlog);
    env->ReleaseStringUTFChars(outPath, path_);
    if (key_)
        env->ReleaseByteArrayElements(key, (jbyte *) key_, 0);

    return (jlong)(intptr_t) ctx;
}

static jint JNICALL BackupKit_nativeRun(JNIEnv *env,
                                        jclass cls,
                                        jlong ctxHandle,
                                        jlong dbHandle,
                                        jobjectArray tableDesc)
{
    mm_backup_ctx *ctx = (mm_backup_ctx *) (intptr_t) ctxHandle;
    sqlite3 *db = (sqlite3 *) (intptr_t) dbHandle;

    mm_backup_tabdesc *tabdesc;
    int num_tabdesc;
    jstring *strarr;

    if (tableDesc) {
        // translate tableDesc to tabdesc array.
        int count = env->GetArrayLength(tableDesc);
        if (count & 0x01)
            return -1; // make sure count is even.

        num_tabdesc = count / 2;
        tabdesc = (mm_backup_tabdesc *) alloca(sizeof(mm_backup_tabdesc) *
                                               num_tabdesc);
        strarr = (jstring *) alloca(sizeof(jstring) * count);
        for (int i = 0; i < num_tabdesc; i++) {
            const int ii = i + i;
            strarr[ii] = (jstring) env->GetObjectArrayElement(tableDesc, ii);
            strarr[ii + 1] =
                (jstring) env->GetObjectArrayElement(tableDesc, ii + 1);
            tabdesc[i].table =
                strarr[ii] ? env->GetStringUTFChars(strarr[ii], NULL) : NULL;
            tabdesc[i].condition =
                strarr[ii + 1] ? env->GetStringUTFChars(strarr[ii + 1], NULL)
                               : NULL;
        }
    } else {
        tabdesc = NULL;
        num_tabdesc = 0;
        strarr = NULL;
    }

    int ret = mm_backup_run(ctx, db, tabdesc, num_tabdesc);

    for (int i = 0; i < num_tabdesc; i++) {
        const int ii = i + i;
        if (strarr[ii])
            env->ReleaseStringUTFChars(strarr[ii], tabdesc[i].table);
        if (strarr[ii + 1])
            env->ReleaseStringUTFChars(strarr[ii + 1], tabdesc[i].condition);
    }

    return ret;
}

static void JNICALL BackupKit_nativeCancel(JNIEnv *env,
                                           jclass cls,
                                           jlong ctxHandle)
{
    mm_backup_ctx *ctx = (mm_backup_ctx *) (intptr_t) ctxHandle;
    mm_backup_cancel(ctx);
}

static void JNICALL BackupKit_nativeFinish(JNIEnv *env,
                                           jclass cls,
                                           jint ctxHandle)
{
    mm_backup_ctx *ctx = (mm_backup_ctx *) (intptr_t) ctxHandle;
    mm_backup_finish(ctx);
}

static jint
BackupKit_nativeStatementCount(JNIEnv *env, jclass cls, jint ctxHandle)
{
    unsigned int stmt_count;
    mm_backup_ctx *ctx = (mm_backup_ctx *) (intptr_t) ctxHandle;
    mm_backup_statistics(ctx, &stmt_count);
    return stmt_count;
}

static jstring
BackupKit_nativeLastError(JNIEnv *env, jclass cls, jint ctxHandle)
{
    mm_backup_ctx *ctx = (mm_backup_ctx *) (intptr_t) ctxHandle;
    const char *errmsg = mm_backup_last_error(ctx);
    return errmsg ? env->NewStringUTF(errmsg) : NULL;
}

static jlong JNICALL RecoverKit_nativeInit(JNIEnv *env,
                                           jclass cls,
                                           jstring inPath,
                                           jbyteArray key)
{
    const char *path_ = env->GetStringUTFChars(inPath, NULL);
    const unsigned char *key_;
    int key_len_;

    if (key) {
        key_len_ = env->GetArrayLength(key);
        key_ = (unsigned char *) env->GetByteArrayElements(key, NULL);
    } else {
        key_len_ = 0;
        key_ = NULL;
    }

    mm_recover_ctx *ctx = mm_recover_init(path_, key_, key_len_, print_xlog);
    env->ReleaseStringUTFChars(inPath, path_);
    if (key_)
        env->ReleaseByteArrayElements(key, (jbyte *) key_, 0);

    return (jlong)(intptr_t) ctx;
}

static jint JNICALL RecoverKit_nativeRun(
    JNIEnv *env, jclass cls, jlong ctxHandle, jlong dbHandle, jboolean fatal)
{
    mm_recover_ctx *ctx = (mm_recover_ctx *) (intptr_t) ctxHandle;
    sqlite3 *db = (sqlite3 *) (intptr_t) dbHandle;

    return mm_recover_run(ctx, db, fatal);
}

static void JNICALL RecoverKit_nativeCancel(JNIEnv *env,
                                            jclass cls,
                                            jlong ctxHandle)
{
    mm_recover_ctx *ctx = (mm_recover_ctx *) (intptr_t) ctxHandle;
    mm_recover_cancel(ctx);
}

static void JNICALL RecoverKit_nativeFinish(JNIEnv *env,
                                            jclass cls,
                                            jlong ctxHandle)
{
    mm_recover_ctx *ctx = (mm_recover_ctx *) (intptr_t) ctxHandle;
    mm_recover_finish(ctx);
}

static jint JNICALL RecoverKit_nativeSuccessCount(JNIEnv *env,
                                                  jclass cls,
                                                  jlong ctxHandle)
{
    unsigned int succ = 0;
    mm_recover_ctx *ctx = (mm_recover_ctx *) (intptr_t) ctxHandle;
    mm_recover_statistics(ctx, &succ, NULL);
    return succ;
}

static jint JNICALL RecoverKit_nativeFailureCount(JNIEnv *env,
                                                  jclass cls,
                                                  jlong ctxHandle)
{
    unsigned int fail = 0;
    mm_recover_ctx *ctx = (mm_recover_ctx *) (intptr_t) ctxHandle;
    mm_recover_statistics(ctx, NULL, &fail);
    return fail;
}

static jstring
RecoverKit_nativeLastError(JNIEnv *env, jclass cls, jint ctxHandle)
{
    mm_recover_ctx *ctx = (mm_recover_ctx *) (intptr_t) ctxHandle;
    const char *errmsg = mm_recover_last_error(ctx);
    return errmsg ? env->NewStringUTF(errmsg) : NULL;
}

static const JNINativeMethod BackupKit_methods[] = {
    {"nativeInit", "(Ljava/lang/String;[BI)J", (void *) BackupKit_nativeInit},
    {"nativeRun", "(JJ[Ljava/lang/String;)I", (void *) BackupKit_nativeRun},
    {"nativeCancel", "(J)V", (void *) BackupKit_nativeCancel},
    {"nativeFinish", "(J)V", (void *) BackupKit_nativeFinish},
    {"nativeStatementCount", "(J)I", (void *) BackupKit_nativeStatementCount},
    {"nativeLastError", "(J)Ljava/lang/String;",
     (void *) BackupKit_nativeLastError},
};

static const JNINativeMethod RecoverKit_methods[] = {
    {"nativeInit", "(Ljava/lang/String;[B)J", (void *) RecoverKit_nativeInit},
    {"nativeRun", "(JJZ)I", (void *) RecoverKit_nativeRun},
    {"nativeCancel", "(J)V", (void *) RecoverKit_nativeCancel},
    {"nativeFinish", "(J)V", (void *) RecoverKit_nativeFinish},
    {"nativeSuccessCount", "(J)I", (void *) RecoverKit_nativeSuccessCount},
    {"nativeFailureCount", "(J)I", (void *) RecoverKit_nativeFailureCount},
    {"nativeLastError", "(J)Ljava/lang/String;",
     (void *) RecoverKit_nativeLastError},
};

static int register_wcdb_repair_backup(JavaVM *vm, JNIEnv *env)
{
    jniRegisterNativeMethods(env, "com/tencent/wcdb/repair/BackupKit",
                             BackupKit_methods, NELEM(BackupKit_methods));
    jniRegisterNativeMethods(env, "com/tencent/wcdb/repair/RecoverKit",
                             RecoverKit_methods, NELEM(RecoverKit_methods));

    return 0;
}
WCDB_JNI_INIT(DBBackup, register_wcdb_repair_backup)
}
