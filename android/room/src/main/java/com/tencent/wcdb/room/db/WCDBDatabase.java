/*
 * Copyright (C) 2016 The Android Open Source Project
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

package com.tencent.wcdb.room.db;

import android.arch.persistence.db.SimpleSQLiteQuery;
import android.arch.persistence.db.SupportSQLiteDatabase;
import android.arch.persistence.db.SupportSQLiteQuery;
import android.arch.persistence.db.SupportSQLiteStatement;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteTransactionListener;
import android.os.Build;
import android.os.CancellationSignal;
import android.support.annotation.RequiresApi;
import android.util.Pair;

import com.tencent.wcdb.database.SQLiteCursor;
import com.tencent.wcdb.database.SQLiteCursorDriver;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteProgram;

import java.io.IOException;
import java.util.List;
import java.util.Locale;

/**
 * Delegates all calls to an implementation of {@link SQLiteDatabase}.
 */
class WCDBDatabase implements SupportSQLiteDatabase {
    private static final String[] CONFLICT_VALUES = new String[]
            {"", " OR ROLLBACK ", " OR ABORT ", " OR FAIL ", " OR IGNORE ", " OR REPLACE "};
    private static final String[] EMPTY_STRING_ARRAY = new String[0];

    private final SQLiteDatabase mDelegate;

    /**
     * Creates a wrapper around {@link SQLiteDatabase}.
     *
     * @param delegate The delegate to receive all calls.
     */
    public WCDBDatabase(SQLiteDatabase delegate) {
        mDelegate = delegate;
    }

    @Override
    public SupportSQLiteStatement compileStatement(String sql) {
        return new WCDBStatement(mDelegate.compileStatement(sql));
    }

    @Override
    public void beginTransaction() {
        mDelegate.beginTransaction();
    }

    @Override
    public void beginTransactionNonExclusive() {
        mDelegate.beginTransactionNonExclusive();
    }

    @Override
    public void beginTransactionWithListener(SQLiteTransactionListener transactionListener) {
        mDelegate.beginTransactionWithListener(transactionListener);
    }

    @Override
    public void beginTransactionWithListenerNonExclusive(
            SQLiteTransactionListener transactionListener) {
        mDelegate.beginTransactionWithListenerNonExclusive(transactionListener);
    }

    @Override
    public void endTransaction() {
        mDelegate.endTransaction();
    }

    @Override
    public void setTransactionSuccessful() {
        mDelegate.setTransactionSuccessful();
    }

    @Override
    public boolean inTransaction() {
        return mDelegate.inTransaction();
    }

    @Override
    public boolean isDbLockedByCurrentThread() {
        return mDelegate.isDbLockedByCurrentThread();
    }

    @Override
    public boolean yieldIfContendedSafely() {
        return mDelegate.yieldIfContendedSafely();
    }

    @Override
    public boolean yieldIfContendedSafely(long sleepAfterYieldDelay) {
        return mDelegate.yieldIfContendedSafely(sleepAfterYieldDelay);
    }

    @Override
    public int getVersion() {
        return mDelegate.getVersion();
    }

    @Override
    public void setVersion(int version) {
        mDelegate.setVersion(version);
    }

    @Override
    public long getMaximumSize() {
        return mDelegate.getMaximumSize();
    }

    @Override
    public long setMaximumSize(long numBytes) {
        return mDelegate.setMaximumSize(numBytes);
    }

    @Override
    public long getPageSize() {
        return mDelegate.getPageSize();
    }

    @Override
    public void setPageSize(long numBytes) {
        mDelegate.setPageSize(numBytes);
    }

    @Override
    public Cursor query(String query) {
        return mDelegate.rawQuery(query, null);
    }

    @Override
    public Cursor query(String query, Object[] bindArgs) {
        return mDelegate.rawQuery(query, bindArgs);
    }


    @Override
    public Cursor query(final SupportSQLiteQuery supportQuery) {
        return mDelegate.rawQueryWithFactory(new SQLiteDatabase.CursorFactory() {
            @Override
            public com.tencent.wcdb.Cursor newCursor(SQLiteDatabase db, SQLiteCursorDriver masterQuery,
                    String editTable, SQLiteProgram query) {
                supportQuery.bindTo(new WCDBProgram(query));
                return SQLiteCursor.FACTORY.newCursor(db, masterQuery, editTable, query);
            }

            @Override
            public SQLiteProgram newQuery(SQLiteDatabase db, String query, Object[] bindArgs,
                    com.tencent.wcdb.support.CancellationSignal cancellationSignalForPrepare) {
                return SQLiteCursor.FACTORY.newQuery(db, query, bindArgs, cancellationSignalForPrepare);
            }
        }, supportQuery.getSql(), null, null);
    }

    @Override
    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
    public Cursor query(final SupportSQLiteQuery supportQuery,
            CancellationSignal cancellationSignal) {

        final com.tencent.wcdb.support.CancellationSignal realSignal;
        if (cancellationSignal != null) {
            // Be compatible to CancellationSignal from official SDK.
            realSignal = new com.tencent.wcdb.support.CancellationSignal();

            if (cancellationSignal.isCanceled()) {
                realSignal.cancel();
            }
            cancellationSignal.setOnCancelListener(new CancellationSignal.OnCancelListener() {
                @Override
                public void onCancel() {
                    realSignal.cancel();
                }
            });
        } else {
            realSignal = null;
        }

        return mDelegate.rawQueryWithFactory(new SQLiteDatabase.CursorFactory() {
            @Override
            public com.tencent.wcdb.Cursor newCursor(SQLiteDatabase db, SQLiteCursorDriver masterQuery,
                    String editTable, SQLiteProgram query) {
                supportQuery.bindTo(new WCDBProgram(query));
                return SQLiteCursor.FACTORY.newCursor(db, masterQuery, editTable, query);
            }

            @Override
            public SQLiteProgram newQuery(SQLiteDatabase db, String query, Object[] bindArgs,
                    com.tencent.wcdb.support.CancellationSignal cancellationSignalForPrepare) {
                return SQLiteCursor.FACTORY.newQuery(db, query, bindArgs, cancellationSignalForPrepare);
            }
        }, supportQuery.getSql(), null, null, realSignal);
    }

    @Override
    public long insert(String table, int conflictAlgorithm, ContentValues values)
            throws SQLException {
        return mDelegate.insertWithOnConflict(table, null, values,
                conflictAlgorithm);
    }

    @Override
    public int delete(String table, String whereClause, Object[] whereArgs) {
        String query = "DELETE FROM " + table
                + (isEmpty(whereClause) ? "" : " WHERE " + whereClause);
        SupportSQLiteStatement statement = compileStatement(query);
        SimpleSQLiteQuery.bind(statement, whereArgs);
        return statement.executeUpdateDelete();
    }


    @Override
    public int update(String table, int conflictAlgorithm, ContentValues values, String whereClause,
            Object[] whereArgs) {
        // taken from SQLiteDatabase class.
        if (values == null || values.size() == 0) {
            throw new IllegalArgumentException("Empty values");
        }
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
        for (String colName : values.keySet()) {
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
        if (!isEmpty(whereClause)) {
            sql.append(" WHERE ");
            sql.append(whereClause);
        }
        SupportSQLiteStatement stmt = compileStatement(sql.toString());
        SimpleSQLiteQuery.bind(stmt, bindArgs);
        return stmt.executeUpdateDelete();
    }

    @Override
    public void execSQL(String sql) throws SQLException {
        mDelegate.execSQL(sql);
    }

    @Override
    public void execSQL(String sql, Object[] bindArgs) throws SQLException {
        mDelegate.execSQL(sql, bindArgs);
    }

    @Override
    public boolean isReadOnly() {
        return mDelegate.isReadOnly();
    }

    @Override
    public boolean isOpen() {
        return mDelegate.isOpen();
    }

    @Override
    public boolean needUpgrade(int newVersion) {
        return mDelegate.needUpgrade(newVersion);
    }

    @Override
    public String getPath() {
        return mDelegate.getPath();
    }

    @Override
    public void setLocale(Locale locale) {
        mDelegate.setLocale(locale);
    }

    @Override
    public void setMaxSqlCacheSize(int cacheSize) {
        mDelegate.setMaxSqlCacheSize(cacheSize);
    }

    @Override
    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
    public void setForeignKeyConstraintsEnabled(boolean enable) {
        mDelegate.setForeignKeyConstraintsEnabled(enable);
    }

    @Override
    public boolean enableWriteAheadLogging() {
        return mDelegate.enableWriteAheadLogging();
    }

    @Override
    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
    public void disableWriteAheadLogging() {
        mDelegate.disableWriteAheadLogging();
    }

    @Override
    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
    public boolean isWriteAheadLoggingEnabled() {
        return mDelegate.isWriteAheadLoggingEnabled();
    }

    @Override
    public List<Pair<String, String>> getAttachedDbs() {
        return mDelegate.getAttachedDbs();
    }

    @Override
    public boolean isDatabaseIntegrityOk() {
        return mDelegate.isDatabaseIntegrityOk();
    }

    @Override
    public void close() throws IOException {
        mDelegate.close();
    }

    private static boolean isEmpty(String input) {
        return input == null || input.length() == 0;
    }
}
