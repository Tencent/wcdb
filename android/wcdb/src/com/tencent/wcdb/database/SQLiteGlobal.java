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

package com.tencent.wcdb.database;

import android.os.Environment;
import android.os.StatFs;

import java.lang.reflect.Method;

/**
 * Provides access to SQLite functions that affect all database connection,
 * such as memory management.
 *
 * The native code associated with SQLiteGlobal is also sets global configuration options
 * using sqlite3_config() then calls sqlite3_initialize() to ensure that the SQLite
 * library is properly initialized exactly once before any other framework or application
 * code has a chance to run.
 *
 * Verbose SQLite logging is enabled if the "log.tag.SQLiteLog" property is set to "V".
 * (per {@link SQLiteDebug#DEBUG_SQL_LOG}).
 *
 * @hide
 */
public final class SQLiteGlobal {
    private static final String TAG = "WCDB.SQLiteGlobal";

    private static native int nativeReleaseMemory();
    private static native void nativeSetDefaultPageSize(int pageSize);

    /** Default page size to use when creating a database. */
    public static final int defaultPageSize;

    /** Default journal mode when WAL is not in use. */
    public static final String defaultJournalMode = "PERSIST";

    /** Journal size limit in bytes. */
    public static final int journalSizeLimit = 524288;  // 512 kB

    /** Default database synchronization mode when WAL is not in use. */
    public static final String defaultSyncMode = "FULL";

    /** Database synchronization mode when in WAL mode. */
    public static final String walSyncMode = "FULL";

    /** WAL auto-checkpoint integer in database pages. */
    public static final int walAutoCheckpoint = 100;

    /** Connection pool size when in WAL mode. */
    public static final int walConnectionPoolSize = 4;

    static {
        // Test if libwcdb.so is already loaded in other routines.
        if (!WCDBInitializationProbe.libLoaded) {
            System.loadLibrary("wcdb");
        }

        int pageSize;
        try {
            String dataPath = Environment.getDataDirectory().getAbsolutePath();
            pageSize = new StatFs(dataPath).getBlockSize();
        } catch (RuntimeException e) {
            pageSize = 4096;
        }
        defaultPageSize = pageSize;
        nativeSetDefaultPageSize(pageSize);
    }
    // Dummy static method to trigger class initialization.
    // See [JLS 12.4.1](http://docs.oracle.com/javase/specs/jls/se7/html/jls-12.html#jls-12.4.1)
    public static void loadLib() {}

    private SQLiteGlobal() {}

    /**
     * Attempts to release memory by pruning the SQLite page cache and other
     * internal data structures.
     *
     * @return The number of bytes that were freed.
     */
    public static int releaseMemory() {
        return nativeReleaseMemory();
    }

}

/**
 * Probe class to detect whether "libwcdb.so" is loaded.
 * It's set to true in JNI initialization routine.
 */
class WCDBInitializationProbe {
    static boolean libLoaded = false;
    private WCDBInitializationProbe() {}
}
