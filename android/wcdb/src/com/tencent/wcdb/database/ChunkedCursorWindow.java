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

import com.tencent.wcdb.CursorWindowAllocationException;


/**
 * An alternative cursor window implementation which store cursor rows
 * on different chunks.
 *
 * <p>The window size is relatively small on initial and grows when
 * more rows are filled in. This saves memory on small queries and
 * prevent row drops on large queries.</p>
 *
 * <p>This class is used in {@link SQLiteAsyncCursor} and under development.</p>
 *
 * @hide
 */
public class ChunkedCursorWindow extends SQLiteClosable {

    public static final long CHUNK_NOT_FOUND = 0xFFFFFFFFFFFFFFFFL;

    /**
     * The native ChunkedCursorWindow object pointer.  (FOR INTERNAL USE ONLY)
     * @hide
     */
    long mWindowPtr;

    private int mNumColumns = 0;


    public ChunkedCursorWindow(int capacity) {
        mWindowPtr = nativeCreate(capacity);
        if (mWindowPtr == 0) {
            throw new CursorWindowAllocationException("Cursor window allocation failed.");
        }
    }

    private void dispose() {
        if (mWindowPtr != 0) {
            nativeDispose(mWindowPtr);
            mWindowPtr = 0;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            dispose();
        } finally {
            super.finalize();
        }
    }

    @Override
    protected void onAllReferencesReleased() {
        dispose();
    }

    public void clear() {
        acquireReference();
        try {
            nativeClear(mWindowPtr);
        } finally {
            releaseReference();
        }
    }

    public long removeChunk(int pos) {
        acquireReference();
        try {
            return nativeRemoveChunk(mWindowPtr, pos);
        } finally {
            releaseReference();
        }
    }

    public int getNumChunks() {
        acquireReference();
        try {
            return nativeGetNumChunks(mWindowPtr);
        } finally {
            releaseReference();
        }
    }

    public int getNumColumns() {
        return mNumColumns;
    }

    public boolean setNumColumns(int columnNum) {
        acquireReference();
        try {
            boolean ok = nativeSetNumColumns(mWindowPtr, columnNum);
            if (ok)
                mNumColumns = columnNum;
            return ok;
        } finally {
            releaseReference();
        }
    }


    // Fast unsafe paths for SQLiteAsyncCursor only.
    long getRowUnsafe(int pos) {
        acquireReference();

        long result = nativeGetRow(mWindowPtr, pos);
        if (result == 0)
            releaseReference();

        return result;
    }

    void endRowUnsafe(long rowObj) {
        if (rowObj == 0) return;

        nativeEndRow(mWindowPtr, rowObj);
        releaseReference();
    }

    int getTypeUnsafe(long rowObj, int column) {
        return nativeGetType(rowObj, column);
    }

    byte[] getBlobUnsafe(long rowObj, int column) {
        return nativeGetBlob(rowObj, column);
    }

    String getStringUnsafe(long rowObj, int column) {
        return nativeGetString(rowObj, column);
    }

    long getLongUnsafe(long rowObj, int column) {
        return nativeGetLong(rowObj, column);
    }

    double getDoubleUnsafe(long rowObj, int column) {
        return nativeGetDouble(rowObj, column);
    }

    // Slow safe paths for general usage.
    public int getType(int row, int column) {
        long rowObj = getRowUnsafe(row);
        if (rowObj == 0)
            throw new IllegalStateException("Couldn't read row " + row + ", column " + column
                    + " from ChunkedCursorWindow.");
        try {
            return nativeGetType(rowObj, column);
        } finally {
            endRowUnsafe(rowObj);
        }
    }

    public byte[] getBlob(int row, int column) {
        long rowObj = getRowUnsafe(row);
        if (rowObj == 0)
            throw new IllegalStateException("Couldn't read row " + row + ", column " + column
                    + " from ChunkedCursorWindow.");
        try {
            return nativeGetBlob(rowObj, column);
        } finally {
            endRowUnsafe(rowObj);
        }
    }

    public String getString(int row, int column) {
        long rowObj = getRowUnsafe(row);
        if (rowObj == 0)
            throw new IllegalStateException("Couldn't read row " + row + ", column " + column
                    + " from ChunkedCursorWindow.");
        try {
            return nativeGetString(rowObj, column);
        } finally {
            endRowUnsafe(rowObj);
        }
    }

    public long getLong(int row, int column) {
        long rowObj = getRowUnsafe(row);
        if (rowObj == 0)
            throw new IllegalStateException("Couldn't read row " + row + ", column " + column
                    + " from ChunkedCursorWindow.");
        try {
            return nativeGetLong(rowObj, column);
        } finally {
            endRowUnsafe(rowObj);
        }
    }

    public double getDouble(int row, int column) {
        long rowObj = getRowUnsafe(row);
        if (rowObj == 0)
            throw new IllegalStateException("Couldn't read row " + row + ", column " + column
                    + " from ChunkedCursorWindow.");
        try {
            return nativeGetDouble(rowObj, column);
        } finally {
            endRowUnsafe(rowObj);
        }
    }


    private static native long nativeCreate(int capacity);
    private static native void nativeDispose(long windowPtr);
    private static native void nativeClear(long windowPtr);
    private static native long nativeRemoveChunk(long windowPtr, int pos);
    private static native int nativeGetNumChunks(long windowPtr);
    private static native boolean nativeSetNumColumns(long windowPtr, int columnNum);

    private static native long nativeGetRow(long windowPtr, int pos);
    private static native void nativeEndRow(long windowPtr, long rowPtr);
    private static native int nativeGetType(long rowPtr, int column);
    private static native byte[] nativeGetBlob(long rowPtr, int column);
    private static native String nativeGetString(long rowPtr, int column);
    private static native long nativeGetLong(long rowPtr, int column);
    private static native double nativeGetDouble(long rowPtr, int column);
}
