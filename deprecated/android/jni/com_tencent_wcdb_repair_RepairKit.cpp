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
#include "SQLiteCommon.h"
#include <sqlite3.h>
#include <jni.h>
#include <SQLiteRepairKit.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

static int g_error_result = 0;
static char g_error_msg[2048] = {0};

namespace wcdb {

static jmethodID sMID_onProgress = nullptr;

static sqliterk_cipher_conf *parseCipherSpec(JNIEnv *env, jobject cipherSpec)
{
    sqliterk_cipher_conf *result = nullptr;
    jfieldID fidKdfIteration;
    jfieldID fidHmacEnabled;
    jfieldID fidPageSize;

    if (!cipherSpec) {
        result = (sqliterk_cipher_conf *) malloc(sizeof(sqliterk_cipher_conf));
        if (!result) return nullptr;

        memset(result, 0, sizeof(sqliterk_cipher_conf));
        result->use_hmac = -1;
        return result;
    }

    jclass clsCipherSpec =
        env->FindClass("com/tencent/wcdb/database/SQLiteCipherSpec");
    if (!clsCipherSpec)
        goto bail;

    fidKdfIteration = env->GetFieldID(clsCipherSpec, "kdfIteration", "I");
    if (!fidKdfIteration)
        goto bail;
    fidHmacEnabled = env->GetFieldID(clsCipherSpec, "hmacEnabled", "Z");
    if (!fidHmacEnabled)
        goto bail;
    fidPageSize = env->GetFieldID(clsCipherSpec, "pageSize", "I");
    if (!fidPageSize)
        goto bail;

    result = (sqliterk_cipher_conf *) malloc(sizeof(sqliterk_cipher_conf));
    if (!result) return nullptr;
    memset(result, 0, sizeof(sqliterk_cipher_conf));

    result->page_size = env->GetIntField(cipherSpec, fidPageSize);
    result->kdf_iter = env->GetIntField(cipherSpec, fidKdfIteration);
    result->use_hmac = env->GetBooleanField(cipherSpec, fidHmacEnabled);

    return result;

bail:
    free(result);
    return nullptr;
}

static JNICALL jlong nativeInit(JNIEnv *env,
                                jclass cls,
                                jstring pathStr,
                                jbyteArray keyArr,
                                jobject cipherSpec,
                                jbyteArray saltArr)
{

    sqliterk *rk = nullptr;
    sqliterk_cipher_conf *conf = parseCipherSpec(env, cipherSpec);
    if (!conf)
        return 0;

    if (keyArr) {
        conf->key_len = env->GetArrayLength(keyArr);
        if (conf->key_len > 4096) {
            free(conf);
            return 0;
        }

        conf->key = alloca(conf->key_len);
        env->GetByteArrayRegion(keyArr, 0, conf->key_len, (jbyte *) conf->key);
    }
    if (saltArr) {
        if (env->GetArrayLength(saltArr) < 16) {
            free(conf);
            return 0;
        }

        conf->kdf_salt = (unsigned char *) alloca(16);
        env->GetByteArrayRegion(saltArr, 0, 16, (jbyte *) conf->kdf_salt);
    } else {
        conf->kdf_salt = nullptr;
    }

    const char *path = env->GetStringUTFChars(pathStr, nullptr);
    int rc = sqliterk_open(path, conf, &rk);
    env->ReleaseStringUTFChars(pathStr, path);

    free(conf);
    return (rc == SQLITERK_OK) ? (jlong)(intptr_t) rk : 0;
}

static JNICALL void nativeFini(JNIEnv *env, jclass cls, jlong rkPtr)
{
    sqliterk *rk = (sqliterk *) (intptr_t) rkPtr;
    sqliterk_close(rk);
}

struct callback_data {
    JNIEnv *env;
    jobject obj;
    jstring last_table;
    int last_root;
};

static int output_callback(void *user, sqliterk *rk, sqliterk_table *table, sqliterk_column *column)
{
    callback_data *d = (callback_data *) user;
    const char *table_name = sqliterk_table_name(table);
    int root = sqliterk_table_root(table);

    if (d->last_root != root) {
        if (d->last_table)
            d->env->DeleteLocalRef(d->last_table);
        
        d->last_table = d->env->NewStringUTF(table_name);
        d->last_root = root;
    }

    jint result = d->env->CallIntMethod(d->obj, sMID_onProgress, d->last_table,
        root, (jlong) (intptr_t) column);

    switch (result) {
        case 0: // RESULT_OK
            return SQLITERK_OK;
        case 1: // RESULT_CANCELLED
            return SQLITERK_CANCELLED;
        case 2: // RESULT_IGNORE
            return SQLITERK_IGNORE;
    }
    return SQLITERK_MISUSE;
}

static JNICALL jint nativeOutput(JNIEnv *env,
                                     jobject obj,
                                     jlong rkPtr,
                                     jlong dbPtr,
                                     jlong masterPtr,
                                     jlong leafPtr,
                                     jint flags)
{
    sqliterk *rk = (sqliterk *) (intptr_t) rkPtr;
    sqlite3 *db = (sqlite3 *) (intptr_t) dbPtr;
    sqliterk_master_info *master =(sqliterk_master_info *) (intptr_t) masterPtr;
    sqliterk_leaf_info *leaf = (sqliterk_leaf_info *) (intptr_t) leafPtr;

    callback_data data;
    data.env = env;
    data.obj = obj;
    data.last_table = nullptr;
    data.last_root = 0;
    
    int rc = sqliterk_output_cb_leaf(rk, db, master, leaf, flags, output_callback, &data);
    if (rc == SQLITERK_OK) return 0;
    if (rc == SQLITERK_CANCELLED) return 1;
    return -1;
}

static JNICALL void nativeCancel(JNIEnv *env, jclass cls, jlong rkPtr)
{
    sqliterk *rk = (sqliterk *) (intptr_t) rkPtr;
    sqliterk_cancel(rk);
}

static JNICALL jint nativeIntegrityFlags(JNIEnv *env, jclass cls, jlong rkPtr)
{
    sqliterk *rk = (sqliterk *) (intptr_t) rkPtr;
    return (jint) sqliterk_integrity(rk);
}

static JNICALL jstring nativeLastError(JNIEnv *env, jclass cls)
{
    return env->NewStringUTF(g_error_msg);
}

static JNICALL jintArray nativeGetStatistics(JNIEnv *env, jclass cls, jlong rkPtr)
{
    sqliterk *rk = (sqliterk *) (intptr_t) rkPtr;
    jint st[4];

    st[0] = sqliterk_page_count(rk);
    st[1] = sqliterk_valid_page_count(rk);
    st[2] = sqliterk_parsed_page_count(rk);
    st[3] = sqliterk_damaged_page_count(rk);

    jintArray result = env->NewIntArray(4);
    env->SetIntArrayRegion(result, 0, 4, st);
    return result;
}

static JNICALL jlong nativeMakeMaster(JNIEnv *env,
                                      jclass cls,
                                      jobjectArray tableArr)
{
    int num_tables = env->GetArrayLength(tableArr);
    const char **tables =
        (const char **) malloc(sizeof(const char *) * num_tables);

    for (int i = 0; i < num_tables; i++) {
        jstring str = (jstring) env->GetObjectArrayElement(tableArr, i);
        tables[i] = env->GetStringUTFChars(str, nullptr);
        env->DeleteLocalRef(str);
    }

    sqliterk_master_info *master = nullptr;
    int rc = sqliterk_make_master(tables, num_tables, &master);

    for (int i = 0; i < num_tables; i++) {
        jstring str = (jstring) env->GetObjectArrayElement(tableArr, i);
        env->ReleaseStringUTFChars(str, tables[i]);
        env->DeleteLocalRef(str);
    }
    free(tables);

    return (rc == SQLITERK_OK) ? (jlong) master : 0;
}

static JNICALL jboolean nativeSaveMaster(
    JNIEnv *env, jclass cls, jlong dbPtr, jstring pathStr, jbyteArray keyArr)
{
    sqlite3 *db = (sqlite3 *) (intptr_t) dbPtr;

    int key_len = 0;
    jbyte *key = nullptr;
    if (keyArr) {
        key_len = env->GetArrayLength(keyArr);
        if (key_len > 4096)
            return 0;

        key = (jbyte *) alloca(key_len);
        env->GetByteArrayRegion(keyArr, 0, key_len, key);
    }

    const char *path = env->GetStringUTFChars(pathStr, nullptr);
    int rc = sqliterk_save_master(db, path, key, key_len);
    env->ReleaseStringUTFChars(pathStr, path);

    return (rc == SQLITERK_OK) ? JNI_TRUE : JNI_FALSE;
}

static JNICALL jlong nativeLoadMaster(JNIEnv *env,
                                      jclass cls,
                                      jstring pathStr,
                                      jbyteArray keyArr,
                                      jobjectArray tableArr,
                                      jbyteArray outSaltArr)
{
    // Path is guaranteed to be non-null by the Java part.
    const char *path = env->GetStringUTFChars(pathStr, nullptr);

    int key_len = 0;
    jbyte *key = nullptr;
    if (keyArr) {
        key_len = env->GetArrayLength(keyArr);
        key = env->GetByteArrayElements(keyArr, nullptr);
    }

    int num_tables = 0;
    const char **tables = nullptr;
    if (tableArr) {
        num_tables = env->GetArrayLength(tableArr);
        tables = (const char **) malloc(sizeof(const char *) * num_tables);

        for (int i = 0; i < num_tables; i++) {
            jstring str = (jstring) env->GetObjectArrayElement(tableArr, i);
            tables[i] = env->GetStringUTFChars(str, nullptr);
            env->DeleteLocalRef(str);
        }
    }

    unsigned char salt[16];
    sqliterk_master_info *master = nullptr;
    int rc = sqliterk_load_master(path, key, key_len, tables, num_tables,
                                  &master, salt);

    if (outSaltArr) {
        int len = env->GetArrayLength(outSaltArr);
        if (len < 16)
            return 0;

        env->SetByteArrayRegion(outSaltArr, 0, sizeof(salt), (jbyte *) salt);
    }

    if (tableArr) {
        for (int i = 0; i < num_tables; i++) {
            jstring str = (jstring) env->GetObjectArrayElement(tableArr, i);
            env->ReleaseStringUTFChars(str, tables[i]);
            env->DeleteLocalRef(str);
        }
        free(tables);
    }

    if (key)
        env->ReleaseByteArrayElements(keyArr, key, 0);

    env->ReleaseStringUTFChars(pathStr, path);

    return (rc == SQLITERK_OK) ? (jlong)(intptr_t) master : 0;
}

static JNICALL void nativeFreeMaster(JNIEnv *env, jclass cls, jlong masterPtr)
{
    sqliterk_master_info *master =
        (sqliterk_master_info *) (intptr_t) masterPtr;
    sqliterk_free_master(master);
}

// TODO: global variable should not be used here
static volatile int sScanCancelFlag = 0;
static JNICALL jlong nativeScanLeaf(JNIEnv *env, jclass cls, jlong dbPtr, jobjectArray tableArr,
                                    jlongArray outCancelFlag)
{
    sqlite3 *db = (sqlite3 *) (intptr_t) dbPtr;
    std::vector<const char *> tables;
    if (tableArr) {
        int num_tables = env->GetArrayLength(tableArr);
        tables = std::vector<const char *>(num_tables);
        for (int i = 0; i < num_tables; ++i) {
            jstring str = static_cast<jstring>(env->GetObjectArrayElement(tableArr, i));
            tables[i] = (env->GetStringUTFChars(str, NULL));
            env->DeleteLocalRef(str);
        }
    }

    if (outCancelFlag) {
        sScanCancelFlag = 0;
        jlong ptr = (jlong) (intptr_t) &sScanCancelFlag;
        env->SetLongArrayRegion(outCancelFlag, 0, 1, &ptr);
    }

    sqliterk_leaf_info *out;
    int rc = sqliterk_scan_leaf(db, tables.data(), tables.size(), &out,
                                outCancelFlag ? &sScanCancelFlag : nullptr);
    jlong result = (rc == SQLITE_OK) ? (jlong) (intptr_t) out : 0;

    for (int i = 0; i < tables.size(); ++i) {
        jstring str = static_cast<jstring>(env->GetObjectArrayElement(tableArr, i));
        env->ReleaseStringUTFChars(str, tables[i]);
        env->DeleteLocalRef(str);
    }

    if (rc != SQLITE_OK) {
        throw_sqlite3_exception(env, db);
    }

    return result;
}

static JNICALL void nativeCancelScanLeaf(JNIEnv *env, jclass cls, jlong cancelFlagPtr) {
    *(volatile int *) (intptr_t) cancelFlagPtr = 1;
}

static JNICALL jboolean nativeSaveLeaf(JNIEnv *env, jclass cls, jlong leafPtr, jstring pathStr) {
    sqliterk_leaf_info *li = (sqliterk_leaf_info *) (intptr_t) leafPtr;

    const char *path = env->GetStringUTFChars(pathStr, nullptr);
    int rc = sqliterk_save_leaf(li, path);

    env->ReleaseStringUTFChars(pathStr, path);
    return rc == SQLITERK_OK;
}

static JNICALL jlong nativeLoadLeaf(JNIEnv *env, jclass cls, jstring pathStr) {
    const char *path = env->GetStringUTFChars(pathStr, nullptr);

    sqliterk_leaf_info *out;
    int rc = sqliterk_load_leaf(path, &out);

    env->ReleaseStringUTFChars(pathStr, path);
    return rc == SQLITERK_OK ? (jlong) (intptr_t) out : 0;
}

static JNICALL void nativeFreeLeaf(JNIEnv *env, jclass cls, jlong leafPtr) {
    sqliterk_free_leaf((sqliterk_leaf_info *) (intptr_t) leafPtr);
}

static JNICALL jint cursorNativeGetColumnCount(JNIEnv *env, jclass, jlong ptr)
{
    sqliterk_column *column = (sqliterk_column *) (intptr_t) ptr;
    return sqliterk_column_count(column);
}

static JNICALL jint cursorNativeGetType(JNIEnv *env, jclass, jlong ptr, jint idx)
{
    sqliterk_column *column = (sqliterk_column *) (intptr_t) ptr;
    return sqliterk_column_type(column, idx);
}

static JNICALL jlong cursorNativeGetLong(JNIEnv *env, jclass, jlong ptr, jint idx)
{
    sqliterk_column *column = (sqliterk_column *) (intptr_t) ptr;
    return sqliterk_column_integer64(column, idx);
}

static JNICALL double cursorNativeGetDouble(JNIEnv *env, jclass, jlong ptr, jint idx)
{
    sqliterk_column *column = (sqliterk_column *) (intptr_t) ptr;
    return sqliterk_column_number(column, idx);
}

static JNICALL jstring cursorNativeGetString(JNIEnv *env, jclass, jlong ptr, jint idx)
{
    sqliterk_column *column = (sqliterk_column *) (intptr_t) ptr;

    // TODO: handle UTF-8 => UTF-16 convertion
    const char *result = sqliterk_column_text(column, idx);
    return env->NewStringUTF(result);
}

static JNICALL jbyteArray cursorNativeGetBlob(JNIEnv *env, jclass, jlong ptr, jint idx)
{
    sqliterk_column *column = (sqliterk_column *) (intptr_t) ptr;
    const void *arr = sqliterk_column_binary(column, idx);
    int size = sqliterk_column_bytes(column, idx);
    jbyteArray result = env->NewByteArray(size);
    env->SetByteArrayRegion(result, 0, size, (const jbyte *) arr);
    return result;
}

static const JNINativeMethod sRepairMethods[] = {
    {"nativeInit",
     "(Ljava/lang/String;[BLcom/tencent/wcdb/database/SQLiteCipherSpec;[B)J",
     (void *) nativeInit},
    {"nativeFini", "(J)V", (void *) nativeFini},
    {"nativeOutput", "(JJJJI)I", (void *) nativeOutput},
    {"nativeCancel", "(J)V", (void *) nativeCancel},
    {"nativeIntegrityFlags", "(J)I", (void *) nativeIntegrityFlags},
    {"nativeLastError", "()Ljava/lang/String;", (void *) nativeLastError},
    {"nativeGetStatistics", "(J)[I", (void *) nativeGetStatistics},
    {"nativeMakeMaster", "([Ljava/lang/String;)J", (void *) nativeMakeMaster},
    {"nativeSaveMaster", "(JLjava/lang/String;[B)Z", (void *) nativeSaveMaster},
    {"nativeLoadMaster", "(Ljava/lang/String;[B[Ljava/lang/String;[B)J",
     (void *) nativeLoadMaster},
    {"nativeFreeMaster", "(J)V", (void *) nativeFreeMaster},
    {"nativeScanLeaf", "(J[Ljava/lang/String;[J)J", (void *) nativeScanLeaf},
    {"nativeCancelScanLeaf", "(J)V", (void *) nativeCancelScanLeaf},
    {"nativeSaveLeaf", "(JLjava/lang/String;)Z", (void *) nativeSaveLeaf},
    {"nativeLoadLeaf", "(Ljava/lang/String;)J", (void *) nativeLoadLeaf},
    {"nativeFreeLeaf", "(J)V", (void *) nativeFreeLeaf},
};

static const JNINativeMethod sCursorMethods[] = {
    {"nativeGetColumnCount", "(J)I", (void *) cursorNativeGetColumnCount},
    {"nativeGetType", "(JI)I", (void *) cursorNativeGetType},
    {"nativeGetLong", "(JI)J", (void *) cursorNativeGetLong},
    {"nativeGetDouble", "(JI)D", (void *) cursorNativeGetDouble},
    {"nativeGetString", "(JI)Ljava/lang/String;", (void *) cursorNativeGetString},
    {"nativeGetBlob", "(JI)[B", (void *) cursorNativeGetBlob},
};

static void xlog(sqliterk_loglevel level, int result, const char *msg)
{
    android_LogPriority lv;
    switch (level) {
        case sqliterk_loglevel_error:
            // XXX: thread-safety is not considered here, which may cause problems.
            g_error_result = result;
            strlcpy(g_error_msg, msg, sizeof(g_error_msg));

            lv = ANDROID_LOG_ERROR;
            break;
        case sqliterk_loglevel_warning:
            lv = ANDROID_LOG_WARN;
            break;
        case sqliterk_loglevel_info:
            lv = ANDROID_LOG_INFO;
            break;
        case sqliterk_loglevel_debug:
            lv = ANDROID_LOG_DEBUG;
            break;
        default:
            lv = ANDROID_LOG_VERBOSE;
    }

    wcdb_log_write(lv, "WCDB.RepairKit", msg);
}

static int register_wcdb_repair(JavaVM *vm, JNIEnv *env)
{
    char msg[256];
    sqliterk_os os = {xlog};
    sqliterk_register(os);

    jclass cls = env->FindClass("com/tencent/wcdb/repair/RepairKit");
    if (!cls) {
        snprintf(msg, sizeof(msg),
                 "Unable to find class '%s', aborting",
                 "com/tencent/wcdb/repair/RepairKit");
        env->FatalError(msg);
    }
    
    sMID_onProgress = env->GetMethodID(cls, "onProgress", "(Ljava/lang/String;IJ)I");
    if (!sMID_onProgress) {
        snprintf(msg, sizeof(msg),
                 "Unable to find method '%s' with signature '%s', aborting",
                 "onProgress", "(Ljava/lang/String;IJ)I");
        env->FatalError(msg);
    }

    if (env->RegisterNatives(cls, sRepairMethods, NELEM(sRepairMethods)) < 0) {
        snprintf(msg, sizeof(msg), "RegisterNatives failed for '%s', aborting",
                 "com/tencent/wcdb/repair/RepairKit");
        env->FatalError(msg);
    }
    env->DeleteLocalRef(cls);

    jniRegisterNativeMethods(env, "com/tencent/wcdb/repair/RepairKit$RepairCursor", 
                             sCursorMethods, NELEM(sCursorMethods));

    return 0;
}
WCDB_JNI_INIT(DBRepair, register_wcdb_repair)
}
