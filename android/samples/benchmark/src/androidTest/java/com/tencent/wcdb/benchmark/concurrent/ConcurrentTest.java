package com.tencent.wcdb.benchmark.concurrent;

import android.content.Context;
import android.database.Cursor;
import android.support.test.InstrumentationRegistry;
import android.util.Log;

import junit.framework.Assert;

import java.io.File;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

/**
 * Created by johnh on 2017/6/29.
 */

public abstract class ConcurrentTest {

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

    private ExecutorService mExecutor = Executors.newFixedThreadPool(4);
    private ArrayList<Future> mFutureList = new ArrayList<>(4);

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
        createDB(dbFile.getPath());
        execSQL("CREATE TABLE message (id INTEGER PRIMARY KEY, user TEXT, content TEXT);");
        execSQL("CREATE INDEX message_user ON message (user);");
    }

    private class WriteOneByOneTask implements Runnable {
        private int mFrom;
        private int mTo;
        private boolean mInTransaction;
        private Object[] mArgs = new Object[3];

        WriteOneByOneTask(int from, int to, boolean transaction) {
            mFrom = from;
            mTo = to;
            mInTransaction = transaction;
        }

        @Override
        public void run() {
            long time = System.nanoTime();

            if (mInTransaction) {
                beginTransaction();
            }
            for (int i = mFrom; i < mTo; i++) {
                mArgs[0] = i;
                mArgs[1] = "u" + i;
                mArgs[2] = "Test message: " + i;
                execSQL("REPLACE INTO message VALUES (?, ?, ?);", mArgs);
            }
            if (mInTransaction) {
                setTransactionSuccessful();
                endTransaction();
            }

            time = System.nanoTime() - time;
            Log.i(TAG, String.format("[DB BENCHMARK] | > WriteOneByOneTask(%d, %d, %b) [%d]",
                    mFrom, mTo, mInTransaction, time));
        }
    }

    private class WriteBatchTask implements Runnable {
        private int mFrom;
        private int mTo;

        WriteBatchTask(int from, int to) {
            mFrom = from;
            mTo = to;
        }

        @Override
        public void run() {
            long time = System.nanoTime();
            execSQL("UPDATE message SET content = 'Test message: ' || id WHERE id >= ? AND id < ?",
                    new Object[] {mFrom, mTo});
            time = System.nanoTime() - time;
            Log.i(TAG, String.format("[DB BENCHMARK] | > WriteBatchTask(%d, %d) [%d]",
                    mFrom, mTo, time));
        }
    }

    private class ReadOneByOneTask implements Runnable {
        private int mFrom;
        private int mTo;
        private String[] mArgs = new String[1];

        ReadOneByOneTask(int from, int to) {
            mFrom = from;
            mTo = to;
        }

        @Override
        public void run() {
            long time = System.nanoTime();
            for (int i = mFrom; i < mTo; i++) {
                mArgs[0] = Integer.toString(i);
                Cursor cursor = rawQuery("SELECT * FROM message WHERE id = ?;", mArgs);
                Assert.assertTrue(cursor.moveToFirst());
                Assert.assertEquals(cursor.getInt(0), i);
                Assert.assertEquals(cursor.getString(1), "u" + i);
                Assert.assertEquals(cursor.getString(2), "Test message: " + i);
                cursor.close();
            }
            time = System.nanoTime() - time;
            Log.i(TAG, String.format("[DB BENCHMARK] | > ReadOneByOneTask(%d, %d) [%d]",
                    mFrom, mTo, time));
        }
    }

    private class ReadBatchTask implements Runnable {
        private int mFrom;
        private int mTo;

        ReadBatchTask(int from, int to) {
            mFrom = from;
            mTo = to;
        }

        @Override
        public void run() {
            long time = System.nanoTime();
            Cursor cursor = rawQuery("SELECT * FROM message WHERE id >= ? AND id < ?;",
                    new String[] {Integer.toString(mFrom), Integer.toString(mTo)});
            for (int i = mFrom; i < mTo; i++) {
                Assert.assertTrue(cursor.moveToNext());
                Assert.assertEquals(cursor.getInt(0), i);
                Assert.assertEquals(cursor.getString(1), "u" + i);
                Assert.assertEquals(cursor.getString(2), "Test message: " + i);
            }
            Assert.assertFalse(cursor.moveToNext());
            cursor.close();
            time = System.nanoTime() - time;
            Log.i(TAG, String.format("[DB BENCHMARK] | > ReadBatchTask(%d, %d) [%d]",
                    mFrom, mTo, time));
        }
    }

    private void submitTask(Runnable r) {
        mFutureList.add(mExecutor.submit(r));
        try { Thread.sleep(10); } catch (InterruptedException e) {}
    }

    private void waitUntilFinished() {
        try {
            for (Future<?> future : mFutureList) {
                future.get();
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
        mFutureList.clear();
    }

    public void doTest() {
        // 4 write tasks (transaction).
        long time = System.nanoTime();
        submitTask(new WriteOneByOneTask(0, 5000, true));
        submitTask(new WriteOneByOneTask(5000, 10000, true));
        submitTask(new WriteOneByOneTask(10000, 15000, true));
        submitTask(new WriteOneByOneTask(15000, 20000, true));
        waitUntilFinished();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | WWWW (transaction) | " + time);

        // 4 write tasks (separated).
        time = System.nanoTime();
        submitTask(new WriteOneByOneTask(0, 500, false));
        submitTask(new WriteOneByOneTask(5000, 5500, false));
        submitTask(new WriteOneByOneTask(10000, 10500, false));
        submitTask(new WriteOneByOneTask(15000, 15500, false));
        waitUntilFinished();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | WWWW (separated) | " + time);

        // 2 write tasks, 2 read tasks (batch).
        time = System.nanoTime();
        submitTask(new WriteBatchTask(5000, 10000));
        submitTask(new WriteBatchTask(15000, 20000));
        submitTask(new ReadBatchTask(0, 5000));
        submitTask(new ReadBatchTask(10000, 15000));
        waitUntilFinished();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | WWRR (batch) | " + time);

        // 1 write task, 3 read tasks.
        time = System.nanoTime();
        submitTask(new WriteBatchTask(0, 5000));
        submitTask(new ReadOneByOneTask(5000, 10000));
        submitTask(new ReadBatchTask(10000, 15000));
        submitTask(new ReadOneByOneTask(15000, 20000));
        waitUntilFinished();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | WRRR | " + time);

        // 4 read tasks.
        time = System.nanoTime();
        submitTask(new ReadOneByOneTask(0, 5000));
        submitTask(new ReadBatchTask(5000, 10000));
        submitTask(new ReadOneByOneTask(10000, 15000));
        submitTask(new ReadBatchTask(15000, 20000));
        waitUntilFinished();
        time = System.nanoTime() - time;
        Log.i(TAG, "[DB BENCHMARK] | RRRR | " + time);
    }

    public void doAfter() {
        closeDB();
        Log.i(TAG, "[DB BENCHMARK] | End | " + getClass().getSimpleName());
    }
}
