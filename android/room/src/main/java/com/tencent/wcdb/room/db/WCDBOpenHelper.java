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

    WCDBOpenHelper(Context context, String name, byte[] passphrase,
            SQLiteCipherSpec cipher, int version, DatabaseErrorHandler errorHandler,
            SupportSQLiteOpenHelper.Callback callback) {
        mDelegate = createDelegate(context, name, passphrase, cipher,
                version, errorHandler, callback);
    }

    private OpenHelper createDelegate(Context context, String name,
            byte[] passphrase, SQLiteCipherSpec cipher,
            int version, DatabaseErrorHandler errorHandler,
            final Callback callback) {
        return new OpenHelper(context, name, passphrase, cipher, null, version, errorHandler) {
            @Override
            public void onCreate(SQLiteDatabase sqLiteDatabase) {
                mWrappedDb = new WCDBDatabase(sqLiteDatabase);
                callback.onCreate(mWrappedDb);
            }

            @Override
            public void onUpgrade(SQLiteDatabase sqLiteDatabase, int oldVersion, int newVersion) {
                callback.onUpgrade(getWrappedDb(sqLiteDatabase), oldVersion, newVersion);
            }

            @Override
            public void onConfigure(SQLiteDatabase db) {
                callback.onConfigure(getWrappedDb(db));
            }

            @Override
            public void onDowngrade(SQLiteDatabase db, int oldVersion, int newVersion) {
                callback.onDowngrade(getWrappedDb(db), oldVersion, newVersion);
            }

            @Override
            public void onOpen(SQLiteDatabase db) {
                callback.onOpen(getWrappedDb(db));
            }
        };
    }

    @Override
    public String getDatabaseName() {
        return mDelegate.getDatabaseName();
    }

    @Override
    public void setWriteAheadLoggingEnabled(boolean enabled) {
        mDelegate.setWriteAheadLoggingEnabled(enabled);
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

    abstract static class OpenHelper extends SQLiteOpenHelper {

        WCDBDatabase mWrappedDb;

        OpenHelper(Context context, String name, byte[] passphrase,
                SQLiteCipherSpec cipher, SQLiteDatabase.CursorFactory factory,
                int version, DatabaseErrorHandler errorHandler) {
            super(context, name, passphrase, cipher, factory, version, errorHandler);
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
            if (mWrappedDb == null) {
                mWrappedDb = new WCDBDatabase(sqLiteDatabase);
            }
            return mWrappedDb;
        }

        @Override
        public synchronized void close() {
            super.close();
            mWrappedDb = null;
        }
    }
}
