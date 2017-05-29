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
#include <jni.h>
#include <repair/SQLiteRepairKit.h>
#include <stdlib.h>

static int g_error_result = 0;
static char g_error_msg[2048] = {0};

namespace wcdb {
static sqliterk_cipher_conf *parseCipherSpec(JNIEnv *env, jobject cipherSpec)
{
    sqliterk_cipher_conf *result = NULL;
    jfieldID fidCipher;
    jfieldID fidKdfIteration;
    jfieldID fidHmacEnabled;
    jfieldID fidPageSize;
    jstring cipherStr;
    const char *cipher;
    int cipher_len;

    if (!cipherSpec) {
        result = (sqliterk_cipher_conf *) malloc(sizeof(sqliterk_cipher_conf));
        memset(result, 0, sizeof(sqliterk_cipher_conf));
        result->use_hmac = -1;
        return result;
    }

    jclass clsCipherSpec =
        env->FindClass("com/tencent/wcdb/database/SQLiteCipherSpec");
    if (!clsCipherSpec)
        goto bail;

    fidCipher = env->GetFieldID(clsCipherSpec, "cipher", "Ljava/lang/String;");
    if (!fidCipher)
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

    cipher = NULL;
    cipher_len = 0;
    cipherStr = (jstring) env->GetObjectField(cipherSpec, fidCipher);
    if (cipherStr) {
        cipher_len = env->GetStringUTFLength(cipherStr) + 1;
        cipher = env->GetStringUTFChars(cipherStr, NULL);
    }

    result = (sqliterk_cipher_conf *) malloc(sizeof(sqliterk_cipher_conf) +
                                             cipher_len);
    if (cipher) {
        result->cipher_name = (const char *) &result[1];
        strlcpy((char *) result->cipher_name, cipher, cipher_len + 1);
        env->ReleaseStringUTFChars(cipherStr, cipher);
    } else
        result->cipher_name = NULL;

    result->page_size = env->GetIntField(cipherSpec, fidPageSize);
    result->kdf_iter = env->GetIntField(cipherSpec, fidKdfIteration);
    result->use_hmac = env->GetBooleanField(cipherSpec, fidHmacEnabled);

    return result;

bail:
    free(result);
    return NULL;
}

static JNICALL jlong nativeInit(JNIEnv *env,
                                jclass cls,
                                jstring pathStr,
                                jbyteArray keyArr,
                                jobject cipherSpec,
                                jbyteArray saltArr)
{

    sqliterk *rk = NULL;
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
    }

    const char *path = env->GetStringUTFChars(pathStr, NULL);
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

static JNICALL jboolean nativeOutput(JNIEnv *env,
                                     jclass cls,
                                     jlong rkPtr,
                                     jlong dbPtr,
                                     jlong masterPtr,
                                     jint flags)
{
    sqliterk *rk = (sqliterk *) (intptr_t) rkPtr;
    sqlite3 *db = (sqlite3 *) (intptr_t) dbPtr;
    sqliterk_master_info *master =
        (sqliterk_master_info *) (intptr_t) masterPtr;

    int rc = sqliterk_output(rk, db, master, flags);
    return (rc == SQLITERK_OK) ? JNI_TRUE : JNI_FALSE;
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

static JNICALL jlong nativeMakeMaster(JNIEnv *env,
                                      jclass cls,
                                      jobjectArray tableArr)
{
    int num_tables = env->GetArrayLength(tableArr);
    const char **tables =
        (const char **) malloc(sizeof(const char *) * num_tables);

    for (int i = 0; i < num_tables; i++) {
        jstring str = (jstring) env->GetObjectArrayElement(tableArr, i);
        tables[i] = env->GetStringUTFChars(str, NULL);
        env->DeleteLocalRef(str);
    }

    sqliterk_master_info *master = NULL;
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
    jbyte *key = NULL;
    if (keyArr) {
        key_len = env->GetArrayLength(keyArr);
        if (key_len > 4096)
            return 0;

        key = (jbyte *) alloca(key_len);
        env->GetByteArrayRegion(keyArr, 0, key_len, key);
    }

    const char *path = env->GetStringUTFChars(pathStr, NULL);
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
    const char *path = env->GetStringUTFChars(pathStr, NULL);

    int key_len = 0;
    jbyte *key = NULL;
    if (keyArr) {
        key_len = env->GetArrayLength(keyArr);
        key = env->GetByteArrayElements(keyArr, NULL);
    }

    int num_tables = 0;
    const char **tables = NULL;
    if (tableArr) {
        num_tables = env->GetArrayLength(tableArr);
        tables = (const char **) malloc(sizeof(const char *) * num_tables);

        for (int i = 0; i < num_tables; i++) {
            jstring str = (jstring) env->GetObjectArrayElement(tableArr, i);
            tables[i] = env->GetStringUTFChars(str, NULL);
            env->DeleteLocalRef(str);
        }
    }

    unsigned char salt[16];
    sqliterk_master_info *master = NULL;
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

static const JNINativeMethod sMethods[] = {
    {"nativeInit",
     "(Ljava/lang/String;[BLcom/tencent/wcdb/database/SQLiteCipherSpec;[B)J",
     (void *) nativeInit},
    {"nativeFini", "(J)V", (void *) nativeFini},
    {"nativeOutput", "(JJJI)Z", (void *) nativeOutput},
    {"nativeIntegrityFlags", "(J)I", (void *) nativeIntegrityFlags},
    {"nativeLastError", "()Ljava/lang/String;", (void *) nativeLastError},
    {"nativeMakeMaster", "([Ljava/lang/String;)J", (void *) nativeMakeMaster},
    {"nativeSaveMaster", "(JLjava/lang/String;[B)Z", (void *) nativeSaveMaster},
    {"nativeLoadMaster", "(Ljava/lang/String;[B[Ljava/lang/String;[B)J",
     (void *) nativeLoadMaster},
    {"nativeFreeMaster", "(J)V", (void *) nativeFreeMaster},
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
    sqliterk_os os = {xlog};
    sqliterk_register(os);

    jniRegisterNativeMethods(env, "com/tencent/wcdb/repair/RepairKit", sMethods,
                             NELEM(sMethods));

    return 0;
}
WCDB_JNI_INIT(DBRepair, register_wcdb_repair)
}
