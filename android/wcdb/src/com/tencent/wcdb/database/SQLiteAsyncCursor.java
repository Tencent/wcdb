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
import com.tencent.wcdb.CursorIndexOutOfBoundsException;
import com.tencent.wcdb.StaleDataException;
import com.tencent.wcdb.support.CancellationSignal;


public class SQLiteAsyncCursor extends AbstractCursor {

    private static final String TAG = "WCDB.SQLiteAsyncCursor";

    private static final int MIN_FETCH_ROWS = 32;
    private static final int MAX_PREFETCH = 256;
    private static final int MAX_KEEP_CHUNKS = 32;


    private final SQLiteAsyncQuery mQuery;
    private final SQLiteCursorDriver mDriver;
    private final String[] mColumns;

    private ChunkedCursorWindow mWindow;
    private long mCurrentRow;
    private volatile int mCount;

    private final Object mWaitLock;

    private QueryThread mQueryThread;


    public SQLiteAsyncCursor(SQLiteCursorDriver driver, String editTable, SQLiteAsyncQuery query) {
        if (query == null) {
            throw new IllegalArgumentException("query object cannot be null");
        }

        mQuery = query;
        mDriver = driver;
        mColumns = query.getColumnNames();

        mCount = -1;

        mWaitLock = new Object();
        mWindow = new ChunkedCursorWindow(16 * 1024 * 1024);
        mQueryThread = new QueryThread();
        mQueryThread.start();
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
    protected void onDeactivateOrClose() {
        if (mCurrentRow != 0) {
            mWindow.endRowUnsafe(mCurrentRow);
            mCurrentRow = 0;
        }

        if (mQueryThread != null) {
            mQueryThread.quit();
            try {
                mQueryThread.join();
            } catch (InterruptedException e) {
                // ignored
            }
            mQueryThread = null;
        }

        if (mWindow != null) {
            mWindow.close();
            mWindow = null;
        }

        mCount = -1;
        mPos = -1;
        super.onDeactivateOrClose();
    }

    @Override
    public String[] getColumnNames() {
        return mColumns;
    }

    @Override
    public int getCount() {
        if (mCount >= 0) return mCount;
        if (mWindow == null) return -1;

        try {
            synchronized (mWaitLock) {
                while (mCount < 0)
                    mWaitLock.wait();
            }
        } catch (InterruptedException e) {
            // do nothing
        }
        return mCount;
    }


    private boolean isValidPosition(int pos) {
        return pos >= 0 && pos < getCount();
    }

    private long waitForRow(int pos) {
        try {
            synchronized (mWaitLock) {
                long result;
                while ((result = mWindow.getRowUnsafe(pos)) == 0) {
                    // Add myself to waiter list and wait for notice, until row is
                    // successfully got, or end of result is reached.
                    if (!isValidPosition(pos)) {
                        throw new CursorIndexOutOfBoundsException(mPos, mCount);
                    }

                    mWaitLock.wait();
                }
                return result;
            }
        } catch (InterruptedException e) {
            // XXX: when thread is interrupted, should we continue waiting or just return failure?
            return 0;
        }
    }

    private boolean requestRow() {
        if (mWindow == null)
            return false;

        assert mCurrentRow == 0;

        if (!isValidPosition(mPos))
            return false;

        mQueryThread.requestPos(mPos);
        mCurrentRow = mWindow.getRowUnsafe(mPos);
        if (mCurrentRow == 0) {
            mCurrentRow = waitForRow(mPos);
        }

        return mCurrentRow != 0;
    }

    private void checkValidRow() {
        if (mCurrentRow != 0) return;

        if (!isValidPosition(mPos))
            throw new CursorIndexOutOfBoundsException(mPos, mCount);
        else
            throw new StaleDataException("Cannot get valid Row object");
    }

    @Override
    public boolean moveToPosition(int position) {
        if (position < -1)
            position = -1;

        // If position changed, release old Row object.
        if (position != mPos) {
            mWindow.endRowUnsafe(mCurrentRow);
            mCurrentRow = 0;
        }

        int count = getCount();
        if (position >= count) {
            mPos = count;
            return false;
        } else {
            mPos = position;
            return position >= 0 && requestRow();
        }
    }

    @Override
    public int getType(int column) {
        checkValidRow();
        return mWindow.getTypeUnsafe(mCurrentRow, column);
    }

    @Override
    public byte[] getBlob(int column) {
        checkValidRow();
        return mWindow.getBlobUnsafe(mCurrentRow, column);
    }

    @Override
    public String getString(int column) {
        checkValidRow();
        return mWindow.getStringUnsafe(mCurrentRow, column);
    }

    @Override
    public short getShort(int column) {
        return (short) getLong(column);
    }

    @Override
    public int getInt(int column) {
        return (int) getLong(column);
    }

    @Override
    public long getLong(int column) {
        checkValidRow();
        return mWindow.getLongUnsafe(mCurrentRow, column);
    }

    @Override
    public float getFloat(int column) {
        return (float) getDouble(column);
    }

    @Override
    public double getDouble(int column) {
        checkValidRow();
        return mWindow.getDoubleUnsafe(mCurrentRow, column);
    }

    @Override
    public boolean isNull(int column) {
        return getType(column) == FIELD_TYPE_NULL;
    }


    private class QueryThread extends Thread {

        private volatile int mRequestPos;
        private int mFetchPos;
        private int mMinPos;


        QueryThread() {
            super("SQLiteAsyncCursor.QueryThread");

            mRequestPos = 0;
            mMinPos = 0;
            mFetchPos = 0;
        }

        @Override
        public void run() {

            try {
                // Iterate to get row count, then notify the main thread.
                int count = mQuery.getCount();
                synchronized (mWaitLock) {
                    mCount = count;
                    mWaitLock.notifyAll();
                }

                while (!interrupted()) {

                    int pos;
                    int targetPos;
                    synchronized (this) {
                        while (mRequestPos + MAX_PREFETCH <= mFetchPos && mRequestPos >= mMinPos)
                            this.wait();

                        pos = mRequestPos;
                        targetPos = pos + MAX_PREFETCH;
                    }

                    // If backward position is forced, we have to rewind the native cursor.
                    if (pos < mMinPos) {
                        mQuery.reset();
                        mFetchPos = 0;

                        // XXX: It's better to keep some data here.
                        mWindow.clear();
                        mMinPos = 0;
                    }

                    if (mFetchPos < targetPos) {
                        // Drop some chunks if we used up quota.
                        if (mWindow.getNumChunks() > MAX_KEEP_CHUNKS) {
                            long result = mWindow.removeChunk(mMinPos);
                            if (result != ChunkedCursorWindow.CHUNK_NOT_FOUND) {
                                mMinPos = (int) result;
                            }
                        }

                        // Fill some rows.
                        synchronized (mWaitLock) {
                            count = mQuery.fillRows(mWindow, mFetchPos, MIN_FETCH_ROWS);

                            // Signal the main thread if we just passed the requested position.
                            if (mFetchPos <= pos && mFetchPos + count > pos) {
                                mWaitLock.notifyAll();
                            }
                        }

                        mFetchPos += count;
                    }
                }
            } catch (InterruptedException e) {
                // do nothing.
            } finally {
                mQuery.release();
            }
        }

        void requestPos(int pos) {
            synchronized (this) {
                mRequestPos = pos;
                this.notifyAll();
            }
        }

        void quit() {
            interrupt();
        }
    }


    public static final SQLiteDatabase.CursorFactory FACTORY = new SQLiteDatabase.CursorFactory() {
        @Override
        public Cursor newCursor(SQLiteDatabase db, SQLiteCursorDriver masterQuery,
                String editTable, SQLiteProgram query) {
            return new SQLiteAsyncCursor(masterQuery, editTable, (SQLiteAsyncQuery) query);
        }

        @Override
        public SQLiteProgram newQuery(SQLiteDatabase db, String query, Object[] bindArgs,
                CancellationSignal cancellationSignalForPrepare) {
            return new SQLiteAsyncQuery(db, query, bindArgs, cancellationSignalForPrepare);
        }
    };
}
