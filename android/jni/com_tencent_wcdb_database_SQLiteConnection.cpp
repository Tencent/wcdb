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

#define LOG_TAG "WCDB.SQLiteConnection"

#include <assert.h>
#include <jni.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <map>
#include <vector>

#include "CursorWindow.h"
#include "Errors.h"
#include "JNIHelp.h"
#include "Logger.h"
#include "ModuleLoader.h"
#include "SQLiteCommon.h"
//#include <sqlite3_android.h>

// Set to 1 to use UTF16 storage for localized indexes.
#define UTF16_STORAGE 0

namespace wcdb {

/* Busy timeout in milliseconds.
 * If another connection (possibly in another process) has the database locked for
 * longer than this amount of time then SQLite will generate a SQLITE_BUSY error.
 * The SQLITE_BUSY error is then raised as a SQLiteDatabaseLockedException.
 *
 * In ordinary usage, busy timeouts are quite rare.  Most databases only ever
 * have a single open connection at a time unless they are using WAL.  When using
 * WAL, a timeout could occur if one connection is busy performing an auto-checkpoint
 * operation.  The busy timeout needs to be long enough to tolerate slow I/O write
 * operations but not so long as to cause the application to hang indefinitely if
 * there is a problem acquiring a database lock.
 */
//static const int BUSY_TIMEOUT_MS = 2500;
static const int BUSY_TIMEOUT_MS = 10 * 1000;

static JavaVM *gVM;

static struct {
    jfieldID name;
    jfieldID numArgs;
    jmethodID dispatchCallback;
} gSQLiteCustomFunctionClassInfo;

static struct {
    jclass clazz;
} gStringClassInfo;

static struct {
    jmethodID notifyCheckpoint;
    jmethodID notifyChange;
} gSQLiteConnectionClassInfo;

struct SQLiteConnection {
    // Open flags.
    // Must be kept in sync with the constants defined in SQLiteDatabase.java.
    enum {
        OPEN_READWRITE = 0x00000000,
        OPEN_READONLY = 0x00000001,
        OPEN_READ_MASK = 0x00000001,
        NO_LOCALIZED_COLLATORS = 0x00000010,
        CREATE_IF_NECESSARY = 0x10000000,
    };

    jobject const dbObj;
    sqlite3 *const db;
    const int openFlags;

    volatile bool canceled;

    // Update notification stuff
    struct NotificationKey {
        std::string table;
        std::string db;

        NotificationKey(const char *d_, const char *t_) : table(t_), db(d_) {}

        friend bool operator<(const NotificationKey &lhs,
                              const NotificationKey &rhs)
        {
            int cmp = lhs.table.compare(rhs.table);
            if (!cmp)
                cmp = lhs.db.compare(rhs.db);
            return cmp < 0;
        }
    };

    struct NotificationItem {
        sqlite_int64 rowid;
        int op;

        enum { OP_NOOP = 0, OP_INSERT = 1, OP_UPDATE = 2, OP_DELETE = 3 };

        NotificationItem(sqlite_int64 r_, int op_) : rowid(r_), op(op_) {}

        friend bool operator<(const NotificationItem &lhs,
                              const NotificationItem &rhs)
        {
            return lhs.rowid < rhs.rowid;
        }
    };

    struct NotificationItems {
        int numNotify[3];
        std::vector<NotificationItem> items;
    };

    typedef std::map<NotificationKey, NotificationItems> NotificationMap;

    bool notifyEnabled; // whether notification is enabled
    bool notifyRowId;   // whether rowid is requested on notification
    bool
        notifyCommited; // whether transaction has been commited and notifications are merged
    NotificationMap notifyMap;   // notification operations
    const char *lastNotifyDb;    // cached DB name of the last operation
    const char *lastNotifyTable; // cached table name of the last operation
    NotificationMap::iterator lastNotifyIterator; // cached iterator

    SQLiteConnection(jobject dbObj, sqlite3 *db, int openFlags)
        : dbObj(dbObj)
        , db(db)
        , openFlags(openFlags)
        , canceled(false)
        , notifyEnabled(false)
        , notifyRowId(false)
        , notifyCommited(false)
        , lastNotifyDb(NULL)
        , lastNotifyTable(NULL)
    {
    }
};

// Notification implementation
static inline int toNotifyOp(int op)
{
    switch (op) {
        case SQLITE_INSERT:
            return SQLiteConnection::NotificationItem::OP_INSERT;
        case SQLITE_UPDATE:
            return SQLiteConnection::NotificationItem::OP_UPDATE;
        case SQLITE_DELETE:
            return SQLiteConnection::NotificationItem::OP_DELETE;
        default:
            return 0;
    }
}

static void sqliteUpdateHookCallback(void *ud,
                                     int op,
                                     const char *dbName,
                                     const char *tableName,
                                     sqlite3_int64 rowid)
{
    SQLiteConnection *conn = (SQLiteConnection *) ud;
    assert(conn->notifyEnabled && !conn->notifyCommited);

    // Find or create the table entry.
    SQLiteConnection::NotificationMap::iterator it;

    // XXX: SQLite passes pointer to internal table description structure, which
    // doesn't change during VDBE execution. Thus it's safe to compare the pointer
    // value instead for the fast path.
    if (dbName == conn->lastNotifyDb && tableName == conn->lastNotifyTable) {
        it = conn->lastNotifyIterator;
    } else {
        it = conn->notifyMap
                 .insert(std::make_pair(
                     SQLiteConnection::NotificationKey(dbName, tableName),
                     SQLiteConnection::NotificationItems()))
                 .first;

        conn->lastNotifyDb = dbName;
        conn->lastNotifyTable = tableName;
        conn->lastNotifyIterator = it;
    }

    if (conn->notifyRowId) {
        it->second.items.push_back(
            SQLiteConnection::NotificationItem(rowid, toNotifyOp(op)));
    }
}

static const int NOTIFY_OP_MERGE[] = {
    /*           N/A  INSERT UPDATE DELETE */
    /* N/A    */ 0x00, 0x01, 0x02, 0x03,
    /* INSERT */ 0x01, 0x81, 0x01, 0x00,
    /* UPDATE */ 0x02, 0x81, 0x02, 0x03,
    /* DELETE */ 0x03, 0x02, 0x82, 0x83,
};
static inline int mergeOp(int op1, int op2)
{
    int outOp = NOTIFY_OP_MERGE[((op1 & 0x03) << 2) | (op2 & 0x03)];

    // TODO: warn unexpected operation pair?
    return (outOp & 0x03);
}

static int sqliteCommitHookCallback(void *ud)
{
    SQLiteConnection *conn = (SQLiteConnection *) ud;
    assert(conn->notifyEnabled && !conn->notifyCommited);

    SQLiteConnection::NotificationMap::iterator it;
    for (it = conn->notifyMap.begin(); it != conn->notifyMap.end(); ++it) {

        memset(it->second.numNotify, 0, sizeof(it->second.numNotify));
        if (conn->notifyRowId && !it->second.items.empty()) {
            // Sort IDs.
            std::vector<SQLiteConnection::NotificationItem> &opList =
                it->second.items;
            std::stable_sort(opList.begin(), opList.end());

            // Merge operations of the same ID.
            sqlite_int64 rowid = opList[0].rowid;
            int op = opList[0].op;
            int startIdx = 0;
            it->second.numNotify[op - 1]++;
            for (size_t i = 1; i < opList.size(); i++) {
                if (opList[i].rowid == rowid) {
                    // found the same rowId, merge with the previous ones.
                    op = opList[startIdx].op = mergeOp(op, opList[i].op);
                    opList[i].op = SQLiteConnection::NotificationItem::OP_NOOP;
                } else {
                    if (op > 0 && op < 4) // INSERT, UPDATE or DELETE
                        it->second.numNotify[op - 1]++;

                    rowid = opList[i].rowid;
                    op = opList[i].op;
                    startIdx = i;
                }
            }
        }
    }

    conn->lastNotifyDb = NULL;
    conn->lastNotifyTable = NULL;
    conn->lastNotifyIterator = conn->notifyMap.end();
    conn->notifyCommited = true;

    return 0;
}

static void sqliteRollbackHookCallback(void *ud)
{
    SQLiteConnection *conn = (SQLiteConnection *) ud;
    assert(conn->notifyEnabled && !conn->notifyCommited);

    // Clear notification records.
    conn->notifyMap.clear();
    conn->lastNotifyDb = NULL;
    conn->lastNotifyTable = NULL;
    conn->lastNotifyIterator = conn->notifyMap.end();
}

static void emitUpdateNotifications(JNIEnv *env, SQLiteConnection *conn)
{

    if (!conn->notifyEnabled || !conn->notifyCommited) {
        conn->notifyCommited = false;
        return;
    }

    SQLiteConnection::NotificationMap::iterator it;
    for (it = conn->notifyMap.begin(); it != conn->notifyMap.end(); ++it) {
        jstring dbNameStr = env->NewStringUTF(it->first.db.c_str());
        jstring tableNameStr = env->NewStringUTF(it->first.table.c_str());

        jlongArray idsArr[3] = {NULL};

        if (conn->notifyRowId) {
            int *numNotify = it->second.numNotify;
            for (int i = 0; i < 3; i++)
                idsArr[i] = env->NewLongArray(numNotify[i]);

            jlong *ids[3];
            int idsIdx[3] = {0};
            for (int i = 0; i < 3; i++)
                ids[i] =
                    (jlong *) env->GetPrimitiveArrayCritical(idsArr[i], NULL);

            std::vector<SQLiteConnection::NotificationItem> &opList =
                it->second.items;
            for (size_t i = 0; i < opList.size(); i++) {
                int op = opList[i].op - 1;
                assert(op >= 0 && op < 3);
                assert(idsIdx[op] < numNotify[op]);

                ids[op][idsIdx[op]++] = opList[i].rowid;
            }

            for (int i = 2; i >= 0; i--)
                env->ReleasePrimitiveArrayCritical(idsArr[i], ids[i], 0);
        }

        env->CallVoidMethod(conn->dbObj,
                            gSQLiteConnectionClassInfo.notifyChange, dbNameStr,
                            tableNameStr, idsArr[0], idsArr[1], idsArr[2]);
        if (env->ExceptionCheck()) {
            jniLogException(env, ANDROID_LOG_ERROR, LOG_TAG);
            return;
        }

        env->DeleteLocalRef(dbNameStr);
        env->DeleteLocalRef(tableNameStr);
        for (int i = 0; i < 3; i++)
            if (idsArr[i])
                env->DeleteLocalRef(idsArr[i]);
    }

    conn->notifyCommited = false;
    conn->notifyMap.clear();
}

// Called after each SQLite VM instruction when cancelation is enabled.
static int sqliteProgressHandlerCallback(void *data)
{
    SQLiteConnection *connection = static_cast<SQLiteConnection *>(data);
    return connection->canceled;
}

// Called after commiting transactions in WAL mode.
static int
sqliteWalHookCallback(void *data, sqlite3 *db, const char *dbName, int pages)
{
    SQLiteConnection *connection = static_cast<SQLiteConnection *>(data);

    JNIEnv *env = NULL;
    bool attached = false;
    jint ret = gVM->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (ret == JNI_EDETACHED) {
        jint ret = gVM->AttachCurrentThread(&env, NULL);
        assert(ret == JNI_OK);
        (void) ret;
        attached = true;
    }

    jstring dbNameStr = env->NewStringUTF(dbName);
    env->CallVoidMethod(connection->dbObj,
                        gSQLiteConnectionClassInfo.notifyCheckpoint, dbNameStr,
                        (jint) pages);
    bool exceptionOccurred = env->ExceptionCheck();
    if (exceptionOccurred) {
        jniLogException(env, ANDROID_LOG_ERROR, LOG_TAG);
        env->ExceptionClear();
    }
    env->DeleteLocalRef(dbNameStr);

    if (attached) {
        gVM->DetachCurrentThread();
    }
    return exceptionOccurred ? SQLITE_ERROR : SQLITE_OK;
}

static void
nativeSetKey(JNIEnv *env, jclass clazz, jlong connectionPtr, jbyteArray keyArr)
{

    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;
    if (!connection)
        return;

    jbyte *key = NULL;
    int keyLen = 0;
    if (keyArr) {
        keyLen = env->GetArrayLength(keyArr);
        if (keyLen > 0 && keyLen <= 4096) {
            key = (jbyte *) alloca(keyLen);
            env->GetByteArrayRegion(keyArr, 0, keyLen, key);
        }
    }

    if (key && keyLen > 0) {
        int status = sqlite3_key(connection->db, key, keyLen);

        if (status != SQLITE_OK) {
            throw_sqlite3_exception(env, connection->db, "Could not set key");
        } else {
            LOGI(LOG_TAG, "sqlite3_key verification passed.");
        }
    }
}

static jlong nativeOpen(JNIEnv *env,
                        jobject obj,
                        jstring pathStr,
                        jint openFlags,
                        jstring vfsNameStr)
{
    int sqliteFlags;
    if (openFlags & SQLiteConnection::CREATE_IF_NECESSARY) {
        sqliteFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    } else {
        // Try read/write mode even if OPEN_READONLY flag is set,
        // for compatibility to WAL.
        sqliteFlags = SQLITE_OPEN_READWRITE;
    }

    const char *pathChars = env->GetStringUTFChars(pathStr, NULL);
    std::string path(pathChars);
    env->ReleaseStringUTFChars(pathStr, pathChars);

    const char *vfsNameChars = NULL;
    if (vfsNameStr)
        vfsNameChars = env->GetStringUTFChars(vfsNameStr, NULL);

    sqlite3 *db;
    int err = sqlite3_open_v2(path.c_str(), &db, sqliteFlags, vfsNameChars);

    if (vfsNameStr)
        env->ReleaseStringUTFChars(vfsNameStr, vfsNameChars);

    if (err != SQLITE_OK) {
        throw_sqlite3_exception_errcode(env, err, "Could not open database");
        return 0;
    }
    // Check that the database is really read/write when that is what we asked for.
    if (!(openFlags & SQLiteConnection::OPEN_READONLY) &&
        sqlite3_db_readonly(db, NULL)) {
        throw_sqlite3_exception(
            env, db, "Could not open the database in read/write mode.");
        sqlite3_close(db);
        return 0;
    }

    // Set the default busy handler to retry automatically before returning SQLITE_BUSY.
    err = sqlite3_busy_timeout(db, BUSY_TIMEOUT_MS);
    if (err != SQLITE_OK) {
        throw_sqlite3_exception(env, db, "Could not set busy timeout");
        sqlite3_close(db);
        return 0;
    }

    // Call connection initializers, which register custom SQLite functions.
    char *errmsg = NULL;
    err = run_dbconn_initializers(db, &errmsg);
    if (err != SQLITE_OK) {
        throw_sqlite3_exception(env, err, errmsg ? errmsg : "Unknown error",
                                "Database connection initializer failed.");
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 0;
    }

    //    // Register custom Android functions.
    //    err = register_android_functions(db, UTF16_STORAGE);
    //    if (err) {
    //        throw_sqlite3_exception(env, db, "Could not register Android SQL functions.");
    //        sqlite3_close(db);
    //        return 0;
    //    }

    // Create wrapper object.
    SQLiteConnection *connection =
        new SQLiteConnection(env->NewGlobalRef(obj), db, openFlags);

    LOGI(LOG_TAG, "Opened connection %p with label '%s'", db, path.c_str());
    return (jlong)(intptr_t) connection;
}

static void nativeClose(JNIEnv *env, jclass clazz, jlong connectionPtr)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;

    if (connection) {
        LOGI(LOG_TAG, "Closing connection %p", connection->db);
        int err = sqlite3_close(connection->db);
        if (err != SQLITE_OK) {
            throw_sqlite3_exception(env, connection->db,
                                    "Failed to close database.");
            return;
        }

        env->DeleteGlobalRef(connection->dbObj);
        delete connection;
    }
}

// Called each time a custom function is evaluated.
static void sqliteCustomFunctionCallback(sqlite3_context *context,
                                         int argc,
                                         sqlite3_value **argv)
{
    JNIEnv *env = jniGetEnv();

    // Get the callback function object.
    // Create a new local reference to it in case the callback tries to do something
    // dumb like unregister the function (thereby destroying the global ref) while it is running.
    jobject functionObjGlobal =
        reinterpret_cast<jobject>(sqlite3_user_data(context));
    jobject functionObj = env->NewLocalRef(functionObjGlobal);

    jobjectArray argsArray =
        env->NewObjectArray(argc, gStringClassInfo.clazz, NULL);
    if (argsArray) {
        for (int i = 0; i < argc; i++) {
            const jchar *arg =
                static_cast<const jchar *>(sqlite3_value_text16(argv[i]));
            if (!arg) {
                LOGW(LOG_TAG, "NULL argument in custom_function_callback.  "
                              "This should not happen.");
            } else {
                size_t argLen = sqlite3_value_bytes16(argv[i]) / sizeof(jchar);
                jstring argStr = env->NewString(arg, argLen);
                if (!argStr) {
                    goto error; // out of memory error
                }
                env->SetObjectArrayElement(argsArray, i, argStr);
                env->DeleteLocalRef(argStr);
            }
        }

        // TODO: Support functions that return values.
        env->CallVoidMethod(functionObj,
                            gSQLiteCustomFunctionClassInfo.dispatchCallback,
                            argsArray);

    error:
        env->DeleteLocalRef(argsArray);
    }

    env->DeleteLocalRef(functionObj);

    if (env->ExceptionCheck()) {
        LOGE(LOG_TAG, "An exception was thrown by custom SQLite function.");
        LOGE_EX(env);
        env->ExceptionClear();
    }
}

// Called when a custom function is destroyed.
static void sqliteCustomFunctionDestructor(void *data)
{
    jobject functionObjGlobal = reinterpret_cast<jobject>(data);

    JNIEnv *env = jniGetEnv();
    env->DeleteGlobalRef(functionObjGlobal);
}

static void nativeRegisterCustomFunction(JNIEnv *env,
                                         jclass clazz,
                                         jlong connectionPtr,
                                         jobject functionObj)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;

    jstring nameStr = jstring(
        env->GetObjectField(functionObj, gSQLiteCustomFunctionClassInfo.name));
    jint numArgs =
        env->GetIntField(functionObj, gSQLiteCustomFunctionClassInfo.numArgs);

    jobject functionObjGlobal = env->NewGlobalRef(functionObj);

    const char *name = env->GetStringUTFChars(nameStr, NULL);
    int err =
        sqlite3_create_function_v2(connection->db, name, numArgs, SQLITE_UTF16,
                                   reinterpret_cast<void *>(functionObjGlobal),
                                   &sqliteCustomFunctionCallback, NULL, NULL,
                                   &sqliteCustomFunctionDestructor);
    env->ReleaseStringUTFChars(nameStr, name);

    if (err != SQLITE_OK) {
        LOGE(LOG_TAG, "sqlite3_create_function returned %d", err);
        env->DeleteGlobalRef(functionObjGlobal);
        throw_sqlite3_exception(env, connection->db);
        return;
    }
}

static void nativeRegisterLocalizedCollators(JNIEnv *env,
                                             jclass clazz,
                                             jlong connectionPtr,
                                             jstring localeStr)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;

    const char *locale = env->GetStringUTFChars(localeStr, NULL);
    int err = SQLITE_OK; //(connection->db, locale, UTF16_STORAGE);
    env->ReleaseStringUTFChars(localeStr, locale);

    if (err != SQLITE_OK) {
        throw_sqlite3_exception(env, connection->db);
    }
}

static jlong nativePrepareStatement(JNIEnv *env,
                                    jclass clazz,
                                    jlong connectionPtr,
                                    jstring sqlString)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;

    jsize sqlLength = env->GetStringLength(sqlString);
    const jchar *sql = env->GetStringChars(sqlString, NULL);
    sqlite3_stmt *statement;

    int err = sqlite3_prepare16_v2(connection->db, sql,
                                   sqlLength * sizeof(jchar), &statement, NULL);

    env->ReleaseStringChars(sqlString, sql);

    if (err != SQLITE_OK) {
        // Error messages like 'near ")": syntax error' are not
        // always helpful enough, so construct an error string that
        // includes the query itself.
        const char *query = env->GetStringUTFChars(sqlString, NULL);
        size_t message_len = strlen(query) + 50;
        char *message = (char *) malloc(message_len);
        if (message) {
            size_t len = strlcpy(message, ", while compiling: ", message_len);
            strlcpy(message + len, query, message_len - len);
        }
        env->ReleaseStringUTFChars(sqlString, query);
        throw_sqlite3_exception(env, connection->db, message);
        free(message);
        return 0;
    }

    //    LOGV(LOG_TAG,"Prepared statement %p on connection %p", statement, connection->db);
    return (jlong)(intptr_t) statement;
}

static void nativeFinalizeStatement(JNIEnv *env,
                                    jclass clazz,
                                    jlong connectionPtr,
                                    jlong statementPtr)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    // We ignore the result of sqlite3_finalize because it is really telling us about
    // whether any errors occurred while executing the statement.  The statement itself
    // is always finalized regardless.
    //    LOGV(LOG_TAG,"Finalized statement %p on connection %p", statement, connection->db);
    sqlite3_finalize(statement);
}

static jint nativeGetParameterCount(JNIEnv *env,
                                    jclass clazz,
                                    jlong connectionPtr,
                                    jlong statementPtr)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    return sqlite3_bind_parameter_count(statement);
}

static jboolean nativeIsReadOnly(JNIEnv *env,
                                 jclass clazz,
                                 jlong connectionPtr,
                                 jlong statementPtr)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    return sqlite3_stmt_readonly(statement) != 0;
}

static jint nativeGetColumnCount(JNIEnv *env,
                                 jclass clazz,
                                 jlong connectionPtr,
                                 jlong statementPtr)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    return sqlite3_column_count(statement);
}

static jstring nativeGetColumnName(JNIEnv *env,
                                   jclass clazz,
                                   jlong connectionPtr,
                                   jlong statementPtr,
                                   jint index)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    const jchar *name =
        static_cast<const jchar *>(sqlite3_column_name16(statement, index));
    if (name) {
        size_t length = 0;
        while (name[length]) {
            length += 1;
        }
        return env->NewString(name, length);
    }
    return NULL;
}

static void nativeBindNull(JNIEnv *env,
                           jclass clazz,
                           jlong connectionPtr,
                           jlong statementPtr,
                           jint index)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = sqlite3_bind_null(statement, index);
    if (err != SQLITE_OK) {
        SQLiteConnection *connection =
            (SQLiteConnection *) (intptr_t) connectionPtr;
        throw_sqlite3_exception(env, connection->db, NULL);
    }
}

static void nativeBindLong(JNIEnv *env,
                           jclass clazz,
                           jlong connectionPtr,
                           jlong statementPtr,
                           jint index,
                           jlong value)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = sqlite3_bind_int64(statement, index, value);
    if (err != SQLITE_OK) {
        SQLiteConnection *connection =
            (SQLiteConnection *) (intptr_t) connectionPtr;
        throw_sqlite3_exception(env, connection->db, NULL);
    }
}

static void nativeBindDouble(JNIEnv *env,
                             jclass clazz,
                             jlong connectionPtr,
                             jlong statementPtr,
                             jint index,
                             jdouble value)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = sqlite3_bind_double(statement, index, value);
    if (err != SQLITE_OK) {
        SQLiteConnection *connection =
            (SQLiteConnection *) (intptr_t) connectionPtr;
        throw_sqlite3_exception(env, connection->db, NULL);
    }
}

static void nativeBindString(JNIEnv *env,
                             jclass clazz,
                             jlong connectionPtr,
                             jlong statementPtr,
                             jint index,
                             jstring valueString)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    jsize valueLength = env->GetStringLength(valueString);
    const jchar *value = env->GetStringCritical(valueString, NULL);
    int err = sqlite3_bind_text16(
        statement, index, value, valueLength * sizeof(jchar), SQLITE_TRANSIENT);
    env->ReleaseStringCritical(valueString, value);
    if (err != SQLITE_OK) {
        SQLiteConnection *connection =
            (SQLiteConnection *) (intptr_t) connectionPtr;
        throw_sqlite3_exception(env, connection->db, NULL);
    }
}

static void nativeBindBlob(JNIEnv *env,
                           jclass clazz,
                           jlong connectionPtr,
                           jlong statementPtr,
                           jint index,
                           jbyteArray valueArray)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    jsize valueLength = env->GetArrayLength(valueArray);
    jbyte *value =
        static_cast<jbyte *>(env->GetPrimitiveArrayCritical(valueArray, NULL));
    int err = sqlite3_bind_blob(statement, index, value, valueLength,
                                SQLITE_TRANSIENT);
    env->ReleasePrimitiveArrayCritical(valueArray, value, JNI_ABORT);
    if (err != SQLITE_OK) {
        SQLiteConnection *connection =
            (SQLiteConnection *) (intptr_t) connectionPtr;
        throw_sqlite3_exception(env, connection->db, NULL);
    }
}

static void nativeResetStatement(JNIEnv *env,
                                 jclass clazz,
                                 jlong connectionPtr,
                                 jlong statementPtr,
                                 jboolean clearBindings)
{
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = sqlite3_reset(statement);
    if (err == SQLITE_OK && clearBindings) {
        err = sqlite3_clear_bindings(statement);
    }
    if (err != SQLITE_OK) {
        SQLiteConnection *connection =
            (SQLiteConnection *) (intptr_t) connectionPtr;
        throw_sqlite3_exception(env, connection->db, NULL);
    }
}

static int executeNonQuery(JNIEnv *env,
                           SQLiteConnection *connection,
                           sqlite3_stmt *statement)
{
    int err;
    do {
        err = sqlite3_step(statement);
    } while (err == SQLITE_ROW);

    if (err != SQLITE_DONE) {
        throw_sqlite3_exception(env, connection->db);
    } else {
        emitUpdateNotifications(env, connection);
    }

    return err;
}

static void nativeExecute(JNIEnv *env,
                          jclass clazz,
                          jlong connectionPtr,
                          jlong statementPtr)
{
    SQLiteConnection *conn = (SQLiteConnection *) (intptr_t) connectionPtr;
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;

    executeNonQuery(env, conn, stmt);
}

static jint nativeExecuteForChangedRowCount(JNIEnv *env,
                                            jclass clazz,
                                            jlong connectionPtr,
                                            jlong statementPtr)
{
    SQLiteConnection *conn = (SQLiteConnection *) (intptr_t) connectionPtr;
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = executeNonQuery(env, conn, stmt);
    return err == SQLITE_DONE ? sqlite3_changes(conn->db) : -1;
}

static jlong nativeExecuteForLastInsertedRowId(JNIEnv *env,
                                               jclass clazz,
                                               jlong connectionPtr,
                                               jlong statementPtr)
{
    SQLiteConnection *conn = (SQLiteConnection *) (intptr_t) connectionPtr;
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = executeNonQuery(env, conn, stmt);
    return err == SQLITE_DONE && sqlite3_changes(conn->db) > 0
               ? sqlite3_last_insert_rowid(conn->db)
               : -1;
}

static int executeOneRowQuery(JNIEnv *env,
                              SQLiteConnection *connection,
                              sqlite3_stmt *statement)
{
    int err = sqlite3_step(statement);
    if (err != SQLITE_ROW) {
        throw_sqlite3_exception(env, connection->db);
    } else {
        emitUpdateNotifications(env, connection);
    }
    return err;
}

static jlong nativeExecuteForLong(JNIEnv *env,
                                  jclass clazz,
                                  jlong connectionPtr,
                                  jlong statementPtr)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;
    sqlite3_stmt *statement = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = executeOneRowQuery(env, connection, statement);
    if (err == SQLITE_ROW && sqlite3_column_count(statement) >= 1) {
        return sqlite3_column_int64(statement, 0);
    }
    return -1;
}

static jstring nativeExecuteForString(JNIEnv *env,
                                      jclass clazz,
                                      jlong connectionPtr,
                                      jlong statementPtr)
{
    SQLiteConnection *conn = (SQLiteConnection *) (intptr_t) connectionPtr;
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;

    int err = executeOneRowQuery(env, conn, stmt);
    if (err == SQLITE_ROW && sqlite3_column_count(stmt) >= 1) {
        const jchar *text = (const jchar *) (sqlite3_column_text16(stmt, 0));
        if (text) {
            size_t length = sqlite3_column_bytes16(stmt, 0) / sizeof(jchar);
            return env->NewString(text, length);
        }
    }
    return NULL;
}

enum CopyRowResult {
    CPR_OK,
    CPR_FULL,
    CPR_ERROR,
};

static CopyRowResult copyRow(JNIEnv *env,
                             CursorWindow *window,
                             sqlite3_stmt *statement,
                             int numColumns,
                             int startPos,
                             int addedRows)
{
    // Allocate a new field directory for the row.
    CursorWindow::RowSlot *rowSlot;
    status_t status = window->allocRow(&rowSlot);
    if (status) {
        LOGV(LOG_TAG,
             "Failed allocating fieldDir at startPos %d row %d, error=%d",
             startPos, addedRows, status);
        return CPR_FULL;
    }

    // Pack the row into the window.
    CopyRowResult result = CPR_OK;
    for (int i = 0; i < numColumns; i++) {
        int type = sqlite3_column_type(statement, i);
        if (type == SQLITE_TEXT) {
            // TEXT data
            const char *text = reinterpret_cast<const char *>(
                sqlite3_column_text(statement, i));
            // SQLite does not include the NULL terminator in size, but does
            // ensure all strings are NULL terminated, so increase size by
            // one to make sure we store the terminator.
            size_t sizeIncludingNull = sqlite3_column_bytes(statement, i) + 1;
            status = window->putString(rowSlot, i, text, sizeIncludingNull);
            if (status) {
                LOGW(LOG_TAG,
                     "Failed allocating %zu bytes for text at %d,%d, error=%d",
                     sizeIncludingNull, startPos + addedRows, i, status);
                result = CPR_FULL;
                break;
            }
        } else if (type == SQLITE_INTEGER) {
            // INTEGER data
            int64_t value = sqlite3_column_int64(statement, i);
            status = window->putLong(rowSlot, i, value);
            if (status) {
                LOGW(
                    LOG_TAG,
                    "Failed allocating space for a long in column %d, error=%d",
                    i, status);
                result = CPR_FULL;
                break;
            }
        } else if (type == SQLITE_FLOAT) {
            // FLOAT data
            double value = sqlite3_column_double(statement, i);
            status = window->putDouble(rowSlot, i, value);
            if (status) {
                LOGW(LOG_TAG,
                     "Failed allocating space for a double in column "
                     "%d, error=%d",
                     i, status);
                result = CPR_FULL;
                break;
            }
        } else if (type == SQLITE_BLOB) {
            // BLOB data
            const void *blob = sqlite3_column_blob(statement, i);
            size_t size = sqlite3_column_bytes(statement, i);
            status = window->putBlob(rowSlot, i, blob, size);
            if (status) {
                LOGW(LOG_TAG,
                     "Failed allocating %zu bytes for blob at %d,%d, error=%d",
                     size, startPos + addedRows, i, status);
                result = CPR_FULL;
                break;
            }
        } else if (type == SQLITE_NULL) {
            // NULL field
            status = window->putNull(rowSlot, i);
            if (status) {
                LOGW(
                    LOG_TAG,
                    "Failed allocating space for a null in column %d, error=%d",
                    i, status);
                result = CPR_FULL;
                break;
            }
        } else {
            // Unknown data
            LOGE(LOG_TAG, "Unknown column type when filling database window");
            throw_sqlite3_exception(env,
                                    "Unknown column type when filling window");
            result = CPR_ERROR;
            break;
        }
    }

    // Free the last row if if was not successfully copied.
    if (result != CPR_OK) {
        window->freeLastRow();
    }
    return result;
}

static jlong nativeExecuteForCursorWindow(JNIEnv *env,
                                          jclass clazz,
                                          jlong connectionPtr,
                                          jlong statementPtr,
                                          jlong windowPtr,
                                          jint startPos,
                                          jint requiredPos,
                                          jboolean countAllRows)
{
    SQLiteConnection *conn = (SQLiteConnection *) (intptr_t) connectionPtr;
    sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;

    status_t status = window->clear();
    if (status) {
        throw_sqlite3_exception_format(
            env, conn->db, "Failed to clear the cursor window, status=%d",
            status);
        return 0;
    }

    int numColumns = -1;
    int retryCount = 0;
    int totalRows = 0;
    int addedRows = 0;
    bool windowFull = false;
    bool gotException = false;
    while (!gotException && (!windowFull || countAllRows)) {
        int err = sqlite3_step(stmt);
        if (err == SQLITE_ROW) {

            // Delay column count determination to the first time we got a row.
            // This is because schema can be changed by other connection but we
            // cannot notice before statement was actually executed by sqlite3_step().
            if (numColumns < 0) {
                numColumns = sqlite3_column_count(stmt);
                status = window->setNumColumns(numColumns);
                if (status) {
                    throw_sqlite3_exception_format(env, conn->db,
                                                   "Failed to set the cursor "
                                                   "window column count to %d, "
                                                   "status=%d",
                                                   numColumns, status);
                    return 0;
                }
            }

            //LOGV(LOG_TAG,"Stepped statement %p to row %d", statement, totalRows);
            retryCount = 0;
            totalRows += 1;

            // Skip the row if the window is full or we haven't reached the start position yet.
            if (startPos >= totalRows || windowFull) {
                continue;
            }

            CopyRowResult cpr =
                copyRow(env, window, stmt, numColumns, startPos, addedRows);
            if (cpr == CPR_FULL && addedRows &&
                startPos + addedRows <= requiredPos) {
                // We filled the window before we got to the one row that we really wanted.
                // Clear the window and start filling it again from here.
                // TODO: Would be nicer if we could progressively replace earlier rows.
                window->clear();
                window->setNumColumns(numColumns);
                startPos += addedRows;
                addedRows = 0;
                cpr =
                    copyRow(env, window, stmt, numColumns, startPos, addedRows);
            }

            if (cpr == CPR_OK) {
                addedRows += 1;
            } else if (cpr == CPR_FULL) {
                windowFull = true;
            } else {
                gotException = true;
            }
        } else if (err == SQLITE_DONE) {
            // All rows processed, bail
            break;
        } else if (err == SQLITE_LOCKED || err == SQLITE_BUSY) {
            // The table is locked, retry
            LOGI(LOG_TAG, "Database locked, retrying error code is %d", err);
            if (retryCount > 5) {
                LOGE(LOG_TAG, "Bailing on database busy retry");
                throw_sqlite3_exception(env, conn->db, "retrycount exceeded");
                gotException = true;
            } else {
                // Sleep to give the thread holding the lock a chance to finish
                usleep(1000);
                retryCount++;
            }
        } else {
            throw_sqlite3_exception(env, conn->db);
            gotException = true;
        }
    }

    //    LOGV(LOG_TAG,"Resetting statement %p after fetching %d rows and adding %d rows"
    //            "to the window in %d bytes",
    //            statement, totalRows, addedRows, window->size() - window->freeSpace());
    sqlite3_reset(stmt);

    // Report the total number of rows on request.
    if (startPos > totalRows) {
        LOGE(LOG_TAG, "startPos %d > actual rows %d", startPos, totalRows);
    }

    if (!gotException)
        emitUpdateNotifications(env, conn);

    jlong result = jlong(startPos) << 32 | jlong(totalRows);
    return result;
}

static jlong
nativeGetDbLookaside(JNIEnv *env, jobject clazz, jlong connectionPtr)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;

    int cur = -1;
    int unused;
    sqlite3_db_status(connection->db, SQLITE_DBSTATUS_LOOKASIDE_USED, &cur,
                      &unused, 0);
    return cur;
}

static void nativeCancel(JNIEnv *env, jobject clazz, jlong connectionPtr)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;
    connection->canceled = true;
}

static void nativeResetCancel(JNIEnv *env,
                              jobject clazz,
                              jlong connectionPtr,
                              jboolean cancelable)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;
    connection->canceled = false;

    if (cancelable) {
        sqlite3_progress_handler(connection->db, 4,
                                 sqliteProgressHandlerCallback, connection);
    } else {
        sqlite3_progress_handler(connection->db, 0, NULL, NULL);
    }
}

static void nativeSetWalHook(JNIEnv *env, jobject clazz, jlong connectionPtr)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;

    sqlite3_wal_hook(connection->db, sqliteWalHookCallback, connection);
}

static jlong nativeWalCheckpoint(JNIEnv *env,
                                 jobject clazz,
                                 jlong connectionPtr,
                                 jstring dbNameStr)
{
    SQLiteConnection *connection =
        (SQLiteConnection *) (intptr_t) connectionPtr;

    int nLog = 0, nCkpt = 0;
    const char *dbName = env->GetStringUTFChars(dbNameStr, NULL);
    int ret = sqlite3_wal_checkpoint_v2(
        connection->db, dbName, SQLITE_CHECKPOINT_PASSIVE, &nLog, &nCkpt);
    env->ReleaseStringUTFChars(dbNameStr, dbName);
    if (ret != SQLITE_OK) {
        // sqlite3_wal_checkpoint_v2 returns SQLITE_BUSY if any other process is running
        // a checkpoint operation at the same time. This is generally not an error.
        if (ret != SQLITE_BUSY)
            throw_sqlite3_exception(env, connection->db,
                                    "Cannot checkpoint the WAL database");
        return -1;
    }

    return jlong(nLog) << 32 | jlong(nCkpt);
}

static jlong nativeSQLiteHandle(JNIEnv *env,
                                jobject clazz,
                                jlong connectionPtr,
                                jboolean acquire)
{
    SQLiteConnection *conn = (SQLiteConnection *) (intptr_t) connectionPtr;
    sqlite3 *db = conn->db;

    if (acquire) {
        if (conn->notifyEnabled) {
            // temporarily disable update hooks.
            sqlite3_update_hook(db, NULL, NULL);
            sqlite3_commit_hook(db, NULL, NULL);
            sqlite3_rollback_hook(db, NULL, NULL);
        }

        return (jlong)(intptr_t) db;
    } else {
        if (conn->notifyEnabled) {
            // reenable update hooks.
            sqlite3_update_hook(db, sqliteUpdateHookCallback, conn);
            sqlite3_commit_hook(db, sqliteCommitHookCallback, conn);
            sqlite3_rollback_hook(db, sqliteRollbackHookCallback, conn);
        }

        return 0;
    }
}

static void nativeSetUpdateNotification(JNIEnv *env,
                                        jclass cls,
                                        jlong connectionPtr,
                                        jboolean enabled,
                                        jboolean notifyRowId)
{

    SQLiteConnection *conn = (SQLiteConnection *) (intptr_t) connectionPtr;
    assert(!conn->notifyCommited);
    assert(!conn->lastNotifyDb && !conn->lastNotifyTable);

    sqlite3 *db = conn->db;

    if (enabled) {
        sqlite3_update_hook(db, sqliteUpdateHookCallback, conn);
        sqlite3_commit_hook(db, sqliteCommitHookCallback, conn);
        sqlite3_rollback_hook(db, sqliteRollbackHookCallback, conn);
    } else {
        sqlite3_update_hook(db, NULL, NULL);
        sqlite3_commit_hook(db, NULL, NULL);
        sqlite3_rollback_hook(db, NULL, NULL);
    }

    conn->notifyEnabled = enabled;
    conn->notifyRowId = notifyRowId;
}

static JNINativeMethod sMethods[] = {
    {"nativeOpen", "(Ljava/lang/String;ILjava/lang/String;)J",
     (void *) nativeOpen},
    {"nativeClose", "(J)V", (void *) nativeClose},
    {"nativeSetKey", "(J[B)V", (void *) nativeSetKey},
    {"nativeRegisterCustomFunction",
     "(JLcom/tencent/wcdb/database/SQLiteCustomFunction;)V",
     (void *) nativeRegisterCustomFunction},
    {"nativeRegisterLocalizedCollators", "(JLjava/lang/String;)V",
     (void *) nativeRegisterLocalizedCollators},
    {"nativePrepareStatement", "(JLjava/lang/String;)J",
     (void *) nativePrepareStatement},
    {"nativeFinalizeStatement", "(JJ)V", (void *) nativeFinalizeStatement},
    {"nativeGetParameterCount", "(JJ)I", (void *) nativeGetParameterCount},
    {"nativeIsReadOnly", "(JJ)Z", (void *) nativeIsReadOnly},
    {"nativeGetColumnCount", "(JJ)I", (void *) nativeGetColumnCount},
    {"nativeGetColumnName", "(JJI)Ljava/lang/String;",
     (void *) nativeGetColumnName},
    {"nativeBindNull", "(JJI)V", (void *) nativeBindNull},
    {"nativeBindLong", "(JJIJ)V", (void *) nativeBindLong},
    {"nativeBindDouble", "(JJID)V", (void *) nativeBindDouble},
    {"nativeBindString", "(JJILjava/lang/String;)V", (void *) nativeBindString},
    {"nativeBindBlob", "(JJI[B)V", (void *) nativeBindBlob},
    {"nativeResetStatement", "(JJZ)V", (void *) nativeResetStatement},
    {"nativeExecute", "(JJ)V", (void *) nativeExecute},
    {"nativeExecuteForLong", "(JJ)J", (void *) nativeExecuteForLong},
    {"nativeExecuteForString", "(JJ)Ljava/lang/String;",
     (void *) nativeExecuteForString},
    {"nativeExecuteForChangedRowCount", "(JJ)I",
     (void *) nativeExecuteForChangedRowCount},
    {"nativeExecuteForLastInsertedRowId", "(JJ)J",
     (void *) nativeExecuteForLastInsertedRowId},
    {"nativeExecuteForCursorWindow", "(JJJIIZ)J",
     (void *) nativeExecuteForCursorWindow},
    {"nativeGetDbLookaside", "(J)I", (void *) nativeGetDbLookaside},
    {"nativeCancel", "(J)V", (void *) nativeCancel},
    {"nativeResetCancel", "(JZ)V", (void *) nativeResetCancel},
    {"nativeSetWalHook", "(J)V", (void *) nativeSetWalHook},
    {"nativeWalCheckpoint", "(JLjava/lang/String;)J",
     (void *) nativeWalCheckpoint},
    {"nativeSQLiteHandle", "(JZ)J", (void *) nativeSQLiteHandle},
    {"nativeSetUpdateNotification", "(JZZ)V",
     (void *) nativeSetUpdateNotification},
};

static int register_wcdb_SQLiteConnection(JavaVM *vm, JNIEnv *env)
{
    gVM = vm;

    jclass clazz;
    FIND_CLASS(clazz, "com/tencent/wcdb/database/SQLiteCustomFunction");

    GET_FIELD_ID(gSQLiteCustomFunctionClassInfo.name, clazz, "name",
                 "Ljava/lang/String;");
    GET_FIELD_ID(gSQLiteCustomFunctionClassInfo.numArgs, clazz, "numArgs", "I");
    GET_METHOD_ID(gSQLiteCustomFunctionClassInfo.dispatchCallback, clazz,
                  "dispatchCallback", "([Ljava/lang/String;)V");
    env->DeleteLocalRef(clazz);

    FIND_CLASS(clazz, "java/lang/String");
    gStringClassInfo.clazz = jclass(env->NewGlobalRef(clazz));
    env->DeleteLocalRef(clazz);

    FIND_CLASS(clazz, "com/tencent/wcdb/database/SQLiteConnection");
    GET_METHOD_ID(gSQLiteConnectionClassInfo.notifyCheckpoint, clazz,
                  "notifyCheckpoint", "(Ljava/lang/String;I)V");
    GET_METHOD_ID(gSQLiteConnectionClassInfo.notifyChange, clazz,
                  "notifyChange",
                  "(Ljava/lang/String;Ljava/lang/String;[J[J[J)V");

    if (env->RegisterNatives(clazz, sMethods, NELEM(sMethods)) < 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "RegisterNatives failed for '%s', aborting",
                 "com/tencent/wcdb/database/SQLiteConnection");
        env->FatalError(msg);
    }
    env->DeleteLocalRef(clazz);

    return 0;
}
WCDB_JNI_INIT(SQLiteConnection, register_wcdb_SQLiteConnection)

} // namespace wcdb
