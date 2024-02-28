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

package com.tencent.wcdb.benchmark.concurrent;

import android.database.Cursor;
import android.support.test.InstrumentationRegistry;

import com.tencent.wcdb.benchmark.single.SingleThreadedTest;

import net.sqlcipher.DatabaseUtils;
import net.sqlcipher.database.SQLiteDatabase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class SQLCipherConcurrentTest extends ConcurrentTest {

    @Override
    @Before
    public void doBefore() {
        SQLiteDatabase.loadLibs(InstrumentationRegistry.getTargetContext());
        super.doBefore();
    }

    @Override
    @After
    public void doAfter() {
        super.doAfter();
    }

    @Override
    @Test
    public void doTest() {
        super.doTest();
    }


    protected SQLiteDatabase mDB;

    @Override
    protected void createDB(String path) {
        mDB = SQLiteDatabase.openOrCreateDatabase(path, (String) null, null, null);
    }

    @Override
    protected void closeDB() {
        mDB.close();
        mDB = null;
    }

    @Override
    protected void execSQL(String sql) {
        mDB.execSQL(sql);
    }

    @Override
    protected void execSQL(String sql, Object[] bindArgs) {
        mDB.execSQL(sql, bindArgs);
    }

    @Override
    protected void beginTransaction() {
        mDB.beginTransaction();
    }

    @Override
    protected void setTransactionSuccessful() {
        mDB.setTransactionSuccessful();
    }

    @Override
    protected void endTransaction() {
        mDB.endTransaction();
    }

    @Override
    protected long longForQuery(String sql, String[] selectionArgs) {
        return DatabaseUtils.longForQuery(mDB, sql, selectionArgs);
    }

    @Override
    protected String stringForQuery(String sql, String[] selectionArgs) {
        return DatabaseUtils.stringForQuery(mDB, sql, selectionArgs);
    }

    @Override
    protected Cursor rawQuery(String sql, String[] selectionArgs) {
        return mDB.rawQuery(sql, selectionArgs);
    }
}
