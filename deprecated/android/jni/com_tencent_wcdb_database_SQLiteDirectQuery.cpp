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
#include "ModuleLoader.h"
#include "SQLiteCommon.h"
#include <jni.h>
#include <sqlite3.h>
#include <stdint.h>

namespace wcdb {

static jlong
nativeGetLong(JNIEnv *env, jclass cls, jlong statementPtr, jint column)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;
    return sqlite3_column_int64(stmt, column);
}

static jdouble
nativeGetDouble(JNIEnv *env, jclass cls, jlong statementPtr, jint column)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;
    return sqlite3_column_double(stmt, column);
}

static jstring
nativeGetString(JNIEnv *env, jclass cls, jlong statementPtr, jint column)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;

    // Use sqlite internal UTF-8 to UTF-16 conversion instead of NewStringUTF.
    int len = sqlite3_column_bytes16(stmt, column) / 2;
    const jchar *str = (const jchar *) sqlite3_column_text16(stmt, column);
    return env->NewString(str, len);
}

static jbyteArray
nativeGetBlob(JNIEnv *env, jclass cls, jlong statementPtr, jint column)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;

    const jbyte *blob = (const jbyte *) sqlite3_column_blob(stmt, column);
    int len = sqlite3_column_bytes(stmt, column);

    jbyteArray result = env->NewByteArray(len);
    env->SetByteArrayRegion(result, 0, len, blob);
    return result;
}

static jint
nativeGetType(JNIEnv *env, jclass cls, jlong statementPtr, jint column)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;
    return sqlite3_column_type(stmt, column);
}

static jint nativeStep(JNIEnv *env, jclass cls, jlong statementPtr, jint count)
{
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;

    jint i;
    for (i = 0; i < count; i++) {
        int ret = sqlite3_step(stmt);
        if (ret == SQLITE_DONE) {
            // Reach end of result, return i as total rows steped.
            break;
        } else if (ret != SQLITE_ROW) {
            throw_sqlite3_exception(env, sqlite3_db_handle(stmt));
            return -1;
        }
    }

    return i;
}

static JNINativeMethod sMethods[] = {
    {"nativeGetLong", "(JI)J", (void *) nativeGetLong},
    {"nativeGetDouble", "(JI)D", (void *) nativeGetDouble},
    {"nativeGetString", "(JI)Ljava/lang/String;", (void *) nativeGetString},
    {"nativeGetBlob", "(JI)[B", (void *) nativeGetBlob},
    {"nativeGetType", "(JI)I", (void *) nativeGetType},
    {"nativeStep", "(JI)I", (void *) nativeStep},
};

static jint register_wcdb_SQLiteDirectQuery(JavaVM *vm, JNIEnv *env)
{
    return jniRegisterNativeMethods(
        env, "com/tencent/wcdb/database/SQLiteDirectQuery", sMethods,
        NELEM(sMethods));
}
WCDB_JNI_INIT(SQLiteDirectCursor, register_wcdb_SQLiteDirectQuery)
}
