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

import android.arch.persistence.db.SupportSQLiteStatement;

import com.tencent.wcdb.database.SQLiteStatement;

/**
 * Delegates all calls to a {@link SQLiteStatement}.
 */
class WCDBStatement implements SupportSQLiteStatement {
    private final SQLiteStatement mDelegate;

    /**
     * Creates a wrapper around a framework {@link SQLiteStatement}.
     *
     * @param delegate The SQLiteStatement to delegate calls to.
     */
    public WCDBStatement(SQLiteStatement delegate) {
        mDelegate = delegate;
    }

    @Override
    public void bindNull(int index) {
        mDelegate.bindNull(index);
    }

    @Override
    public void bindLong(int index, long value) {
        mDelegate.bindLong(index, value);
    }

    @Override
    public void bindDouble(int index, double value) {
        mDelegate.bindDouble(index, value);
    }

    @Override
    public void bindString(int index, String value) {
        mDelegate.bindString(index, value);
    }

    @Override
    public void bindBlob(int index, byte[] value) {
        mDelegate.bindBlob(index, value);
    }

    @Override
    public void clearBindings() {
        mDelegate.clearBindings();
    }

    @Override
    public void execute() {
        mDelegate.execute();
    }

    @Override
    public int executeUpdateDelete() {
        return mDelegate.executeUpdateDelete();
    }

    @Override
    public long executeInsert() {
        return mDelegate.executeInsert();
    }

    @Override
    public long simpleQueryForLong() {
        return mDelegate.simpleQueryForLong();
    }

    @Override
    public String simpleQueryForString() {
        return mDelegate.simpleQueryForString();
    }

    @Override
    public void close() throws Exception {
        mDelegate.close();
    }
}
