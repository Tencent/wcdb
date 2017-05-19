/*
 * Copyright (C) 2007 The Android Open Source Project
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

import com.tencent.wcdb.Cursor;
import com.tencent.wcdb.database.SQLiteDatabase.CursorFactory;
import com.tencent.wcdb.support.CancellationSignal;

/**
 * A cursor driver that uses the given query directly.
 * 
 * @hide
 */
public final class SQLiteDirectCursorDriver implements SQLiteCursorDriver {
    private final SQLiteDatabase mDatabase;
    private final String mEditTable; 
    private final String mSql;
    private final CancellationSignal mCancellationSignal;
    private SQLiteProgram mQuery;

    private static CursorFactory DEFAULT_FACTORY = SQLiteCursor.FACTORY;


    public SQLiteDirectCursorDriver(SQLiteDatabase db, String sql, String editTable,
            CancellationSignal cancellationSignal) {
        mDatabase = db;
        mEditTable = editTable;
        mSql = sql;
        mCancellationSignal = cancellationSignal;
    }

    public Cursor query(CursorFactory factory, String[] selectionArgs) {
        if (factory == null)
            factory = DEFAULT_FACTORY;

        SQLiteProgram query = null;
        final Cursor cursor;
        try {
            query = factory.newQuery(mDatabase, mSql, selectionArgs, mCancellationSignal);
            cursor = factory.newCursor(mDatabase, this, mEditTable, query);
        } catch (RuntimeException ex) {
            if (query != null)
                query.close();
            throw ex;
        }

        mQuery = query;
        return cursor;
    }

    public void cursorClosed() {
        // Do nothing
    }

    public void setBindArguments(String[] bindArgs) {
        mQuery.bindAllArgsAsStrings(bindArgs);
    }

    public void cursorDeactivated() {
        // Do nothing
    }

    public void cursorRequeried(Cursor cursor) {
        // Do nothing
    }

    @Override
    public String toString() {
        return "SQLiteDirectCursorDriver: " + mSql;
    }
}
