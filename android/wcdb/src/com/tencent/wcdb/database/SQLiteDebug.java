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

package com.tencent.wcdb.database;

import java.util.ArrayList;

import android.annotation.SuppressLint;
import android.util.Printer;

/**
 * Provides debugging info about all SQLite databases running in the current process.
 *
 * {@hide}
 */
public final class SQLiteDebug {
	
    private static native void nativeGetPagerStats(PagerStats stats);
    private static native int nativeGetLastErrorLine();
    private static native void nativeSetIOTraceFlags(int flags);
    private static native void nativeGetIOTraceStats(long connectionPtr, ArrayList<IOTraceStats> statsList);

    static {
        // Ensure libmmdb.so is loaded.
        SQLiteDatabase.loadLib();
    }

    private SQLiteDebug() {
    }

    /**
     * Determines whether a query should be logged.
     *
     * Reads the "db.log.slow_query_threshold" system property, which can be changed
     * by the user at any time.  If the value is zero, then all queries will
     * be considered slow.  If the value does not exist or is negative, then no queries will
     * be considered slow.
     *
     * This value can be changed dynamically while the system is running.
     * For example, "adb shell setprop db.log.slow_query_threshold 200" will
     * log all queries that take 200ms or longer to run.
     * @hide
     */
    public static final boolean shouldLogSlowQuery(long elapsedTimeMillis) {
        return elapsedTimeMillis > 300;
    }

    /**
     * Contains statistics about the active pagers in the current process.
     *
     * @see #nativeGetPagerStats(PagerStats)
     */
    public static class PagerStats {
        /** the current amount of memory checked out by sqlite using sqlite3_malloc().
         * documented at http://www.sqlite.org/c3ref/c_status_malloc_size.html
         */
        public int memoryUsed;

        /** the number of bytes of page cache allocation which could not be sattisfied by the
         * SQLITE_CONFIG_PAGECACHE buffer and where forced to overflow to sqlite3_malloc().
         * The returned value includes allocations that overflowed because they where too large
         * (they were larger than the "sz" parameter to SQLITE_CONFIG_PAGECACHE) and allocations
         * that overflowed because no space was left in the page cache.
         * documented at http://www.sqlite.org/c3ref/c_status_malloc_size.html
         */
        public int pageCacheOverflow;

        /** records the largest memory allocation request handed to sqlite3.
         * documented at http://www.sqlite.org/c3ref/c_status_malloc_size.html
         */
        public int largestMemAlloc;

        /** a list of {@link DbStats} - one for each main database opened by the applications
         * running on the android device
         */
        public ArrayList<DbStats> dbStats;
    }

    /**
     * contains statistics about a database
     */
    public static class DbStats {
        /** name of the database */
        public String dbName;

        /** the page size for the database */
        public long pageSize;

        /** the database size */
        public long dbSize;

        /** documented here http://www.sqlite.org/c3ref/c_dbstatus_lookaside_used.html */
        public int lookaside;

        /** statement cache stats: hits/misses/cachesize */
        public String cache;

        public DbStats(String dbName, long pageCount, long pageSize, int lookaside,
            int hits, int misses, int cachesize) {
            this.dbName = dbName;
            this.pageSize = pageSize / 1024;
            dbSize = (pageCount * pageSize) / 1024;
            this.lookaside = lookaside;
            this.cache = hits + "/" + misses + "/" + cachesize;
        }
    }

    /**
     * return all pager and database stats for the current process.
     * @return {@link PagerStats}
     */
    public static PagerStats getDatabaseInfo() {
        PagerStats stats = new PagerStats();
        nativeGetPagerStats(stats);
        stats.dbStats = SQLiteDatabase.getDbStats();
        return stats;
    }


    public static class IOTraceStats {
        public String dbName;
        public String path;
        public long pageSize;
        public long pageCount;
        public String journalMode;

        public long lastReadOffset;
        public byte[] lastReadPage;
        public long lastWriteOffset;
        public byte[] lastWritePage;

        public long lastJournalReadOffset;
        public byte[] lastJournalReadPage;
        public long lastJournalWriteOffset;
        public byte[] lastJournalWritePage;

        @SuppressLint("DefaultLocale")
        @Override
        public String toString() {
            return String.format("[%s | %s] pageSize: %d, pageCount: %d, journal: %s, lastRead: %d, lastWrite: %d, " +
                "lastJournalRead: %d, lastJournalWrite: %d",
                    dbName, path, pageSize, pageCount, journalMode,
                    lastReadOffset, lastWriteOffset, lastJournalReadOffset, lastJournalWriteOffset);
        }
    }

    public static void setIOTraceFlags(int flags) {
        nativeSetIOTraceFlags(flags);
    }


    private static int sLastErrorLine;
    private static ArrayList<IOTraceStats> sLastIOTraceStats;

    public static int getLastErrorLine() {
        return sLastErrorLine;
    }

    public static ArrayList<IOTraceStats> getLastIOTraceStats() {
        return sLastIOTraceStats;
    }

    static void collectLastIOTraceStats(SQLiteConnection connection) {
        sLastErrorLine = nativeGetLastErrorLine();

        ArrayList<IOTraceStats> stats = new ArrayList<>();
        long ptr = connection.getNativeHandle(null);
        if (ptr != 0) {
            nativeGetIOTraceStats(ptr, stats);
            connection.endNativeHandle(null);
        }

        sLastIOTraceStats = stats;
    }

    static void collectLastIOTraceStats(SQLiteDatabase db) {
        sLastErrorLine = nativeGetLastErrorLine();

        ArrayList<IOTraceStats> stats = new ArrayList<>();
        long ptr = db.acquireNativeConnectionHandle("collectIoStat", false, false);
        if (ptr != 0) {
            nativeGetIOTraceStats(ptr, stats);
        }
        db.releaseNativeConnection(ptr, null);

        sLastIOTraceStats = stats;
    }


    /**
     * Dumps detailed information about all databases used by the process.
     * @param printer The printer for dumping database state.
     * @param args Command-line arguments supplied to dumpsys dbinfo
     */
    public static void dump(Printer printer, String[] args) {
        boolean verbose = false;
        for (String arg : args) {
            if (arg.equals("-v")) {
                verbose = true;
            }
        }

        SQLiteDatabase.dumpAll(printer, verbose);
    }
}
