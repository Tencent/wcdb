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

package com.tencent.wcdb;

import android.content.ContentResolver;
import android.database.CharArrayBuffer;
import android.database.ContentObservable;
import android.database.ContentObserver;
import android.database.DataSetObservable;
import android.database.DataSetObserver;
import android.net.Uri;
import android.os.Bundle;
import com.tencent.wcdb.support.Log;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;

/**
 * This is an abstract cursor class that handles a lot of the common code
 * that all cursors need to deal with and is provided for convenience reasons.
 */
public abstract class AbstractCursor implements CrossProcessCursor {
    private static final String TAG = "Cursor";

    /**
     * @deprecated This is never updated by this class and should not be used
     */
    @Deprecated
    protected HashMap<Long, Map<String, Object>> mUpdatedRows;

    protected int mPos;

    /**
     * This must be set to the index of the row ID column by any
     * subclass that wishes to support updates.
     *
     * @deprecated This field should not be used.
     */
    @Deprecated
    protected int mRowIdColumnIndex;

    /**
     * If {@link #mRowIdColumnIndex} is not -1 this contains contains the value of
     * the column at {@link #mRowIdColumnIndex} for the current row this cursor is
     * pointing at.
     *
     * @deprecated This field should not be used.
     */
    @Deprecated
    protected Long mCurrentRowID;

    protected boolean mClosed;
    protected ContentResolver mContentResolver;
    private Uri mNotifyUri;

    private final Object mSelfObserverLock = new Object();
    private ContentObserver mSelfObserver;
    private boolean mSelfObserverRegistered;

    private final DataSetObservable mDataSetObservable = new DataSetObservable();
    private final ContentObservable mContentObservable = new ContentObservable();

    private Bundle mExtras = Bundle.EMPTY;

    /* -------------------------------------------------------- */
    /* These need to be implemented by subclasses */
    abstract public int getCount();

    abstract public String[] getColumnNames();

    abstract public String getString(int column);
    abstract public short getShort(int column);
    abstract public int getInt(int column);
    abstract public long getLong(int column);
    abstract public float getFloat(int column);
    abstract public double getDouble(int column);
    abstract public boolean isNull(int column);

    public int getType(int column) {
        // Reflects the assumption that all commonly used field types (meaning everything
        // but blobs) are convertible to strings so it should be safe to call
        // getString to retrieve them.
        return FIELD_TYPE_STRING;
    }

    // TODO implement getBlob in all cursor types
    public byte[] getBlob(int column) {
        throw new UnsupportedOperationException("getBlob is not supported");
    }
    /* -------------------------------------------------------- */
    /* Methods that may optionally be implemented by subclasses */

    /**
     * If the cursor is backed by a {@link CursorWindow}, returns a pre-filled
     * window with the contents of the cursor, otherwise null.
     *
     * @return The pre-filled window that backs this cursor, or null if none.
     */
    public CursorWindow getWindow() {
        return null;
    }

    public int getColumnCount() {
        return getColumnNames().length;
    }

    public void deactivate() {
        onDeactivateOrClose();
    }

    /** @hide */
    protected void onDeactivateOrClose() {
        if (mSelfObserver != null) {
            mContentResolver.unregisterContentObserver(mSelfObserver);
            mSelfObserverRegistered = false;
        }
        mDataSetObservable.notifyInvalidated();
    }

    public boolean requery() {
        if (mSelfObserver != null && mSelfObserverRegistered == false) {
            mContentResolver.registerContentObserver(mNotifyUri, true, mSelfObserver);
            mSelfObserverRegistered = true;
        }
        mDataSetObservable.notifyChanged();
        return true;
    }

    public boolean isClosed() {
        return mClosed;
    }

    public void close() {
        mClosed = true;
        mContentObservable.unregisterAll();
        onDeactivateOrClose();
    }

    /**
     * This function is called every time the cursor is successfully scrolled
     * to a new position, giving the subclass a chance to update any state it
     * may have. If it returns false the move function will also do so and the
     * cursor will scroll to the beforeFirst position.
     *
     * @param oldPosition the position that we're moving from
     * @param newPosition the position that we're moving to
     * @return true if the move is successful, false otherwise
     */
    public boolean onMove(int oldPosition, int newPosition) {
        return true;
    }


    public void copyStringToBuffer(int columnIndex, CharArrayBuffer buffer) {
        // Default implementation, uses getString
        String result = getString(columnIndex);
        if (result != null) {
            char[] data = buffer.data;
            if (data == null || data.length < result.length()) {
                buffer.data = result.toCharArray();
            } else {
                result.getChars(0, result.length(), data, 0);
            }
            buffer.sizeCopied = result.length();
        } else {
            buffer.sizeCopied = 0;
        }
    }

    /* -------------------------------------------------------- */
    /* Implementation */
    public AbstractCursor() {
        mPos = -1;
        mRowIdColumnIndex = -1;
        mCurrentRowID = null;
        mUpdatedRows = new HashMap<Long, Map<String, Object>>();
    }

    public final int getPosition() {
        return mPos;
    }

    public boolean moveToPosition(int position) {
        // Make sure position isn't past the end of the cursor
        final int count = getCount();
        if (position >= count) {
            mPos = count;
            return false;
        }

        // Make sure position isn't before the beginning of the cursor
        if (position < 0) {
            mPos = -1;
            return false;
        }

        // Check for no-op moves, and skip the rest of the work for them
        if (position == mPos) {
            return true;
        }

        boolean result = onMove(mPos, position);
        if (result == false) {
            mPos = -1;
        } else {
            mPos = position;
            if (mRowIdColumnIndex != -1) {
                mCurrentRowID = Long.valueOf(getLong(mRowIdColumnIndex));
            }
        }

        return result;
    }

    @Override
    public void fillWindow(int position, CursorWindow window) {
        DatabaseUtils.cursorFillWindow(this, position, window);
    }

    public final boolean move(int offset) {
        return moveToPosition(mPos + offset);
    }

    public final boolean moveToFirst() {
        return moveToPosition(0);
    }

    public final boolean moveToLast() {
        return moveToPosition(getCount() - 1);
    }

    public final boolean moveToNext() {
        return moveToPosition(mPos + 1);
    }

    public final boolean moveToPrevious() {
        return moveToPosition(mPos - 1);
    }

    public final boolean isFirst() {
        return mPos == 0 && getCount() != 0;
    }

    public final boolean isLast() {
        int cnt = getCount();
        return mPos == (cnt - 1) && cnt != 0;
    }

    public final boolean isBeforeFirst() {
        if (getCount() == 0) {
            return true;
        }
        return mPos == -1;
    }

    public final boolean isAfterLast() {
        if (getCount() == 0) {
            return true;
        }
        return mPos == getCount();
    }

    public int getColumnIndex(String columnName) {
        // Hack according to bug 903852
        final int periodIndex = columnName.lastIndexOf('.');
        if (periodIndex != -1) {
            Exception e = new Exception();
            Log.e(TAG, "requesting column name with table name -- " + columnName, e);
            columnName = columnName.substring(periodIndex + 1);
        }

        String columnNames[] = getColumnNames();
        int length = columnNames.length;
        for (int i = 0; i < length; i++) {
            if (columnNames[i].equalsIgnoreCase(columnName)) {
                return i;
            }
        }

        return -1;
    }

    public int getColumnIndexOrThrow(String columnName) {
        final int index = getColumnIndex(columnName);
        if (index < 0) {
            throw new IllegalArgumentException("column '" + columnName + "' does not exist");
        }
        return index;
    }

    public String getColumnName(int columnIndex) {
        return getColumnNames()[columnIndex];
    }

    public void registerContentObserver(ContentObserver observer) {
        mContentObservable.registerObserver(observer);
    }

    public void unregisterContentObserver(ContentObserver observer) {
        // cursor will unregister all observers when it close
        if (!mClosed) {
            mContentObservable.unregisterObserver(observer);
        }
    }

    public void registerDataSetObserver(DataSetObserver observer) {
        mDataSetObservable.registerObserver(observer);
    }

    public void unregisterDataSetObserver(DataSetObserver observer) {
        mDataSetObservable.unregisterObserver(observer);
    }

    /**
     * Subclasses must call this method when they finish committing updates to notify all
     * observers.
     *
     * @param selfChange
     */
    protected void onChange(boolean selfChange) {
        synchronized (mSelfObserverLock) {
            mContentObservable.dispatchChange(selfChange);
            if (mNotifyUri != null && selfChange) {
                mContentResolver.notifyChange(mNotifyUri, mSelfObserver);
            }
        }
    }

    /**
     * Specifies a content URI to watch for changes.
     *
     * @param cr The content resolver from the caller's context.
     * @param notifyUri The URI to watch for changes. This can be a
     * specific row URI, or a base URI for a whole class of content.
     */
    public void setNotificationUri(ContentResolver cr, Uri notifyUri) {
        synchronized (mSelfObserverLock) {
            mNotifyUri = notifyUri;
            mContentResolver = cr;
            if (mSelfObserver != null) {
                mContentResolver.unregisterContentObserver(mSelfObserver);
            }
            mSelfObserver = new SelfContentObserver(this);
            mContentResolver.registerContentObserver(mNotifyUri, true, mSelfObserver);
            mSelfObserverRegistered = true;
        }
    }

    public Uri getNotificationUri() {
        return mNotifyUri;
    }

    public boolean getWantsAllOnMoveCalls() {
        return false;
    }

    /**
     * Sets a {@link Bundle} that will be returned by {@link #getExtras()}.  <code>null</code> will
     * be converted into {@link Bundle#EMPTY}.
     *
     * @param extras {@link Bundle} to set.
     * @hide
     */
    public void setExtras(Bundle extras) {
        mExtras = (extras == null) ? Bundle.EMPTY : extras;
    }

    public Bundle getExtras() {
        return mExtras;
    }

    public Bundle respond(Bundle extras) {
        return Bundle.EMPTY;
    }

    /**
     * @deprecated Always returns false since Cursors do not support updating rows
     */
    @Deprecated
    protected boolean isFieldUpdated(int columnIndex) {
        return false;
    }

    /**
     * @deprecated Always returns null since Cursors do not support updating rows
     */
    @Deprecated
    protected Object getUpdatedField(int columnIndex) {
        return null;
    }

    /**
     * This function throws CursorIndexOutOfBoundsException if
     * the cursor position is out of bounds. Subclass implementations of
     * the get functions should call this before attempting
     * to retrieve data.
     *
     * @throws CursorIndexOutOfBoundsException
     */
    protected void checkPosition() {
        if (-1 == mPos || getCount() == mPos) {
            throw new CursorIndexOutOfBoundsException(mPos, getCount());
        }
    }

    @Override
    protected void finalize() {
        if (mSelfObserver != null && mSelfObserverRegistered == true) {
            mContentResolver.unregisterContentObserver(mSelfObserver);
        }
        try {
            if (!mClosed) close();
        } catch(Exception e) { }
    }

    /**
     * Cursors use this class to track changes others make to their URI.
     */
    protected static class SelfContentObserver extends ContentObserver {
        WeakReference<AbstractCursor> mCursor;

        public SelfContentObserver(AbstractCursor cursor) {
            super(null);
            mCursor = new WeakReference<AbstractCursor>(cursor);
        }

        @Override
        public boolean deliverSelfNotifications() {
            return false;
        }

        @Override
        public void onChange(boolean selfChange) {
            AbstractCursor cursor = mCursor.get();
            if (cursor != null) {
                cursor.onChange(false);
            }
        }
    }
}
