/*
 * Copyright (C) 2006 The Android Open Source Project
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

import android.content.ContentValues;
import android.os.Looper;
import android.text.TextUtils;

import com.tencent.wcdb.support.Log;

import android.util.Pair;
import android.util.Printer;

import java.io.File;
import java.io.FileFilter;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.WeakHashMap;

import com.tencent.wcdb.Cursor;
import com.tencent.wcdb.DatabaseErrorHandler;
import com.tencent.wcdb.DatabaseUtils;
import com.tencent.wcdb.DefaultDatabaseErrorHandler;
import com.tencent.wcdb.SQLException;
import com.tencent.wcdb.database.SQLiteDebug.DbStats;
import com.tencent.wcdb.support.CancellationSignal;
import com.tencent.wcdb.support.OperationCanceledException;

/**
 * Exposes methods to manage a SQLite database.
 * <p/>
 * <p>
 * SQLiteDatabase has methods to create, delete, execute SQL commands, and
 * perform other common database management tasks.
 * </p><p>
 * See the Notepad sample application in the SDK for an example of creating
 * and managing a database.
 * </p><p>
 * Database names must be unique within an application, not across all applications.
 * </p>
 * <p/>
 * <h3>Localized Collation - ORDER BY</h3>
 * <p>
 * In addition to SQLite's default <code>BINARY</code> collator, Android supplies
 * two more, <code>LOCALIZED</code>, which changes with the system's current locale,
 * and <code>UNICODE</code>, which is the Unicode Collation Algorithm and not tailored
 * to the current locale.
 * </p>
 */
public final class SQLiteDatabase extends SQLiteClosable {
    private static final String TAG = "WCDB.SQLiteDatabase";

    static {
        System.loadLibrary("wcdb");
    }
    // Dummy static method to trigger class initialization.
    // See [JLS 12.4.1](http://docs.oracle.com/javase/specs/jls/se7/html/jls-12.html#jls-12.4.1)
    public static void loadLib() {}

    // Stores reference to all databases opened in the current process.
    // (The referent Object is not used at this time.)
    // INVARIANT: Guarded by sActiveDatabases.
    private static final WeakHashMap<SQLiteDatabase, Object> sActiveDatabases =
            new WeakHashMap<SQLiteDatabase, Object>();

    // Thread-local for database sessions that belong to this database.
    // Each thread has its own database session.
    // INVARIANT: Immutable.
    private final ThreadLocal<SQLiteSession> mThreadSession = new ThreadLocal<SQLiteSession>() {
        @Override
        protected SQLiteSession initialValue() {
            return createSession();
        }
    };

    // The optional factory to use when creating new Cursors.  May be null.
    // INVARIANT: Immutable.
    private final CursorFactory mCursorFactory;

    // Error handler to be used when SQLite returns corruption errors.
    // INVARIANT: Immutable.
    private final DatabaseErrorHandler mErrorHandler;

    // Shared database state lock.
    // This lock guards all of the shared state of the database, such as its
    // configuration, whether it is open or closed, and so on.  This lock should
    // be held for as little time as possible.
    //
    // The lock MUST NOT be held while attempting to acquire database connections or
    // while executing SQL statements on behalf of the client as it can lead to deadlock.
    //
    // It is ok to hold the lock while reconfiguring the connection pool or dumping
    // statistics because those operations are non-reentrant and do not try to acquire
    // connections that might be held by other threads.
    //
    // Basic rule: grab the lock, access or modify global state, release the lock, then
    // do the required SQL work.
    private final Object mLock = new Object();

    // The database configuration.
    // INVARIANT: Guarded by mLock.
    private final SQLiteDatabaseConfiguration mConfigurationLocked;

    // The connection pool for the database, null when closed.
    // The pool itself is thread-safe, but the reference to it can only be acquired
    // when the lock is held.
    // INVARIANT: Guarded by mLock.
    private SQLiteConnectionPool mConnectionPoolLocked;

    // True if the database has attached databases.
    // INVARIANT: Guarded by mLock.
    private boolean mHasAttachedDbsLocked;

    /**
     * When a constraint violation occurs, an immediate ROLLBACK occurs,
     * thus ending the current transaction, and the command aborts with a
     * return code of SQLITE_CONSTRAINT. If no transaction is active
     * (other than the implied transaction that is created on every command)
     * then this algorithm works the same as ABORT.
     */
    public static final int CONFLICT_ROLLBACK = 1;

    /**
     * When a constraint violation occurs,no ROLLBACK is executed
     * so changes from prior commands within the same transaction
     * are preserved. This is the default behavior.
     */
    public static final int CONFLICT_ABORT = 2;

    /**
     * When a constraint violation occurs, the command aborts with a return
     * code SQLITE_CONSTRAINT. But any changes to the database that
     * the command made prior to encountering the constraint violation
     * are preserved and are not backed out.
     */
    public static final int CONFLICT_FAIL = 3;

    /**
     * When a constraint violation occurs, the one row that contains
     * the constraint violation is not inserted or changed.
     * But the command continues executing normally. Other rows before and
     * after the row that contained the constraint violation continue to be
     * inserted or updated normally. No error is returned.
     */
    public static final int CONFLICT_IGNORE = 4;

    /**
     * When a UNIQUE constraint violation occurs, the pre-existing rows that
     * are causing the constraint violation are removed prior to inserting
     * or updating the current row. Thus the insert or update always occurs.
     * The command continues executing normally. No error is returned.
     * If a NOT NULL constraint violation occurs, the NULL value is replaced
     * by the default value for that column. If the column has no default
     * value, then the ABORT algorithm is used. If a CHECK constraint
     * violation occurs then the IGNORE algorithm is used. When this conflict
     * resolution strategy deletes rows in order to satisfy a constraint,
     * it does not invoke delete triggers on those rows.
     * This behavior might change in a future release.
     */
    public static final int CONFLICT_REPLACE = 5;

    /**
     * Use the following when no conflict action is specified.
     */
    public static final int CONFLICT_NONE = 0;

    private static final String[] CONFLICT_VALUES = new String[]
            {"", " OR ROLLBACK ", " OR ABORT ", " OR FAIL ", " OR IGNORE ", " OR REPLACE "};

    /**
     * Maximum Length Of A LIKE Or GLOB Pattern
     * The pattern matching algorithm used in the default LIKE and GLOB implementation
     * of SQLite can exhibit O(N^2) performance (where N is the number of characters in
     * the pattern) for certain pathological cases. To avoid denial-of-service attacks
     * the length of the LIKE or GLOB pattern is limited to SQLITE_MAX_LIKE_PATTERN_LENGTH bytes.
     * The default value of this limit is 50000. A modern workstation can evaluate
     * even a pathological LIKE or GLOB pattern of 50000 bytes relatively quickly.
     * The denial of service problem only comes into play when the pattern length gets
     * into millions of bytes. Nevertheless, since most useful LIKE or GLOB patterns
     * are at most a few dozen bytes in length, paranoid application developers may
     * want to reduce this parameter to something in the range of a few hundred
     * if they know that external users are able to generate arbitrary patterns.
     */
    public static final int SQLITE_MAX_LIKE_PATTERN_LENGTH = 50000;

    /**
     * Open flag: Flag for {@link #openDatabase} to open the database for reading and writing.
     * If the disk is full, this may fail even before you actually write anything.
     * <p/>
     * {@more} Note that the value of this flag is 0, so it is the default.
     */
    public static final int OPEN_READWRITE = 0x00000000;          // update native code if changing

    /**
     * Open flag: Flag for {@link #openDatabase} to open the database for reading only.
     * This is the only reliable way to open a database if the disk may be full.
     */
    public static final int OPEN_READONLY = 0x00000001;           // update native code if changing

    private static final int OPEN_READ_MASK = 0x00000001;         // update native code if changing

    /**
     * Open flag: Flag for {@link #openDatabase} to open the database without support for
     * localized collators.
     * <p/>
     * {@more} This causes the collator <code>LOCALIZED</code> not to be created.
     * You must be consistent when using this flag to use the setting the database was
     * created with.  If this is set, {@link #setLocale} will do nothing.
     */
    public static final int NO_LOCALIZED_COLLATORS = 0x00000010;  // update native code if changing

    /**
     * Open flag: Flag for {@link #openDatabase} to open the database with I/O trace enabled.
     * <p/>
     * This causes database to log I/O operations to file.
     */
    public static final int ENABLE_IO_TRACE = 0x00000100;

    /**
     * Open flag: Flag for {@link #openDatabase} that indicates no backup for database files is
     * done when corruption is detected.
     */
    public static final int NO_CORRUPTION_BACKUP = 0x00000200;

    /**
     * Open flag: Flag for {@link #openDatabase} to create the database file if it does not
     * already exist.
     */
    public static final int CREATE_IF_NECESSARY = 0x10000000;     // update native code if changing

    /**
     * Open flag: Flag for {@link #openDatabase} to open the database file with
     * write-ahead logging enabled by default.  Using this flag is more efficient
     * than calling {@link #enableWriteAheadLogging}.
     * <p/>
     * Write-ahead logging cannot be used with read-only databases so the value of
     * this flag is ignored if the database is opened read-only.
     *
     * @see #enableWriteAheadLogging
     */
    public static final int ENABLE_WRITE_AHEAD_LOGGING = 0x20000000;

    /**
     * Absolute max value that can be set by {@link #setMaxSqlCacheSize(int)}.
     * <p/>
     * Each prepared-statement is between 1K - 6K, depending on the complexity of the
     * SQL statement & schema.  A large SQL cache may use a significant amount of memory.
     */
    public static final int MAX_SQL_CACHE_SIZE = 100;

    private SQLiteDatabase(String path, int openFlags, CursorFactory cursorFactory,
            DatabaseErrorHandler errorHandler) {
        mCursorFactory = cursorFactory;
        mErrorHandler = errorHandler != null ? errorHandler :
                new DefaultDatabaseErrorHandler((openFlags & NO_CORRUPTION_BACKUP) != 0);
        mConfigurationLocked = new SQLiteDatabaseConfiguration(path, openFlags);
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            dispose(true);
        } finally {
            super.finalize();
        }
    }

    @Override
    protected void onAllReferencesReleased() {
        dispose(false);
    }

    private void dispose(boolean finalized) {
        final SQLiteConnectionPool pool;
        synchronized (mLock) {
            pool = mConnectionPoolLocked;
            mConnectionPoolLocked = null;
        }

        if (!finalized) {
            synchronized (sActiveDatabases) {
                sActiveDatabases.remove(this);
            }

            if (pool != null) {
                pool.close();
            }
        }
    }

    /**
     * Attempts to release memory that SQLite holds but does not require to
     * operate properly. Typically this memory will come from the page cache.
     *
     * @return the number of bytes actually released
     */
    public static int releaseMemory() {
        return SQLiteGlobal.releaseMemory();
    }

    /**
     * Control whether or not the SQLiteDatabase is made thread-safe by using locks
     * around critical sections. This is pretty expensive, so if you know that your
     * DB will only be used by a single thread then you should set this to false.
     * The default is true.
     *
     * @param lockingEnabled set to true to enable locks, false otherwise
     * @deprecated This method now does nothing.  Do not use.
     */
    @Deprecated
    public void setLockingEnabled(boolean lockingEnabled) {
    }

    /**
     * Gets a label to use when describing the database in log messages.
     *
     * @return The label.
     */
    String getLabel() {
        synchronized (mLock) {
            return mConfigurationLocked.label;
        }
    }

    /**
     * Sends a corruption message to the database error handler.
     */
    void onCorruption() {
        mErrorHandler.onCorruption(this);
    }

    /**
     * Gets the {@link SQLiteSession} that belongs to this thread for this database.
     * Once a thread has obtained a session, it will continue to obtain the same
     * session even after the database has been closed (although the session will not
     * be usable).  However, a thread that does not already have a session cannot
     * obtain one after the database has been closed.
     * <p/>
     * The idea is that threads that have active connections to the database may still
     * have work to complete even after the call to {@link #close}.  Active database
     * connections are not actually disposed until they are released by the threads
     * that own them.
     *
     * @return The session, never null.
     * @throws IllegalStateException if the thread does not yet have a session and
     *                               the database is not open.
     */
    SQLiteSession getThreadSession() {
        return mThreadSession.get(); // initialValue() throws if database closed
    }

    SQLiteSession createSession() {
        final SQLiteConnectionPool pool;
        synchronized (mLock) {
            throwIfNotOpenLocked();
            pool = mConnectionPoolLocked;
        }
        return new SQLiteSession(pool);
    }

    /**
     * Gets default connection flags that are appropriate for this thread, taking into
     * account whether the thread is acting on behalf of the UI.
     *
     * @param readOnly True if the connection should be read-only.
     * @return The connection flags.
     */
    int getThreadDefaultConnectionFlags(boolean readOnly) {
        int flags = readOnly ? SQLiteConnectionPool.CONNECTION_FLAG_READ_ONLY :
                SQLiteConnectionPool.CONNECTION_FLAG_PRIMARY_CONNECTION_AFFINITY;
        if (isMainThread()) {
            flags |= SQLiteConnectionPool.CONNECTION_FLAG_INTERACTIVE;
        }
        return flags;
    }

    private static boolean isMainThread() {
        // FIXME: There should be a better way to do this.
        // Would also be nice to have something that would work across Binder calls.
        Looper looper = Looper.myLooper();
        return looper != null && looper == Looper.getMainLooper();
    }

    /**
     * Begins a transaction in EXCLUSIVE mode.
     * <p>
     * Transactions can be nested.
     * When the outer transaction is ended all of
     * the work done in that transaction and all of the nested transactions will be committed or
     * rolled back. The changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they will be committed.
     * </p>
     * <p>Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransaction();
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     */
    public void beginTransaction() {
        beginTransaction(null /* transactionStatusCallback */, true);
    }

    /**
     * Begins a transaction in IMMEDIATE mode. Transactions can be nested. When
     * the outer transaction is ended all of the work done in that transaction
     * and all of the nested transactions will be committed or rolled back. The
     * changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they
     * will be committed.
     * <p/>
     * Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransactionNonExclusive();
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     */
    public void beginTransactionNonExclusive() {
        beginTransaction(null /* transactionStatusCallback */, false);
    }

    /**
     * Begins a transaction in EXCLUSIVE mode.
     * <p>
     * Transactions can be nested.
     * When the outer transaction is ended all of
     * the work done in that transaction and all of the nested transactions will be committed or
     * rolled back. The changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they will be committed.
     * </p>
     * <p>Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransactionWithListener(listener);
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     *
     * @param transactionListener listener that should be notified when the transaction begins,
     *                            commits, or is rolled back, either explicitly or by a call to
     *                            {@link #yieldIfContendedSafely}.
     */
    public void beginTransactionWithListener(SQLiteTransactionListener transactionListener) {
        beginTransaction(transactionListener, true);
    }

    /**
     * Begins a transaction in IMMEDIATE mode. Transactions can be nested. When
     * the outer transaction is ended all of the work done in that transaction
     * and all of the nested transactions will be committed or rolled back. The
     * changes will be rolled back if any transaction is ended without being
     * marked as clean (by calling setTransactionSuccessful). Otherwise they
     * will be committed.
     * <p/>
     * Here is the standard idiom for transactions:
     * <p/>
     * <pre>
     *   db.beginTransactionWithListenerNonExclusive(listener);
     *   try {
     *     ...
     *     db.setTransactionSuccessful();
     *   } finally {
     *     db.endTransaction();
     *   }
     * </pre>
     *
     * @param transactionListener listener that should be notified when the
     *                            transaction begins, commits, or is rolled back, either
     *                            explicitly or by a call to {@link #yieldIfContendedSafely}.
     */
    public void beginTransactionWithListenerNonExclusive(
            SQLiteTransactionListener transactionListener) {
        beginTransaction(transactionListener, false);
    }

    private void beginTransaction(SQLiteTransactionListener transactionListener,
            boolean exclusive) {
        acquireReference();
        try {
            getThreadSession().beginTransaction(
                    exclusive ? SQLiteSession.TRANSACTION_MODE_EXCLUSIVE :
                            SQLiteSession.TRANSACTION_MODE_IMMEDIATE,
                    transactionListener,
                    getThreadDefaultConnectionFlags(false /*readOnly*/), null);
        } finally {
            releaseReference();
        }
    }

    /**
     * End a transaction. See beginTransaction for notes about how to use this and when transactions
     * are committed and rolled back.
     */
    public void endTransaction() {
        acquireReference();
        try {
            getThreadSession().endTransaction(null);
        } finally {
            releaseReference();
        }
    }

    /**
     * Marks the current transaction as successful. Do not do any more database work between
     * calling this and calling endTransaction. Do as little non-database work as possible in that
     * situation too. If any errors are encountered between this and endTransaction the transaction
     * will still be committed.
     *
     * @throws IllegalStateException if the current thread is not in a transaction or the
     *                               transaction is already marked as successful.
     */
    public void setTransactionSuccessful() {
        acquireReference();
        try {
            getThreadSession().setTransactionSuccessful();
        } finally {
            releaseReference();
        }
    }

    /**
     * Returns true if the current thread has a transaction pending.
     *
     * @return True if the current thread is in a transaction.
     */
    public boolean inTransaction() {
        acquireReference();
        try {
            return getThreadSession().hasTransaction();
        } finally {
            releaseReference();
        }
    }

    /**
     * Returns true if the current thread is holding an active connection to the database.
     * <p>
     * The name of this method comes from a time when having an active connection
     * to the database meant that the thread was holding an actual lock on the
     * database.  Nowadays, there is no longer a true "database lock" although threads
     * may block if they cannot acquire a database connection to perform a
     * particular operation.
     * </p>
     *
     * @return True if the current thread is holding an active connection to the database.
     */
    public boolean isDbLockedByCurrentThread() {
        acquireReference();
        try {
            return getThreadSession().hasConnection();
        } finally {
            releaseReference();
        }
    }

    /**
     * Always returns false.
     * <p>
     * There is no longer the concept of a database lock, so this method always returns false.
     * </p>
     *
     * @return False.
     * @deprecated Always returns false.  Do not use this method.
     */
    @Deprecated
    public boolean isDbLockedByOtherThreads() {
        return false;
    }

    /**
     * Temporarily end the transaction to let other threads run. The transaction is assumed to be
     * successful so far. Do not call setTransactionSuccessful before calling this. When this
     * returns a new transaction will have been created but not marked as successful.
     *
     * @return true if the transaction was yielded
     * @deprecated if the db is locked more than once (becuase of nested transactions) then the lock
     * will not be yielded. Use yieldIfContendedSafely instead.
     */
    @Deprecated
    public boolean yieldIfContended() {
        return yieldIfContendedHelper(false /* do not check yielding */,
                -1 /* sleepAfterYieldDelay */);
    }

    /**
     * Temporarily end the transaction to let other threads run. The transaction is assumed to be
     * successful so far. Do not call setTransactionSuccessful before calling this. When this
     * returns a new transaction will have been created but not marked as successful. This assumes
     * that there are no nested transactions (beginTransaction has only been called once) and will
     * throw an exception if that is not the case.
     *
     * @return true if the transaction was yielded
     */
    public boolean yieldIfContendedSafely() {
        return yieldIfContendedHelper(true /* check yielding */, -1 /* sleepAfterYieldDelay*/);
    }

    /**
     * Temporarily end the transaction to let other threads run. The transaction is assumed to be
     * successful so far. Do not call setTransactionSuccessful before calling this. When this
     * returns a new transaction will have been created but not marked as successful. This assumes
     * that there are no nested transactions (beginTransaction has only been called once) and will
     * throw an exception if that is not the case.
     *
     * @param sleepAfterYieldDelay if > 0, sleep this long before starting a new transaction if
     *                             the lock was actually yielded. This will allow other background threads to make some
     *                             more progress than they would if we started the transaction immediately.
     * @return true if the transaction was yielded
     */
    public boolean yieldIfContendedSafely(long sleepAfterYieldDelay) {
        return yieldIfContendedHelper(true /* check yielding */, sleepAfterYieldDelay);
    }

    private boolean yieldIfContendedHelper(boolean throwIfUnsafe, long sleepAfterYieldDelay) {
        acquireReference();
        try {
            return getThreadSession().yieldTransaction(sleepAfterYieldDelay, throwIfUnsafe, null);
        } finally {
            releaseReference();
        }
    }

    /**
     * Deprecated.
     *
     * @deprecated This method no longer serves any useful purpose and has been deprecated.
     */
    @Deprecated
    public Map<String, String> getSyncedTables() {
        return new HashMap<String, String>(0);
    }

    /**
     * Open the database according to the flags {@link #OPEN_READWRITE}
     * {@link #OPEN_READONLY} {@link #CREATE_IF_NECESSARY} and/or {@link #NO_LOCALIZED_COLLATORS}.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     *
     * @param path    to database file to open and/or create
     * @param factory an optional factory class that is called to instantiate a
     *                cursor when query is called, or null for default
     * @param flags   to control database access mode
     * @return the newly opened database
     * @throws SQLiteException if the database cannot be opened
     */
    public static SQLiteDatabase openDatabase(String path, CursorFactory factory, int flags) {
        return openDatabase(path, factory, flags, null);
    }

    /**
     * Open the database according to the flags {@link #OPEN_READWRITE}
     * {@link #OPEN_READONLY} {@link #CREATE_IF_NECESSARY} and/or {@link #NO_LOCALIZED_COLLATORS}.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     * <p/>
     * <p>Accepts input param: a concrete instance of {@link DatabaseErrorHandler} to be
     * used to handle corruption when sqlite reports database corruption.</p>
     *
     * @param path         to database file to open and/or create
     * @param password     password to the encrypted database, or null for plain-text database
     * @param cipher       cipher description, or null for default settings
     * @param factory      an optional factory class that is called to instantiate a
     *                     cursor when query is called, or null for default
     * @param flags        to control database access mode
     * @param errorHandler the {@link DatabaseErrorHandler} obj to be used to handle corruption
     *                     when sqlite reports database corruption
     * @param poolSize     size of the connection pool to the database, 0 for default size
     * @return the newly opened database
     * @throws SQLiteException if the database cannot be opened
     */
    public static SQLiteDatabase openDatabase(String path, byte[] password, SQLiteCipherSpec cipher,
            CursorFactory factory, int flags, DatabaseErrorHandler errorHandler, int poolSize) {
        SQLiteDatabase db = new SQLiteDatabase(path, flags, factory, errorHandler);
        db.open(password, cipher, poolSize);
        return db;
    }

    /**
     * Open the database according to the flags {@link #OPEN_READWRITE}
     * {@link #OPEN_READONLY} {@link #CREATE_IF_NECESSARY} and/or {@link #NO_LOCALIZED_COLLATORS}.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     * <p/>
     * <p>Accepts input param: a concrete instance of {@link DatabaseErrorHandler} to be
     * used to handle corruption when sqlite reports database corruption.</p>
     *
     * @param path         to database file to open and/or create
     * @param password     password to the encrypted database, or null for plain-text database
     * @param cipher       cipher description, or null for default settings
     * @param factory      an optional factory class that is called to instantiate a
     *                     cursor when query is called, or null for default
     * @param flags        to control database access mode
     * @param errorHandler the {@link DatabaseErrorHandler} obj to be used to handle corruption
     *                     when sqlite reports database corruption
     * @return the newly opened database
     * @throws SQLiteException if the database cannot be opened
     */
    public static SQLiteDatabase openDatabase(String path, byte[] password, SQLiteCipherSpec cipher,
            CursorFactory factory, int flags, DatabaseErrorHandler errorHandler) {
        return openDatabase(path, password, cipher, factory, flags, errorHandler, 0);
    }

    /**
     * Open the database according to the flags {@link #OPEN_READWRITE}
     * {@link #OPEN_READONLY} {@link #CREATE_IF_NECESSARY} and/or {@link #NO_LOCALIZED_COLLATORS}.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     * <p/>
     * <p>Accepts input param: a concrete instance of {@link DatabaseErrorHandler} to be
     * used to handle corruption when sqlite reports database corruption.</p>
     *
     * @param path         to database file to open and/or create
     * @param factory      an optional factory class that is called to instantiate a
     *                     cursor when query is called, or null for default
     * @param flags        to control database access mode
     * @param errorHandler the {@link DatabaseErrorHandler} obj to be used to handle corruption
     *                     when sqlite reports database corruption
     * @return the newly opened database
     * @throws SQLiteException if the database cannot be opened
     */
    public static SQLiteDatabase openDatabase(String path, CursorFactory factory, int flags,
            DatabaseErrorHandler errorHandler) {
        return openDatabase(path, null, null, factory, flags, errorHandler, 0);
    }

    /**
     * Open the database according to the flags {@link #OPEN_READWRITE}
     * {@link #OPEN_READONLY} {@link #CREATE_IF_NECESSARY} and/or {@link #NO_LOCALIZED_COLLATORS}.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     * <p/>
     * <p>Accepts input param: a concrete instance of {@link DatabaseErrorHandler} to be
     * used to handle corruption when sqlite reports database corruption.</p>
     *
     * @param path         to database file to open and/or create
     * @param factory      an optional factory class that is called to instantiate a
     *                     cursor when query is called, or null for default
     * @param flags        to control database access mode
     * @param errorHandler the {@link DatabaseErrorHandler} obj to be used to handle corruption
     *                     when sqlite reports database corruption
     * @param poolSize     size of the connection pool to the database, 0 for default size
     * @return the newly opened database
     * @throws SQLiteException if the database cannot be opened
     */
    public static SQLiteDatabase openDatabase(String path, CursorFactory factory, int flags,
            DatabaseErrorHandler errorHandler, int poolSize) {
        return openDatabase(path, null, null, factory, flags, errorHandler, poolSize);
    }

    /**
     * Equivalent to openDatabase(file.getPath(), factory, CREATE_IF_NECESSARY).
     */
    public static SQLiteDatabase openOrCreateDatabase(File file, CursorFactory factory) {
        return openOrCreateDatabase(file.getPath(), factory);
    }

    /**
     * Equivalent to openDatabase(file.getPath(), password, cipher, factory, CREATE_IF_NECESSARY, errorHandler).
     */
    public static SQLiteDatabase openOrCreateDatabase(File file, byte[] password, SQLiteCipherSpec cipher,
            CursorFactory factory, DatabaseErrorHandler errorHandler) {
        return openOrCreateDatabase(file.getPath(), password, cipher, factory, errorHandler, 0);
    }

    /**
     * Equivalent to openDatabase(file.getPath(), password, cipher, factory, CREATE_IF_NECESSARY, errorHandler, poolSize).
     */
    public static SQLiteDatabase openOrCreateDatabase(File file, byte[] password, SQLiteCipherSpec cipher,
            CursorFactory factory, DatabaseErrorHandler errorHandler, int poolSize) {
        return openOrCreateDatabase(file.getPath(), password, cipher, factory, errorHandler, poolSize);
    }

    /**
     * Equivalent to openDatabase(file.getPath(), password, null, factory, CREATE_IF_NECESSARY, errorHandler).
     */
    public static SQLiteDatabase openOrCreateDatabase(File file, byte[] password, CursorFactory factory,
            DatabaseErrorHandler errorHandler) {
        return openOrCreateDatabase(file.getPath(), password, null, factory, errorHandler, 0);
    }

    /**
     * Equivalent to openDatabase(file.getPath(), password, null, factory, CREATE_IF_NECESSARY, errorHandler, poolSize).
     */
    public static SQLiteDatabase openOrCreateDatabase(File file, byte[] password, CursorFactory factory,
            DatabaseErrorHandler errorHandler, int poolSize) {
        return openOrCreateDatabase(file.getPath(), password, null, factory, errorHandler, poolSize);
    }

    /**
     * Equivalent to openDatabase(path, factory, CREATE_IF_NECESSARY, null, poolSize).
     */
    public static SQLiteDatabase openOrCreateDatabase(String path, CursorFactory factory, int poolSize) {
        return openDatabase(path, null, null, factory, CREATE_IF_NECESSARY, null, poolSize);
    }

    /**
     * Equivalent to openDatabase(path, factory, CREATE_IF_NECESSARY | ENABLE_WRITE_AHEAD_LOGGING).
     */
    public static SQLiteDatabase openOrCreateDatabaseInWalMode(String path, CursorFactory factory) {
        return openDatabase(path, null, null, factory, CREATE_IF_NECESSARY | ENABLE_WRITE_AHEAD_LOGGING,
                null, 0);
    }

    /**
     * Equivalent to openDatabase(path, factory, CREATE_IF_NECESSARY | ENABLE_WRITE_AHEAD_LOGGING, poolSize).
     */
    public static SQLiteDatabase openOrCreateDatabaseInWalMode(String path, CursorFactory factory, int poolSize) {
        return openDatabase(path, null, null, factory, CREATE_IF_NECESSARY | ENABLE_WRITE_AHEAD_LOGGING,
                null, poolSize);
    }

    /**
     * Equivalent to openDatabase(path, factory, CREATE_IF_NECESSARY).
     */
    public static SQLiteDatabase openOrCreateDatabase(String path, CursorFactory factory) {
        return openDatabase(path, null, null, factory, CREATE_IF_NECESSARY, null, 0);
    }

    /**
     * Equivalent to openDatabase(path, factory, CREATE_IF_NECESSARY).
     */
    public static SQLiteDatabase openOrCreateDatabase(String path, CursorFactory factory, boolean wal) {
        int flag = CREATE_IF_NECESSARY;
        if (wal) {
            flag = flag | ENABLE_WRITE_AHEAD_LOGGING;
        }
        return openDatabase(path, null, null, factory, flag, null, 0);
    }

    public static SQLiteDatabase openOrCreateDatabase(String path, byte[] password, SQLiteCipherSpec cipher,
            CursorFactory factory, DatabaseErrorHandler errorHandler, int poolSize) {
        return openDatabase(path, password, cipher, factory, CREATE_IF_NECESSARY, errorHandler, poolSize);
    }

    public static SQLiteDatabase openOrCreateDatabaseInWalMode(String path, byte[] password, SQLiteCipherSpec cipher,
            CursorFactory factory, DatabaseErrorHandler errorHandler, int poolSize) {
        return openDatabase(path, password, cipher, factory, CREATE_IF_NECESSARY | ENABLE_WRITE_AHEAD_LOGGING,
                errorHandler, poolSize);
    }

    public static SQLiteDatabase openOrCreateDatabaseInWalMode(String path, byte[] password, SQLiteCipherSpec cipher,
            CursorFactory factory) {
        return openDatabase(path, password, cipher, factory, CREATE_IF_NECESSARY | ENABLE_WRITE_AHEAD_LOGGING,
                null, 0);
    }

    public static SQLiteDatabase openOrCreateMemoryDatabaseInWalMode(CursorFactory factory) {
        return openDatabase(":memory:", null, null, factory, CREATE_IF_NECESSARY | ENABLE_WRITE_AHEAD_LOGGING,
                null, 0);
    }

    public static SQLiteDatabase openOrCreateDatabase(String path, byte[] password, CursorFactory factory,
            DatabaseErrorHandler errorHandler, int poolSize) {
        return openOrCreateDatabase(path, password, null, factory, errorHandler, poolSize);
    }


    public static SQLiteDatabase openOrCreateDatabase(String path, byte[] password, CursorFactory factory,
            DatabaseErrorHandler errorHandler) {
        return openOrCreateDatabase(path, password, null, factory, errorHandler, 0);
    }

    /**
     * Equivalent to openDatabase(path, factory, CREATE_IF_NECESSARY, errorHandler).
     */
    public static SQLiteDatabase openOrCreateDatabase(String path, CursorFactory factory,
            DatabaseErrorHandler errorHandler) {
        return openDatabase(path, factory, CREATE_IF_NECESSARY, errorHandler);
    }

    /**
     * Deletes a database including its journal file and other auxiliary files
     * that may have been created by the database engine.
     *
     * @param file The database file path.
     * @return True if the database was successfully deleted.
     */
    public static boolean deleteDatabase(File file) {
        if (file == null) {
            throw new IllegalArgumentException("file must not be null");
        }

        boolean deleted = false;
        deleted |= file.delete();
        deleted |= new File(file.getPath() + "-journal").delete();
        deleted |= new File(file.getPath() + "-shm").delete();
        deleted |= new File(file.getPath() + "-wal").delete();

        File dir = file.getParentFile();
        if (dir != null) {
            final String prefix = file.getName() + "-mj";
            final FileFilter filter = new FileFilter() {
                @Override
                public boolean accept(File candidate) {
                    return candidate.getName().startsWith(prefix);
                }
            };
            for (File masterJournal : dir.listFiles(filter)) {
                deleted |= masterJournal.delete();
            }
        }
        return deleted;
    }

    /**
     * Reopens the database in read-write mode.
     * If the database is already read-write, does nothing.
     *
     * @throws SQLiteException       if the database could not be reopened as requested, in which
     *                               case it remains open in read only mode.
     * @throws IllegalStateException if the database is not open.
     * @hide
     * @see #isReadOnly()
     */
    public void reopenReadWrite() {
        synchronized (mLock) {
            throwIfNotOpenLocked();

            if (!isReadOnlyLocked()) {
                return; // nothing to do
            }

            // Reopen the database in read-write mode.
            final int oldOpenFlags = mConfigurationLocked.openFlags;
            mConfigurationLocked.openFlags = (mConfigurationLocked.openFlags & ~OPEN_READ_MASK)
                    | OPEN_READWRITE;
            try {
                mConnectionPoolLocked.reconfigure(mConfigurationLocked);
            } catch (RuntimeException ex) {
                mConfigurationLocked.openFlags = oldOpenFlags;
                throw ex;
            }
        }
    }

    private void open(byte[] password, SQLiteCipherSpec cipher, int poolSize) {
        try {
            try {
                openInner(password, cipher, poolSize);
            } catch (SQLiteDatabaseCorruptException ex) {
                // NOTE: For encrypted databases not using HMAC (SQLCipher version 1.x),
                // wrong password is likely reported as SQLITE_NOTADB error code, thus
                // SQLiteDatabaseCorruptException. No retry is possible unless using a
                // different DatabaseErrorHandler.
                onCorruption();
                openInner(password, cipher, poolSize);
            }
        } catch (SQLiteException ex) {
            Log.e(TAG, "Failed to open database '" + getLabel() + "'.", ex);
            close();
            throw ex;
        }
    }

    private void openInner(byte[] password, SQLiteCipherSpec cipher, int poolSize) {
        synchronized (mLock) {
            assert mConnectionPoolLocked == null;
            mConnectionPoolLocked = SQLiteConnectionPool.open(this, mConfigurationLocked,
                    password, cipher, poolSize);
        }

        synchronized (sActiveDatabases) {
            sActiveDatabases.put(this, null);
        }
    }

    /**
     * Create a memory backed SQLite database.  Its contents will be destroyed
     * when the database is closed.
     * <p/>
     * <p>Sets the locale of the database to the  the system's current locale.
     * Call {@link #setLocale} if you would like something else.</p>
     *
     * @param factory an optional factory class that is called to instantiate a
     *                cursor when query is called
     * @return a SQLiteDatabase object, or null if the database can't be created
     */
    public static SQLiteDatabase create(CursorFactory factory) {
        // This is a magic string with special meaning for SQLite.
        return openDatabase(SQLiteDatabaseConfiguration.MEMORY_DB_PATH,
                factory, CREATE_IF_NECESSARY);
    }

    /**
     * Registers a CustomFunction callback as a function that can be called from
     * SQLite database triggers.
     *
     * @param name     the name of the sqlite3 function
     * @param numArgs  the number of arguments for the function
     * @param function callback to call when the function is executed
     * @hide
     */
    public void addCustomFunction(String name, int numArgs, CustomFunction function) {
        // Create wrapper (also validates arguments).
        SQLiteCustomFunction wrapper = new SQLiteCustomFunction(name, numArgs, function);

        synchronized (mLock) {
            throwIfNotOpenLocked();

            mConfigurationLocked.customFunctions.add(wrapper);
            try {
                mConnectionPoolLocked.reconfigure(mConfigurationLocked);
            } catch (RuntimeException ex) {
                mConfigurationLocked.customFunctions.remove(wrapper);
                throw ex;
            }
        }
    }

    /**
     * Gets the database version.
     *
     * @return the database version
     */
    public int getVersion() {
        return ((Long) DatabaseUtils.longForQuery(this, "PRAGMA user_version;", null)).intValue();
    }

    /**
     * Sets the database version.
     *
     * @param version the new database version
     */
    public void setVersion(int version) {
        execSQL("PRAGMA user_version = " + version);
    }

    /**
     * Returns the maximum size the database may grow to.
     *
     * @return the new maximum database size
     */
    public long getMaximumSize() {
        long pageCount = DatabaseUtils.longForQuery(this, "PRAGMA max_page_count;", null);
        return pageCount * getPageSize();
    }

    /**
     * Sets the maximum size the database will grow to. The maximum size cannot
     * be set below the current size.
     *
     * @param numBytes the maximum database size, in bytes
     * @return the new maximum database size
     */
    public long setMaximumSize(long numBytes) {
        long pageSize = getPageSize();
        long numPages = numBytes / pageSize;
        // If numBytes isn't a multiple of pageSize, bump up a page
        if ((numBytes % pageSize) != 0) {
            numPages++;
        }
        long newPageCount = DatabaseUtils.longForQuery(this, "PRAGMA max_page_count = " + numPages,
                null);
        return newPageCount * pageSize;
    }

    /**
     * Returns the current database page size, in bytes.
     *
     * @return the database page size, in bytes
     */
    public long getPageSize() {
        return DatabaseUtils.longForQuery(this, "PRAGMA page_size;", null);
    }

    /**
     * Sets the database page size. The page size must be a power of two. This
     * method does not work if any data has been written to the database file,
     * and must be called right after the database has been created.
     *
     * @param numBytes the database page size, in bytes
     */
    public void setPageSize(long numBytes) {
        execSQL("PRAGMA page_size = " + numBytes);
    }

    /**
     * Mark this table as syncable. When an update occurs in this table the
     * _sync_dirty field will be set to ensure proper syncing operation.
     *
     * @param table        the table to mark as syncable
     * @param deletedTable The deleted table that corresponds to the
     *                     syncable table
     * @deprecated This method no longer serves any useful purpose and has been deprecated.
     */
    @Deprecated
    public void markTableSyncable(String table, String deletedTable) {
    }

    /**
     * Mark this table as syncable, with the _sync_dirty residing in another
     * table. When an update occurs in this table the _sync_dirty field of the
     * row in updateTable with the _id in foreignKey will be set to
     * ensure proper syncing operation.
     *
     * @param table       an update on this table will trigger a sync time removal
     * @param foreignKey  this is the column in table whose value is an _id in
     *                    updateTable
     * @param updateTable this is the table that will have its _sync_dirty
     * @deprecated This method no longer serves any useful purpose and has been deprecated.
     */
    @Deprecated
    public void markTableSyncable(String table, String foreignKey, String updateTable) {
    }

    /**
     * Finds the name of the first table, which is editable.
     *
     * @param tables a list of tables
     * @return the first table listed
     */
    public static String findEditTable(String tables) {
        if (!TextUtils.isEmpty(tables)) {
            // find the first word terminated by either a space or a comma
            int spacepos = tables.indexOf(' ');
            int commapos = tables.indexOf(',');

            if (spacepos > 0 && (spacepos < commapos || commapos < 0)) {
                return tables.substring(0, spacepos);
            } else if (commapos > 0 && (commapos < spacepos || spacepos < 0)) {
                return tables.substring(0, commapos);
            }
            return tables;
        } else {
            throw new IllegalStateException("Invalid tables");
        }
    }

    /**
     * Compiles an SQL statement into a reusable pre-compiled statement object.
     * The parameters are identical to {@link #execSQL(String)}. You may put ?s in the
     * statement and fill in those values with {@link SQLiteProgram#bindString}
     * and {@link SQLiteProgram#bindLong} each time you want to run the
     * statement. Statements may not return result sets larger than 1x1.
     * <p/>
     * No two threads should be using the same {@link SQLiteStatement} at the same time.
     *
     * @param sql The raw SQL statement, may contain ? for unknown values to be
     *            bound later.
     * @return A pre-compiled {@link SQLiteStatement} object. Note that
     * {@link SQLiteStatement}s are not synchronized, see the documentation for more details.
     */
    public SQLiteStatement compileStatement(String sql) throws SQLException {
        acquireReference();
        try {
            return new SQLiteStatement(this, sql, null);
        } finally {
            releaseReference();
        }
    }

    /**
     * Query the given URL, returning a {@link Cursor} over the result set.
     *
     * @param distinct      true if you want each row to be unique, false otherwise.
     * @param table         The table name to compile the query against.
     * @param columns       A list of which columns to return. Passing null will
     *                      return all columns, which is discouraged to prevent reading
     *                      data from storage that isn't going to be used.
     * @param selection     A filter declaring which rows to return, formatted as an
     *                      SQL WHERE clause (excluding the WHERE itself). Passing null
     *                      will return all rows for the given table.
     * @param selectionArgs You may include ?s in selection, which will be
     *                      replaced by the values from selectionArgs, in order that they
     *                      appear in the selection. The values will be bound as Strings.
     * @param groupBy       A filter declaring how to group rows, formatted as an SQL
     *                      GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                      will cause the rows to not be grouped.
     * @param having        A filter declare which row groups to include in the cursor,
     *                      if row grouping is being used, formatted as an SQL HAVING
     *                      clause (excluding the HAVING itself). Passing null will cause
     *                      all row groups to be included, and is required when row
     *                      grouping is not being used.
     * @param orderBy       How to order the rows, formatted as an SQL ORDER BY clause
     *                      (excluding the ORDER BY itself). Passing null will use the
     *                      default sort order, which may be unordered.
     * @param limit         Limits the number of rows returned by the query,
     *                      formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(boolean distinct, String table, String[] columns, String selection, String[] selectionArgs, String groupBy, String having, String orderBy, String limit) {
        return queryWithFactory(null, distinct, table, columns, selection, selectionArgs, groupBy, having, orderBy, limit, null);
    }

    /**
     * Query the given URL, returning a {@link Cursor} over the result set.
     *
     * @param distinct           true if you want each row to be unique, false otherwise.
     * @param table              The table name to compile the query against.
     * @param columns            A list of which columns to return. Passing null will
     *                           return all columns, which is discouraged to prevent reading
     *                           data from storage that isn't going to be used.
     * @param selection          A filter declaring which rows to return, formatted as an
     *                           SQL WHERE clause (excluding the WHERE itself). Passing null
     *                           will return all rows for the given table.
     * @param selectionArgs      You may include ?s in selection, which will be
     *                           replaced by the values from selectionArgs, in order that they
     *                           appear in the selection. The values will be bound as Strings.
     * @param groupBy            A filter declaring how to group rows, formatted as an SQL
     *                           GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                           will cause the rows to not be grouped.
     * @param having             A filter declare which row groups to include in the cursor,
     *                           if row grouping is being used, formatted as an SQL HAVING
     *                           clause (excluding the HAVING itself). Passing null will cause
     *                           all row groups to be included, and is required when row
     *                           grouping is not being used.
     * @param orderBy            How to order the rows, formatted as an SQL ORDER BY clause
     *                           (excluding the ORDER BY itself). Passing null will use the
     *                           default sort order, which may be unordered.
     * @param limit              Limits the number of rows returned by the query,
     *                           formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(boolean distinct, String table, String[] columns,
            String selection, String[] selectionArgs, String groupBy,
            String having, String orderBy, String limit, CancellationSignal cancellationSignal) {
        return queryWithFactory(null, distinct, table, columns, selection, selectionArgs, groupBy, having, orderBy, limit, cancellationSignal);
    }

    /**
     * Query the given URL, returning a {@link Cursor} over the result set.
     *
     * @param cursorFactory the cursor factory to use, or null for the default factory
     * @param distinct      true if you want each row to be unique, false otherwise.
     * @param table         The table name to compile the query against.
     * @param columns       A list of which columns to return. Passing null will
     *                      return all columns, which is discouraged to prevent reading
     *                      data from storage that isn't going to be used.
     * @param selection     A filter declaring which rows to return, formatted as an
     *                      SQL WHERE clause (excluding the WHERE itself). Passing null
     *                      will return all rows for the given table.
     * @param selectionArgs You may include ?s in selection, which will be
     *                      replaced by the values from selectionArgs, in order that they
     *                      appear in the selection. The values will be bound as Strings.
     * @param groupBy       A filter declaring how to group rows, formatted as an SQL
     *                      GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                      will cause the rows to not be grouped.
     * @param having
     * @param orderBy       How to order the rows, formatteA filter declare which row groups to include in the cursor,
     *                      if row grouping is being used, formatted as an SQL HAVING
     *                      clause (excluding the HAVING itself). Passing null will cause
     *                      all row groups to be included, and is required when row
     *                      grouping is not being used.d as an SQL ORDER BY clause
     *                      (excluding the ORDER BY itself). Passing null will use the
     *                      default sort order, which may be unordered.
     * @param limit         Limits the number of rows returned by the query,
     *                      formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor queryWithFactory(CursorFactory cursorFactory,
            boolean distinct, String table, String[] columns,
            String selection, String[] selectionArgs, String groupBy,
            String having, String orderBy, String limit) {
        return queryWithFactory(cursorFactory, distinct, table, columns, selection,
                selectionArgs, groupBy, having, orderBy, limit, null);
    }

    /**
     * Query the given URL, returning a {@link Cursor} over the result set.
     *
     * @param cursorFactory      the cursor factory to use, or null for the default factory
     * @param distinct           true if you want each row to be unique, false otherwise.
     * @param table              The table name to compile the query against.
     * @param columns            A list of which columns to return. Passing null will
     *                           return all columns, which is discouraged to prevent reading
     *                           data from storage that isn't going to be used.
     * @param selection          A filter declaring which rows to return, formatted as an
     *                           SQL WHERE clause (excluding the WHERE itself). Passing null
     *                           will return all rows for the given table.
     * @param selectionArgs      You may include ?s in selection, which will be
     *                           replaced by the values from selectionArgs, in order that they
     *                           appear in the selection. The values will be bound as Strings.
     * @param groupBy            A filter declaring how to group rows, formatted as an SQL
     *                           GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                           will cause the rows to not be grouped.
     * @param having             A filter declare which row groups to include in the cursor,
     *                           if row grouping is being used, formatted as an SQL HAVING
     *                           clause (excluding the HAVING itself). Passing null will cause
     *                           all row groups to be included, and is required when row
     *                           grouping is not being used.
     * @param orderBy            How to order the rows, formatted as an SQL ORDER BY clause
     *                           (excluding the ORDER BY itself). Passing null will use the
     *                           default sort order, which may be unordered.
     * @param limit              Limits the number of rows returned by the query,
     *                           formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor queryWithFactory(CursorFactory cursorFactory,
            boolean distinct, String table, String[] columns,
            String selection, String[] selectionArgs, String groupBy,
            String having, String orderBy, String limit, CancellationSignal cancellationSignal) {
        acquireReference();
        try {
            String sql = SQLiteQueryBuilder.buildQueryString(
                    distinct, table, columns, selection, groupBy, having, orderBy, limit);

            return rawQueryWithFactory(cursorFactory, sql, selectionArgs,
                    findEditTable(table), cancellationSignal);
        } finally {
            releaseReference();
        }
    }

    /**
     * Query the given table, returning a {@link Cursor} over the result set.
     *
     * @param table         The table name to compile the query against.
     * @param columns       A list of which columns to return. Passing null will
     *                      return all columns, which is discouraged to prevent reading
     *                      data from storage that isn't going to be used.
     * @param selection     A filter declaring which rows to return, formatted as an
     *                      SQL WHERE clause (excluding the WHERE itself). Passing null
     *                      will return all rows for the given table.
     * @param selectionArgs You may include ?s in selection, which will be
     *                      replaced by the values from selectionArgs, in order that they
     *                      appear in the selection. The values will be bound as Strings.
     * @param groupBy       A filter declaring how to group rows, formatted as an SQL
     *                      GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                      will cause the rows to not be grouped.
     * @param having        A filter declare which row groups to include in the cursor,
     *                      if row grouping is being used, formatted as an SQL HAVING
     *                      clause (excluding the HAVING itself). Passing null will cause
     *                      all row groups to be included, and is required when row
     *                      grouping is not being used.
     * @param orderBy       How to order the rows, formatted as an SQL ORDER BY clause
     *                      (excluding the ORDER BY itself). Passing null will use the
     *                      default sort order, which may be unordered.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(String table, String[] columns, String selection,
            String[] selectionArgs, String groupBy, String having,
            String orderBy) {

        return query(false, table, columns, selection, selectionArgs, groupBy,
                having, orderBy, null /* limit */);
    }


    /**
     * Query the given table, returning a {@link Cursor} over the result set.
     *
     * @param table         The table name to compile the query against.
     * @param columns       A list of which columns to return. Passing null will
     *                      return all columns, which is discouraged to prevent reading
     *                      data from storage that isn't going to be used.
     * @param selection     A filter declaring which rows to return, formatted as an
     *                      SQL WHERE clause (excluding the WHERE itself). Passing null
     *                      will return all rows for the given table.
     * @param selectionArgs You may include ?s in selection, which will be
     *                      replaced by the values from selectionArgs, in order that they
     *                      appear in the selection. The values will be bound as Strings.
     * @param groupBy       A filter declaring how to group rows, formatted as an SQL
     *                      GROUP BY clause (excluding the GROUP BY itself). Passing null
     *                      will cause the rows to not be grouped.
     * @param having        A filter declare which row groups to include in the cursor,
     *                      if row grouping is being used, formatted as an SQL HAVING
     *                      clause (excluding the HAVING itself). Passing null will cause
     *                      all row groups to be included, and is required when row
     *                      grouping is not being used.
     * @param orderBy       How to order the rows, formatted as an SQL ORDER BY clause
     *                      (excluding the ORDER BY itself). Passing null will use the
     *                      default sort order, which may be unordered.
     * @param limit         Limits the number of rows returned by the query,
     *                      formatted as LIMIT clause. Passing null denotes no LIMIT clause.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     * @see Cursor
     */
    public Cursor query(String table, String[] columns, String selection,
            String[] selectionArgs, String groupBy, String having,
            String orderBy, String limit) {

        return query(false, table, columns, selection, selectionArgs, groupBy,
                having, orderBy, limit);
    }

    /**
     * Runs the provided SQL and returns a {@link Cursor} over the result set.
     *
     * @param sql           the SQL query. The SQL string must not be ; terminated
     * @param selectionArgs You may include ?s in where clause in the query,
     *                      which will be replaced by the values from selectionArgs. The
     *                      values will be bound as Strings.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     */
    public Cursor rawQuery(String sql, String[] selectionArgs) {
        return rawQueryWithFactory(null, sql, selectionArgs, null, null);
    }

    /**
     * Runs the provided SQL and returns a {@link Cursor} over the result set.
     *
     * @param sql                the SQL query. The SQL string must not be ; terminated
     * @param selectionArgs      You may include ?s in where clause in the query,
     *                           which will be replaced by the values from selectionArgs. The
     *                           values will be bound as Strings.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     */
    public Cursor rawQuery(String sql, String[] selectionArgs,
            CancellationSignal cancellationSignal) {
        return rawQueryWithFactory(null, sql, selectionArgs, null, cancellationSignal);
    }

    /**
     * Runs the provided SQL and returns a cursor over the result set.
     *
     * @param cursorFactory the cursor factory to use, or null for the default factory
     * @param sql           the SQL query. The SQL string must not be ; terminated
     * @param selectionArgs You may include ?s in where clause in the query,
     *                      which will be replaced by the values from selectionArgs. The
     *                      values will be bound as Strings.
     * @param editTable     the name of the first table, which is editable
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     */
    public Cursor rawQueryWithFactory(
            CursorFactory cursorFactory, String sql, String[] selectionArgs,
            String editTable) {
        return rawQueryWithFactory(cursorFactory, sql, selectionArgs, editTable, null);
    }

    /**
     * Runs the provided SQL and returns a cursor over the result set.
     *
     * @param cursorFactory      the cursor factory to use, or null for the default factory
     * @param sql                the SQL query. The SQL string must not be ; terminated
     * @param selectionArgs      You may include ?s in where clause in the query,
     *                           which will be replaced by the values from selectionArgs. The
     *                           values will be bound as Strings.
     * @param editTable          the name of the first table, which is editable
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *                           If the operation is canceled, then {@link OperationCanceledException} will be thrown
     *                           when the query is executed.
     * @return A {@link Cursor} object, which is positioned before the first entry. Note that
     * {@link Cursor}s are not synchronized, see the documentation for more details.
     */
    public Cursor rawQueryWithFactory(
            CursorFactory cursorFactory, String sql, String[] selectionArgs,
            String editTable, CancellationSignal cancellationSignal) {
        acquireReference();
        try {
            SQLiteCursorDriver driver = new SQLiteDirectCursorDriver(this, sql, editTable,
                    cancellationSignal);
            return driver.query(cursorFactory != null ? cursorFactory : mCursorFactory,
                    selectionArgs);
        } finally {
            releaseReference();
        }
    }

    /**
     * Convenience method for inserting a row into the database.
     *
     * @param table          the table to insert the row into
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>values</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>values</code> is empty.
     * @param values         this map contains the initial column values for the
     *                       row. The keys should be the column names and the values the
     *                       column values
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     */
    public long insert(String table, String nullColumnHack, ContentValues values) {
        try {
            return insertWithOnConflict(table, nullColumnHack, values, CONFLICT_NONE);
        } catch (SQLiteDatabaseCorruptException e) {
            throw e;
        } catch (SQLException e) {
            Log.e(TAG, "Error inserting %s: %s", values, e);
            return -1;
        }
    }

    /**
     * Convenience method for inserting a row into the database.
     *
     * @param table          the table to insert the row into
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>values</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>values</code> is empty.
     * @param values         this map contains the initial column values for the
     *                       row. The keys should be the column names and the values the
     *                       column values
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     * @throws SQLException
     */
    public long insertOrThrow(String table, String nullColumnHack, ContentValues values)
            throws SQLException {
        return insertWithOnConflict(table, nullColumnHack, values, CONFLICT_NONE);
    }

    /**
     * Convenience method for replacing a row in the database.
     *
     * @param table          the table in which to replace the row
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>initialValues</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>initialValues</code> is empty.
     * @param initialValues  this map contains the initial column values for
     *                       the row.
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     */
    public long replace(String table, String nullColumnHack, ContentValues initialValues) {
        try {
            return insertWithOnConflict(table, nullColumnHack, initialValues,
                    CONFLICT_REPLACE);
        } catch (SQLiteDatabaseCorruptException e) {
            throw e;
        } catch (SQLException e) {
            Log.e(TAG, "Error inserting " + initialValues, e);
            return -1;
        }
    }

    /**
     * Convenience method for replacing a row in the database.
     *
     * @param table          the table in which to replace the row
     * @param nullColumnHack optional; may be <code>null</code>.
     *                       SQL doesn't allow inserting a completely empty row without
     *                       naming at least one column name.  If your provided <code>initialValues</code> is
     *                       empty, no column names are known and an empty row can't be inserted.
     *                       If not set to null, the <code>nullColumnHack</code> parameter
     *                       provides the name of nullable column name to explicitly insert a NULL into
     *                       in the case where your <code>initialValues</code> is empty.
     * @param initialValues  this map contains the initial column values for
     *                       the row. The key
     * @return the row ID of the newly inserted row, or -1 if an error occurred
     * @throws SQLException
     */
    public long replaceOrThrow(String table, String nullColumnHack,
            ContentValues initialValues) throws SQLException {
        return insertWithOnConflict(table, nullColumnHack, initialValues,
                CONFLICT_REPLACE);
    }

    @SuppressWarnings("unchecked")
    private Set<String> keySet(ContentValues cv) {
        HashMap<String, Object> mValues = null;
        try {
            Class<?> cls = Class.forName("android.content.ContentValues");
            Field fd = cls.getDeclaredField("mValues");
            fd.setAccessible(true);
            mValues = (HashMap<String, Object>) fd.get(cv);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return mValues.keySet();
    }

    /**
     * General method for inserting a row into the database.
     *
     * @param table             the table to insert the row into
     * @param nullColumnHack    optional; may be <code>null</code>.
     *                          SQL doesn't allow inserting a completely empty row without
     *                          naming at least one column name.  If your provided <code>initialValues</code> is
     *                          empty, no column names are known and an empty row can't be inserted.
     *                          If not set to null, the <code>nullColumnHack</code> parameter
     *                          provides the name of nullable column name to explicitly insert a NULL into
     *                          in the case where your <code>initialValues</code> is empty.
     * @param initialValues     this map contains the initial column values for the
     *                          row. The keys should be the column names and the values the
     *                          column values
     * @param conflictAlgorithm for insert conflict resolver
     * @return the row ID of the newly inserted row
     * OR the primary key of the existing row if the input param 'conflictAlgorithm' =
     * {@link #CONFLICT_IGNORE}
     * OR -1 if any error
     */
    public long insertWithOnConflict(String table, String nullColumnHack,
            ContentValues initialValues, int conflictAlgorithm) {
        acquireReference();
        try {
            StringBuilder sql = new StringBuilder();
            sql.append("INSERT");
            sql.append(CONFLICT_VALUES[conflictAlgorithm]);
            sql.append(" INTO ");
            sql.append(table);
            sql.append('(');

            Object[] bindArgs = null;
            int size = (initialValues != null && initialValues.size() > 0)
                    ? initialValues.size() : 0;
            if (size > 0) {
                bindArgs = new Object[size];
                int i = 0;
                for (String colName : keySet(initialValues)) {
                    sql.append((i > 0) ? "," : "");
                    sql.append(colName);
                    bindArgs[i++] = initialValues.get(colName);
                }
                sql.append(')');
                sql.append(" VALUES (");
                for (i = 0; i < size; i++) {
                    sql.append((i > 0) ? ",?" : "?");
                }
            } else {
                sql.append(nullColumnHack + ") VALUES (NULL");
            }
            sql.append(')');

            SQLiteStatement statement = new SQLiteStatement(this, sql.toString(), bindArgs);
            try {
                return statement.executeInsert();
            } finally {
                statement.close();
            }
        } finally {
            releaseReference();
        }
    }

    /**
     * Convenience method for deleting rows in the database.
     *
     * @param table       the table to delete from
     * @param whereClause the optional WHERE clause to apply when deleting.
     *                    Passing null will delete all rows.
     * @param whereArgs   You may include ?s in the where clause, which
     *                    will be replaced by the values from whereArgs. The values
     *                    will be bound as Strings.
     * @return the number of rows affected if a whereClause is passed in, 0
     * otherwise. To remove all rows and get a count pass "1" as the
     * whereClause.
     */
    public int delete(String table, String whereClause, String[] whereArgs) {
        acquireReference();
        try {
            SQLiteStatement statement = new SQLiteStatement(this, "DELETE FROM " + table +
                    (!TextUtils.isEmpty(whereClause) ? " WHERE " + whereClause : ""), whereArgs);
            try {
                return statement.executeUpdateDelete();
            } finally {
                statement.close();
            }
        } finally {
            releaseReference();
        }
    }

    /**
     * Convenience method for updating rows in the database.
     *
     * @param table       the table to update in
     * @param values      a map from column names to new column values. null is a
     *                    valid value that will be translated to NULL.
     * @param whereClause the optional WHERE clause to apply when updating.
     *                    Passing null will update all rows.
     * @param whereArgs   You may include ?s in the where clause, which
     *                    will be replaced by the values from whereArgs. The values
     *                    will be bound as Strings.
     * @return the number of rows affected
     */
    public int update(String table, ContentValues values, String whereClause, String[] whereArgs) {
        return updateWithOnConflict(table, values, whereClause, whereArgs, CONFLICT_NONE);
    }

    /**
     * Convenience method for updating rows in the database.
     *
     * @param table             the table to update in
     * @param values            a map from column names to new column values. null is a
     *                          valid value that will be translated to NULL.
     * @param whereClause       the optional WHERE clause to apply when updating.
     *                          Passing null will update all rows.
     * @param whereArgs         You may include ?s in the where clause, which
     *                          will be replaced by the values from whereArgs. The values
     *                          will be bound as Strings.
     * @param conflictAlgorithm for update conflict resolver
     * @return the number of rows affected
     */
    public int updateWithOnConflict(String table, ContentValues values,
            String whereClause, String[] whereArgs, int conflictAlgorithm) {
        if (values == null || values.size() == 0) {
            throw new IllegalArgumentException("Empty values");
        }

        acquireReference();
        try {
            StringBuilder sql = new StringBuilder(120);
            sql.append("UPDATE ");
            sql.append(CONFLICT_VALUES[conflictAlgorithm]);
            sql.append(table);
            sql.append(" SET ");

            // move all bind args to one array
            int setValuesSize = values.size();
            int bindArgsSize = (whereArgs == null) ? setValuesSize : (setValuesSize + whereArgs.length);
            Object[] bindArgs = new Object[bindArgsSize];
            int i = 0;
            for (String colName : keySet(values)) {
                sql.append((i > 0) ? "," : "");
                sql.append(colName);
                bindArgs[i++] = values.get(colName);
                sql.append("=?");
            }
            if (whereArgs != null) {
                for (i = setValuesSize; i < bindArgsSize; i++) {
                    bindArgs[i] = whereArgs[i - setValuesSize];
                }
            }
            if (!TextUtils.isEmpty(whereClause)) {
                sql.append(" WHERE ");
                sql.append(whereClause);
            }

            SQLiteStatement statement = new SQLiteStatement(this, sql.toString(), bindArgs);
            try {
                return statement.executeUpdateDelete();
            } finally {
                statement.close();
            }
        } finally {
            releaseReference();
        }
    }

    /**
     * Execute a single SQL statement that is NOT a SELECT
     * or any other SQL statement that returns data.
     * <p>
     * It has no means to return any data (such as the number of affected rows).
     * Instead, you're encouraged to use {@link #insert(String, String, ContentValues)},
     * {@link #update(String, ContentValues, String, String[])}, et al, when possible.
     * </p>
     * <p>
     * When using {@link #enableWriteAheadLogging()}, journal_mode is
     * automatically managed by this class. So, do not set journal_mode
     * using "PRAGMA journal_mode'<value>" statement if your app is using
     * {@link #enableWriteAheadLogging()}
     * </p>
     *
     * @param sql the SQL statement to be executed. Multiple statements separated by semicolons are
     *            not supported.
     * @throws SQLException if the SQL string is invalid
     */
    public void execSQL(String sql) throws SQLException {
        executeSql(sql, null);
    }

    /**
     * Execute a single SQL statement that is NOT a SELECT/INSERT/UPDATE/DELETE.
     * <p>
     * For INSERT statements, use any of the following instead.
     * <ul>
     * <li>{@link #insert(String, String, ContentValues)}</li>
     * <li>{@link #insertOrThrow(String, String, ContentValues)}</li>
     * <li>{@link #insertWithOnConflict(String, String, ContentValues, int)}</li>
     * </ul>
     * <p>
     * For UPDATE statements, use any of the following instead.
     * <ul>
     * <li>{@link #update(String, ContentValues, String, String[])}</li>
     * <li>{@link #updateWithOnConflict(String, ContentValues, String, String[], int)}</li>
     * </ul>
     * <p>
     * For DELETE statements, use any of the following instead.
     * <ul>
     * <li>{@link #delete(String, String, String[])}</li>
     * </ul>
     * <p>
     * For example, the following are good candidates for using this method:
     * <ul>
     * <li>ALTER TABLE</li>
     * <li>CREATE or DROP table / trigger / view / index / virtual table</li>
     * <li>REINDEX</li>
     * <li>RELEASE</li>
     * <li>SAVEPOINT</li>
     * <li>PRAGMA that returns no data</li>
     * </ul>
     * </p>
     * <p>
     * When using {@link #enableWriteAheadLogging()}, journal_mode is
     * automatically managed by this class. So, do not set journal_mode
     * using "PRAGMA journal_mode'<value>" statement if your app is using
     * {@link #enableWriteAheadLogging()}
     * </p>
     *
     * @param sql      the SQL statement to be executed. Multiple statements separated by semicolons are
     *                 not supported.
     * @param bindArgs only byte[], String, Long and Double are supported in bindArgs.
     * @throws SQLException if the SQL string is invalid
     */
    public void execSQL(String sql, Object[] bindArgs) throws SQLException {
        if (bindArgs == null) {
            throw new IllegalArgumentException("Empty bindArgs");
        }
        executeSql(sql, bindArgs);
    }

    private int executeSql(String sql, Object[] bindArgs) throws SQLException {
        acquireReference();
        try {
            if (DatabaseUtils.getSqlStatementType(sql) == DatabaseUtils.STATEMENT_ATTACH) {
                boolean disableWal = false;
                synchronized (mLock) {
                    if (!mHasAttachedDbsLocked) {
                        mHasAttachedDbsLocked = true;
                        disableWal = true;
                    }
                }
                if (disableWal) {
                    disableWriteAheadLogging();
                }
            }

            SQLiteStatement statement = new SQLiteStatement(this, sql, bindArgs);
            try {
                return statement.executeUpdateDelete();
            } finally {
                statement.close();
            }
        } finally {
            releaseReference();
        }
    }

    /**
     * Returns true if the database is opened as read only.
     *
     * @return True if database is opened as read only.
     */
    public boolean isReadOnly() {
        synchronized (mLock) {
            return isReadOnlyLocked();
        }
    }

    private boolean isReadOnlyLocked() {
        return (mConfigurationLocked.openFlags & OPEN_READ_MASK) == OPEN_READONLY;
    }

    /**
     * Returns true if the database is in-memory db.
     *
     * @return True if the database is in-memory.
     * @hide
     */
    public boolean isInMemoryDatabase() {
        synchronized (mLock) {
            return mConfigurationLocked.isInMemoryDb();
        }
    }

    /**
     * Returns true if the database is currently open.
     *
     * @return True if the database is currently open (has not been closed).
     */
    public boolean isOpen() {
        synchronized (mLock) {
            return mConnectionPoolLocked != null;
        }
    }

    /**
     * Returns true if the new version code is greater than the current database version.
     *
     * @param newVersion The new version code.
     * @return True if the new version code is greater than the current database version.
     */
    public boolean needUpgrade(int newVersion) {
        return newVersion > getVersion();
    }

    /**
     * Gets the path to the database file.
     *
     * @return The path to the database file.
     */
    public final String getPath() {
        synchronized (mLock) {
            return mConfigurationLocked.path;
        }
    }

    /**
     * Sets the locale for this database.  Does nothing if this database has
     * the {@link #NO_LOCALIZED_COLLATORS} flag set or was opened read only.
     *
     * @param locale The new locale.
     * @throws SQLException if the locale could not be set.  The most common reason
     *                      for this is that there is no collator available for the locale you requested.
     *                      In this case the database remains unchanged.
     */
    public void setLocale(Locale locale) {
        if (locale == null) {
            throw new IllegalArgumentException("locale must not be null.");
        }

        synchronized (mLock) {
            throwIfNotOpenLocked();

            final Locale oldLocale = mConfigurationLocked.locale;
            mConfigurationLocked.locale = locale;
            try {
                mConnectionPoolLocked.reconfigure(mConfigurationLocked);
            } catch (RuntimeException ex) {
                mConfigurationLocked.locale = oldLocale;
                throw ex;
            }
        }
    }

    /**
     * Sets the maximum size of the prepared-statement cache for this database.
     * (size of the cache = number of compiled-sql-statements stored in the cache).
     * <p/>
     * Maximum cache size can ONLY be increased from its current size (default = 10).
     * If this method is called with smaller size than the current maximum value,
     * then IllegalStateException is thrown.
     * <p/>
     * This method is thread-safe.
     *
     * @param cacheSize the size of the cache. can be (0 to {@link #MAX_SQL_CACHE_SIZE})
     * @throws IllegalStateException if input cacheSize > {@link #MAX_SQL_CACHE_SIZE}.
     */
    public void setMaxSqlCacheSize(int cacheSize) {
        if (cacheSize > MAX_SQL_CACHE_SIZE || cacheSize < 0) {
            throw new IllegalStateException(
                    "expected value between 0 and " + MAX_SQL_CACHE_SIZE);
        }

        synchronized (mLock) {
            throwIfNotOpenLocked();

            final int oldMaxSqlCacheSize = mConfigurationLocked.maxSqlCacheSize;
            mConfigurationLocked.maxSqlCacheSize = cacheSize;
            try {
                mConnectionPoolLocked.reconfigure(mConfigurationLocked);
            } catch (RuntimeException ex) {
                mConfigurationLocked.maxSqlCacheSize = oldMaxSqlCacheSize;
                throw ex;
            }
        }
    }

    /**
     * Sets whether foreign key constraints are enabled for the database.
     * <p>
     * By default, foreign key constraints are not enforced by the database.
     * This method allows an application to enable foreign key constraints.
     * It must be called each time the database is opened to ensure that foreign
     * key constraints are enabled for the session.
     * </p><p>
     * A good time to call this method is right after calling {@link #openOrCreateDatabase}
     * or in the {@link SQLiteOpenHelper#onConfigure} callback.
     * </p><p>
     * When foreign key constraints are disabled, the database does not check whether
     * changes to the database will violate foreign key constraints.  Likewise, when
     * foreign key constraints are disabled, the database will not execute cascade
     * delete or update triggers.  As a result, it is possible for the database
     * state to become inconsistent.  To perform a database integrity check,
     * call {@link #isDatabaseIntegrityOk}.
     * </p><p>
     * This method must not be called while a transaction is in progress.
     * </p><p>
     * See also <a href="http://sqlite.org/foreignkeys.html">SQLite Foreign Key Constraints</a>
     * for more details about foreign key constraint support.
     * </p>
     *
     * @param enable True to enable foreign key constraints, false to disable them.
     * @throws IllegalStateException if the are transactions is in progress
     *                               when this method is called.
     */
    public void setForeignKeyConstraintsEnabled(boolean enable) {
        synchronized (mLock) {
            throwIfNotOpenLocked();

            if (mConfigurationLocked.foreignKeyConstraintsEnabled == enable) {
                return;
            }

            mConfigurationLocked.foreignKeyConstraintsEnabled = enable;
            try {
                mConnectionPoolLocked.reconfigure(mConfigurationLocked);
            } catch (RuntimeException ex) {
                mConfigurationLocked.foreignKeyConstraintsEnabled = !enable;
                throw ex;
            }
        }
    }

    /**
     * This method enables parallel execution of queries from multiple threads on the
     * same database.  It does this by opening multiple connections to the database
     * and using a different database connection for each query.  The database
     * journal mode is also changed to enable writes to proceed concurrently with reads.
     * <p>
     * When write-ahead logging is not enabled (the default), it is not possible for
     * reads and writes to occur on the database at the same time.  Before modifying the
     * database, the writer implicitly acquires an exclusive lock on the database which
     * prevents readers from accessing the database until the write is completed.
     * </p><p>
     * In contrast, when write-ahead logging is enabled (by calling this method), write
     * operations occur in a separate log file which allows reads to proceed concurrently.
     * While a write is in progress, readers on other threads will perceive the state
     * of the database as it was before the write began.  When the write completes, readers
     * on other threads will then perceive the new state of the database.
     * </p><p>
     * It is a good idea to enable write-ahead logging whenever a database will be
     * concurrently accessed and modified by multiple threads at the same time.
     * However, write-ahead logging uses significantly more memory than ordinary
     * journaling because there are multiple connections to the same database.
     * So if a database will only be used by a single thread, or if optimizing
     * concurrency is not very important, then write-ahead logging should be disabled.
     * </p><p>
     * After calling this method, execution of queries in parallel is enabled as long as
     * the database remains open.  To disable execution of queries in parallel, either
     * call {@link #disableWriteAheadLogging} or close the database and reopen it.
     * </p><p>
     * The maximum number of connections used to execute queries in parallel is
     * dependent upon the device memory and possibly other properties.
     * </p><p>
     * If a query is part of a transaction, then it is executed on the same database handle the
     * transaction was begun.
     * </p><p>
     * Writers should use {@link #beginTransactionNonExclusive()} or
     * {@link #beginTransactionWithListenerNonExclusive(SQLiteTransactionListener)}
     * to start a transaction.  Non-exclusive mode allows database file to be in readable
     * by other threads executing queries.
     * </p><p>
     * If the database has any attached databases, then execution of queries in parallel is NOT
     * possible.  Likewise, write-ahead logging is not supported for read-only databases
     * or memory databases.  In such cases, {@link #enableWriteAheadLogging} returns false.
     * </p><p>
     * The best way to enable write-ahead logging is to pass the
     * {@link #ENABLE_WRITE_AHEAD_LOGGING} flag to {@link #openDatabase}.  This is
     * more efficient than calling {@link #enableWriteAheadLogging}.
     * <code><pre>
     *     SQLiteDatabase db = SQLiteDatabase.openDatabase("db_filename", cursorFactory,
     *             SQLiteDatabase.CREATE_IF_NECESSARY | SQLiteDatabase.ENABLE_WRITE_AHEAD_LOGGING,
     *             myDatabaseErrorHandler);
     *     db.enableWriteAheadLogging();
     * </pre></code>
     * </p><p>
     * Another way to enable write-ahead logging is to call {@link #enableWriteAheadLogging}
     * after opening the database.
     * <code><pre>
     *     SQLiteDatabase db = SQLiteDatabase.openDatabase("db_filename", cursorFactory,
     *             SQLiteDatabase.CREATE_IF_NECESSARY, myDatabaseErrorHandler);
     *     db.enableWriteAheadLogging();
     * </pre></code>
     * </p><p>
     * See also <a href="http://sqlite.org/wal.html">SQLite Write-Ahead Logging</a> for
     * more details about how write-ahead logging works.
     * </p>
     *
     * @return True if write-ahead logging is enabled.
     * @throws IllegalStateException if there are transactions in progress at the
     *                               time this method is called.  WAL mode can only be changed when there are no
     *                               transactions in progress.
     * @see #ENABLE_WRITE_AHEAD_LOGGING
     * @see #disableWriteAheadLogging
     */
    public boolean enableWriteAheadLogging() {
        synchronized (mLock) {
            throwIfNotOpenLocked();

            if ((mConfigurationLocked.openFlags & ENABLE_WRITE_AHEAD_LOGGING) != 0) {
                return true;
            }

            if (isReadOnlyLocked()) {
                // WAL doesn't make sense for readonly-databases.
                // TODO: True, but connection pooling does still make sense...
                return false;
            }

            if (mConfigurationLocked.isInMemoryDb()) {
                Log.i(TAG, "can't enable WAL for memory databases.");
                return false;
            }

            // make sure this database has NO attached databases because sqlite's write-ahead-logging
            // doesn't work for databases with attached databases
            if (mHasAttachedDbsLocked) {
//                if (Log.isLoggable(TAG, Log.DEBUG)) {
                Log.i(TAG, "this database: " + mConfigurationLocked.label
                        + " has attached databases. can't  enable WAL.");
//                }
                return false;
            }

            mConfigurationLocked.openFlags |= ENABLE_WRITE_AHEAD_LOGGING;
            try {
                mConnectionPoolLocked.reconfigure(mConfigurationLocked);
            } catch (RuntimeException ex) {
                mConfigurationLocked.openFlags &= ~ENABLE_WRITE_AHEAD_LOGGING;
                throw ex;
            }
        }
        return true;
    }

    /**
     * This method disables the features enabled by {@link #enableWriteAheadLogging()}.
     *
     * @throws IllegalStateException if there are transactions in progress at the
     *                               time this method is called.  WAL mode can only be changed when there are no
     *                               transactions in progress.
     * @see #enableWriteAheadLogging
     */
    public void disableWriteAheadLogging() {
        synchronized (mLock) {
            throwIfNotOpenLocked();

            if ((mConfigurationLocked.openFlags & ENABLE_WRITE_AHEAD_LOGGING) == 0) {
                return;
            }

            mConfigurationLocked.openFlags &= ~ENABLE_WRITE_AHEAD_LOGGING;
            try {
                mConnectionPoolLocked.reconfigure(mConfigurationLocked);
            } catch (RuntimeException ex) {
                mConfigurationLocked.openFlags |= ENABLE_WRITE_AHEAD_LOGGING;
                throw ex;
            }
        }
    }

    /**
     * Returns true if write-ahead logging has been enabled for this database.
     *
     * @return True if write-ahead logging has been enabled for this database.
     * @see #enableWriteAheadLogging
     * @see #ENABLE_WRITE_AHEAD_LOGGING
     */
    public boolean isWriteAheadLoggingEnabled() {
        synchronized (mLock) {
            throwIfNotOpenLocked();

            return (mConfigurationLocked.openFlags & ENABLE_WRITE_AHEAD_LOGGING) != 0;
        }
    }

    public SQLiteTrace getTraceCallback() {
        synchronized (mLock) {
            throwIfNotOpenLocked();
            return mConnectionPoolLocked.getTraceCallback();
        }
    }
    public void setTraceCallback(SQLiteTrace callback) {
        synchronized (mLock) {
            throwIfNotOpenLocked();
            mConnectionPoolLocked.setTraceCallback(callback);
        }
    }

    /**
     * Collect statistics about all open databases in the current process.
     * Used by bug report.
     */
    static ArrayList<DbStats> getDbStats() {
        ArrayList<DbStats> dbStatsList = new ArrayList<DbStats>();
        for (SQLiteDatabase db : getActiveDatabases()) {
            db.collectDbStats(dbStatsList);
        }
        return dbStatsList;
    }

    private void collectDbStats(ArrayList<DbStats> dbStatsList) {
        synchronized (mLock) {
            if (mConnectionPoolLocked != null) {
                mConnectionPoolLocked.collectDbStats(dbStatsList);
            }
        }
    }

    private static ArrayList<SQLiteDatabase> getActiveDatabases() {
        ArrayList<SQLiteDatabase> databases = new ArrayList<SQLiteDatabase>();
        synchronized (sActiveDatabases) {
            databases.addAll(sActiveDatabases.keySet());
        }
        return databases;
    }

    /**
     * Dump detailed information about all open databases in the current process.
     * Used by bug report.
     */
    public static void dumpAll(Printer printer, boolean verbose) {
        for (SQLiteDatabase db : getActiveDatabases()) {
            db.dump(printer, verbose);
        }
    }

    public void dump(Printer printer, boolean verbose) {
        synchronized (mLock) {
            if (mConnectionPoolLocked != null) {
                mConnectionPoolLocked.dump(printer, verbose);
            }
        }
    }

    /**
     * Returns list of full pathnames of all attached databases including the main database
     * by executing 'pragma database_list' on the database.
     *
     * @return ArrayList of pairs of (database name, database file path) or null if the database
     * is not open.
     */
    public List<Pair<String, String>> getAttachedDbs() {
        ArrayList<Pair<String, String>> attachedDbs = new ArrayList<Pair<String, String>>();
        synchronized (mLock) {
            if (mConnectionPoolLocked == null) {
                return null; // not open
            }

            if (!mHasAttachedDbsLocked) {
                // No attached databases.
                // There is a small window where attached databases exist but this flag is not
                // set yet.  This can occur when this thread is in a race condition with another
                // thread that is executing the SQL statement: "attach database <blah> as <foo>"
                // If this thread is NOT ok with such a race condition (and thus possibly not
                // receivethe entire list of attached databases), then the caller should ensure
                // that no thread is executing any SQL statements while a thread is calling this
                // method.  Typically, this method is called when 'adb bugreport' is done or the
                // caller wants to collect stats on the database and all its attached databases.
                attachedDbs.add(new Pair<String, String>("main", mConfigurationLocked.path));
                return attachedDbs;
            }

            acquireReference();
        }

        try {
            // has attached databases. query sqlite to get the list of attached databases.
            Cursor c = null;
            try {
                c = rawQuery("pragma database_list;", null);
                while (c.moveToNext()) {
                    // sqlite returns a row for each database in the returned list of databases.
                    //   in each row,
                    //       1st column is the database name such as main, or the database
                    //                              name specified on the "ATTACH" command
                    //       2nd column is the database file path.
                    attachedDbs.add(new Pair<String, String>(c.getString(1), c.getString(2)));
                }
            } finally {
                if (c != null) {
                    c.close();
                }
            }
            return attachedDbs;
        } finally {
            releaseReference();
        }
    }

    /**
     * Runs 'pragma integrity_check' on the given database (and all the attached databases)
     * and returns true if the given database (and all its attached databases) pass integrity_check,
     * false otherwise.
     * <p/>
     * If the result is false, then this method logs the errors reported by the integrity_check
     * command execution.
     * <p/>
     * Note that 'pragma integrity_check' on a database can take a long time.
     *
     * @return true if the given database (and all its attached databases) pass integrity_check,
     * false otherwise.
     */
    public boolean isDatabaseIntegrityOk() {
        acquireReference();
        try {
            List<Pair<String, String>> attachedDbs = null;
            try {
                attachedDbs = getAttachedDbs();
                if (attachedDbs == null) {
                    throw new IllegalStateException("databaselist for: " + getPath() + " couldn't " +
                            "be retrieved. probably because the database is closed");
                }
            } catch (SQLiteException e) {
                // can't get attachedDb list. do integrity check on the main database
                attachedDbs = new ArrayList<Pair<String, String>>();
                attachedDbs.add(new Pair<String, String>("main", getPath()));
            }

            for (int i = 0; i < attachedDbs.size(); i++) {
                Pair<String, String> p = attachedDbs.get(i);
                SQLiteStatement prog = null;
                try {
                    prog = compileStatement("PRAGMA " + p.first + ".integrity_check(1);");
                    String rslt = prog.simpleQueryForString();
                    if (!rslt.equalsIgnoreCase("ok")) {
                        // integrity_checker failed on main or attached databases
                        Log.e(TAG, "PRAGMA integrity_check on " + p.second + " returned: " + rslt);
                        return false;
                    }
                } finally {
                    if (prog != null) prog.close();
                }
            }
        } finally {
            releaseReference();
        }
        return true;
    }

    /**
     * Acquire database connection from connection pool and return its native handle
     * (a.k.a {@code sqlite3 *}).
     *
     * <p>The caller must release the returned handle when finished with it by calling
     * {@link #releaseNativeConnection(long, Exception)} <strong>within the same thread</strong>.
     * Failed to release the returned handle will cause connection leakage and probably
     * ANR.</p>
     *
     * @param operation     string describing usage of the connection, for logging
     * @param readOnly      whether operations on returned connection is read-only
     * @param interactive   whether operations on returned connection is for interactive
     * @return {@code sqlite3 *} handle casted to {@code long}
     */
    public long acquireNativeConnectionHandle(String operation, boolean readOnly, boolean interactive) {
        if (operation == null)
            operation = "unnamedNative";

        int connectionFlags;
        if (readOnly) connectionFlags = SQLiteConnectionPool.CONNECTION_FLAG_READ_ONLY;
        else connectionFlags = SQLiteConnectionPool.CONNECTION_FLAG_PRIMARY_CONNECTION_AFFINITY;
        if (interactive) connectionFlags |= SQLiteConnectionPool.CONNECTION_FLAG_INTERACTIVE;

        return getThreadSession().acquireConnectionForNativeHandle(connectionFlags)
                .getNativeHandle(operation);
    }

    /**
     * Release connection returned by {@link #acquireNativeConnectionHandle(String, boolean, boolean)}.
     *
     * @param nativePtr {@code sqlite3 *} handle to release
     * @param ex        exception occurred during operations on the native handle,
     *                  or null for successful operations
     */
    public void releaseNativeConnection(long nativePtr, Exception ex) {
        getThreadSession().releaseConnectionForNativeHandle(ex);
    }


    @Override
    public String toString() {
        return "SQLiteDatabase: " + getPath();
    }

    private void throwIfNotOpenLocked() {
        if (mConnectionPoolLocked == null) {
            throw new IllegalStateException("The database '" + mConfigurationLocked.label
                    + "' is not open.");
        }
    }

    /**
     * Used to allow returning sub-classes of {@link Cursor} when calling query.
     */
    public interface CursorFactory {
        /**
         * See {@link SQLiteCursor#SQLiteCursor(SQLiteCursorDriver, String, SQLiteQuery)}.
         */
        Cursor newCursor(SQLiteDatabase db,
                SQLiteCursorDriver masterQuery, String editTable,
                SQLiteProgram query);

        SQLiteProgram newQuery(SQLiteDatabase db, String query,
                Object[] bindArgs, CancellationSignal cancellationSignalForPrepare);
    }

    /**
     * A callback interface for a custom sqlite3 function.
     * This can be used to create a function that can be called from
     * sqlite3 database triggers.
     *
     * @hide
     */
    public interface CustomFunction {
        public void callback(String[] args);
    }
}
