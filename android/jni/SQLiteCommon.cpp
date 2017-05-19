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
#define LOG_TAG "WCDB.SQLiteCommon"
#include "SQLiteCommon.h"
#include <stdio.h>
#include <stdlib.h>


namespace wcdb {


    /* throw a SQLiteException with a message appropriate for the error in handle */
    void throw_sqlite3_exception(JNIEnv* env, sqlite3* handle) {
        throw_sqlite3_exception(env, handle, NULL);
    }

    /* throw a SQLiteException with the given message */
    void throw_sqlite3_exception(JNIEnv* env, const char* message) {
        throw_sqlite3_exception(env, NULL, message);
    }

    /* throw a SQLiteException with a message appropriate for the error in handle
       concatenated with the given message
     */
    void throw_sqlite3_exception(JNIEnv* env, sqlite3* handle, const char* message) {
        if (handle) {
            // get the error code and message from the SQLite connection
            // the error message may contain more information than the error code
            // because it is based on the extended error code rather than the simplified
            // error code that SQLite normally returns.
            throw_sqlite3_exception(env, sqlite3_extended_errcode(handle),
                sqlite3_system_errno(handle), sqlite3_errmsg(handle), message);
        }
        else {
            // we use SQLITE_OK so that a generic SQLiteException is thrown;
            // any code not specified in the switch statement below would do.
            throw_sqlite3_exception(env, SQLITE_OK, "unknown error", message);
        }
    }

    /* throw a SQLiteException for a given error code
     * should only be used when the database connection is not available because the
     * error information will not be quite as rich */
    void throw_sqlite3_exception_errcode(JNIEnv* env, int errcode, const char* message) {
        throw_sqlite3_exception(env, errcode, "unknown error", message);
    }

    /* throw a SQLiteException for a given error code, sqlite3message, and
       user message
     */
    void throw_sqlite3_exception(JNIEnv* env, int errcode, const char* sqlite3Message,
        const char* message) {
        throw_sqlite3_exception(env, errcode, -1, sqlite3Message, message);
    }

    /* throw a SQLiteException for a given error code, errno, sqlite3message, and
       user message
     */
    void throw_sqlite3_exception(JNIEnv* env, int errcode, int sysErrno,
        const char* sqlite3Message, const char* message) {
        const char* exceptionClass;
        switch (errcode & 0xff) { /* mask off extended error code */
        case SQLITE_IOERR:
            exceptionClass = "com/tencent/wcdb/database/SQLiteDiskIOException";
            break;
        case SQLITE_CORRUPT:
        case SQLITE_NOTADB: // treat "unsupported file format" error as corruption also
            exceptionClass = "com/tencent/wcdb/database/SQLiteDatabaseCorruptException";
            break;
        case SQLITE_CONSTRAINT:
            exceptionClass = "com/tencent/wcdb/database/SQLiteConstraintException";
            break;
        case SQLITE_ABORT:
            exceptionClass = "com/tencent/wcdb/database/SQLiteAbortException";
            break;
        case SQLITE_DONE:
            exceptionClass = "com/tencent/wcdb/database/SQLiteDoneException";
            sqlite3Message = NULL; // SQLite error message is irrelevant in this case
            break;
        case SQLITE_FULL:
            exceptionClass = "com/tencent/wcdb/database/SQLiteFullException";
            break;
        case SQLITE_MISUSE:
            exceptionClass = "com/tencent/wcdb/database/SQLiteMisuseException";
            break;
        case SQLITE_PERM:
            exceptionClass = "com/tencent/wcdb/database/SQLiteAccessPermException";
            break;
        case SQLITE_BUSY:
            exceptionClass = "com/tencent/wcdb/database/SQLiteDatabaseLockedException";
            break;
        case SQLITE_LOCKED:
            exceptionClass = "com/tencent/wcdb/database/SQLiteTableLockedException";
            break;
        case SQLITE_READONLY:
            exceptionClass = "com/tencent/wcdb/database/SQLiteReadOnlyDatabaseException";
            break;
        case SQLITE_CANTOPEN:
            exceptionClass = "com/tencent/wcdb/database/SQLiteCantOpenDatabaseException";
            break;
        case SQLITE_TOOBIG:
            exceptionClass = "com/tencent/wcdb/database/SQLiteBlobTooBigException";
            break;
        case SQLITE_RANGE:
            exceptionClass = "com/tencent/wcdb/database/SQLiteBindOrColumnIndexOutOfRangeException";
            break;
        case SQLITE_NOMEM:
            exceptionClass = "com/tencent/wcdb/database/SQLiteOutOfMemoryException";
            break;
        case SQLITE_MISMATCH:
            exceptionClass = "com/tencent/wcdb/database/SQLiteDatatypeMismatchException";
            break;
        case SQLITE_INTERRUPT:
            exceptionClass = "com/tencent/wcdb/support/OperationCanceledException";
            break;
        default:
            exceptionClass = "com/tencent/wcdb/database/SQLiteException";
            break;
        }

        if (sqlite3Message) {
            char buf[2048];
            snprintf(buf, sizeof(buf), "%s (code %d, errno %d): %s", sqlite3Message, errcode,
                sysErrno, message ? message : "");
            jniThrowException(env, exceptionClass, buf);
        }
        else {
            jniThrowException(env, exceptionClass, message);
        }
    }

} // namespace wcdb
