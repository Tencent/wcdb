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

import java.util.Arrays;

import com.tencent.wcdb.DatabaseUtils;
import com.tencent.wcdb.support.CancellationSignal;

/**
 * A base class for compiled SQLite programs.
 * <p>
 * This class is not thread-safe.
 * </p>
 */
public abstract class SQLiteProgram extends SQLiteClosable {
    private static final String TAG = "WCDB.SQLiteProgram";
    private static final String[] EMPTY_STRING_ARRAY = new String[0];

    private final SQLiteDatabase mDatabase;
    private final String mSql;
    private final boolean mReadOnly;
    private final String[] mColumnNames;
    private final int mNumParameters;
    private final Object[] mBindArgs;

    protected SQLiteConnection.PreparedStatement mPreparedStatement;
    private SQLiteSession mBoundSession;


    protected SQLiteProgram(SQLiteDatabase db, String sql, Object[] bindArgs,
            CancellationSignal cancellationSignalForPrepare) {
        mDatabase = db;
        mSql = sql.trim();

        int n = DatabaseUtils.getSqlStatementType(mSql);
        switch (n) {
            case DatabaseUtils.STATEMENT_BEGIN:
            case DatabaseUtils.STATEMENT_COMMIT:
            case DatabaseUtils.STATEMENT_ABORT:
                mReadOnly = false;
                mColumnNames = EMPTY_STRING_ARRAY;
                mNumParameters = 0;
                break;

            default:
                boolean assumeReadOnly = (n == DatabaseUtils.STATEMENT_SELECT);
                SQLiteStatementInfo info = new SQLiteStatementInfo();
                db.getThreadSession().prepare(mSql,
                        db.getThreadDefaultConnectionFlags(assumeReadOnly),
                        cancellationSignalForPrepare, info);

                // Always treat CREATE/DROP/ALTER statements as read-write.
                // This fixes read-only exceptions while running CREATE xxx IF NOT EXISTS statements
                // because we assume these statements to be read-write and prepare them using the
                // primary connection but the results are read-only and run on non-primary connections.
                // The non-primary connections may be using outdated cache and treat the statement as
                // read-write.
                mReadOnly = (n != DatabaseUtils.STATEMENT_DDL) && info.readOnly;
                mColumnNames = info.columnNames;
                mNumParameters = info.numParameters;
                break;
        }

        if (bindArgs != null && bindArgs.length > mNumParameters) {
            throw new IllegalArgumentException("Too many bind arguments.  "
                    + bindArgs.length + " arguments were provided but the statement needs "
                    + mNumParameters + " arguments.");
        }

        if (mNumParameters != 0) {
            mBindArgs = new Object[mNumParameters];
            if (bindArgs != null) {
                System.arraycopy(bindArgs, 0, mBindArgs, 0, bindArgs.length);
            }
        } else {
            mBindArgs = null;
        }

        mPreparedStatement = null;
        mBoundSession = null;
    }

    public final SQLiteDatabase getDatabase() {
        return mDatabase;
    }

    protected final String getSql() {
        return mSql;
    }

    protected final Object[] getBindArgs() {
        return mBindArgs;
    }

    public final String[] getColumnNames() {
        return mColumnNames;
    }

    /** @hide */
    protected final SQLiteSession getSession() {
        return mDatabase.getThreadSession();
    }

    /** @hide */
    protected final int getConnectionFlags() {
        return mDatabase.getThreadDefaultConnectionFlags(mReadOnly);
    }

    /** @hide */
    protected final void checkCorruption(SQLiteException e) {
        boolean isCorruption = false;
        if (e instanceof SQLiteDatabaseCorruptException) {
            isCorruption = true;
        } else if (e instanceof SQLiteFullException) {
            // When SQLite executing OP_Column opcode during SELECT or UPDATE statements,
            // it may return SQLITE_FULL when trying to access unreachable pages.
            // It's considered corruption on this scenario. We cannot distinguish such
            // situation here, so we treat all SQLiteFullException thrown by a SELECT
            // statement a signal on corruption.
            if (mReadOnly) {
                isCorruption = true;
            }
        }

        if (isCorruption) {
            SQLiteDebug.collectLastIOTraceStats(mDatabase);
            mDatabase.onCorruption();
        }
    }

    /**
     * Unimplemented.
     * @deprecated This method is deprecated and must not be used.
     */
    @Deprecated
    public final int getUniqueId() {
        return -1;
    }

    /**
     * Bind a NULL value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind null to
     */
    public void bindNull(int index) {
        bind(index, null);
    }

    /**
     * Bind a long value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *addToBindArgs
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind
     */
    public void bindLong(int index, long value) {
        bind(index, value);
    }

    /**
     * Bind a double value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind
     */
    public void bindDouble(int index, double value) {
        bind(index, value);
    }

    /**
     * Bind a String value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind, must not be null
     */
    public void bindString(int index, String value) {
        if (value == null) {
            throw new IllegalArgumentException("the bind value at index " + index + " is null");
        }
        bind(index, value);
    }

    /**
     * Bind a byte array value to this statement. The value remains bound until
     * {@link #clearBindings} is called.
     *
     * @param index The 1-based index to the parameter to bind
     * @param value The value to bind, must not be null
     */
    public void bindBlob(int index, byte[] value) {
        if (value == null) {
            throw new IllegalArgumentException("the bind value at index " + index + " is null");
        }
        bind(index, value);
    }

    /**
     * Clears all existing bindings. Unset bindings are treated as NULL.
     */
    public void clearBindings() {
        if (mBindArgs != null) {
            Arrays.fill(mBindArgs, null);
        }
    }

    /**
     * Given an array of String bindArgs, this method binds all of them in one single call.
     *
     * @param bindArgs the String array of bind args, none of which must be null.
     */
    public void bindAllArgsAsStrings(String[] bindArgs) {
        if (bindArgs != null) {
            for (int i = bindArgs.length; i != 0; i--) {
                bindString(i, bindArgs[i - 1]);
            }
        }
    }

    @Override
    protected void onAllReferencesReleased() {
        releasePreparedStatement();
        clearBindings();
    }

    private void bind(int index, Object value) {
        if (index < 1 || index > mNumParameters) {
            throw new IllegalArgumentException("Cannot bind argument at index "
                    + index + " because the index is out of range.  "
                    + "The statement has " + mNumParameters + " parameters.");
        }
        mBindArgs[index - 1] = value;
    }

    protected synchronized boolean acquirePreparedStatement(boolean persist) {
        SQLiteSession session = mDatabase.getThreadSession();
        if (session == mBoundSession)
            return false;

        if (mBoundSession != null) {
            throw new IllegalStateException("SQLiteProgram has bound to another thread.");
        }

        mPreparedStatement = session.acquirePreparedStatement(mSql,
                mDatabase.getThreadDefaultConnectionFlags(mReadOnly), persist);
        mPreparedStatement.bindArguments(mBindArgs);
        mBoundSession = session;
        return true;
    }

    protected synchronized void releasePreparedStatement() {
        if (mBoundSession == null && mPreparedStatement == null)
            return;

        if (mBoundSession == null || mPreparedStatement == null) {
            throw new IllegalStateException("Internal state error.");
        }

        if (mBoundSession != mDatabase.getThreadSession()) {
            throw new IllegalStateException("SQLiteProgram has bound to another thread.");
        }

        mBoundSession.releasePreparedStatement(mPreparedStatement);
        mPreparedStatement = null;
        mBoundSession = null;
    }

    @Override
    protected void finalize() throws Throwable {
        synchronized (this) {
            if (mBoundSession != null || mPreparedStatement != null)
                throw new SQLiteMisuseException("Acquired prepared statement is not released.");
        }

        super.finalize();
    }
}
