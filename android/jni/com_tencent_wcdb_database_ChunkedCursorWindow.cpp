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

#define LOG_TAG "WCDB.ChunkedCursorWindow"

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <jni.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "ChunkedCursorWindow.h"
#include "Logger.h"
#include "ModuleLoader.h"
#include "JNIHelp.h"
#include "SQLiteCommon.h"
#include "Unicode.h"


namespace wcdb {

    static void throwExceptionWithRowCol(JNIEnv* env, jint row, jint column) {
        char buf[256];
        snprintf(buf, sizeof(buf), "Couldn't read row %d, col %d from ChunkedCursorWindow.",
            row, column);
        jniThrowException(env, "java/lang/IllegalStateException", buf);
    }

    static void throwUnknownTypeException(JNIEnv * env, jint type) {
        char buf[128];
        snprintf(buf, sizeof(buf), "UNKNOWN type %d", type);
        jniThrowException(env, "java/lang/IllegalStateException", buf);
    }

    /****************************
         ChunkedCursorWindow
    *****************************/
    static jlong nativeCreate(JNIEnv *env, jclass cls, jint capacity) {
        ChunkedCursorWindow *window;

        status_t status = ChunkedCursorWindow::create(capacity, &window);
        if (status != OK || !window) {
            LOGE(LOG_TAG, "Could not allocate CursorWindow of size %d due to error %d.",
                 capacity, status);
            return 0;
        }

        return (jlong) (intptr_t) window;
    }

    static void nativeDispose(JNIEnv *env, jclass cls, jlong windowPtr) {
        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;
        
        if (window)
            delete window;
    }

    static void nativeClear(JNIEnv *env, jclass cls, jlong windowPtr) {
        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;
        
        status_t status = window->clear();
        if (status != OK) {
            LOGE(LOG_TAG, "Could not clear window. error=%d", status);
        }
    }

    static jlong nativeRemoveChunk(JNIEnv *env, jclass cls, jlong windowPtr, jint pos) {
        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;

        uint32_t start, end;
        status_t status = window->removeChunk(pos, start, end);
        if (status != OK)
            return (jlong) 0xFFFFFFFFFFFFFFFFLL;
        else
            return ((jlong) start << 32) | end;
    }

    static jint nativeGetNumChunks(JNIEnv *env, jclass cls, jlong windowPtr) {
        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;
        return window->getNumChunks();
    }

    static jboolean nativeSetNumColumns(JNIEnv *env, jclass cls, jlong windowPtr, jint columnNum) {
        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;
        
        status_t status = window->setNumColumns(columnNum);
        return status == OK;
    }

    static jlong nativeGetRow(JNIEnv *env, jclass cls, jlong windowPtr, jint pos) {
        if (pos < 0) return 0;

        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;
        ChunkedCursorWindow::Row *row = window->getRow(pos);
        
        return (jlong) (intptr_t) row;
    }

    static void nativeEndRow(JNIEnv *env, jclass cls, jlong windowPtr, jlong rowPtr) {
        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;
        ChunkedCursorWindow::Row *row = (ChunkedCursorWindow::Row *) (intptr_t) rowPtr;

        window->endRow(row);
    }

    static jint nativeGetType(JNIEnv *env, jclass cls, jlong rowPtr, jint column) {
        ChunkedCursorWindow::Row *row = (ChunkedCursorWindow::Row *) (intptr_t) rowPtr;
        
        ChunkedCursorWindow::Field field = row->getField(column);
        if (!row->isFieldValid(field)) {
            throwExceptionWithRowCol(env, row->getRowPos(), column);
            return 0;
        }

        return row->getFieldType(field);
    }

    static jbyteArray nativeGetBlob(JNIEnv *env, jclass cls, jlong rowPtr, jint column) {
        ChunkedCursorWindow::Row *row = (ChunkedCursorWindow::Row *) (intptr_t) rowPtr;

        ChunkedCursorWindow::Field field = row->getField(column);
        if (!row->isFieldValid(field)) {
            throwExceptionWithRowCol(env, row->getRowPos(), column);
            return 0;
        }

        int32_t type = row->getFieldType(field);
        if (type == CursorWindow::FIELD_TYPE_BLOB || type == CursorWindow::FIELD_TYPE_STRING) {
            size_t size;
            const void *value = row->getFieldBlob(field, size);
            jbyteArray byteArray = env->NewByteArray(size);
            if (!byteArray) {
                env->ExceptionClear();
                throw_sqlite3_exception(env, "Native could not create new byte[]");
                return NULL;
            }
            env->SetByteArrayRegion(byteArray, 0, size, static_cast<const jbyte*>(value));
            return byteArray;
        } else if (type == CursorWindow::FIELD_TYPE_INTEGER) {
            throw_sqlite3_exception(env, "Unable to convert INTEGER to BLOB");
        } else if (type == CursorWindow::FIELD_TYPE_FLOAT) {
            throw_sqlite3_exception(env, "Unable to convert FLOAT to BLOB");
        } else if (type == CursorWindow::FIELD_TYPE_NULL) {
            // do nothing
        } else {
            throwUnknownTypeException(env, type);
        }
        return NULL;
    }

    static jstring nativeGetString(JNIEnv *env, jclass cls, jlong rowPtr, jint column) {
        ChunkedCursorWindow::Row *row = (ChunkedCursorWindow::Row *) (intptr_t) rowPtr;

        ChunkedCursorWindow::Field field = row->getField(column);
        if (!row->isFieldValid(field)) {
            throwExceptionWithRowCol(env, row->getRowPos(), column);
            return 0;
        }

        int32_t type = row->getFieldType(field);
        if (type == CursorWindow::FIELD_TYPE_STRING) {
            size_t sizeIncludingNull;
            const char *value = row->getFieldString(field, sizeIncludingNull);

            if (sizeIncludingNull <= 1) {
                return env->NewStringUTF("");
            }

            // Convert to UTF-16 here instead of calling NewStringUTF.  NewStringUTF
            // doesn't like UTF-8 strings with high codepoints.  It actually expects
            // Modified UTF-8 with encoded surrogate pairs.
            ssize_t u16_size = utf8_to_utf16_length((const uint8_t *) value, sizeIncludingNull - 1);
            if (u16_size < 0) {
                // Invalid UTF-16 detected, return empty string.
                return env->NewStringUTF("");
            }
            uint16_t *u16_buf = new uint16_t[u16_size + 1];
            utf8_to_utf16((const uint8_t *) value, sizeIncludingNull - 1, u16_buf);
            jstring ret = env->NewString(u16_buf, u16_size);
            delete[] u16_buf;
            return ret;
        } else if (type == CursorWindow::FIELD_TYPE_INTEGER) {
            int64_t value = row->getFieldLong(field);
            char buf[32];
            snprintf(buf, sizeof(buf), "%" PRId64, value);
            return env->NewStringUTF(buf);
        } else if (type == CursorWindow::FIELD_TYPE_FLOAT) {
            double value = row->getFieldDouble(field);
            char buf[32];
            snprintf(buf, sizeof(buf), "%g", value);
            return env->NewStringUTF(buf);
        } else if (type == CursorWindow::FIELD_TYPE_BLOB) {
            throw_sqlite3_exception(env, "Unable to covert BLOB to string");
            return NULL;
        } else if (type == CursorWindow::FIELD_TYPE_NULL) {
            return NULL;
        } else {
            throwUnknownTypeException(env, type);
            return NULL;
        }
    }

    static jlong nativeGetLong(JNIEnv *env, jclass cls, jlong rowPtr, jint column) {
        ChunkedCursorWindow::Row *row = (ChunkedCursorWindow::Row *) (intptr_t) rowPtr;

        ChunkedCursorWindow::Field field = row->getField(column);
        if (!row->isFieldValid(field)) {
            throwExceptionWithRowCol(env, row->getRowPos(), column);
            return 0;
        }

        int32_t type = row->getFieldType(field);
        if (type == CursorWindow::FIELD_TYPE_INTEGER) {
            return row->getFieldLong(field);
        } else if (type == CursorWindow::FIELD_TYPE_STRING) {
            size_t sizeIncludingNull;
            const char *value = row->getFieldString(field, sizeIncludingNull);
            return sizeIncludingNull > 1 ? strtoll(value, NULL, 0) : 0L;
        } else if (type == CursorWindow::FIELD_TYPE_FLOAT) {
            return jlong(row->getFieldDouble(field));
        } else if (type == CursorWindow::FIELD_TYPE_NULL) {
            return 0;
        } else if (type == CursorWindow::FIELD_TYPE_BLOB) {
            throw_sqlite3_exception(env, "Unable to convert BLOB to long");
            return 0;
        } else {
            throwUnknownTypeException(env, type);
            return 0;
        }
    }

    static jdouble nativeGetDouble(JNIEnv* env, jclass clazz, jlong rowPtr, jint column) {
        ChunkedCursorWindow::Row *row = (ChunkedCursorWindow::Row *) (intptr_t) rowPtr;

        ChunkedCursorWindow::Field field = row->getField(column);
        if (!row->isFieldValid(field)) {
            throwExceptionWithRowCol(env, row->getRowPos(), column);
            return 0;
        }

        int32_t type = row->getFieldType(field);
        if (type == CursorWindow::FIELD_TYPE_FLOAT) {
            return row->getFieldDouble(field);
        } else if (type == CursorWindow::FIELD_TYPE_STRING) {
            size_t sizeIncludingNull;
            const char *value = row->getFieldString(field, sizeIncludingNull);
            return sizeIncludingNull > 1 ? strtod(value, NULL) : 0.0;
        } else if (type == CursorWindow::FIELD_TYPE_INTEGER) {
            return jdouble(row->getFieldLong(field));
        } else if (type == CursorWindow::FIELD_TYPE_NULL) {
            return 0.0;
        } else if (type == CursorWindow::FIELD_TYPE_BLOB) {
            throw_sqlite3_exception(env, "Unable to convert BLOB to double");
            return 0.0;
        } else {
            throwUnknownTypeException(env, type);
            return 0.0;
        }
    }


    /****************************
        SQLiteAsyncQuery
    *****************************/
    static status_t copyRow(sqlite3_stmt *statement, ChunkedCursorWindow::Row *row,
        int numColumns, int pos) {

        status_t status;

        // Pack the row into the window.
        int i;
        for (i = 0; i < numColumns; i++) {
            int type = sqlite3_column_type(statement, i);
            if (type == SQLITE_TEXT) {
                // TEXT data
                const char *text = (const char *) sqlite3_column_text(statement, i);
                // SQLite does not include the NULL terminator in size, but does
                // ensure all strings are NULL terminated, so increase size by
                // one to make sure we store the terminator.
                size_t size = sqlite3_column_bytes(statement, i) + 1;

                status = row->putString(i, text, size);
                if (status != OK) {
                    return status;
                }
            } else if (type == SQLITE_BLOB) {
                // BLOB data
                const void *blob = sqlite3_column_blob(statement, i);
                size_t size = sqlite3_column_bytes(statement, i);
                
                status = row->putBlob(i, blob, size);
                if (status != OK) {
                    return status;
                }
            }
            else if (type == SQLITE_INTEGER) {
                // INTEGER data
                int64_t value = sqlite3_column_int64(statement, i);

                status = row->putLong(i, value);
                if (status != OK) {
                    return status;
                }
            } else if (type == SQLITE_FLOAT) {
                // FLOAT data
                double value = sqlite3_column_double(statement, i);

                status = row->putDouble(i, value);
                if (status != OK) {
                    return status;
                }
            } else if (type == SQLITE_NULL) {
                // NULL data
                status = row->putNull(i);
                if (status != OK) {
                    return status;
                }
            } else {
                // Unknown type
                return BAD_TYPE;
            }
        }

        return OK;
    }

    static jint nativeFillRows(JNIEnv *env, jclass cls, jlong statementPtr, jlong windowPtr,
        jint startPos, jint count) {

        sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;
        ChunkedCursorWindow *window = (ChunkedCursorWindow *) (intptr_t) windowPtr;

        int i;
        for (i = 0; i < count; i++) {
            int rc = sqlite3_step(stmt);
            if (rc == SQLITE_DONE) {
                // Reached the end of the result set, no more rows can be filled.
                break;
            } else if (rc != SQLITE_ROW) {
                // Error occured, throw exception.
                throw_sqlite3_exception(env, sqlite3_db_handle(stmt));
                return -1;
            } else {
                // Got a row, fill it to the window.
                int pos = startPos + i;

                // Allocate row slot.
                ChunkedCursorWindow::Row *row = window->newRow(pos, false);
                if (!row) {
                    // Probably no space left on the current chunk, allocate new chunk and retry.
                    row = window->newRow(pos, true);
                    if (!row) {
                        // Cannot allocate new chunk, throw exception.
                        throw_sqlite3_exception(env, "Failed to allocate new chunk in ChunkedCursorWindow.");
                        return -1;
                    }
                }

                // Copy columns to the window.
                int numColumns = sqlite3_column_count(stmt);
                status_t status = copyRow(stmt, row, numColumns, pos);
                
                if (status == NO_MEMORY) {
                    // Existing chunk is full, allocate new chunk and retry.
                    window->rollbackRow(row);
                    row = window->newRow(pos, true);
                    if (!row) {
                        // Cannot allocate new chunk, throw exception.
                        throw_sqlite3_exception(env, "Failed to allocate new chunk in ChunkedCursorWindow.");
                        return -1;
                    }

                    status = copyRow(stmt, row, numColumns, pos);
                }

                if (status != OK) {
                    // Failed, rollback row and throw exception.
                    window->rollbackRow(row);
                    char msg[256];
                    snprintf(msg, sizeof(msg), "nativeFillRows failed, row = %d, status = %d",
                        pos, status);
                    throw_sqlite3_exception(env, msg);
                    return -1;
                }

                // status == OK
                window->endRow(row);
            }
        }

        return i;
    }

    static jint nativeCount(JNIEnv *env, jclass cls, jlong statementPtr) {
        sqlite3_stmt *stmt = (sqlite3_stmt *) (intptr_t) statementPtr;
        int result = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result++;
        }

        int rc = sqlite3_reset(stmt);
        if (rc != SQLITE_OK) {
            throw_sqlite3_exception(env, sqlite3_db_handle(stmt));
            return -1;
        }
        return result;
    }


    static const JNINativeMethod sWindowMethods[] = {
        { "nativeCreate", "(I)J", (void *) nativeCreate },
        { "nativeDispose", "(J)V", (void *) nativeDispose },
        { "nativeClear", "(J)V", (void *) nativeClear },
        { "nativeRemoveChunk", "(JI)J", (void *) nativeRemoveChunk },
        { "nativeGetNumChunks", "(J)I", (void *) nativeGetNumChunks },
        { "nativeSetNumColumns", "(JI)Z", (void *) nativeSetNumColumns },
        { "nativeGetRow", "(JI)J", (void *) nativeGetRow },
        { "nativeEndRow", "(JJ)V", (void *) nativeEndRow },
        { "nativeGetType", "(JI)I", (void *) nativeGetType },
        { "nativeGetBlob", "(JI)[B", (void *) nativeGetBlob },
        { "nativeGetString", "(JI)Ljava/lang/String;", (void *) nativeGetString },
        { "nativeGetLong", "(JI)J", (void *) nativeGetLong },
        { "nativeGetDouble", "(JI)D", (void *) nativeGetDouble },
    };

    static const JNINativeMethod sQueryMethods[] = {
        { "nativeFillRows", "(JJII)I", (void *) nativeFillRows },
        { "nativeCount", "(J)I", (void *) nativeCount },
    };

    static jint register_wcdb_ChunkedCursorWindow(JavaVM *vm, JNIEnv *env) {
        int ret = jniRegisterNativeMethods(env, "com/tencent/wcdb/database/ChunkedCursorWindow",
            sWindowMethods, NELEM(sWindowMethods));
        if (ret == 0)
            ret = jniRegisterNativeMethods(env, "com/tencent/wcdb/database/SQLiteAsyncQuery",
                sQueryMethods, NELEM(sQueryMethods));
        return ret;
    }
    WCDB_JNI_INIT(ChunkedCursorWindow, register_wcdb_ChunkedCursorWindow)

}
