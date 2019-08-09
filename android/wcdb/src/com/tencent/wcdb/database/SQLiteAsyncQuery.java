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


import com.tencent.wcdb.support.CancellationSignal;
import com.tencent.wcdb.support.Log;


/**
 * Query object used by {@link SQLiteAsyncCursor}. Under development.
 *
 * @hide
 */
public class SQLiteAsyncQuery extends SQLiteProgram {

    private static final String TAG = "WCDB.SQLiteAsyncQuery";
    private final int mResultColumns;


    public SQLiteAsyncQuery(SQLiteDatabase db, String query, Object[] bindArgs,
            CancellationSignal cancellationSignal) {
        super(db, query, bindArgs, cancellationSignal);

        mResultColumns = getColumnNames().length;
    }

    void acquire() {
        if (mPreparedStatement == null) {
            acquirePreparedStatement(true);
            mPreparedStatement.bindArguments(getBindArgs());
        }
    }

    void release() {
        releasePreparedStatement();
    }

    int fillRows(ChunkedCursorWindow window, int startPos, int count) {

        // Ensure prepared statement is acquired.
        acquire();

        // Check whether number of columns has been set.
        if (window.getNumColumns() != mResultColumns) {
            window.setNumColumns(mResultColumns);
        }

        try {
            return nativeFillRows(mPreparedStatement.getPtr(), window.mWindowPtr, startPos, count);
        } catch (SQLiteException e) {
            Log.e(TAG, "Got exception on fillRows: " + e.getMessage() + ", SQL: " + getSql());
            checkCorruption(e);
            throw e;
        }
    }

    int getCount() {
        // Ensure prepared statement is acquired.
        acquire();

        try {
            return nativeCount(mPreparedStatement.getPtr());
        } catch (SQLiteException e) {
            Log.e(TAG, "Got exception on getCount: " + e.getMessage() + ", SQL: " + getSql());
            checkCorruption(e);
            throw e;
        }
    }

    void reset() {
        if (mPreparedStatement != null)
            mPreparedStatement.reset(false);
    }

    private static native int nativeFillRows(long statementPtr, long windowPtr, int startPos, int count);
    private static native int nativeCount(long statementPtr);
}
