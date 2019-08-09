/*
 * Copyright (C) 2007 The Android Open Source Project
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

#define LOG_TAG "WCDB.SQLiteDebug"

#include <jni.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "JNIHelp.h"
#include "Logger.h"
#include "ModuleLoader.h"
#include "SQLiteCommon.h"
#include <vfslog.h>

namespace wcdb {

volatile int sLastErrorLine = 0;

static struct {
    jfieldID memoryUsed;
    jfieldID pageCacheOverflow;
    jfieldID largestMemAlloc;
} gSQLiteDebugPagerStatsClassInfo;

static void nativeGetPagerStats(JNIEnv *env, jobject clazz, jobject statsObj)
{
    int memoryUsed;
    int pageCacheOverflow;
    int largestMemAlloc;
    int unused;

    sqlite3_status(SQLITE_STATUS_MEMORY_USED, &memoryUsed, &unused, 0);
    sqlite3_status(SQLITE_STATUS_MALLOC_SIZE, &unused, &largestMemAlloc, 0);
    sqlite3_status(SQLITE_STATUS_PAGECACHE_OVERFLOW, &pageCacheOverflow,
                   &unused, 0);
    env->SetIntField(statsObj, gSQLiteDebugPagerStatsClassInfo.memoryUsed,
                     memoryUsed);
    env->SetIntField(statsObj,
                     gSQLiteDebugPagerStatsClassInfo.pageCacheOverflow,
                     pageCacheOverflow);
    env->SetIntField(statsObj, gSQLiteDebugPagerStatsClassInfo.largestMemAlloc,
                     largestMemAlloc);
}

static jint nativeGetLastErrorLine(JNIEnv *env, jclass cls)
{
    return sLastErrorLine;
}

static void nativeSetIOTraceFlags(JNIEnv *env, jclass cls, jint flags)
{
    vlogDefaultLogFlags = (uint32_t) flags;
}

static void nativeGetIOTraceStats(JNIEnv *env,
                                  jclass cls,
                                  jlong connectionPtr,
                                  jobject statsList)
{
    struct {
        jfieldID dbName;
        jfieldID path;
        jfieldID pageSize;
        jfieldID pageCount;
        jfieldID journalMode;
        jfieldID lastReadOffset;
        jfieldID lastReadPage;
        jfieldID lastWriteOffset;
        jfieldID lastWritePage;
        jfieldID lastJournalReadOffset;
        jfieldID lastJournalReadPage;
        jfieldID lastJournalWriteOffset;
        jfieldID lastJournalWritePage;
    } fieldsStats;

    // Gather information from Java.
    jclass clsArrayList = env->FindClass("java/util/ArrayList");
    if (!clsArrayList)
        return;
    jmethodID midArrayListAdd =
        env->GetMethodID(clsArrayList, "add", "(Ljava/lang/Object;)Z");
    if (!midArrayListAdd)
        return;

    jclass clsIOTraceStats =
        env->FindClass("com/tencent/wcdb/database/SQLiteDebug$IOTraceStats");
    if (!clsIOTraceStats)
        return;
    jmethodID midIOTraceStatsCtor =
        env->GetMethodID(clsIOTraceStats, "<init>", "()V");
    if (!midIOTraceStatsCtor)
        return;

#define GET_FID(var, cls, name, sig)                                           \
    do {                                                                       \
        (var).name = env->GetFieldID((cls), #name, (sig));                     \
        if (!(var).name)                                                       \
            return;                                                            \
    } while (0)

    GET_FID(fieldsStats, clsIOTraceStats, dbName, "Ljava/lang/String;");
    GET_FID(fieldsStats, clsIOTraceStats, path, "Ljava/lang/String;");
    GET_FID(fieldsStats, clsIOTraceStats, pageSize, "J");
    GET_FID(fieldsStats, clsIOTraceStats, pageCount, "J");
    GET_FID(fieldsStats, clsIOTraceStats, journalMode, "Ljava/lang/String;");
    GET_FID(fieldsStats, clsIOTraceStats, lastReadOffset, "J");
    GET_FID(fieldsStats, clsIOTraceStats, lastReadPage, "[B");
    GET_FID(fieldsStats, clsIOTraceStats, lastWriteOffset, "J");
    GET_FID(fieldsStats, clsIOTraceStats, lastWritePage, "[B");
    GET_FID(fieldsStats, clsIOTraceStats, lastJournalReadOffset, "J");
    GET_FID(fieldsStats, clsIOTraceStats, lastJournalReadPage, "[B");
    GET_FID(fieldsStats, clsIOTraceStats, lastJournalWriteOffset, "J");
    GET_FID(fieldsStats, clsIOTraceStats, lastJournalWritePage, "[B");

#undef GET_FID

    // List all attached databases.
    sqlite3 *db = (sqlite3 *) (intptr_t) connectionPtr;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, "PRAGMA database_list;", -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        // Failed to compile PRAGMA database_list; probably caused by database corruption.
        // Use hardcoded name and path for main database.
        const char *path = sqlite3_db_filename(db, "main");
        if (!path)
            path = "";

        char sql[256];
        sqlite3_snprintf(sizeof(sql), sql,
                         "SELECT 0 as seq, 'main' as name, %Q as file;", path);
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            // Still failed, throw exception.
            throw_sqlite3_exception(env, db, "Cannot get I/O trace stats.");
            return;
        }
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Create new IOTraceStats object.
        jobject statsObj = env->NewObject(clsIOTraceStats, midIOTraceStatsCtor);
        if (!statsObj) {
            sqlite3_finalize(stmt);
            return;
        }

        // Get and fill dbName.
        const char *dbName = (const char *) sqlite3_column_text(stmt, 1);
        const char *dbPath = (const char *) sqlite3_column_text(stmt, 2);
        int pageSize = 4096;
        jstring jstr = env->NewStringUTF(dbName);
        env->SetObjectField(statsObj, fieldsStats.dbName, jstr);
        env->DeleteLocalRef(jstr);
        jstr = env->NewStringUTF(dbPath);
        env->SetObjectField(statsObj, fieldsStats.path, jstr);
        env->DeleteLocalRef(jstr);

        char sql[256];
        sqlite3_stmt *stmt2;

        // pageSize
        snprintf(sql, sizeof(sql), "PRAGMA %s.page_size;", dbName);
        if (sqlite3_prepare_v2(db, sql, -1, &stmt2, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt2) == SQLITE_ROW) {
                pageSize = sqlite3_column_int(stmt2, 0);
                env->SetLongField(statsObj, fieldsStats.pageSize, pageSize);
            }
            sqlite3_finalize(stmt2);
        }

        // pageCount
        snprintf(sql, sizeof(sql), "PRAGMA %s.page_count;", dbName);
        if (sqlite3_prepare_v2(db, sql, -1, &stmt2, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt2) == SQLITE_ROW) {
                int64_t pageCount = sqlite3_column_int64(stmt2, 0);
                env->SetLongField(statsObj, fieldsStats.pageCount, pageCount);
            }
            sqlite3_finalize(stmt2);
        }

        // journalMode
        snprintf(sql, sizeof(sql), "PRAGMA %s.journal_mode;", dbName);
        if (sqlite3_prepare_v2(db, sql, -1, &stmt2, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt2) == SQLITE_ROW) {
                const char *journalMode =
                    (const char *) sqlite3_column_text(stmt2, 0);
                jstring journalModeStr = env->NewStringUTF(journalMode);
                env->SetObjectField(statsObj, fieldsStats.journalMode,
                                    journalModeStr);
                env->DeleteLocalRef(journalModeStr);
            }
            sqlite3_finalize(stmt2);
        }

        // last*Offset and last*Page
        VLogStat stats;
        rc = vlogGetStats(db, dbName, &stats);
        if (rc == SQLITE_OK) {
            sqlite3_file *f = nullptr;
            sqlite3_file_control(db, dbName, SQLITE_FCNTL_FILE_POINTER, &f);
            if (f) {
                jbyteArray pageArr;
                jbyte *page;

                if (stats.lastMainReadOffset >= 0) {
                    pageArr = env->NewByteArray(pageSize);
                    if (!pageArr) {
                        sqlite3_finalize(stmt);
                        return;
                    }
                    jbyte *page = env->GetByteArrayElements(pageArr, nullptr);
                    f->pMethods->xRead(f, page, pageSize,
                                       stats.lastMainReadOffset);
                    env->ReleaseByteArrayElements(pageArr, page, 0);
                    env->SetObjectField(statsObj, fieldsStats.lastReadPage,
                                        pageArr);
                    env->DeleteLocalRef(pageArr);
                } else {
                    env->SetObjectField(statsObj, fieldsStats.lastReadPage,
                                        nullptr);
                }

                if (stats.lastMainWriteOffset >= 0) {
                    pageArr = env->NewByteArray(pageSize);
                    if (!pageArr) {
                        sqlite3_finalize(stmt);
                        return;
                    }
                    page = env->GetByteArrayElements(pageArr, nullptr);
                    f->pMethods->xRead(f, page, pageSize,
                                       stats.lastMainWriteOffset);
                    env->ReleaseByteArrayElements(pageArr, page, 0);
                    env->SetObjectField(statsObj, fieldsStats.lastWritePage,
                                        pageArr);
                    env->DeleteLocalRef(pageArr);
                } else {
                    env->SetObjectField(statsObj, fieldsStats.lastWritePage,
                                        nullptr);
                }
            }

            f = nullptr;
            sqlite3_file_control(db, dbName, SQLITE_FCNTL_JOURNAL_POINTER, &f);
            if (f) {
                jbyteArray pageArr;
                jbyte *page;

                if (stats.lastJournalReadOffset >= 0) {
                    pageArr = env->NewByteArray(pageSize);
                    if (!pageArr) {
                        sqlite3_finalize(stmt);
                        return;
                    }
                    jbyte *page = env->GetByteArrayElements(pageArr, nullptr);
                    f->pMethods->xRead(f, page, pageSize,
                                       stats.lastJournalReadOffset);
                    env->ReleaseByteArrayElements(pageArr, page, 0);
                    env->SetObjectField(
                        statsObj, fieldsStats.lastJournalReadPage, pageArr);
                    env->DeleteLocalRef(pageArr);
                } else {
                    env->SetObjectField(statsObj,
                                        fieldsStats.lastJournalReadPage, nullptr);
                }

                if (stats.lastJournalWriteOffset >= 0) {
                    pageArr = env->NewByteArray(pageSize);
                    if (!pageArr) {
                        sqlite3_finalize(stmt);
                        return;
                    }
                    page = env->GetByteArrayElements(pageArr, nullptr);
                    f->pMethods->xRead(f, page, pageSize,
                                       stats.lastJournalWriteOffset);
                    env->ReleaseByteArrayElements(pageArr, page, 0);
                    env->SetObjectField(
                        statsObj, fieldsStats.lastJournalWritePage, pageArr);
                    env->DeleteLocalRef(pageArr);
                } else {
                    env->SetObjectField(statsObj,
                                        fieldsStats.lastJournalWritePage, nullptr);
                }
            }

            env->SetLongField(statsObj, fieldsStats.lastReadOffset,
                              stats.lastMainReadOffset);
            env->SetLongField(statsObj, fieldsStats.lastWriteOffset,
                              stats.lastMainWriteOffset);
            env->SetLongField(statsObj, fieldsStats.lastJournalReadOffset,
                              stats.lastJournalReadOffset);
            env->SetLongField(statsObj, fieldsStats.lastJournalWriteOffset,
                              stats.lastJournalWriteOffset);
        }

        env->CallBooleanMethod(statsList, midArrayListAdd, statsObj);
        env->DeleteLocalRef(statsObj);
    } // while (sqlite3_step(stmt)
    sqlite3_finalize(stmt);
}

/*
     * JNI registration.
     */

static JNINativeMethod gMethods[] = {
    {"nativeGetPagerStats",
     "(Lcom/tencent/wcdb/database/SQLiteDebug$PagerStats;)V",
     (void *) nativeGetPagerStats},
    {"nativeGetLastErrorLine", "()I", (void *) nativeGetLastErrorLine},
    {"nativeSetIOTraceFlags", "(I)V", (void *) nativeSetIOTraceFlags},
    {"nativeGetIOTraceStats", "(JLjava/util/ArrayList;)V",
     (void *) nativeGetIOTraceStats},
};

static int register_wcdb_SQLiteDebug(JavaVM *vm, JNIEnv *env)
{
    // Default vfslog flags.
    vlogDefaultLogFlags = 0x001F407F;

    jclass clazz;
    FIND_CLASS(clazz, "com/tencent/wcdb/database/SQLiteDebug$PagerStats");

    GET_FIELD_ID(gSQLiteDebugPagerStatsClassInfo.memoryUsed, clazz,
                 "memoryUsed", "I");
    GET_FIELD_ID(gSQLiteDebugPagerStatsClassInfo.largestMemAlloc, clazz,
                 "largestMemAlloc", "I");
    GET_FIELD_ID(gSQLiteDebugPagerStatsClassInfo.pageCacheOverflow, clazz,
                 "pageCacheOverflow", "I");

    return jniRegisterNativeMethods(env,
                                    "com/tencent/wcdb/database/SQLiteDebug",
                                    gMethods, NELEM(gMethods));
}
WCDB_JNI_INIT(SQLiteDebug, register_wcdb_SQLiteDebug)

} // namespace wcdb
