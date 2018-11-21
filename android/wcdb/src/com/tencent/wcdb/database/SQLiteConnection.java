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


import android.annotation.SuppressLint;
import android.os.Process;
import android.util.Pair;
import android.util.Printer;

import com.tencent.wcdb.BuildConfig;
import com.tencent.wcdb.Cursor;
import com.tencent.wcdb.CursorWindow;
import com.tencent.wcdb.DatabaseUtils;
import com.tencent.wcdb.database.SQLiteDebug.DbStats;
import com.tencent.wcdb.support.CancellationSignal;
import com.tencent.wcdb.support.Log;
import com.tencent.wcdb.support.LruCache;
import com.tencent.wcdb.support.OperationCanceledException;

import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Map;
import java.util.regex.Pattern;

/**
 * Represents a SQLite database connection.
 * Each connection wraps an instance of a native <code>sqlite3</code> object.
 * <p>
 * When database connection pooling is enabled, there can be multiple active
 * connections to the same database.  Otherwise there is typically only one
 * connection per database.
 * </p><p>
 * When the SQLite WAL feature is enabled, multiple readers and one writer
 * can concurrently access the database.  Without WAL, readers and writers
 * are mutually exclusive.
 * </p>
 * <p/>
 * <h2>Ownership and concurrency guarantees</h2>
 * <p>
 * Connection objects are not thread-safe.  They are acquired as needed to
 * perform a database operation and are then returned to the pool.  At any
 * given time, a connection is either owned and used by a {@link SQLiteSession}
 * object or the {@link SQLiteConnectionPool}.  Those classes are
 * responsible for serializing operations to guard against concurrent
 * use of a connection.
 * </p><p>
 * The guarantee of having a single owner allows this class to be implemented
 * without locks and greatly simplifies resource management.
 * </p>
 * <p/>
 * <h2>Encapsulation guarantees</h2>
 * <p>
 * The connection object object owns *all* of the SQLite related native
 * objects that are associated with the connection.  What's more, there are
 * no other objects in the system that are capable of obtaining handles to
 * those native objects.  Consequently, when the connection is closed, we do
 * not have to worry about what other components might have references to
 * its associated SQLite state -- there are none.
 * </p><p>
 * Encapsulation is what ensures that the connection object's
 * lifecycle does not become a tortured mess of finalizers and reference
 * queues.
 * </p>
 * <p/>
 * <h2>Reentrance</h2>
 * <p>
 * This class must tolerate reentrant execution of SQLite operations because
 * triggers may call custom SQLite functions that perform additional queries.
 * </p>
 *
 * @hide
 */
public final class SQLiteConnection implements CancellationSignal.OnCancelListener {
    private static final String TAG = "WCDB.SQLiteConnection";
    private static final boolean DEBUG = false;

    private static final String[] EMPTY_STRING_ARRAY = new String[0];
    private static final byte[] EMPTY_BYTE_ARRAY = new byte[0];

    private static final Pattern TRIM_SQL_PATTERN = Pattern.compile("[\\s]*\\n+[\\s]*");

    private final SQLiteConnectionPool mPool;
    private final SQLiteDatabaseConfiguration mConfiguration;
    private final int mConnectionId;
    private final boolean mIsPrimaryConnection;
    private final boolean mIsReadOnlyConnection;
    private final PreparedStatementCache mPreparedStatementCache;
    private PreparedStatement mPreparedStatementPool;

    // The recent operations log.
    private final OperationLog mRecentOperations = new OperationLog();
    private Thread mAcquiredThread;
    private int mAcquiredTid;
    private StackTraceElement[] mAcquiredStack;
    private long mAcquiredTimestamp;

    // The native SQLiteConnection pointer.  (FOR INTERNAL USE ONLY)
    private long mConnectionPtr;

    private boolean mOnlyAllowReadOnlyOperations;

    // The number of times attachCancellationSignal has been called.
    // Because SQLite statement execution can be reentrant, we keep track of how many
    // times we have attempted to attach a cancellation signal to the connection so that
    // we can ensure that we detach the signal at the right time.
    private int mCancellationSignalAttachCount;

    private native long nativeOpen(String path, int openFlags, String vfsName);
    private static native void nativeClose(long connectionPtr);
    private static native void nativeRegisterCustomFunction(long connectionPtr,
            SQLiteCustomFunction function);
    private static native void nativeRegisterLocalizedCollators(long connectionPtr, String locale);
    private static native long nativePrepareStatement(long connectionPtr, String sql);
    private static native void nativeFinalizeStatement(long connectionPtr, long statementPtr);
    private static native int nativeGetParameterCount(long connectionPtr, long statementPtr);
    private static native boolean nativeIsReadOnly(long connectionPtr, long statementPtr);
    private static native int nativeGetColumnCount(long connectionPtr, long statementPtr);
    private static native String nativeGetColumnName(long connectionPtr, long statementPtr,
            int index);
    private static native void nativeBindNull(long connectionPtr, long statementPtr,
            int index);
    private static native void nativeBindLong(long connectionPtr, long statementPtr,
            int index, long value);
    private static native void nativeBindDouble(long connectionPtr, long statementPtr,
            int index, double value);
    private static native void nativeBindString(long connectionPtr, long statementPtr,
            int index, String value);
    private static native void nativeBindBlob(long connectionPtr, long statementPtr,
            int index, byte[] value);
    private static native void nativeResetStatement(long connectionPtr, long statementPtr,
            boolean clearBinding);
    private static native void nativeExecute(long connectionPtr, long statementPtr);
    private static native long nativeExecuteForLong(long connectionPtr, long statementPtr);
    private static native String nativeExecuteForString(long connectionPtr, long statementPtr);
    private static native int nativeExecuteForChangedRowCount(long connectionPtr, long statementPtr);
    private static native long nativeExecuteForLastInsertedRowId(long connectionPtr, long statementPtr);
    private static native long nativeExecuteForCursorWindow(long connectionPtr, long statementPtr,
            long windowPtr, int startPos, int requiredPos, boolean countAllRows);
    private static native int nativeGetDbLookaside(long connectionPtr);
    private static native void nativeCancel(long connectionPtr);
    private static native void nativeResetCancel(long connectionPtr, boolean cancelable);
    private static native void nativeSetKey(long connectionPtr, byte[] password);
    private static native void nativeSetWalHook(long connectionPtr);
    private static native long nativeWalCheckpoint(long connectionPtr, String dbName);
    private static native long nativeSQLiteHandle(long connectionPtr, boolean acquire);
    private static native void nativeSetUpdateNotification(long connectionPtr, boolean enabled,
            boolean notifyRowId);

    // Password for encrypted database.
    private byte[] mPassword;

    // Cipher description for encrypted database.
    private SQLiteCipherSpec mCipher;

    // Operation record for getNativeHandle().
    private Operation mNativeOperation;

    // Recurse count for getNativeHandle().
    private int mNativeHandleCount;


    private SQLiteConnection(SQLiteConnectionPool pool, SQLiteDatabaseConfiguration configuration,
            int connectionId, boolean primaryConnection, byte[] password, SQLiteCipherSpec cipher) {

        mPassword = password;
        mCipher = (cipher == null) ? null : new SQLiteCipherSpec(cipher);

        mPool = pool;
        mConfiguration = new SQLiteDatabaseConfiguration(configuration);
        mConnectionId = connectionId;
        mIsPrimaryConnection = primaryConnection;
        mIsReadOnlyConnection = (configuration.openFlags & SQLiteDatabase.OPEN_READONLY) != 0;
        mPreparedStatementCache = new PreparedStatementCache(mConfiguration.maxSqlCacheSize);
    }

    long getNativeHandle(String operation) {
        if (mConnectionPtr == 0)
            return 0;

        if (operation != null && mNativeOperation == null) {
            mNativeOperation = mRecentOperations.beginOperation(operation, null, null);
            mNativeOperation.mType = DatabaseUtils.STATEMENT_OTHER;
        }

        mNativeHandleCount++;
        return nativeSQLiteHandle(mConnectionPtr, true);
    }

    void endNativeHandle(Exception ex) {
        if (--mNativeHandleCount == 0 && mNativeOperation != null) {
            nativeSQLiteHandle(mConnectionPtr, false);

            if (ex == null) {
                mRecentOperations.endOperationDeferLog(mNativeOperation.mCookie);
                // Don't log native operations for now. Drop return value.
            } else {
                mRecentOperations.failOperation(mNativeOperation.mCookie, ex);
            }

            mNativeOperation = null;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mPool != null && mConnectionPtr != 0) {
                mPool.onConnectionLeaked();
            }

            dispose(true);
        } finally {
            super.finalize();
        }
    }

    // Called by SQLiteConnectionPool only.
    static SQLiteConnection open(SQLiteConnectionPool pool, SQLiteDatabaseConfiguration configuration,
            int connectionId, boolean primaryConnection, byte[] password, SQLiteCipherSpec cipher) {

        SQLiteConnection connection = new SQLiteConnection(pool, configuration,
                connectionId, primaryConnection, password, cipher);
        try {
            connection.open();
            return connection;
        } catch (SQLiteException ex) {
            SQLiteDebug.collectLastIOTraceStats(connection);
            connection.dispose(false);
            throw ex;
        }
    }

    // Closes the database closes and releases all of its associated resources.
    // Do not call methods on the connection after it is closed.  It will probably crash.
    void close() {
        dispose(false);
    }

    private void open() {
        mConnectionPtr = nativeOpen(mConfiguration.path, mConfiguration.openFlags,
                mConfiguration.vfsName);
        if (mPassword != null && mPassword.length == 0)
            mPassword = null;

        // 1. Key must be specified first as it's the initialization to codec context.
        if (mPassword != null) {
            nativeSetKey(mConnectionPtr, mPassword);

            // 2. Specify codec parameters such as cipher_name, kdf_iter and cipher_use_hmac.
            setCipherSpec();
        }

        // 3. Set page size. This must be done before any I/O operations take place.
        setPageSize();

        // 4. Other initialization steps from original SQLiteConnection.
        setReadOnlyFromConfiguration();
        setForeignKeyModeFromConfiguration();
        setWalModeFromConfiguration();
        setSyncModeFromConfiguration();
        setJournalSizeLimit();
        setCheckpointStrategy();
        setLocaleFromConfiguration();
        setUpdateNotificationFromConfiguration();

        // Register custom functions.
        final int functionCount = mConfiguration.customFunctions.size();
        for (int i = 0; i < functionCount; i++) {
            SQLiteCustomFunction function = mConfiguration.customFunctions.get(i);
            nativeRegisterCustomFunction(mConnectionPtr, function);
        }
    }

    private void dispose(boolean finalized) {
        if (mConnectionPtr != 0) {
            final int cookie = mRecentOperations.beginOperation("close", null, null).mCookie;
            try {
                mPreparedStatementCache.evictAll();
                nativeClose(mConnectionPtr);
                mConnectionPtr = 0;
            } finally {
                mRecentOperations.endOperation(cookie);
            }
        }
    }

    private void setPageSize() {
        if (!mConfiguration.isInMemoryDb()) {
            String pragmaCmd;
            long newValue;

            if (mPassword != null) {
                // For encrypted databases, call 'PRAGMA cipher_page_size' instead.
                pragmaCmd = "PRAGMA cipher_page_size";
                newValue = (mCipher == null || mCipher.pageSize <= 0) ?
                        SQLiteGlobal.defaultPageSize : mCipher.pageSize;
            } else {
                pragmaCmd = "PRAGMA page_size";
                newValue = SQLiteGlobal.defaultPageSize;
            }

            long value = executeForLong(pragmaCmd, null, null);
            if (value != newValue) {
                execute(pragmaCmd + "=" + newValue, null, null);
            }
        }
    }

    private void setCipherSpec() {
        if (mCipher != null) {
            if (mCipher.cipher != null)
                execute("PRAGMA cipher=" + DatabaseUtils.sqlEscapeString(mCipher.cipher),
                        null, null);

            if (mCipher.kdfIteration != 0)
                execute("PRAGMA kdf_iter=" + mCipher.kdfIteration, null, null);

            execute("PRAGMA cipher_use_hmac=" + mCipher.hmacEnabled, null, null);
        }
    }

    @SuppressWarnings("unused")
    private void notifyCheckpoint(String dbName, int pages) {
        mPool.notifyCheckpoint(dbName, pages);
    }

    private void setCheckpointStrategy() {
        if (!mConfiguration.isInMemoryDb() && !mIsReadOnlyConnection) {
            if (mConfiguration.customWALHookEnabled) {
                nativeSetWalHook(mConnectionPtr);
            } else {
                final long newValue = SQLiteGlobal.walAutoCheckpoint;
                long value = executeForLong("PRAGMA wal_autocheckpoint", null, null);
                if (value != newValue) {
                    executeForLong("PRAGMA wal_autocheckpoint=" + newValue, null, null);
                }
            }
        }
    }

    private void setJournalSizeLimit() {
        if (!mConfiguration.isInMemoryDb() && !mIsReadOnlyConnection) {
            final long newValue = SQLiteGlobal.journalSizeLimit;
            long value = executeForLong("PRAGMA journal_size_limit", null, null);
            if (value != newValue) {
                executeForLong("PRAGMA journal_size_limit=" + newValue, null, null);
            }
        }
    }

    private void setForeignKeyModeFromConfiguration() {
        if (!mIsReadOnlyConnection) {
            final long newValue = mConfiguration.foreignKeyConstraintsEnabled ? 1 : 0;
            long value = executeForLong("PRAGMA foreign_keys", null, null);
            if (value != newValue) {
                execute("PRAGMA foreign_keys=" + newValue, null, null);
            }
        }
    }

    private void setWalModeFromConfiguration() {
        if (!mConfiguration.isInMemoryDb() && !mIsReadOnlyConnection) {
            String journalMode;
            if ((mConfiguration.openFlags & SQLiteDatabase.ENABLE_WRITE_AHEAD_LOGGING) != 0) {
                journalMode = "WAL";
            } else {
                journalMode = SQLiteGlobal.defaultJournalMode;
            }
            setJournalMode(journalMode);
        }
    }

    private void setSyncModeFromConfiguration() {
        int syncMode = mConfiguration.synchronousMode;
        execute("PRAGMA synchronous=" + syncMode, null, null);
    }

    private void setJournalMode(String newValue) {
        String value = executeForString("PRAGMA journal_mode", null, null);
        if (!value.equalsIgnoreCase(newValue)) {
            try {
                String result = executeForString("PRAGMA journal_mode=" + newValue, null, null);
                if (result.equalsIgnoreCase(newValue)) {
                    return;
                }
                // PRAGMA journal_mode silently fails and returns the original journal
                // mode in some cases if the journal mode could not be changed.
            } catch (SQLiteDatabaseLockedException ex) {
                // This error (SQLITE_BUSY) occurs if one connection has the database
                // open in WAL mode and another tries to change it to non-WAL.
            }
            // Because we always disable WAL mode when a database is first opened
            // (even if we intend to re-enable it), we can encounter problems if
            // there is another open connection to the database somewhere.
            // This can happen for a variety of reasons such as an application opening
            // the same database in multiple processes at the same time or if there is a
            // crashing content provider service that the ActivityManager has
            // removed from its registry but whose process hasn't quite died yet
            // by the time it is restarted in a new process.
            //
            // If we don't change the journal mode, nothing really bad happens.
            // In the worst case, an application that enables WAL might not actually
            // get it, although it can still use connection pooling.
            Log.w(TAG, "Could not change the database journal mode of '"
                    + mConfiguration.label + "' from '" + value + "' to '" + newValue
                    + "' because the database is locked.  This usually means that "
                    + "there are other open connections to the database which prevents "
                    + "the database from enabling or disabling write-ahead logging mode.  "
                    + "Proceeding without changing the journal mode.");
        }
    }

    private void setLocaleFromConfiguration() {
        mConfiguration.openFlags |= SQLiteDatabase.NO_LOCALIZED_COLLATORS;
        if ((mConfiguration.openFlags & SQLiteDatabase.NO_LOCALIZED_COLLATORS) != 0) {
            return;
        }

        // Register the localized collators.
        final String newLocale = mConfiguration.locale.toString();
        nativeRegisterLocalizedCollators(mConnectionPtr, newLocale);

        // If the database is read-only, we cannot modify the android metadata table
        // or existing indexes.
        if (mIsReadOnlyConnection) {
            return;
        }

        try {
            // Ensure the android metadata table exists.
            execute("CREATE TABLE IF NOT EXISTS android_metadata (locale TEXT)", null, null);

            // Check whether the locale was actually changed.
            final String oldLocale = executeForString("SELECT locale FROM android_metadata "
                    + "UNION SELECT NULL ORDER BY locale DESC LIMIT 1", null, null);
            if (oldLocale != null && oldLocale.equals(newLocale)) {
                return;
            }

            // Go ahead and update the indexes using the new locale.
            execute("BEGIN", null, null);
            boolean success = false;
            try {
                execute("DELETE FROM android_metadata", null, null);
                execute("INSERT INTO android_metadata (locale) VALUES(?)",
                        new Object[]{newLocale}, null);
                execute("REINDEX LOCALIZED", null, null);
                success = true;
            } finally {
                execute(success ? "COMMIT" : "ROLLBACK", null, null);
            }
        } catch (RuntimeException ex) {
            throw new SQLiteException("Failed to change locale for db '" + mConfiguration.label
                    + "' to '" + newLocale + "'.", ex);
        }
    }

    private void setReadOnlyFromConfiguration() {
        // Currently, read-only flag can only be changed via reopening connection, so no operation
        // needed for read/write connections.
        if (mIsReadOnlyConnection) {
            execute("PRAGMA query_only = 1", null, null);
        }
    }

    @SuppressWarnings("unused")
    private void notifyChange(String db, String table, long[] insertIds, long[] updateIds, long[] deleteIds) {
        mPool.notifyChanges(db, table, insertIds, updateIds, deleteIds);
    }

    private void setUpdateNotificationFromConfiguration() {
        nativeSetUpdateNotification(mConnectionPtr,
                mConfiguration.updateNotificationEnabled,
                mConfiguration.updateNotificationRowID);
    }

    // Called by SQLiteConnectionPool only.
    void reconfigure(SQLiteDatabaseConfiguration configuration) {
        mOnlyAllowReadOnlyOperations = false;

        // Register custom functions.
        final int functionCount = configuration.customFunctions.size();
        for (int i = 0; i < functionCount; i++) {
            SQLiteCustomFunction function = configuration.customFunctions.get(i);
            if (!mConfiguration.customFunctions.contains(function)) {
                nativeRegisterCustomFunction(mConnectionPtr, function);
            }
        }

        // Remember what changed.
        int openFlagsChanged = configuration.openFlags ^ mConfiguration.openFlags;
        boolean walModeChanged = (openFlagsChanged & SQLiteDatabase.ENABLE_WRITE_AHEAD_LOGGING) != 0;
        boolean foreignKeyModeChanged = configuration.foreignKeyConstraintsEnabled
                != mConfiguration.foreignKeyConstraintsEnabled;
        boolean localeChanged = !configuration.locale.equals(mConfiguration.locale);
        boolean checkpointStrategyChanged = configuration.customWALHookEnabled
                != mConfiguration.customWALHookEnabled;
        boolean synchronousChanged = configuration.synchronousMode
                != mConfiguration.synchronousMode;
        boolean updateNotificationChanged =
                (configuration.updateNotificationEnabled != mConfiguration.updateNotificationEnabled) ||
                (configuration.updateNotificationRowID != mConfiguration.updateNotificationRowID);

        // Update configuration parameters.
        mConfiguration.updateParametersFrom(configuration);

        // Update prepared statement cache size.
        mPreparedStatementCache.resize(configuration.maxSqlCacheSize);

        // Update foreign key mode.
        if (foreignKeyModeChanged) {
            setForeignKeyModeFromConfiguration();
        }

        // Update WAL.
        if (walModeChanged) {
            setWalModeFromConfiguration();
        }

        // Update synchronous mode.
        if (synchronousChanged) {
            setSyncModeFromConfiguration();
        }

        // Update checkpoint strategy. This must be done after setting WAL mode.
        if (checkpointStrategyChanged) {
            setCheckpointStrategy();
        }

        // Update locale.
        if (localeChanged) {
            setLocaleFromConfiguration();
        }

        // Update notification.
        if (updateNotificationChanged) {
            setUpdateNotificationFromConfiguration();
        }
    }

    // Called by SQLiteConnectionPool only.
    // When set to true, executing write operations will throw SQLiteException.
    // Preparing statements that might write is ok, just don't execute them.
    void setOnlyAllowReadOnlyOperations(boolean readOnly) {
        mOnlyAllowReadOnlyOperations = readOnly;
    }

    // Called by SQLiteSession only.
    // Mark acquisition state for debug purpose.
    void setAcquisitionState(boolean acquired, boolean persist) {
        if (acquired) {
            mAcquiredThread = Thread.currentThread();
            mAcquiredTid = Process.myTid();
            if (persist) {
                mAcquiredStack = mAcquiredThread.getStackTrace();
                mAcquiredTimestamp = System.currentTimeMillis();
            } else {
                mAcquiredStack = null;
                mAcquiredTimestamp = 0;
            }
        } else {
            mAcquiredThread = null;
            mAcquiredTid = 0;
            mAcquiredStack = null;
            mAcquiredTimestamp = 0;
        }
    }

    // Called by SQLiteConnectionPool only.
    // Returns true if the prepared statement cache contains the specified SQL.
    boolean isPreparedStatementInCache(String sql) {
        return mPreparedStatementCache.get(sql) != null;
    }

    /**
     * Gets the unique id of this connection.
     *
     * @return The connection id.
     */
    public int getConnectionId() {
        return mConnectionId;
    }

    /**
     * Returns true if this is the primary database connection.
     *
     * @return True if this is the primary database connection.
     */
    public boolean isPrimaryConnection() {
        return mIsPrimaryConnection;
    }

    /**
     * Prepares a statement for execution but does not bind its parameters or execute it.
     * <p>
     * This method can be used to check for syntax errors during compilation
     * prior to execution of the statement.  If the {@code outStatementInfo} argument
     * is not null, the provided {@link SQLiteStatementInfo} object is populated
     * with information about the statement.
     * </p><p>
     * A prepared statement makes no reference to the arguments that may eventually
     * be bound to it, consequently it it possible to cache certain prepared statements
     * such as SELECT or INSERT/UPDATE statements.  If the statement is cacheable,
     * then it will be stored in the cache for later.
     * </p><p>
     * To take advantage of this behavior as an optimization, the connection pool
     * provides a method to acquire a connection that already has a given SQL statement
     * in its prepared statement cache so that it is ready for execution.
     * </p>
     *
     * @param sql              The SQL statement to prepare.
     * @param outStatementInfo The {@link SQLiteStatementInfo} object to populate
     *                         with information about the statement, or null if none.
     * @throws SQLiteException if an error occurs, such as a syntax error.
     */
    public void prepare(String sql, SQLiteStatementInfo outStatementInfo) {
        if (sql == null) {
            throw new IllegalArgumentException("sql must not be null.");
        }
        Operation operation = mRecentOperations.beginOperation("prepare", sql, null);
        final int cookie = operation.mCookie;
        try {
            final PreparedStatement statement = acquirePreparedStatement(sql);
            operation.mType = statement.mType;
            try {
                if (outStatementInfo != null) {
                    outStatementInfo.numParameters = statement.mNumParameters;
                    outStatementInfo.readOnly = statement.mReadOnly;

                    final int columnCount = nativeGetColumnCount(
                            mConnectionPtr, statement.getPtr());
                    if (columnCount == 0) {
                        outStatementInfo.columnNames = EMPTY_STRING_ARRAY;
                    } else {
                        outStatementInfo.columnNames = new String[columnCount];
                        for (int i = 0; i < columnCount; i++) {
                            outStatementInfo.columnNames[i] = nativeGetColumnName(
                                    mConnectionPtr, statement.getPtr(), i);
                        }
                    }
                }
            } finally {
                releasePreparedStatement(statement);
            }
        } catch (RuntimeException ex) {
            mRecentOperations.failOperation(cookie, ex);
            throw ex;
        } finally {
            mRecentOperations.endOperation(cookie);
        }
    }

    /**
     * Executes a statement that does not return a result.
     *
     * @param sql                The SQL statement to execute.
     * @param bindArgs           The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @throws SQLiteException            if an error occurs, such as a syntax error
     *                                    or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    public void execute(String sql, Object[] bindArgs,
            CancellationSignal cancellationSignal) {
        if (sql == null) {
            throw new IllegalArgumentException("sql must not be null.");
        }
        Operation operation = mRecentOperations.beginOperation("execute", sql, bindArgs);
        final int cookie = operation.mCookie;
        try {
            final PreparedStatement statement = acquirePreparedStatement(sql);
            operation.mType = statement.mType;
            try {
                throwIfStatementForbidden(statement);
                bindArguments(statement, bindArgs);
                applyBlockGuardPolicy(statement);
                attachCancellationSignal(cancellationSignal);
                try {
                    nativeExecute(mConnectionPtr, statement.getPtr());
                } finally {
                    detachCancellationSignal(cancellationSignal);
                }
            } finally {
                releasePreparedStatement(statement);
            }
        } catch (RuntimeException ex) {
            mRecentOperations.failOperation(cookie, ex);
            throw ex;
        } finally {
            mRecentOperations.endOperation(cookie);
        }
    }

    /**
     * Executes a statement that returns a single <code>long</code> result.
     *
     * @param sql                The SQL statement to execute.
     * @param bindArgs           The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The value of the first column in the first row of the result set
     * as a <code>long</code>, or zero if none.
     * @throws SQLiteException            if an error occurs, such as a syntax error
     *                                    or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    public long executeForLong(String sql, Object[] bindArgs,
            CancellationSignal cancellationSignal) {
        if (sql == null) {
            throw new IllegalArgumentException("sql must not be null.");
        }

        Operation operation = mRecentOperations.beginOperation("executeForLong", sql, bindArgs);
        final int cookie = operation.mCookie;
        try {
            final PreparedStatement statement = acquirePreparedStatement(sql);
            operation.mType = statement.mType;
            try {
                throwIfStatementForbidden(statement);
                bindArguments(statement, bindArgs);
                applyBlockGuardPolicy(statement);
                attachCancellationSignal(cancellationSignal);
                try {
                    return nativeExecuteForLong(mConnectionPtr, statement.getPtr());
                } finally {
                    detachCancellationSignal(cancellationSignal);
                }
            } finally {
                releasePreparedStatement(statement);
            }
        } catch (RuntimeException ex) {
            mRecentOperations.failOperation(cookie, ex);
            throw ex;
        } finally {
            mRecentOperations.endOperation(cookie);
        }
    }

    /**
     * Executes a statement that returns a single {@link String} result.
     *
     * @param sql                The SQL statement to execute.
     * @param bindArgs           The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The value of the first column in the first row of the result set
     * as a <code>String</code>, or null if none.
     * @throws SQLiteException            if an error occurs, such as a syntax error
     *                                    or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    public String executeForString(String sql, Object[] bindArgs,
            CancellationSignal cancellationSignal) {
        if (sql == null) {
            throw new IllegalArgumentException("sql must not be null.");
        }
        Operation operation = mRecentOperations.beginOperation("executeForString", sql, bindArgs);
        final int cookie = operation.mCookie;
        try {
            final PreparedStatement statement = acquirePreparedStatement(sql);
            operation.mType = statement.mType;
            try {
                throwIfStatementForbidden(statement);
                bindArguments(statement, bindArgs);
                applyBlockGuardPolicy(statement);
                attachCancellationSignal(cancellationSignal);
                try {
                    return nativeExecuteForString(mConnectionPtr, statement.getPtr());
                } finally {
                    detachCancellationSignal(cancellationSignal);
                }
            } finally {
                releasePreparedStatement(statement);
            }
        } catch (RuntimeException ex) {
            mRecentOperations.failOperation(cookie, ex);
            throw ex;
        } finally {
            mRecentOperations.endOperation(cookie);
        }
    }

    /**
     * Executes a statement that returns a count of the number of rows
     * that were changed.  Use for UPDATE or DELETE SQL statements.
     *
     * @param sql                The SQL statement to execute.
     * @param bindArgs           The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The number of rows that were changed.
     * @throws SQLiteException            if an error occurs, such as a syntax error
     *                                    or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    public int executeForChangedRowCount(String sql, Object[] bindArgs,
            CancellationSignal cancellationSignal) {
        if (sql == null) {
            throw new IllegalArgumentException("sql must not be null.");
        }

        int changedRows = 0;
        Operation operation = mRecentOperations.beginOperation("executeForChangedRowCount", sql, bindArgs);
        final int cookie = operation.mCookie;
        try {
            final PreparedStatement statement = acquirePreparedStatement(sql);
            operation.mType = statement.mType;
            try {
                throwIfStatementForbidden(statement);
                bindArguments(statement, bindArgs);
                applyBlockGuardPolicy(statement);
                attachCancellationSignal(cancellationSignal);
                try {
                    changedRows = nativeExecuteForChangedRowCount(
                            mConnectionPtr, statement.getPtr());
                    return changedRows;
                } finally {
                    detachCancellationSignal(cancellationSignal);
                }
            } finally {
                releasePreparedStatement(statement);
            }
        } catch (RuntimeException ex) {
            mRecentOperations.failOperation(cookie, ex);
            throw ex;
        } finally {
            if (mRecentOperations.endOperationDeferLog(cookie)) {
                mRecentOperations.logOperation(cookie, "changedRows=" + changedRows);
            }
        }
    }

    /**
     * Executes a statement that returns the row id of the last row inserted
     * by the statement.  Use for INSERT SQL statements.
     *
     * @param sql                The SQL statement to execute.
     * @param bindArgs           The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The row id of the last row that was inserted, or 0 if none.
     * @throws SQLiteException            if an error occurs, such as a syntax error
     *                                    or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    public long executeForLastInsertedRowId(String sql, Object[] bindArgs,
            CancellationSignal cancellationSignal) {
        if (sql == null) {
            throw new IllegalArgumentException("sql must not be null.");
        }
        Operation operation = mRecentOperations.beginOperation("executeForLastInsertedRowId", sql, bindArgs);
        final int cookie = operation.mCookie;
        try {
            final PreparedStatement statement = acquirePreparedStatement(sql);
            operation.mType = statement.mType;
            try {
                throwIfStatementForbidden(statement);
                bindArguments(statement, bindArgs);
                applyBlockGuardPolicy(statement);
                attachCancellationSignal(cancellationSignal);
                try {
                    return nativeExecuteForLastInsertedRowId(
                            mConnectionPtr, statement.getPtr());
                } finally {
                    detachCancellationSignal(cancellationSignal);
                }
            } finally {
                releasePreparedStatement(statement);
            }
        } catch (RuntimeException ex) {
            mRecentOperations.failOperation(cookie, ex);
            throw ex;
        } finally {
            mRecentOperations.endOperation(cookie);
        }
    }

    public int executeForCursorWindow(String sql, Object[] bindArgs, CursorWindow window,
            int startPos, int requiredPos, boolean countAllRows,
            CancellationSignal cancellationSignal) {
        if (sql == null) {
            throw new IllegalArgumentException("sql must not be null.");
        }
        if (window == null) {
            throw new IllegalArgumentException("window must not be null.");
        }

        window.acquireReference();
        try {
            int actualPos = -1;
            int countedRows = -1;
            int filledRows = -1;
            Operation operation = mRecentOperations.beginOperation("executeForCursorWindow", sql, bindArgs);
            final int cookie = operation.mCookie;
            try {
                final PreparedStatement statement = acquirePreparedStatement(sql);
                operation.mType = statement.mType;
                try {
                    throwIfStatementForbidden(statement);
                    bindArguments(statement, bindArgs);
                    applyBlockGuardPolicy(statement);
                    attachCancellationSignal(cancellationSignal);
                    try {
                        final long result = nativeExecuteForCursorWindow(
                                mConnectionPtr, statement.getPtr(), window.mWindowPtr,
                                startPos, requiredPos, countAllRows);
                        actualPos = (int) (result >> 32);
                        countedRows = (int) result;
                        filledRows = window.getNumRows();
                        window.setStartPosition(actualPos);
                        return countedRows;
                    } finally {
                        detachCancellationSignal(cancellationSignal);
                    }
                } finally {
                    releasePreparedStatement(statement);
                }
            } catch (RuntimeException ex) {
                mRecentOperations.failOperation(cookie, ex);
                throw ex;
            } finally {
                if (mRecentOperations.endOperationDeferLog(cookie)) {
                    mRecentOperations.logOperation(cookie, "window='" + window
                            + "', startPos=" + startPos
                            + ", actualPos=" + actualPos
                            + ", filledRows=" + filledRows
                            + ", countedRows=" + countedRows);
                }
            }
        } finally {
            window.releaseReference();
        }
    }

    public Pair<Integer, Integer> walCheckpoint(String dbName) {
        if (dbName == null || dbName.isEmpty())
            dbName = "main";

        long result = nativeWalCheckpoint(mConnectionPtr, dbName);
        int walPages = (int) (result >> 32);
        int checkpointedPages = (int) (result & 0xFFFFFFFFL);
        return new Pair<>(walPages, checkpointedPages);
    }

    /*package*/ PreparedStatement acquirePreparedStatement(String sql) {
        PreparedStatement statement = mPreparedStatementCache.get(sql);
        boolean skipCache = false;
        if (statement != null) {
            if (!statement.mInUse) {
                statement.mInUse = true;
                return statement;
            }
            // The statement is already in the cache but is in use (this statement appears
            // to be not only re-entrant but recursive!).  So prepare a new copy of the
            // statement but do not cache it.
            skipCache = true;
        }

        final long statementPtr = nativePrepareStatement(mConnectionPtr, sql);
        try {
            final int numParameters = nativeGetParameterCount(mConnectionPtr, statementPtr);
            final int type = DatabaseUtils.getSqlStatementType(sql);
            final boolean readOnly = nativeIsReadOnly(mConnectionPtr, statementPtr);
            statement = obtainPreparedStatement(sql, statementPtr, numParameters, type, readOnly);
            if (!skipCache && isCacheable(type)) {
                mPreparedStatementCache.put(sql, statement);
                statement.mInCache = true;
            }
        } catch (RuntimeException ex) {
            // Finalize the statement if an exception occurred and we did not add
            // it to the cache.  If it is already in the cache, then leave it there.
            if (statement == null || !statement.mInCache) {
                nativeFinalizeStatement(mConnectionPtr, statementPtr);
            }
            throw ex;
        }
        statement.mInUse = true;
        return statement;
    }

    /*package*/ void releasePreparedStatement(PreparedStatement statement) {
        statement.mInUse = false;
        if (statement.mInCache) {
            try {
                resetStatement(statement, true);
            } catch (SQLiteException ex) {
                // The statement could not be reset due to an error.  Remove it from the cache.
                // When remove() is called, the cache will invoke its entryRemoved() callback,
                // which will in turn call finalizePreparedStatement() to finalize and
                // recycle the statement.
                if (DEBUG) {
                    Log.d(TAG, "Could not reset prepared statement due to an exception.  "
                            + "Removing it from the cache.  SQL: "
                            + trimSqlForDisplay(statement.mSql), ex);
                }

                mPreparedStatementCache.remove(statement.mSql);
            }
        } else {
            finalizePreparedStatement(statement);
        }
    }

    private void finalizePreparedStatement(PreparedStatement statement) {
        nativeFinalizeStatement(mConnectionPtr, statement.getPtr());
        recyclePreparedStatement(statement);
    }

    private void attachCancellationSignal(CancellationSignal cancellationSignal) {
        if (cancellationSignal != null) {
            cancellationSignal.throwIfCanceled();

            mCancellationSignalAttachCount += 1;
            if (mCancellationSignalAttachCount == 1) {
                // Reset cancellation flag before executing the statement.
                nativeResetCancel(mConnectionPtr, true /*cancelable*/);

                // After this point, onCancel() may be called concurrently.
                cancellationSignal.setOnCancelListener(this);
            }
        }
    }

    private void detachCancellationSignal(CancellationSignal cancellationSignal) {
        if (cancellationSignal != null) {
            if (BuildConfig.DEBUG && !(mCancellationSignalAttachCount > 0))
                throw new AssertionError("mCancellationSignalAttachCount > 0");

            mCancellationSignalAttachCount -= 1;
            if (mCancellationSignalAttachCount == 0) {
                // After this point, onCancel() cannot be called concurrently.
                cancellationSignal.setOnCancelListener(null);

                // Reset cancellation flag after executing the statement.
                nativeResetCancel(mConnectionPtr, false /*cancelable*/);
            }
        }
    }

    // CancellationSignal.OnCancelListener callback.
    // This method may be called on a different thread than the executing statement.
    // However, it will only be called between calls to attachCancellationSignal and
    // detachCancellationSignal, while a statement is executing.  We can safely assume
    // that the SQLite connection is still alive.
    @Override
    public void onCancel() {
        nativeCancel(mConnectionPtr);
    }

    private void bindArguments(PreparedStatement statement, Object[] bindArgs) {
        final int count = bindArgs != null ? bindArgs.length : 0;
        if (count != statement.mNumParameters) {
            throw new SQLiteBindOrColumnIndexOutOfRangeException(
                    "Expected " + statement.mNumParameters + " bind arguments but "
                            + count + " were provided.");
        }
        if (count == 0) {
            return;
        }

        final long statementPtr = statement.getPtr();
        for (int i = 0; i < count; i++) {
            final Object arg = bindArgs[i];
            switch (DatabaseUtils.getTypeOfObject(arg)) {
                case Cursor.FIELD_TYPE_NULL:
                    nativeBindNull(mConnectionPtr, statementPtr, i + 1);
                    break;
                case Cursor.FIELD_TYPE_INTEGER:
                    nativeBindLong(mConnectionPtr, statementPtr, i + 1,
                            ((Number) arg).longValue());
                    break;
                case Cursor.FIELD_TYPE_FLOAT:
                    nativeBindDouble(mConnectionPtr, statementPtr, i + 1,
                            ((Number) arg).doubleValue());
                    break;
                case Cursor.FIELD_TYPE_BLOB:
                    nativeBindBlob(mConnectionPtr, statementPtr, i + 1, (byte[]) arg);
                    break;
                case Cursor.FIELD_TYPE_STRING:
                default:
                    if (arg instanceof Boolean) {
                        // Provide compatibility with legacy applications which may pass
                        // Boolean values in bind args.
                        nativeBindLong(mConnectionPtr, statementPtr, i + 1,
                                ((Boolean) arg).booleanValue() ? 1 : 0);
                    } else {
                        nativeBindString(mConnectionPtr, statementPtr, i + 1, arg.toString());
                    }
                    break;
            }
        }
    }

    private void resetStatement(PreparedStatement statement, boolean clearBindings) {
        nativeResetStatement(mConnectionPtr, statement.getPtr(), clearBindings);
    }

    private void throwIfStatementForbidden(PreparedStatement statement) {
        if (mOnlyAllowReadOnlyOperations && !statement.mReadOnly) {
            throw new SQLiteException("Cannot execute this statement because it "
                    + "might modify the database but the connection is read-only.");
        }
    }

    private static boolean isCacheable(int statementType) {
        if (statementType == DatabaseUtils.STATEMENT_UPDATE
                || statementType == DatabaseUtils.STATEMENT_SELECT) {
            return true;
        }
        return false;
    }

    private void applyBlockGuardPolicy(PreparedStatement statement) {
        // do nothing
    }

    /**
     * Dumps debugging information about this connection.
     *
     * @param printer The printer to receive the dump, not null.
     * @param verbose True to dump more verbose information.
     */
    public void dump(Printer printer, boolean verbose) {
        dumpUnsafe(printer, verbose);
    }

    /**
     * Dumps debugging information about this connection, in the case where the
     * caller might not actually own the connection.
     * <p/>
     * This function is written so that it may be called by a thread that does not
     * own the connection.  We need to be very careful because the connection state is
     * not synchronized.
     * <p/>
     * At worst, the method may return stale or slightly wrong data, however
     * it should not crash.  This is ok as it is only used for diagnostic purposes.
     *
     * @param printer The printer to receive the dump, not null.
     * @param verbose True to dump more verbose information.
     */
    void dumpUnsafe(Printer printer, boolean verbose) {
        printer.println("Connection #" + mConnectionId + ":");
        if (verbose) {
            printer.println("  connectionPtr: 0x" + Long.toHexString(mConnectionPtr));
        }
        printer.println("  isPrimaryConnection: " + mIsPrimaryConnection);
        printer.println("  onlyAllowReadOnlyOperations: " + mOnlyAllowReadOnlyOperations);
        if (mAcquiredThread != null) {
            printer.println("  acquiredThread: " + mAcquiredThread + " (tid: " + mAcquiredTid + ")");
        }

        mRecentOperations.dump(printer, verbose);

        if (verbose) {
            mPreparedStatementCache.dump(printer);
        }
    }

    /**
     * Describes the currently executing operation, in the case where the
     * caller might not actually own the connection.
     * <p/>
     * This function is written so that it may be called by a thread that does not
     * own the connection.  We need to be very careful because the connection state is
     * not synchronized.
     * <p/>
     * At worst, the method may return stale or slightly wrong data, however
     * it should not crash.  This is ok as it is only used for diagnostic purposes.
     *
     * @return A description of the current operation including how long it has been running,
     * or null if none.
     */
    String describeCurrentOperationUnsafe() {
        return mRecentOperations.describeCurrentOperation();
    }

    SQLiteTrace.TraceInfo<String> traceCurrentOperationUnsafe() {
        return mRecentOperations.traceCurrentOperation();
    }

    SQLiteTrace.TraceInfo<StackTraceElement[]> tracePersistAcquisitionUnsafe() {
        StackTraceElement[] stack = mAcquiredStack;
        return (stack == null) ? null :
                new SQLiteTrace.TraceInfo<>(stack, mAcquiredTimestamp, mAcquiredTid);
    }

    /**
     * Collects statistics about database connection memory usage.
     *
     * @param dbStatsList The list to populate.
     */
    void collectDbStats(ArrayList<DbStats> dbStatsList) {
        // Get information about the main database.
        int lookaside = nativeGetDbLookaside(mConnectionPtr);
        long pageCount = 0;
        long pageSize = 0;
        try {
            pageCount = executeForLong("PRAGMA page_count;", null, null);
            pageSize = executeForLong("PRAGMA page_size;", null, null);
        } catch (SQLiteException ex) {
            // Ignore.
        }
        dbStatsList.add(getMainDbStatsUnsafe(lookaside, pageCount, pageSize));

        // Get information about attached databases.
        // We ignore the first row in the database list because it corresponds to
        // the main database which we have already described.
        CursorWindow window = new CursorWindow("collectDbStats");
        try {
            executeForCursorWindow("PRAGMA database_list;", null, window, 0, 0, false, null);
            for (int i = 1; i < window.getNumRows(); i++) {
                String name = window.getString(i, 1);
                String path = window.getString(i, 2);
                pageCount = 0;
                pageSize = 0;
                try {
                    pageCount = executeForLong("PRAGMA " + name + ".page_count;", null, null);
                    pageSize = executeForLong("PRAGMA " + name + ".page_size;", null, null);
                } catch (SQLiteException ex) {
                    // Ignore.
                }
                String label = "  (attached) " + name;
                if (path.length() != 0) {
                    label += ": " + path;
                }
                dbStatsList.add(new DbStats(label, pageCount, pageSize, 0, 0, 0, 0));
            }
        } catch (SQLiteException ex) {
            // Ignore.
        } finally {
            window.close();
        }
    }

    /**
     * Collects statistics about database connection memory usage, in the case where the
     * caller might not actually own the connection.
     *
     * @return The statistics object, never null.
     */
    void collectDbStatsUnsafe(ArrayList<DbStats> dbStatsList) {
        dbStatsList.add(getMainDbStatsUnsafe(0, 0, 0));
    }

    private DbStats getMainDbStatsUnsafe(int lookaside, long pageCount, long pageSize) {
        // The prepared statement cache is thread-safe so we can access its statistics
        // even if we do not own the database connection.
        String label = mConfiguration.path;
        if (!mIsPrimaryConnection) {
            label += " (" + mConnectionId + ")";
        }
        return new DbStats(label, pageCount, pageSize, lookaside,
                mPreparedStatementCache.hitCount(),
                mPreparedStatementCache.missCount(),
                mPreparedStatementCache.size());
    }

    @Override
    public String toString() {
        return "SQLiteConnection: " + mConfiguration.path + " (" + mConnectionId + ")";
    }

    private PreparedStatement obtainPreparedStatement(String sql, long statementPtr,
            int numParameters, int type, boolean readOnly) {
        PreparedStatement statement = mPreparedStatementPool;
        if (statement != null) {
            mPreparedStatementPool = statement.mPoolNext;
            statement.mPoolNext = null;
            statement.mInCache = false;
        } else {
            statement = new PreparedStatement(this);
        }
        statement.mSql = sql;
        statement.mStatementPtr = statementPtr;
        statement.mNumParameters = numParameters;
        statement.mType = type;
        statement.mReadOnly = readOnly;
        return statement;
    }

    private void recyclePreparedStatement(PreparedStatement statement) {
        statement.mSql = null;
        statement.mPoolNext = mPreparedStatementPool;
        mPreparedStatementPool = statement;
    }

    private static String trimSqlForDisplay(String sql) {
        return TRIM_SQL_PATTERN.matcher(sql).replaceAll(" ");
    }

    /**
     * Holder type for a prepared statement.
     * <p/>
     * Although this object holds a pointer to a native statement object, it
     * does not have a finalizer.  This is deliberate.  The {@link SQLiteConnection}
     * owns the statement object and will take care of freeing it when needed.
     * In particular, closing the connection requires a guarantee of deterministic
     * resource disposal because all native statement objects must be freed before
     * the native database object can be closed.  So no finalizers here.
     */
    /*package*/ static final class PreparedStatement {
        // Reference to owner.
        private WeakReference<SQLiteConnection> mConnection;

        // Next item in pool.
        private PreparedStatement mPoolNext;

        // The SQL from which the statement was prepared.
        private String mSql;

        // The native sqlite3_stmt object pointer.
        // Lifetime is managed explicitly by the connection.
        private long mStatementPtr;

        // The number of parameters that the prepared statement has.
        private int mNumParameters;

        // The statement type.
        private int mType;

        // True if the statement is read-only.
        private boolean mReadOnly;

        // True if the statement is in the cache.
        private boolean mInCache;

        // True if the statement is in use (currently executing).
        // We need this flag because due to the use of custom functions in triggers, it's
        // possible for SQLite calls to be re-entrant.  Consequently we need to prevent
        // in use statements from being finalized until they are no longer in use.
        private boolean mInUse;

        // Operation record.
        private Operation mOperation;

        /*package*/ PreparedStatement(SQLiteConnection connection) {
            mConnection = new WeakReference<>(connection);
        }

        public void bindArguments(Object[] bindArgs) {
            SQLiteConnection connection = mConnection.get();
            if (connection == null) return;

            connection.bindArguments(this, bindArgs);
        }

        public void reset(boolean clearBindings) {
            SQLiteConnection connection = mConnection.get();
            if (connection == null) return;

            connection.resetStatement(this, clearBindings);
        }

        public long getPtr() {
            return mStatementPtr;
        }

        public String getSQL() {
            return mSql;
        }

        public boolean isReadOnly() {
            return mReadOnly;
        }

        public int getType() {
            return mType;
        }

        public void attachCancellationSignal(CancellationSignal cancellationSignal) {
            SQLiteConnection connection = mConnection.get();
            if (connection == null) return;

            connection.attachCancellationSignal(cancellationSignal);
        }

        public void detachCancellationSignal(CancellationSignal cancellationSignal) {
            SQLiteConnection connection = mConnection.get();
            if (connection == null) return;

            connection.detachCancellationSignal(cancellationSignal);
        }

        public void beginOperation(String kind, Object[] bindArgs) {
            SQLiteConnection connection = mConnection.get();
            if (connection == null) return;

            mOperation = connection.mRecentOperations.beginOperation(kind, mSql, bindArgs);
            mOperation.mType = mType;
        }

        public void failOperation(Exception ex) {
            if (mOperation == null) return;
            SQLiteConnection connection = mConnection.get();
            if (connection == null) return;

            connection.mRecentOperations.failOperation(mOperation.mCookie, ex);
        }

        public void endOperation(String detail) {
            if (mOperation == null) return;
            SQLiteConnection connection = mConnection.get();
            if (connection == null) return;

            if (connection.mRecentOperations.endOperationDeferLog(mOperation.mCookie)) {
                connection.mRecentOperations.logOperation(mOperation.mCookie, detail);
            }
            mOperation = null;
        }
    }

    private final class PreparedStatementCache
            extends LruCache<String, PreparedStatement> {
        public PreparedStatementCache(int size) {
            super(size);
        }

        @Override
        protected void entryRemoved(boolean evicted, String key,
                PreparedStatement oldValue, PreparedStatement newValue) {
            oldValue.mInCache = false;
            if (!oldValue.mInUse) {
                finalizePreparedStatement(oldValue);
            }
        }

        public void dump(Printer printer) {
            printer.println("  Prepared statement cache:");
            Map<String, PreparedStatement> cache = snapshot();
            if (!cache.isEmpty()) {
                int i = 0;
                for (Map.Entry<String, PreparedStatement> entry : cache.entrySet()) {
                    PreparedStatement statement = entry.getValue();
                    if (statement.mInCache) { // might be false due to a race with entryRemoved
                        String sql = entry.getKey();
                        printer.println("    " + i + ": statementPtr=0x"
                                + Long.toHexString(statement.getPtr())
                                + ", numParameters=" + statement.mNumParameters
                                + ", type=" + statement.mType
                                + ", readOnly=" + statement.mReadOnly
                                + ", sql=\"" + trimSqlForDisplay(sql) + "\"");
                    }
                    i += 1;
                }
            } else {
                printer.println("    <none>");
            }
        }
    }

    private final class OperationLog {
        private static final int MAX_RECENT_OPERATIONS = 20;
        private static final int COOKIE_GENERATION_SHIFT = 8;
        private static final int COOKIE_INDEX_MASK = 0xff;

        private final Operation[] mOperations = new Operation[MAX_RECENT_OPERATIONS];
        private int mIndex;
        private int mGeneration;

        public Operation beginOperation(String kind, String sql, Object[] bindArgs) {
            synchronized (mOperations) {
                final int index = (mIndex + 1) % MAX_RECENT_OPERATIONS;
                Operation operation = mOperations[index];
                if (operation == null) {
                    operation = new Operation();
                    mOperations[index] = operation;
                } else {
                    operation.mFinished = false;
                    operation.mException = null;
                    if (operation.mBindArgs != null) {
                        operation.mBindArgs.clear();
                    }
                }
                operation.mStartTime = System.currentTimeMillis();
                operation.mKind = kind;
                operation.mSql = sql;
                if (bindArgs != null) {
                    if (operation.mBindArgs == null) {
                        operation.mBindArgs = new ArrayList<Object>();
                    } else {
                        operation.mBindArgs.clear();
                    }
                    for (int i = 0; i < bindArgs.length; i++) {
                        final Object arg = bindArgs[i];
                        if (arg != null && arg instanceof byte[]) {
                            // Don't hold onto the real byte array longer than necessary.
                            operation.mBindArgs.add(EMPTY_BYTE_ARRAY);
                        } else {
                            operation.mBindArgs.add(arg);
                        }
                    }
                }
                operation.mCookie = newOperationCookieLocked(index);
                operation.mTid = mAcquiredTid;
                mIndex = index;
                return operation;
            }
        }


        public void failOperation(int cookie, Exception ex) {
            synchronized (mOperations) {
                final Operation operation = getOperationLocked(cookie);
                if (operation != null) {
                    operation.mException = ex;
                }
            }
        }

        public void endOperation(int cookie) {
            final String sql;
            final String kind;
            final int type;
            final long elapsedTimeMillis;

            synchronized (mOperations) {
                Operation operation = getOperationLocked(cookie);
                if (endOperationDeferLogLocked(operation)) {
                    logOperationLocked(operation, null);
                }

                sql = operation.mSql;
                kind = operation.mKind;
                type = operation.mType;
                elapsedTimeMillis = operation.mEndTime - operation.mStartTime;
            }

            if (!"prepare".equals(kind))
                mPool.traceExecute(sql, type, elapsedTimeMillis);
        }

        public boolean endOperationDeferLog(int cookie) {
            final String sql;
            final String kind;
            final int type;
            final long elapsedTimeMillis;
            final boolean result;

            synchronized (mOperations) {
                Operation operation = getOperationLocked(cookie);
                if (operation == null)
                    return false;

                result = endOperationDeferLogLocked(operation);
                sql = operation.mSql;
                kind = operation.mKind;
                type = operation.mType;
                elapsedTimeMillis = operation.mEndTime - operation.mStartTime;
            }

            if (!"prepare".equals(kind))
                mPool.traceExecute(sql, type, elapsedTimeMillis);
            return result;
        }

        public void logOperation(int cookie, String detail) {
            synchronized (mOperations) {
                final Operation operation = getOperationLocked(cookie);
                if (operation != null)
                    logOperationLocked(operation, detail);
            }
        }

        private boolean endOperationDeferLogLocked(final Operation operation) {
            if (operation != null) {
                operation.mEndTime = System.currentTimeMillis();
                operation.mFinished = true;
                if (operation.mException != null && operation.mException.getMessage() != null) {
                    return true;
                }

                long elapsedTimeMillis = operation.mEndTime - operation.mStartTime;
                return SQLiteDebug.shouldLogSlowQuery(elapsedTimeMillis);
            }
            return false;
        }


        private void logOperationLocked(final Operation operation, String detail) {
            StringBuilder msg = new StringBuilder();
            operation.describe(msg, false);
            if (detail != null) {
                msg.append(", ").append(detail);
            }
            Log.i(TAG, msg.toString());
        }

        private int newOperationCookieLocked(int index) {
            final int generation = mGeneration++;
            return generation << COOKIE_GENERATION_SHIFT | index;
        }

        private Operation getOperationLocked(int cookie) {
            final int index = cookie & COOKIE_INDEX_MASK;
            final Operation operation = mOperations[index];
            return operation.mCookie == cookie ? operation : null;
        }

        public String describeCurrentOperation() {
            synchronized (mOperations) {
                final Operation operation = mOperations[mIndex];
                if (operation != null && !operation.mFinished) {
                    StringBuilder msg = new StringBuilder();
                    operation.describe(msg, false);
                    return msg.toString();
                }
                return null;
            }
        }

        SQLiteTrace.TraceInfo<String> traceCurrentOperation() {
            synchronized (mOperations) {
                final Operation operation = mOperations[mIndex];
                if (operation != null && !operation.mFinished) {
                    return new SQLiteTrace.TraceInfo<>(operation.mSql,
                            operation.mStartTime, operation.mTid);
                }
                return null;
            }
        }

        public void dump(Printer printer, boolean verbose) {
            synchronized (mOperations) {
                printer.println("  Most recently executed operations:");
                int index = mIndex;
                Operation operation = mOperations[index];
                if (operation != null) {
                    int n = 0;
                    do {
                        StringBuilder msg = new StringBuilder();
                        msg.append("    ").append(n).append(": [");
                        msg.append(operation.getFormattedStartTime());
                        msg.append("] ");
                        operation.describe(msg, verbose);
                        printer.println(msg.toString());

                        if (index > 0) {
                            index -= 1;
                        } else {
                            index = MAX_RECENT_OPERATIONS - 1;
                        }
                        n += 1;
                        operation = mOperations[index];
                    } while (operation != null && n < MAX_RECENT_OPERATIONS);
                } else {
                    printer.println("    <none>");
                }
            }
        }
    }

    @SuppressLint("SimpleDateFormat")
    private static final class Operation {
        private static final SimpleDateFormat sDateFormat =
                new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");

        long mStartTime;
        long mEndTime;
        String mKind;
        String mSql;
        ArrayList<Object> mBindArgs;
        boolean mFinished;
        Exception mException;
        int mCookie;
        int mType;
        int mTid;

        public void describe(StringBuilder msg, boolean verbose) {
            msg.append(mKind);
            if (mFinished) {
                msg.append(" took ").append(mEndTime - mStartTime).append("ms");
            } else {
                msg.append(" started ").append(System.currentTimeMillis() - mStartTime)
                        .append("ms ago");
            }
            msg.append(" - ").append(getStatus());
            if (mSql != null) {
                msg.append(", sql=\"").append(trimSqlForDisplay(mSql)).append("\"");
            }
            if (mTid > 0) {
                msg.append(", tid=").append(mTid);
            }
            if (verbose && mBindArgs != null && mBindArgs.size() != 0) {
                msg.append(", bindArgs=[");
                final int count = mBindArgs.size();
                for (int i = 0; i < count; i++) {
                    final Object arg = mBindArgs.get(i);
                    if (i != 0) {
                        msg.append(", ");
                    }
                    if (arg == null) {
                        msg.append("null");
                    } else if (arg instanceof byte[]) {
                        msg.append("<byte[]>");
                    } else if (arg instanceof String) {
                        msg.append("\"").append((String) arg).append("\"");
                    } else {
                        msg.append(arg);
                    }
                }
                msg.append("]");
            }
            if (mException != null && mException.getMessage() != null) {
                msg.append(", exception=\"").append(mException.getMessage()).append("\"");
            }
        }

        private String getStatus() {
            if (!mFinished) {
                return "running";
            }
            return mException != null ? "failed" : "succeeded";
        }

        private String getFormattedStartTime() {
            return sDateFormat.format(new Date(mStartTime));
        }
    }

}
