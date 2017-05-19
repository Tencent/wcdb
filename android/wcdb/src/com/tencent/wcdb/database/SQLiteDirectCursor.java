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

import com.tencent.wcdb.AbstractCursor;
import com.tencent.wcdb.Cursor;
import com.tencent.wcdb.support.CancellationSignal;
import com.tencent.wcdb.support.Log;


public class SQLiteDirectCursor extends AbstractCursor {

    private static final String TAG = "WCDB.SQLiteDirectCursor";


    private final SQLiteDirectQuery mQuery;
    private final SQLiteCursorDriver mDriver;
    private final String[] mColumns;

    private int mCount;
    private boolean mCountFinished;


    public SQLiteDirectCursor(SQLiteCursorDriver driver, String editTable, SQLiteDirectQuery query) {
        if (query == null) {
            throw new IllegalArgumentException("query object cannot be null");
        }

        mQuery = query;
        mDriver = driver;
        mColumns = query.getColumnNames();
        mCount = -1;
        mCountFinished = false;
    }

    @Override
    public void close() {
        super.close();

        mQuery.close();
        mDriver.cursorClosed();
    }

    @Override
    public void deactivate() {
        super.deactivate();
        mDriver.cursorDeactivated();
    }

    @Override
    public String[] getColumnNames() {
        return mColumns;
    }

    @Override
    public String getString(int column) {
        return mQuery.getString(column);
    }

    @Override
    public byte[] getBlob(int column) {
        return mQuery.getBlob(column);
    }

    @Override
    public short getShort(int column) {
        return (short) mQuery.getLong(column);
    }

    @Override
    public int getInt(int column) {
        return (int) mQuery.getLong(column);
    }

    @Override
    public long getLong(int column) {
        return mQuery.getLong(column);
    }

    @Override
    public float getFloat(int column) {
        return (float) mQuery.getDouble(column);
    }

    @Override
    public double getDouble(int column) {
        return mQuery.getDouble(column);
    }

    @Override
    public int getType(int column) {
        return mQuery.getType(column);
    }

    @Override
    public boolean isNull(int column) {
        return getType(column) == FIELD_TYPE_NULL;
    }

    @Override
    public boolean moveToPosition(int position) {
        int actualPos;

        if (position < 0) {
            mQuery.reset(false);
            mPos = -1;
            return false;
        }

        if (mCountFinished && position >= mCount) {
            mPos = mCount;
            return false;
        } else if (position < mPos) {
            Log.w(TAG, "Moving backward on SQLiteDirectCursor is slow. Get rid of backward movement "
                    + "or use other implementations.");

            mQuery.reset(false);
            actualPos = mQuery.step(position + 1) - 1;
        } else if (position == mPos) {
            return true;
        } else { // position > mPos
            actualPos = mPos + mQuery.step(position - mPos);
        }

        if (actualPos < position) {
            // Returned position is smaller than requested. This is caused by reaching
            // the end of the result, so we mark counting finished.
            mCount = actualPos + 1;
            mCountFinished = true;
            mPos = mCount;
        } else {
            mPos = actualPos;
            if (actualPos >= mCount) {
                // We are not finished counting, update count accordingly.
                mCount = actualPos + 1;
                mCountFinished = false;
            }
        }

        return mPos < mCount;
    }

    @Override
    public int getCount() {
        if (!mCountFinished) {
            Log.w(TAG, "Count query on SQLiteDirectCursor is slow. Iterate through the end to get count " +
                    "or use other implementations.");

            int actualPos = mPos + mQuery.step(Integer.MAX_VALUE);
            mCount = actualPos + 1;
            mCountFinished = true;

            mQuery.reset(false);

            // Update mPos in case of data set changed during last query.
            mPos = mQuery.step(mPos + 1) - 1;
        }

        return mCount;
    }

    @Override
    public boolean requery() {
        if (isClosed())
            return false;

        synchronized (this) {
            if (!mQuery.getDatabase().isOpen())
                return false;

            mPos = -1;
            mCountFinished = false;
            mCount = -1;

            mDriver.cursorRequeried(this);
        }

        mQuery.reset(false);

        try {
            return super.requery();
        } catch (IllegalStateException e) {
            // for backwards compatibility, just return false
            Log.w(TAG, "requery() failed " + e.getMessage(), e);
            return false;
        }
    }


    public static final SQLiteDatabase.CursorFactory FACTORY = new SQLiteDatabase.CursorFactory() {
        @Override
        public Cursor newCursor(SQLiteDatabase db, SQLiteCursorDriver masterQuery,
                String editTable, SQLiteProgram query) {
            return new SQLiteDirectCursor(masterQuery, editTable, (SQLiteDirectQuery) query);
        }

        @Override
        public SQLiteProgram newQuery(SQLiteDatabase db, String query, Object[] bindArgs,
                CancellationSignal cancellationSignalForPrepare) {
            return new SQLiteDirectQuery(db, query, bindArgs, cancellationSignalForPrepare);
        }
    };
}
