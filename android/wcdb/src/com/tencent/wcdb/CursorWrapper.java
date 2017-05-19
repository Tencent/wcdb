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
import android.database.ContentObserver;
import android.database.DataSetObserver;
import android.net.Uri;
import android.os.Bundle;

/**
 * Wrapper class for Cursor that delegates all calls to the actual cursor object.  The primary
 * use for this class is to extend a cursor while overriding only a subset of its methods.
 */
public class CursorWrapper implements Cursor {
    /** @hide */
    protected final Cursor mCursor;

    /**
     * Creates a cursor wrapper.
     * @param cursor The underlying cursor to wrap.
     */
    public CursorWrapper(Cursor cursor) {
        mCursor = cursor;
    }

    /**
     * Gets the underlying cursor that is wrapped by this instance.
     *
     * @return The wrapped cursor.
     */
    public Cursor getWrappedCursor() {
        return mCursor;
    }

    public void close() {
        mCursor.close(); 
    }
 
    public boolean isClosed() {
        return mCursor.isClosed();
    }

    public int getCount() {
        return mCursor.getCount();
    }

    @SuppressWarnings("deprecation")
	public void deactivate() {
        mCursor.deactivate();
    }

    public boolean moveToFirst() {
        return mCursor.moveToFirst();
    }

    public int getColumnCount() {
        return mCursor.getColumnCount();
    }

    public int getColumnIndex(String columnName) {
        return mCursor.getColumnIndex(columnName);
    }

    public int getColumnIndexOrThrow(String columnName)
            throws IllegalArgumentException {
        return mCursor.getColumnIndexOrThrow(columnName);
    }

    public String getColumnName(int columnIndex) {
         return mCursor.getColumnName(columnIndex);
    }

    public String[] getColumnNames() {
        return mCursor.getColumnNames();
    }

    public double getDouble(int columnIndex) {
        return mCursor.getDouble(columnIndex);
    }

    public Bundle getExtras() {
        return mCursor.getExtras();
    }

    public float getFloat(int columnIndex) {
        return mCursor.getFloat(columnIndex);
    }

    public int getInt(int columnIndex) {
        return mCursor.getInt(columnIndex);
    }

    public long getLong(int columnIndex) {
        return mCursor.getLong(columnIndex);
    }

    public short getShort(int columnIndex) {
        return mCursor.getShort(columnIndex);
    }

    public String getString(int columnIndex) {
        return mCursor.getString(columnIndex);
    }
    
    public void copyStringToBuffer(int columnIndex, CharArrayBuffer buffer) {
        mCursor.copyStringToBuffer(columnIndex, buffer);
    }

    public byte[] getBlob(int columnIndex) {
        return mCursor.getBlob(columnIndex);
    }
    
    public boolean getWantsAllOnMoveCalls() {
        return mCursor.getWantsAllOnMoveCalls();
    }

    public void setExtras(Bundle extras) {}

    public boolean isAfterLast() {
        return mCursor.isAfterLast();
    }

    public boolean isBeforeFirst() {
        return mCursor.isBeforeFirst();
    }

    public boolean isFirst() {
        return mCursor.isFirst();
    }

    public boolean isLast() {
        return mCursor.isLast();
    }

    public int getType(int columnIndex) {
        return mCursor.getType(columnIndex);
    }

    public boolean isNull(int columnIndex) {
        return mCursor.isNull(columnIndex);
    }

    public boolean moveToLast() {
        return mCursor.moveToLast();
    }

    public boolean move(int offset) {
        return mCursor.move(offset);
    }

    public boolean moveToPosition(int position) {
        return mCursor.moveToPosition(position);
    }

    public boolean moveToNext() {
        return mCursor.moveToNext();
    }

    public int getPosition() {
        return mCursor.getPosition();
    }

    public boolean moveToPrevious() {
        return mCursor.moveToPrevious();
    }

    public void registerContentObserver(ContentObserver observer) {
        mCursor.registerContentObserver(observer);   
    }

    public void registerDataSetObserver(DataSetObserver observer) {
        mCursor.registerDataSetObserver(observer);   
    }

    @SuppressWarnings("deprecation")
	public boolean requery() {
        return mCursor.requery();
    }

    public Bundle respond(Bundle extras) {
        return mCursor.respond(extras);
    }

    public void setNotificationUri(ContentResolver cr, Uri uri) {
        mCursor.setNotificationUri(cr, uri);        
    }

    @Override
    public Uri getNotificationUri() {
        return null;
    }

    public void unregisterContentObserver(ContentObserver observer) {
        mCursor.unregisterContentObserver(observer);        
    }

    public void unregisterDataSetObserver(DataSetObserver observer) {
        mCursor.unregisterDataSetObserver(observer);
    }

}

