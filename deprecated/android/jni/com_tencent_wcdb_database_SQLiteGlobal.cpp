/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <sqlite3.h>
#include <stdio.h>
#include <assert.h>

#include "JNIHelp.h"
#include "Logger.h"
#include "ModuleLoader.h"
#include "SQLiteCommon.h"

// Forward declarations
extern "C" {
    void sqlcipher_set_default_pagesize(int page_size);
    void sqlcipher_set_default_kdf_iter(int iter);
    void sqlcipher_set_default_use_hmac(int use);
    int sqlcipher_set_default_hmac_algorithm(int algorithm);
    int sqlcipher_set_default_kdf_algorithm(int algorithm);
    void sqlcipher_set_mem_security(int);
    int sqlite3_register_vfslog(const char *);
}
extern volatile uint32_t vlogDefaultLogFlags;

namespace wcdb {

static JavaVM *gVM = nullptr;

// Limit heap to 8MB for now.  This is 4 times the maximum cursor window
// size, as has been used by the original code in SQLiteDatabase for
// a long time.
static const int SOFT_HEAP_LIMIT = 8 * 1024 * 1024;

extern volatile int sLastErrorLine;

// Called each time a message is logged.
static void sqliteLogCallback(void *data, int iErrCode, const char *zMsg)
{
    // Extract line number for specific error codes.
    const char *pattern = nullptr;
    int priority = ANDROID_LOG_WARN;
    switch (iErrCode & 0xFF) {
        case SQLITE_NOTICE:
            priority = ANDROID_LOG_INFO;
            break;
        case SQLITE_OK:
        case SQLITE_SCHEMA:
        case SQLITE_CONSTRAINT:
            priority = ANDROID_LOG_DEBUG;
            break;
        case SQLITE_CORRUPT:
            priority = ANDROID_LOG_ERROR;
            pattern = "database corruption at line %d of [";
            break;
        case SQLITE_MISUSE:
            priority = ANDROID_LOG_ERROR;
            pattern = "misuse at line %d of [";
            break;
        case SQLITE_CANTOPEN:
            priority = ANDROID_LOG_ERROR;
            pattern = "cannot open file at line %d of [";
            break;
    }

    if (pattern) {
        int lineno;
        int ret = sscanf(zMsg, pattern, &lineno);
        if (ret > 0)
            sLastErrorLine = lineno;
    }

    wcdb_log_print(priority, "WCDB.SQLite", "[SQLite ErrCode: %d] %s\n",
                   iErrCode, zMsg);
}

static int sqliteExtensionApiStealer(sqlite3 *db, const char **pzErrMsg,
                                     const sqlite3_api_routines *pThunk)
{
    // Set the global API environment pointer then exit.
    // XXX: Currently SQLite calls extension entry point with a static global sqlite3_api_routines
    // structure that never change. We can safely store and reuse this structure.
    JNIEnv *env = nullptr;
    bool attached = false;
    jint ret = gVM->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (ret == JNI_EDETACHED) {
        jint ret = gVM->AttachCurrentThread(&env, nullptr);
        assert(ret == JNI_OK);
        (void) ret;
        attached = true;
    }

    jclass clazz;
    FIND_CLASS(clazz, "com/tencent/wcdb/database/WCDBInitializationProbe");

    jfieldID fidApiEnv = env->GetStaticFieldID(clazz, "apiEnv", "J");
    env->SetStaticLongField(clazz, fidApiEnv, (jlong)(intptr_t) pThunk);
    env->DeleteLocalRef(clazz);

    if (attached) {
        gVM->DetachCurrentThread();
    }
    return SQLITE_OK;
}

// Sets the global SQLite configuration.
// This must be called before any other SQLite functions are called.
static void sqliteInitialize()
{
    // Enable multi-threaded mode.  In this mode, SQLite is safe to use by multiple
    // threads as long as no two threads use the same database connection at the same
    // time (which we guarantee in the SQLite database wrappers).
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);

    // Redirect SQLite log messages to the Android log.
    sqlite3_config(SQLITE_CONFIG_LOG, &sqliteLogCallback, (void *) 1);

    // The soft heap limit prevents the page cache allocations from growing
    // beyond the given limit, no matter what the max page cache sizes are
    // set to. The limit does not, as of 3.5.0, affect any other allocations.
    sqlite3_soft_heap_limit(SOFT_HEAP_LIMIT);

    // Register vfslog VFS.
    sqlite3_register_vfslog(nullptr);

    // Initialize SQLite.
    sqlite3_initialize();

    // Register the API environment stealer as the auto-loaded extension.
    sqlite3_auto_extension((void (*)(void)) sqliteExtensionApiStealer);
}

static jint nativeReleaseMemory(JNIEnv *env, jclass clazz)
{
    return sqlite3_release_memory(SOFT_HEAP_LIMIT);
}

static void nativeInitialize(JNIEnv *env, jclass clazz, jint pageSize)
{
    sqliteInitialize();
    sqlcipher_set_default_pagesize(pageSize);

    // Keep compatibility to WCDB version 1.0, mainly the same as SQLCipher 3.
    sqlcipher_set_default_kdf_iter(64000);
    sqlcipher_set_default_use_hmac(1);
    sqlcipher_set_default_hmac_algorithm(0);    // SHA1
    sqlcipher_set_default_kdf_algorithm(0);     // SHA1
    sqlcipher_set_mem_security(0);
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"nativeInitialize", "(I)V", (void *) nativeInitialize},
    {"nativeReleaseMemory", "()I", (void *) nativeReleaseMemory},
};

static int register_wcdb_SQLiteGlobal(JavaVM *vm, JNIEnv *env)
{
    gVM = vm;

    jclass clazz;
    FIND_CLASS(clazz, "com/tencent/wcdb/database/WCDBInitializationProbe");

    jfieldID fidLibLoaded = env->GetStaticFieldID(clazz, "libLoaded", "Z");
    env->SetStaticBooleanField(clazz, fidLibLoaded, JNI_TRUE);
    env->DeleteLocalRef(clazz);

    return jniRegisterNativeMethods(env,
                                    "com/tencent/wcdb/database/SQLiteGlobal",
                                    sMethods, NELEM(sMethods));
}
WCDB_JNI_INIT(SQLiteGlobal, register_wcdb_SQLiteGlobal)

} // namespace wcdb
