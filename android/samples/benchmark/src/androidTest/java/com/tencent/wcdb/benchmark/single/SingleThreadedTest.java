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

package com.tencent.wcdb.benchmark.single;

import android.content.Context;
import android.database.Cursor;
import android.support.test.InstrumentationRegistry;
import android.util.Log;

import junit.framework.Assert;

import java.io.File;

public abstract class SingleThreadedTest {

    private static final String TAG = "WCDB.Benchmark";
    private static final String DATABASE_NAME = "test.db";

    protected abstract void createDB(String path);
    protected abstract void closeDB();
    protected abstract void execSQL(String sql);
    protected abstract void execSQL(String sql, Object[] bindArgs);
    protected abstract void beginTransaction();
    protected abstract void setTransactionSuccessful();
    protected abstract void endTransaction();
    protected abstract long longForQuery(String sql, String[] selectionArgs);
    protected abstract String stringForQuery(String sql, String[] selectionArgs);
    protected abstract Cursor rawQuery(String sql, String[] selectionArgs);

    public void doBefore() {
        Log.i("WCDB.Benchmark", "[DB BENCHMARK] | Begin | " + getClass().getSimpleName());
        Context context = InstrumentationRegistry.getTargetContext();

        // Remove pre-existing database.
        File dbFile = context.getDatabasePath(DATABASE_NAME);
        dbFile.getParentFile().mkdirs();
        dbFile.delete();
        new File(dbFile.getParentFile(), dbFile.getName() + "-journal").delete();
        new File(dbFile.getParentFile(), dbFile.getName() + "-wal").delete();

        // Create database and table, measuring time.
        long time = System.nanoTime();
        createDB(dbFile.getPath());
        execSQL("CREATE TABLE message (id INTEGER PRIMARY KEY, user TEXT, content TEXT);");
        execSQL("CREATE INDEX message_user ON message (user);");
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Create database | " + time);
    }

    public void doTest() {
        // MEASUREMENT: Insertion in one transaction
        long time = System.nanoTime();
        beginTransaction();
        for (int i = 0; i < 19000; i++) {
            execSQL("INSERT INTO message (id, user, content) VALUES (?, ?, ?);",
                    new Object[] {i, "u" + i, "Test message: " + i});
        }
        setTransactionSuccessful();
        endTransaction();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Insertion in one transaction | " + time);

        // TEST: count rows
        int count = (int) longForQuery("SELECT count(*) FROM message;", null);
        Assert.assertEquals(count, 19000);

        // MEASUREMENT: Insertion in separated transactions
        time = System.nanoTime();
        for (int i = 19000; i < 20000; i++) {
            execSQL("INSERT INTO message (id, user, content) VALUES (?, ?, ?);",
                    new Object[] {i, "u" + i, "Test message: " + i});
        }
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Insertion in separated transactions | " + time);

        // TEST: count rows
        count = (int) longForQuery("SELECT count(*) FROM message;", null);
        Assert.assertEquals(count, 20000);

        // MEASUREMENT: Insertion rollback
        time = System.nanoTime();
        beginTransaction();
        for (int i = 20000; i < 21000; i++) {
            execSQL("INSERT INTO message (id, user, content) VALUES (?, ?, ?);",
                    new Object[] {i, "u" + i, "Test message: " + i});
        }
        endTransaction();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Insertion rollback | " + time);

        // MEASUREMENT & TEST: count rows
        time = System.nanoTime();
        count = (int) longForQuery("SELECT count(*) FROM message;", null);
        time = System.nanoTime() - time;
        Assert.assertEquals(count, 20000);
        Log.i(TAG, "[DB BENCHMARK] | Count rows (B-tree traversal) | " + time);

        // MEASUREMENT & TEST: update in one SQL
        time = System.nanoTime();
        execSQL("UPDATE message SET content = 'Modified message: ' || id WHERE id < ?;",
                new Object[] {10000});
        time = System.nanoTime() - time;

        String text = stringForQuery("SELECT content FROM message WHERE id = ?;",
                new String[] {"4567"});
        Assert.assertEquals(text, "Modified message: 4567");
        Log.i(TAG, "[DB BENCHMARK] | Update in one SQL | " + time);

        // MEASUREMENT & TEST: update in one transaction
        time = System.nanoTime();
        beginTransaction();
        for (int i = 5000; i < 15000; i++) {
            execSQL("UPDATE message SET content = ? WHERE id = ?;",
                    new Object[]{"Again modified message: " + i, i});
        }
        setTransactionSuccessful();
        endTransaction();
        time = System.nanoTime() - time;

        text = stringForQuery("SELECT content FROM message WHERE id = ?;",
                new String[] {"12345"});
        Assert.assertEquals(text, "Again modified message: 12345");
        Log.i(TAG, "[DB BENCHMARK] | Update in one transaction | " + time);

        // MEASUREMENT & TEST: update in separated transactions
        time = System.nanoTime();
        for (int i = 15000; i < 16000; i++) {
            execSQL("UPDATE message SET content = ? WHERE id = ?;",
                    new Object[]{"Again2 modified message: " + i, i});
        }
        time = System.nanoTime() - time;

        text = stringForQuery("SELECT content FROM message WHERE id = ?;",
                new String[] {"15678"});
        Assert.assertEquals(text, "Again2 modified message: 15678");
        Log.i(TAG, "[DB BENCHMARK] | Update in separated transactions | " + time);

        // MEASUREMENT & TEST: query one by one
        Cursor cursor;
        time = System.nanoTime();
        for (int i = 0; i < 10000; i++) {
            cursor = rawQuery("SELECT * FROM message WHERE id = ?;",
                    new String[] {Integer.toString(i)});
            Assert.assertTrue(cursor.moveToFirst());
            Assert.assertEquals(cursor.getInt(0), i);
            Assert.assertEquals(cursor.getString(1), "u" + i);
            Assert.assertEquals(cursor.getString(2), ((i < 5000) ?
                    "Modified message: " : "Again modified message: ") + i);
            cursor.close();
            Assert.assertTrue(cursor.isClosed());
        }
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Query one by one | " + time);

        // MEASUREMENT & TEST: query fill
        count = 10000;
        time = System.nanoTime();
        cursor = rawQuery("SELECT * FROM message WHERE id >= ?;",
                new String[] {Integer.toString(10000)});
        while (cursor.moveToNext()) {
            String expected;
            if (count < 15000)
                expected = "Again modified message: ";
            else if (count < 16000)
                expected = "Again2 modified message: ";
            else
                expected = "Test message: ";

            Assert.assertEquals(cursor.getInt(0), count);
            Assert.assertEquals(cursor.getString(1), "u" + count);
            Assert.assertEquals(cursor.getString(2), expected + count);
            count++;
        }
        cursor.close();
        Assert.assertTrue(cursor.isClosed());
        Assert.assertEquals(count, 20000);
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Query fill | " + time);

        // MEASUREMENT: delete in one SQL
        time = System.nanoTime();
        execSQL("DELETE FROM message WHERE id >= ?;", new Object[] {15000});
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Delete in one SQL | " + time);

        // MEASUREMENT: delete in one transaction
        time = System.nanoTime();
        beginTransaction();
        for (int i = 10000; i < 15000; i++) {
            execSQL("DELETE FROM message WHERE id = ?;", new Object[] {i});
        }
        setTransactionSuccessful();
        endTransaction();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Delete in one transaction | " + time);

        // MEASUREMENT: delete in separated transactions
        time = System.nanoTime();
        for (int i = 5000; i < 6000; i++) {
            execSQL("DELETE FROM message WHERE id = ?;", new Object[] {i});
        }
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Delete in separated transactions | " + time);

        // TEST: count
        count = (int) longForQuery("SELECT count(*) FROM message;", null);
        Assert.assertEquals(count, 9000);
    }

    public void doAfter() {
        long time = System.nanoTime();
        closeDB();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | Close database | " + time);
        Log.i("WCDB.Benchmark", "[DB BENCHMARK] | End | " + getClass().getSimpleName());
    }
}
