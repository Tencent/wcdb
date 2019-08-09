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

import android.arch.persistence.db.SupportSQLiteDatabase;
import android.arch.persistence.db.SupportSQLiteOpenHelper;
import android.content.Context;

import com.tencent.wcdb.DatabaseErrorHandler;
import com.tencent.wcdb.database.SQLiteCipherSpec;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteOpenHelper;

class WCDBOpenHelper implements SupportSQLiteOpenHelper {
    private final OpenHelper mDelegate;

    WCDBOpenHelper(Context context, String name, byte[] passphrase, SQLiteCipherSpec cipherSpec,
                   Callback callback) {
        mDelegate = createDelegate(context, name, passphrase, cipherSpec, callback);
    }

    private OpenHelper createDelegate(Context context, String name, byte[] passphrase,
                                      SQLiteCipherSpec cipherSpec, Callback callback) {
        final WCDBDatabase[] dbRef = new WCDBDatabase[1];
        return new OpenHelper(context, name, dbRef, passphrase, cipherSpec, callback);
    }

    @Override
    public String getDatabaseName() {
        return mDelegate.getDatabaseName();
    }

    @Override
    public void setWriteAheadLoggingEnabled(boolean enabled) {
        mDelegate.setWriteAheadLoggingEnabled(enabled);
    }

    void setAsyncCheckpointEnabled(boolean enabled) {
        mDelegate.mAsyncCheckpoint = enabled;
    }

    @Override
    public SupportSQLiteDatabase getWritableDatabase() {
        return mDelegate.getWritableSupportDatabase();
    }

    @Override
    public SupportSQLiteDatabase getReadableDatabase() {
        return mDelegate.getReadableSupportDatabase();
    }

    @Override
    public void close() {
        mDelegate.close();
    }

    static class OpenHelper extends SQLiteOpenHelper {
        /**
         * This is used as an Object reference so that we can access the wrapped database inside
         * the constructor. SQLiteOpenHelper requires the error handler to be passed in the
         * constructor.
         */
        final WCDBDatabase[] mDbRef;
        final Callback mCallback;

        boolean mAsyncCheckpoint;

        OpenHelper(Context context, String name, final WCDBDatabase[] dbRef,
                   byte[] passphrase, SQLiteCipherSpec cipherSpec,
                   final Callback callback) {
            super(context, name, passphrase, cipherSpec, null, callback.version,
                    new DatabaseErrorHandler() {
                        @Override
                        public void onCorruption(SQLiteDatabase dbObj) {
                            WCDBDatabase db = dbRef[0];
                            if (db != null) {
                                callback.onCorruption(db);
                            }
                        }
                    });
            mCallback = callback;
            mDbRef = dbRef;
            mAsyncCheckpoint = false;
        }

        SupportSQLiteDatabase getWritableSupportDatabase() {
            SQLiteDatabase db = super.getWritableDatabase();
            return getWrappedDb(db);
        }

        SupportSQLiteDatabase getReadableSupportDatabase() {
            SQLiteDatabase db = super.getReadableDatabase();
            return getWrappedDb(db);
        }

        WCDBDatabase getWrappedDb(SQLiteDatabase sqLiteDatabase) {
            WCDBDatabase dbRef = mDbRef[0];
            if (dbRef == null) {
                dbRef = new WCDBDatabase(sqLiteDatabase);
                mDbRef[0] = dbRef;
            }
            return mDbRef[0];
        }

        @Override
        public void onCreate(SQLiteDatabase sqLiteDatabase) {
            mCallback.onCreate(getWrappedDb(sqLiteDatabase));
        }

        @Override
        public void onUpgrade(SQLiteDatabase sqLiteDatabase, int oldVersion, int newVersion) {
            mCallback.onUpgrade(getWrappedDb(sqLiteDatabase), oldVersion, newVersion);
        }

        @Override
        public void onConfigure(SQLiteDatabase db) {
            db.setAsyncCheckpointEnabled(mAsyncCheckpoint);
            mCallback.onConfigure(getWrappedDb(db));
        }

        @Override
        public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            mCallback.onDowngrade(getWrappedDb(db), oldVersion, newVersion);
        }

        @Override
        public void onOpen(SQLiteDatabase db) {
            mCallback.onOpen(getWrappedDb(db));
        }

        @Override
        public synchronized void close() {
            super.close();
            mDbRef[0] = null;
        }
    }

}
