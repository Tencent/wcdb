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

package com.tencent.wcdb.database;


import com.tencent.wcdb.Cursor;
import com.tencent.wcdb.support.CancellationSignal;
import com.tencent.wcdb.support.Log;


/**
 * Query class used by {@link SQLiteDirectCursor}.
 *
 * <p>This query implementation holds a database connection on the first time
 * calling {@link #step(int)}, until being closed or released.</p>
 *
 * @hide
 */
public class SQLiteDirectQuery extends SQLiteProgram {

    private static final String TAG = "WCDB.SQLiteDirectQuery";

    private final CancellationSignal mCancellationSignal;


    public SQLiteDirectQuery(SQLiteDatabase db, String query, Object[] bindArgs, CancellationSignal cancellationSignal) {
        super(db, query, bindArgs, cancellationSignal);
        mCancellationSignal = cancellationSignal;
    }

    public long getLong(int column) {
        return nativeGetLong(mPreparedStatement.getPtr(), column);
    }

    public double getDouble(int column) {
        return nativeGetDouble(mPreparedStatement.getPtr(), column);
    }

    public String getString(int column) {
        return nativeGetString(mPreparedStatement.getPtr(), column);
    }

    public byte[] getBlob(int column) {
        return nativeGetBlob(mPreparedStatement.getPtr(), column);
    }

    public int getType(int column) {
        return SQLITE_TYPE_MAPPING[nativeGetType(mPreparedStatement.getPtr(), column)];
    }

    public int step(int count) {
        try {
            if (acquirePreparedStatement()) {
                mPreparedStatement.beginOperation("directQuery", getBindArgs());
                mPreparedStatement.attachCancellationSignal(mCancellationSignal);
            }

            return nativeStep(mPreparedStatement.getPtr(), count);
        } catch (RuntimeException e) {
            if (e instanceof SQLiteDatabaseCorruptException) {
                onCorruption();
            } else if (e instanceof  SQLiteException) {
                Log.e(TAG, "Got exception on stepping: " + e.getMessage() + ", SQL: " + getSql());
            }

            // Mark operation failed and release prepared statement.
            if (mPreparedStatement != null) {
                mPreparedStatement.detachCancellationSignal(mCancellationSignal);
                mPreparedStatement.failOperation(e);
            }
            releasePreparedStatement();
            throw e;
        }
    }

    public synchronized void reset(boolean release) {
        if (mPreparedStatement != null) {
            mPreparedStatement.reset(false);

            if (release) {
                mPreparedStatement.detachCancellationSignal(mCancellationSignal);
                mPreparedStatement.endOperation(null);
                releasePreparedStatement();
            }
        }
    }

    @Override
    protected void onAllReferencesReleased() {
        synchronized (this) {
            if (mPreparedStatement != null) {
                mPreparedStatement.detachCancellationSignal(mCancellationSignal);
                mPreparedStatement.endOperation(null);
            }
        }
        super.onAllReferencesReleased();
    }

    private static final int[] SQLITE_TYPE_MAPPING = new int[] {
            Cursor.FIELD_TYPE_STRING,      // 0, INVALID VALUE, default to STRING
            Cursor.FIELD_TYPE_INTEGER,     // SQLITE_INTEGER = 1
            Cursor.FIELD_TYPE_FLOAT,       // SQLITE_FLOAT = 2
            Cursor.FIELD_TYPE_STRING,      // SQLITE_STRING = 3
            Cursor.FIELD_TYPE_BLOB,        // SQLITE_BLOB = 4
            Cursor.FIELD_TYPE_NULL         // SQLITE_NULL = 5
    };

    private static native long nativeGetLong(long statementPtr, int column);
    private static native double nativeGetDouble(long statementPtr, int column);
    private static native String nativeGetString(long statementPtr, int column);
    private static native byte[] nativeGetBlob(long statementPtr, int column);
    private static native int nativeGetType(long statementPtr, int column);
    private static native int nativeStep(long statementPtr, int count);
}
