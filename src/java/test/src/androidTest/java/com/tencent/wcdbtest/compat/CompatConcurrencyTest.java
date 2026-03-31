package com.tencent.wcdbtest.compat;

import static org.junit.Assert.*;

import android.content.ContentValues;
import android.database.Cursor;

import com.tencent.wcdb.compat.DatabaseUtils;
import com.tencent.wcdb.compat.SQLiteDatabase;
import com.tencent.wcdb.compat.SQLiteStatement;
import com.tencent.wcdbtest.base.BaseTestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

public class CompatConcurrencyTest extends BaseTestCase {
    private SQLiteDatabase db;
    private String dbPath;

    private static final int THREAD_COUNT = 20;
    private static final int OPS_PER_THREAD = 100;

    @Before
    public void setup() {
        super.currentDirectory = tempFolder.getRoot().getPath() + File.separator + getClass().getSimpleName();
        new File(currentDirectory).mkdirs();
        dbPath = currentDirectory + File.separator + "concurrent_test.db";
        db = SQLiteDatabase.openDatabase(dbPath);
        db.execSQL("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, value INTEGER)");
        db.execSQL("CREATE TABLE IF NOT EXISTS counter (id INTEGER PRIMARY KEY, count INTEGER DEFAULT 0)");
        db.execSQL("INSERT INTO counter(id, count) VALUES(1, 0)");
    }

    @After
    public void teardown() {
        if (db != null) {
            db.close();
        }
    }

    @Test
    public void testConcurrentInserts() throws Exception {
        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < OPS_PER_THREAD; i++) {
                            ContentValues values = new ContentValues();
                            values.put("name", "thread" + threadId + "_row" + i);
                            values.put("value", threadId * OPS_PER_THREAD + i);
                            long rowId = db.insert("test", null, values);
                            if (rowId < 0) {
                                errorCount.incrementAndGet();
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
        long totalCount = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(THREAD_COUNT * OPS_PER_THREAD, totalCount);
    }

    @Test
    public void testConcurrentReads() throws Exception {
        for (int i = 0; i < 500; i++) {
            ContentValues values = new ContentValues();
            values.put("name", "read_test_" + i);
            values.put("value", i);
            db.insert("test", null, values);
        }

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < OPS_PER_THREAD; i++) {
                            Cursor cursor = db.rawQuery("SELECT * FROM test ORDER BY id LIMIT 50 OFFSET ?",
                                    new Object[]{(i * 10) % 450});
                            if (cursor.getCount() != 50) {
                                errorCount.incrementAndGet();
                            }
                            while (cursor.moveToNext()) {
                                cursor.getString(cursor.getColumnIndex("name"));
                                cursor.getInt(cursor.getColumnIndex("value"));
                            }
                            cursor.close();
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
    }

    @Test
    public void testConcurrentReadWrite() throws Exception {
        for (int i = 0; i < 200; i++) {
            ContentValues values = new ContentValues();
            values.put("name", "init_" + i);
            values.put("value", i);
            db.insert("test", null, values);
        }

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < OPS_PER_THREAD; i++) {
                            if (threadId % 4 == 0) {
                                ContentValues values = new ContentValues();
                                values.put("name", "writer_" + threadId + "_" + i);
                                values.put("value", i);
                                db.insert("test", null, values);
                            } else if (threadId % 4 == 1) {
                                ContentValues values = new ContentValues();
                                values.put("name", "updated_" + threadId);
                                db.update("test", values, "value=?",
                                        new String[]{String.valueOf(i % 200)});
                            } else if (threadId % 4 == 2) {
                                Cursor cursor = db.rawQuery(
                                        "SELECT COUNT(*) FROM test", null);
                                cursor.moveToFirst();
                                assertTrue(cursor.getLong(0) >= 0);
                                cursor.close();
                            } else {
                                Cursor cursor = db.query("test", null,
                                        "value>=?", new String[]{String.valueOf(i % 100)},
                                        null, null, "id ASC", "10");
                                while (cursor.moveToNext()) {
                                    cursor.getString(1);
                                }
                                cursor.close();
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
    }

    @Test
    public void testConcurrentTransactions() throws Exception {
        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicInteger successCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < 20; i++) {
                            db.beginTransaction();
                            try {
                                for (int j = 0; j < 5; j++) {
                                    ContentValues values = new ContentValues();
                                    values.put("name", "txn_" + threadId + "_" + i + "_" + j);
                                    values.put("value", threadId * 1000 + i * 10 + j);
                                    db.insert("test", null, values);
                                }
                                db.setTransactionSuccessful();
                                successCount.incrementAndGet();
                            } finally {
                                db.endTransaction();
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        assertEquals(THREAD_COUNT * 20, successCount.get());

        long totalCount = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(THREAD_COUNT * 20 * 5, totalCount);
    }

    @Test
    public void testConcurrentTransactionsWithRollback() throws Exception {
        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicInteger commitCount = new AtomicInteger(0);
        final AtomicInteger rollbackCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < 20; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues values = new ContentValues();
                                values.put("name", "mixed_" + threadId + "_" + i);
                                values.put("value", threadId);
                                db.insert("test", null, values);

                                if (i % 3 == 0) {
                                    rollbackCount.incrementAndGet();
                                } else {
                                    db.setTransactionSuccessful();
                                    commitCount.incrementAndGet();
                                }
                            } finally {
                                db.endTransaction();
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
        assertEquals(THREAD_COUNT * 20, commitCount.get() + rollbackCount.get());

        long totalCount = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(commitCount.get(), totalCount);
    }

    @Test
    public void testConcurrentStatementExecution() throws Exception {
        db.execSQL("CREATE TABLE IF NOT EXISTS stmt_test (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT)");

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < OPS_PER_THREAD; i++) {
                            try (SQLiteStatement stmt = db.compileStatement(
                                    "INSERT INTO stmt_test(data) VALUES(?)")) {
                                stmt.bindString(1, "t" + threadId + "_i" + i);
                                long rowId = stmt.executeInsert();
                                if (rowId < 0) {
                                    errorCount.incrementAndGet();
                                }
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM stmt_test", null);
        assertEquals(THREAD_COUNT * OPS_PER_THREAD, count);
    }

    @Test
    public void testConcurrentInsertAndDelete() throws Exception {
        for (int i = 0; i < 500; i++) {
            ContentValues values = new ContentValues();
            values.put("name", "deletable_" + i);
            values.put("value", i);
            db.insert("test", null, values);
        }

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        if (threadId % 2 == 0) {
                            for (int i = 0; i < 50; i++) {
                                ContentValues values = new ContentValues();
                                values.put("name", "new_" + threadId + "_" + i);
                                values.put("value", 1000 + threadId * 100 + i);
                                db.insert("test", null, values);
                            }
                        } else {
                            for (int i = 0; i < 50; i++) {
                                db.delete("test", "value=?",
                                        new String[]{String.valueOf(threadId * 10 + i)});
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());

        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertTrue(count > 0);
    }

    @Test
    public void testConcurrentExecSQL() throws Exception {
        db.execSQL("CREATE TABLE IF NOT EXISTS exec_test (id INTEGER PRIMARY KEY AUTOINCREMENT, val TEXT)");

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < OPS_PER_THREAD; i++) {
                            db.execSQL("INSERT INTO exec_test(val) VALUES(?)",
                                    new Object[]{"exec_" + threadId + "_" + i});
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM exec_test", null);
        assertEquals(THREAD_COUNT * OPS_PER_THREAD, count);
    }

    @Test
    public void testConcurrentQueryDuringBulkInsert() throws Exception {
        for (int i = 0; i < 100; i++) {
            ContentValues values = new ContentValues();
            values.put("name", "base_" + i);
            values.put("value", i);
            db.insert("test", null, values);
        }

        final AtomicInteger errorCount = new AtomicInteger(0);
        final CountDownLatch startLatch = new CountDownLatch(1);
        final CountDownLatch doneLatch = new CountDownLatch(2);

        Thread writer = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    startLatch.await();
                    db.beginTransaction();
                    try {
                        for (int i = 0; i < 500; i++) {
                            ContentValues values = new ContentValues();
                            values.put("name", "bulk_" + i);
                            values.put("value", 1000 + i);
                            db.insert("test", null, values);
                        }
                        db.setTransactionSuccessful();
                    } finally {
                        db.endTransaction();
                    }
                } catch (Exception e) {
                    errorCount.incrementAndGet();
                } finally {
                    doneLatch.countDown();
                }
            }
        });

        Thread reader = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    startLatch.await();
                    for (int i = 0; i < 100; i++) {
                        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
                        assertTrue(cursor.moveToFirst());
                        long count = cursor.getLong(0);
                        assertTrue(count >= 100);
                        cursor.close();
                        sleep(1);
                    }
                } catch (Exception e) {
                    errorCount.incrementAndGet();
                } finally {
                    doneLatch.countDown();
                }
            }
        });

        writer.start();
        reader.start();
        startLatch.countDown();
        doneLatch.await(30, TimeUnit.SECONDS);

        assertEquals(0, errorCount.get());
    }

    @Test
    public void testConcurrentNestedTransactions() throws Exception {
        final CyclicBarrier barrier = new CyclicBarrier(10);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[10];

        for (int t = 0; t < 10; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < 10; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues outer = new ContentValues();
                                outer.put("name", "outer_" + threadId + "_" + i);
                                outer.put("value", 0);
                                db.insert("test", null, outer);

                                db.beginTransaction();
                                try {
                                    ContentValues inner = new ContentValues();
                                    inner.put("name", "inner_" + threadId + "_" + i);
                                    inner.put("value", 1);
                                    db.insert("test", null, inner);
                                    db.setTransactionSuccessful();
                                } finally {
                                    db.endTransaction();
                                }

                                db.setTransactionSuccessful();
                            } finally {
                                db.endTransaction();
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(10 * 10 * 2, count);
    }

    @Test
    public void testConcurrentConflictReplace() throws Exception {
        db.execSQL("CREATE TABLE IF NOT EXISTS conflict_test (id INTEGER PRIMARY KEY, val TEXT)");
        for (int i = 0; i < 100; i++) {
            ContentValues values = new ContentValues();
            values.put("id", i);
            values.put("val", "init_" + i);
            db.insert("conflict_test", null, values);
        }

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < 100; i++) {
                            ContentValues values = new ContentValues();
                            values.put("id", i);
                            values.put("val", "thread" + threadId + "_" + i);
                            db.insertWithOnConflict("conflict_test", null, values,
                                    SQLiteDatabase.CONFLICT_REPLACE);
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM conflict_test", null);
        assertEquals(100, count);
    }

    @Test
    public void testConcurrentUpdateSameRow() throws Exception {
        db.execSQL("INSERT INTO counter(id, count) VALUES(2, 0)");

        final int threadCount = 10;
        final int incrementsPerThread = 50;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < incrementsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                db.execSQL("UPDATE counter SET count = count + 1 WHERE id = 2");
                                db.setTransactionSuccessful();
                            } finally {
                                db.endTransaction();
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
        long finalCount = DatabaseUtils.longForQuery(db,
                "SELECT count FROM counter WHERE id=2", null);
        assertEquals(threadCount * incrementsPerThread, finalCount);
    }

    @Test
    public void testHighContentionInsertQuery() throws Exception {
        final int writerCount = 5;
        final int readerCount = 15;
        final int totalThreads = writerCount + readerCount;
        final CyclicBarrier barrier = new CyclicBarrier(totalThreads);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicLong maxRowsSeen = new AtomicLong(0);
        Thread[] threads = new Thread[totalThreads];

        for (int t = 0; t < totalThreads; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        if (threadId < writerCount) {
                            for (int i = 0; i < OPS_PER_THREAD; i++) {
                                ContentValues values = new ContentValues();
                                values.put("name", "w" + threadId + "_" + i);
                                values.put("value", i);
                                db.insert("test", null, values);
                            }
                        } else {
                            for (int i = 0; i < OPS_PER_THREAD; i++) {
                                long count = DatabaseUtils.longForQuery(db,
                                        "SELECT COUNT(*) FROM test", null);
                                long current;
                                do {
                                    current = maxRowsSeen.get();
                                    if (count <= current) break;
                                } while (!maxRowsSeen.compareAndSet(current, count));
                                sleep(1);
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long finalCount = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(writerCount * OPS_PER_THREAD, finalCount);
    }

    @Test
    public void testRapidOpenClose() throws Exception {
        final int threadCount = 10;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        String threadDbPath = currentDirectory + File.separator + "rapid_" + threadId + ".db";
                        for (int i = 0; i < 20; i++) {
                            SQLiteDatabase tempDb = SQLiteDatabase.openDatabase(threadDbPath);
                            tempDb.execSQL("CREATE TABLE IF NOT EXISTS t (id INTEGER PRIMARY KEY, v TEXT)");
                            ContentValues values = new ContentValues();
                            values.put("id", i);
                            values.put("v", "val" + i);
                            tempDb.insertWithOnConflict("t", null, values,
                                    SQLiteDatabase.CONFLICT_REPLACE);
                            tempDb.close();
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
    }

    @Test
    public void testConcurrentDatabaseUtils() throws Exception {
        for (int i = 1; i <= 100; i++) {
            ContentValues values = new ContentValues();
            values.put("name", "util_" + i);
            values.put("value", i);
            db.insert("test", null, values);
        }

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < OPS_PER_THREAD; i++) {
                            if (threadId % 2 == 0) {
                                long count = DatabaseUtils.longForQuery(db,
                                        "SELECT COUNT(*) FROM test WHERE value>?",
                                        new Object[]{i % 100});
                                assertTrue(count >= 0);
                            } else {
                                String name = DatabaseUtils.stringForQuery(db,
                                        "SELECT name FROM test WHERE value=?",
                                        new Object[]{(i % 100) + 1});
                                assertNotNull(name);
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());
    }

    @Test
    public void testConcurrentInsertWithExecutorService() throws Exception {
        ExecutorService executor = Executors.newFixedThreadPool(THREAD_COUNT);
        List<Future<?>> futures = new ArrayList<>();

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            futures.add(executor.submit(new Runnable() {
                @Override
                public void run() {
                    for (int i = 0; i < OPS_PER_THREAD; i++) {
                        ContentValues values = new ContentValues();
                        values.put("name", "pool_" + threadId + "_" + i);
                        values.put("value", threadId * OPS_PER_THREAD + i);
                        db.insert("test", null, values);
                    }
                }
            }));
        }

        for (Future<?> future : futures) {
            future.get(30, TimeUnit.SECONDS);
        }
        executor.shutdown();

        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(THREAD_COUNT * OPS_PER_THREAD, count);
    }

    @Test
    public void testConcurrentTransactionsWithDifferentTables() throws Exception {
        db.execSQL("CREATE TABLE IF NOT EXISTS table_a (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT)");
        db.execSQL("CREATE TABLE IF NOT EXISTS table_b (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT)");

        final CyclicBarrier barrier = new CyclicBarrier(THREAD_COUNT);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[THREAD_COUNT];

        for (int t = 0; t < THREAD_COUNT; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        String table = (threadId % 2 == 0) ? "table_a" : "table_b";
                        for (int i = 0; i < 50; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues values = new ContentValues();
                                values.put("data", "t" + threadId + "_" + i);
                                db.insert(table, null, values);
                                db.setTransactionSuccessful();
                            } finally {
                                db.endTransaction();
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(30000);
        }

        assertEquals(0, errorCount.get());

        long countA = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM table_a", null);
        long countB = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM table_b", null);
        assertEquals((THREAD_COUNT / 2) * 50, countA);
        assertEquals((THREAD_COUNT / 2) * 50, countB);
    }

    @Test
    public void testStressTestMixedOperations() throws Exception {
        db.execSQL("CREATE TABLE IF NOT EXISTS stress (id INTEGER PRIMARY KEY AUTOINCREMENT, category TEXT, val INTEGER)");
        for (int i = 0; i < 100; i++) {
            db.execSQL("INSERT INTO stress(category, val) VALUES(?, ?)",
                    new Object[]{"cat" + (i % 5), i});
        }

        final int stressThreads = 30;
        final CyclicBarrier barrier = new CyclicBarrier(stressThreads);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[stressThreads];

        for (int t = 0; t < stressThreads; t++) {
            final int threadId = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < 50; i++) {
                            switch (threadId % 6) {
                                case 0: {
                                    ContentValues values = new ContentValues();
                                    values.put("category", "cat" + (threadId % 5));
                                    values.put("val", 1000 + i);
                                    db.insert("stress", null, values);
                                    break;
                                }
                                case 1: {
                                    ContentValues values = new ContentValues();
                                    values.put("val", 9999);
                                    db.update("stress", values, "val=?",
                                            new String[]{String.valueOf(i)});
                                    break;
                                }
                                case 2: {
                                    Cursor c = db.rawQuery(
                                            "SELECT COUNT(*) FROM stress WHERE category=?",
                                            new Object[]{"cat" + (i % 5)});
                                    c.moveToFirst();
                                    c.close();
                                    break;
                                }
                                case 3: {
                                    Cursor c = db.query("stress",
                                            new String[]{"category", "SUM(val)"},
                                            null, null, "category", null, null);
                                    while (c.moveToNext()) {
                                        c.getString(0);
                                    }
                                    c.close();
                                    break;
                                }
                                case 4: {
                                    db.beginTransaction();
                                    try {
                                        ContentValues values = new ContentValues();
                                        values.put("category", "txn_cat");
                                        values.put("val", threadId * 100 + i);
                                        db.insert("stress", null, values);
                                        db.setTransactionSuccessful();
                                    } finally {
                                        db.endTransaction();
                                    }
                                    break;
                                }
                                case 5: {
                                    db.execSQL("INSERT INTO stress(category, val) VALUES(?, ?)",
                                            new Object[]{"exec_cat", threadId * 100 + i});
                                    break;
                                }
                            }
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM stress", null);
        assertTrue(count >= 100);
    }
}
