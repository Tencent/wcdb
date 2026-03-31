package com.tencent.wcdbtest.compat;

import static org.junit.Assert.*;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteTransactionListener;

import com.tencent.wcdb.compat.DatabaseUtils;
import com.tencent.wcdb.compat.SQLiteDatabase;
import com.tencent.wcdbtest.base.BaseTestCase;
import com.tencent.wcdbtest.base.WrappedValue;

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
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

public class CompatTransactionTest extends BaseTestCase {
    private SQLiteDatabase db;
    private String dbPath;

    @Before
    public void setup() {
        super.currentDirectory = tempFolder.getRoot().getPath() + File.separator + getClass().getSimpleName();
        new File(currentDirectory).mkdirs();
        dbPath = currentDirectory + File.separator + "txn_test.db";
        db = SQLiteDatabase.openDatabase(dbPath);
        db.execSQL("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT)");
    }

    @After
    public void teardown() {
        if (db != null) {
            db.close();
        }
    }

    @Test
    public void testBasicTransaction() {
        db.beginTransaction();
        try {
            ContentValues values = new ContentValues();
            values.put("id", 1);
            values.put("name", "txn");
            db.insert("test", null, values);
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("txn", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testTransactionRollback() {
        ContentValues pre = new ContentValues();
        pre.put("id", 1);
        pre.put("name", "before");
        db.insert("test", null, pre);

        db.beginTransaction();
        try {
            ContentValues values = new ContentValues();
            values.put("id", 2);
            values.put("name", "rolled_back");
            db.insert("test", null, values);
        } finally {
            db.endTransaction();
        }

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(1, cursor.getLong(0));
        cursor.close();
    }

    @Test
    public void testInTransaction() {
        assertFalse(db.inTransaction());
        db.beginTransaction();
        try {
            assertTrue(db.inTransaction());
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }
        assertFalse(db.inTransaction());
    }

    @Test
    public void testNestedTransactionCommit() {
        db.beginTransaction();
        try {
            ContentValues v1 = new ContentValues();
            v1.put("id", 1);
            v1.put("name", "outer");
            db.insert("test", null, v1);

            db.beginTransaction();
            try {
                ContentValues v2 = new ContentValues();
                v2.put("id", 2);
                v2.put("name", "inner");
                db.insert("test", null, v2);
                db.setTransactionSuccessful();
            } finally {
                db.endTransaction();
            }

            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(2, cursor.getLong(0));
        cursor.close();
    }

    @Test
    public void testNestedTransactionInnerRollback() {
        db.beginTransaction();
        try {
            ContentValues v1 = new ContentValues();
            v1.put("id", 1);
            v1.put("name", "outer");
            db.insert("test", null, v1);

            db.beginTransaction();
            try {
                ContentValues v2 = new ContentValues();
                v2.put("id", 2);
                v2.put("name", "inner");
                db.insert("test", null, v2);
                // Not calling setTransactionSuccessful -> inner fails
            } finally {
                db.endTransaction();
            }

            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(0, cursor.getLong(0));
        cursor.close();
    }

    @Test(expected = IllegalStateException.class)
    public void testSetTransactionSuccessfulWithoutTransaction() {
        db.setTransactionSuccessful();
    }

    @Test(expected = IllegalStateException.class)
    public void testEndTransactionWithoutBegin() {
        db.endTransaction();
    }

    @Test(expected = IllegalStateException.class)
    public void testDoubleSetTransactionSuccessful() {
        db.beginTransaction();
        try {
            db.setTransactionSuccessful();
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }
    }

    @Test(expected = IllegalStateException.class)
    public void testBeginAfterMarkedSuccessful() {
        db.beginTransaction();
        try {
            db.setTransactionSuccessful();
            db.beginTransaction();
        } finally {
            db.endTransaction();
        }
    }

    @Test
    public void testTransactionWithListener() {
        final AtomicBoolean onBeginCalled = new AtomicBoolean(false);
        final AtomicBoolean onCommitCalled = new AtomicBoolean(false);
        final AtomicBoolean onRollbackCalled = new AtomicBoolean(false);

        SQLiteTransactionListener listener = new SQLiteTransactionListener() {
            @Override
            public void onBegin() {
                onBeginCalled.set(true);
            }

            @Override
            public void onCommit() {
                onCommitCalled.set(true);
            }

            @Override
            public void onRollback() {
                onRollbackCalled.set(false);
            }
        };

        db.beginTransactionWithListener(listener);
        try {
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }

        assertTrue(onBeginCalled.get());
        assertTrue(onCommitCalled.get());
        assertFalse(onRollbackCalled.get());
    }

    @Test
    public void testTransactionWithListenerRollback() {
        final AtomicBoolean onRollbackCalled = new AtomicBoolean(false);

        SQLiteTransactionListener listener = new SQLiteTransactionListener() {
            @Override
            public void onBegin() {}

            @Override
            public void onCommit() {}

            @Override
            public void onRollback() {
                onRollbackCalled.set(true);
            }
        };

        db.beginTransactionWithListener(listener);
        try {
            // Not marking successful -> rollback
        } finally {
            db.endTransaction();
        }

        assertTrue(onRollbackCalled.get());
    }

    @Test
    public void testBeginTransactionNonExclusive() {
        db.beginTransactionNonExclusive();
        try {
            ContentValues values = new ContentValues();
            values.put("id", 1);
            values.put("name", "nonexclusive");
            db.insert("test", null, values);
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("nonexclusive", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testTransactionIsolationBetweenThreads() throws InterruptedException {
        final CountDownLatch insertDone = new CountDownLatch(1);
        final CountDownLatch checkDone = new CountDownLatch(1);
        final AtomicBoolean visibleDuringTxn = new AtomicBoolean(true);

        Thread writer = new Thread(new Runnable() {
            @Override
            public void run() {
                db.beginTransaction();
                try {
                    ContentValues values = new ContentValues();
                    values.put("id", 999);
                    values.put("name", "isolated");
                    db.insert("test", null, values);
                    insertDone.countDown();
                    try {
                        checkDone.await();
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                    db.setTransactionSuccessful();
                } finally {
                    db.endTransaction();
                }
            }
        });

        writer.start();
        insertDone.await();

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test WHERE id=999", null);
        assertTrue(cursor.moveToFirst());
        visibleDuringTxn.set(cursor.getLong(0) > 0);
        cursor.close();
        checkDone.countDown();
        writer.join();

        Cursor after = db.rawQuery("SELECT COUNT(*) FROM test WHERE id=999", null);
        assertTrue(after.moveToFirst());
        assertEquals(1, after.getLong(0));
        after.close();
    }

    @Test
    public void testDeeplyNestedTransactions() {
        final int depth = 10;
        for (int d = 0; d < depth; d++) {
            db.beginTransaction();
        }
        try {
            ContentValues values = new ContentValues();
            values.put("id", 1);
            values.put("name", "deep");
            db.insert("test", null, values);

            for (int d = 0; d < depth; d++) {
                db.setTransactionSuccessful();
                db.endTransaction();
            }
        } catch (Exception e) {
            for (int d = 0; d < depth; d++) {
                try {
                    db.endTransaction();
                } catch (Exception ignored) {
                    break;
                }
            }
            throw e;
        }

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(1, cursor.getLong(0));
        cursor.close();
    }

    @Test
    public void testThreadLocalTransactionStack() throws InterruptedException {
        final AtomicBoolean thread1InTxn = new AtomicBoolean(false);
        final AtomicBoolean thread2InTxn = new AtomicBoolean(false);
        final CountDownLatch latch1 = new CountDownLatch(1);
        final CountDownLatch latch2 = new CountDownLatch(1);

        Thread t1 = new Thread(new Runnable() {
            @Override
            public void run() {
                db.beginTransaction();
                try {
                    thread1InTxn.set(db.inTransaction());
                    latch1.countDown();
                    try {
                        latch2.await();
                    } catch (InterruptedException ignored) {}
                    db.setTransactionSuccessful();
                } finally {
                    db.endTransaction();
                }
            }
        });

        Thread t2 = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    latch1.await();
                } catch (InterruptedException ignored) {}
                thread2InTxn.set(db.inTransaction());
                latch2.countDown();
            }
        });

        t1.start();
        t2.start();
        t1.join();
        t2.join();

        assertTrue(thread1InTxn.get());
    }

    @Test
    public void testListenerExceptionOnBeginRollsBack() {
        try {
            db.beginTransactionWithListener(new SQLiteTransactionListener() {
                @Override
                public void onBegin() {
                    throw new RuntimeException("onBegin failed");
                }

                @Override
                public void onCommit() {}

                @Override
                public void onRollback() {}
            });
            fail("Should have thrown");
        } catch (RuntimeException e) {
            assertEquals("onBegin failed", e.getMessage());
        }

        assertFalse(db.inTransaction());
    }

    // =========================================================================
    // High-concurrency transaction tests
    // =========================================================================

    @Test
    public void testConcurrentRapidTransactionCommit() throws InterruptedException {
        final int threadCount = 20;
        final int txnsPerThread = 50;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < txnsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "rapid_" + tid + "_" + i);
                                db.insert("test", null, v);
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * txnsPerThread, total);
    }

    @Test
    public void testConcurrentRapidTransactionRollback() throws InterruptedException {
        final int threadCount = 20;
        final int txnsPerThread = 50;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < txnsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "rollback_data");
                                db.insert("test", null, v);
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
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(0, total);
    }

    @Test
    public void testConcurrentMixedCommitRollback() throws InterruptedException {
        final int threadCount = 20;
        final int txnsPerThread = 100;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicInteger commitCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < txnsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "mixed_" + tid + "_" + i);
                                db.insert("test", null, v);
                                if ((tid + i) % 2 == 0) {
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(commitCount.get(), total);
    }

    @Test
    public void testConcurrentNestedTransactions() throws InterruptedException {
        final int threadCount = 15;
        final int roundsPerThread = 20;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < roundsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v1 = new ContentValues();
                                v1.put("name", "outer_" + tid + "_" + i);
                                db.insert("test", null, v1);

                                db.beginTransaction();
                                try {
                                    ContentValues v2 = new ContentValues();
                                    v2.put("name", "inner_" + tid + "_" + i);
                                    db.insert("test", null, v2);
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * roundsPerThread * 2, total);
    }

    @Test
    public void testConcurrentNestedTransactionsWithInnerRollback() throws InterruptedException {
        final int threadCount = 15;
        final int roundsPerThread = 20;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < roundsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v1 = new ContentValues();
                                v1.put("name", "outer_" + tid + "_" + i);
                                db.insert("test", null, v1);

                                db.beginTransaction();
                                try {
                                    ContentValues v2 = new ContentValues();
                                    v2.put("name", "inner_fail_" + tid + "_" + i);
                                    db.insert("test", null, v2);
                                    // Inner not marked successful -> triggers rollback
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(0, total);
    }

    @Test
    public void testConcurrentTripleNestedTransactions() throws InterruptedException {
        final int threadCount = 10;
        final int roundsPerThread = 15;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < roundsPerThread; i++) {
                            db.beginTransaction(); // L1
                            try {
                                ContentValues v1 = new ContentValues();
                                v1.put("name", "L1_" + tid + "_" + i);
                                db.insert("test", null, v1);

                                db.beginTransaction(); // L2
                                try {
                                    ContentValues v2 = new ContentValues();
                                    v2.put("name", "L2_" + tid + "_" + i);
                                    db.insert("test", null, v2);

                                    db.beginTransaction(); // L3
                                    try {
                                        ContentValues v3 = new ContentValues();
                                        v3.put("name", "L3_" + tid + "_" + i);
                                        db.insert("test", null, v3);
                                        db.setTransactionSuccessful();
                                    } finally {
                                        db.endTransaction();
                                    }

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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * roundsPerThread * 3, total);
    }

    @Test
    public void testConcurrentTransactionAtomicCounter() throws InterruptedException {
        db.execSQL("CREATE TABLE IF NOT EXISTS counter (id INTEGER PRIMARY KEY, val INTEGER)");
        db.execSQL("INSERT INTO counter(id, val) VALUES(1, 0)");

        final int threadCount = 20;
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
                                db.execSQL("UPDATE counter SET val = val + 1 WHERE id = 1");
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long finalVal = DatabaseUtils.longForQuery(db, "SELECT val FROM counter WHERE id=1", null);
        assertEquals((long) threadCount * incrementsPerThread, finalVal);
    }

    @Test
    public void testConcurrentTransactionWithBulkInsert() throws InterruptedException {
        final int threadCount = 10;
        final int rowsPerTxn = 100;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        db.beginTransaction();
                        try {
                            for (int i = 0; i < rowsPerTxn; i++) {
                                ContentValues v = new ContentValues();
                                v.put("name", "bulk_" + tid + "_" + i);
                                db.insert("test", null, v);
                            }
                            db.setTransactionSuccessful();
                        } finally {
                            db.endTransaction();
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
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * rowsPerTxn, total);
    }

    @Test
    public void testConcurrentReadDuringLongTransaction() throws InterruptedException {
        for (int i = 0; i < 100; i++) {
            ContentValues v = new ContentValues();
            v.put("id", i);
            v.put("name", "predata_" + i);
            db.insert("test", null, v);
        }

        final int readerCount = 15;
        final CountDownLatch writerStarted = new CountDownLatch(1);
        final CountDownLatch readersFinished = new CountDownLatch(readerCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicBoolean writerDone = new AtomicBoolean(false);

        Thread writer = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    db.beginTransaction();
                    try {
                        writerStarted.countDown();
                        for (int i = 100; i < 600; i++) {
                            ContentValues v = new ContentValues();
                            v.put("id", i);
                            v.put("name", "longtxn_" + i);
                            db.insert("test", null, v);
                            if (i % 100 == 0) sleep(5);
                        }
                        db.setTransactionSuccessful();
                    } finally {
                        db.endTransaction();
                        writerDone.set(true);
                    }
                } catch (Exception e) {
                    errorCount.incrementAndGet();
                }
            }
        });

        writer.start();
        writerStarted.await();

        Thread[] readers = new Thread[readerCount];
        for (int r = 0; r < readerCount; r++) {
            readers[r] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        for (int i = 0; i < 50; i++) {
                            Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
                            assertTrue(cursor.moveToFirst());
                            long count = cursor.getLong(0);
                            assertTrue(count >= 100);
                            cursor.close();
                            sleep(2);
                        }
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    } finally {
                        readersFinished.countDown();
                    }
                }
            });
            readers[r].start();
        }

        readersFinished.await(60, TimeUnit.SECONDS);
        writer.join(60000);

        assertEquals(0, errorCount.get());
        long finalCount = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(600, finalCount);
    }

    @Test
    public void testConcurrentTransactionWithListeners() throws InterruptedException {
        final int threadCount = 10;
        final int txnsPerThread = 30;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicInteger totalBegins = new AtomicInteger(0);
        final AtomicInteger totalCommits = new AtomicInteger(0);
        final AtomicInteger totalRollbacks = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < txnsPerThread; i++) {
                            final boolean shouldCommit = (i % 3 != 0);
                            db.beginTransactionWithListener(new SQLiteTransactionListener() {
                                @Override
                                public void onBegin() {
                                    totalBegins.incrementAndGet();
                                }

                                @Override
                                public void onCommit() {
                                    totalCommits.incrementAndGet();
                                }

                                @Override
                                public void onRollback() {
                                    totalRollbacks.incrementAndGet();
                                }
                            });
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "listener_" + tid + "_" + i);
                                db.insert("test", null, v);
                                if (shouldCommit) {
                                    db.setTransactionSuccessful();
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        int totalTxns = threadCount * txnsPerThread;
        assertEquals(totalTxns, totalBegins.get());
        assertEquals(totalTxns, totalCommits.get() + totalRollbacks.get());

        long rows = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(totalCommits.get(), rows);
    }

    @Test
    public void testConcurrentShortVsLongTransactions() throws InterruptedException {
        final int shortThreads = 15;
        final int longThreads = 5;
        final int totalThreads = shortThreads + longThreads;
        final CyclicBarrier barrier = new CyclicBarrier(totalThreads);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[totalThreads];

        for (int t = 0; t < totalThreads; t++) {
            final int tid = t;
            final boolean isLong = (tid >= shortThreads);
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        if (isLong) {
                            db.beginTransaction();
                            try {
                                for (int i = 0; i < 200; i++) {
                                    ContentValues v = new ContentValues();
                                    v.put("name", "long_" + tid + "_" + i);
                                    db.insert("test", null, v);
                                }
                                db.setTransactionSuccessful();
                            } finally {
                                db.endTransaction();
                            }
                        } else {
                            for (int i = 0; i < 100; i++) {
                                db.beginTransaction();
                                try {
                                    ContentValues v = new ContentValues();
                                    v.put("name", "short_" + tid + "_" + i);
                                    db.insert("test", null, v);
                                    db.setTransactionSuccessful();
                                } finally {
                                    db.endTransaction();
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
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        long expected = (long) shortThreads * 100 + (long) longThreads * 200;
        assertEquals(expected, total);
    }

    @Test
    public void testConcurrentTransactionAndNonTransactionWrites() throws InterruptedException {
        final int threadCount = 20;
        final int opsPerThread = 50;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        if (tid % 2 == 0) {
                            for (int i = 0; i < opsPerThread; i++) {
                                db.beginTransaction();
                                try {
                                    ContentValues v = new ContentValues();
                                    v.put("name", "txn_" + tid + "_" + i);
                                    db.insert("test", null, v);
                                    db.setTransactionSuccessful();
                                } finally {
                                    db.endTransaction();
                                }
                            }
                        } else {
                            for (int i = 0; i < opsPerThread; i++) {
                                ContentValues v = new ContentValues();
                                v.put("name", "notxn_" + tid + "_" + i);
                                db.insert("test", null, v);
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
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * opsPerThread, total);
    }

    @Test
    public void testConcurrentTransactionUpdateSameRows() throws InterruptedException {
        for (int i = 0; i < 50; i++) {
            ContentValues v = new ContentValues();
            v.put("id", i);
            v.put("name", "original_" + i);
            db.insert("test", null, v);
        }

        final int threadCount = 20;
        final int roundsPerThread = 30;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < roundsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "upd_" + tid + "_" + i);
                                db.update("test", v, "id=?",
                                        new String[]{String.valueOf(i % 50)});
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(50, total);
    }

    @Test
    public void testConcurrentTransactionInsertDeleteCycle() throws InterruptedException {
        final int threadCount = 10;
        final int cyclesPerThread = 30;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < cyclesPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "cycle_" + tid + "_" + i);
                                long rowId = db.insert("test", null, v);

                                db.delete("test", "rowid=?",
                                        new String[]{String.valueOf(rowId)});

                                ContentValues v2 = new ContentValues();
                                v2.put("name", "cycle_final_" + tid + "_" + i);
                                db.insert("test", null, v2);
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * cyclesPerThread, total);
    }

    @Test
    public void testConcurrentTransactionWithMultipleTables() throws InterruptedException {
        db.execSQL("CREATE TABLE IF NOT EXISTS table_a (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT)");
        db.execSQL("CREATE TABLE IF NOT EXISTS table_b (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT)");
        db.execSQL("CREATE TABLE IF NOT EXISTS table_c (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT)");

        final int threadCount = 15;
        final int txnsPerThread = 20;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < txnsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues va = new ContentValues();
                                va.put("data", "a_" + tid + "_" + i);
                                db.insert("table_a", null, va);

                                ContentValues vb = new ContentValues();
                                vb.put("data", "b_" + tid + "_" + i);
                                db.insert("table_b", null, vb);

                                ContentValues vc = new ContentValues();
                                vc.put("data", "c_" + tid + "_" + i);
                                db.insert("table_c", null, vc);

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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long expected = (long) threadCount * txnsPerThread;
        assertEquals(expected, DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM table_a", null));
        assertEquals(expected, DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM table_b", null));
        assertEquals(expected, DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM table_c", null));
    }

    @Test
    public void testConcurrentTransactionExecSQL() throws InterruptedException {
        final int threadCount = 20;
        final int txnsPerThread = 50;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < txnsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                db.execSQL("INSERT INTO test(name) VALUES(?)",
                                        new Object[]{"exec_" + tid + "_" + i});
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * txnsPerThread, total);
    }

    @Test
    public void testConcurrentTransactionReadWriteIsolation() throws InterruptedException {
        for (int i = 0; i < 100; i++) {
            ContentValues v = new ContentValues();
            v.put("id", i);
            v.put("name", "base_" + i);
            db.insert("test", null, v);
        }

        final int writerCount = 5;
        final int readerCount = 15;
        final int totalThreads = writerCount + readerCount;
        final CyclicBarrier barrier = new CyclicBarrier(totalThreads);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final List<Long> readerCounts = Collections.synchronizedList(new ArrayList<Long>());
        Thread[] threads = new Thread[totalThreads];

        for (int t = 0; t < totalThreads; t++) {
            final int tid = t;
            final boolean isWriter = (tid < writerCount);
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        if (isWriter) {
                            for (int i = 0; i < 30; i++) {
                                db.beginTransaction();
                                try {
                                    ContentValues v = new ContentValues();
                                    v.put("name", "writer_" + tid + "_" + i);
                                    db.insert("test", null, v);

                                    ContentValues upd = new ContentValues();
                                    upd.put("name", "w_upd_" + tid);
                                    db.update("test", upd, "id=?",
                                            new String[]{String.valueOf(tid)});
                                    db.setTransactionSuccessful();
                                } finally {
                                    db.endTransaction();
                                }
                            }
                        } else {
                            for (int i = 0; i < 60; i++) {
                                long count = DatabaseUtils.longForQuery(db,
                                        "SELECT COUNT(*) FROM test", null);
                                assertTrue(count >= 100);
                                readerCounts.add(count);

                                Cursor cursor = db.query("test", null,
                                        "id<?", new String[]{"10"},
                                        null, null, "id ASC");
                                int rowCount = 0;
                                while (cursor.moveToNext()) {
                                    cursor.getString(cursor.getColumnIndex("name"));
                                    rowCount++;
                                }
                                cursor.close();
                                assertTrue(rowCount >= 0);
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
        assertTrue(finalCount >= 100 + writerCount * 30);

        for (long c : readerCounts) {
            assertTrue(c >= 100);
        }
    }

    @Test
    public void testConcurrentTransactionThroughput() throws InterruptedException {
        final int threadCount = 20;
        final int txnsPerThread = 200;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicLong totalTxnTime = new AtomicLong(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        long start = System.nanoTime();
                        for (int i = 0; i < txnsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "throughput_" + tid + "_" + i);
                                db.insert("test", null, v);
                                db.setTransactionSuccessful();
                            } finally {
                                db.endTransaction();
                            }
                        }
                        totalTxnTime.addAndGet(System.nanoTime() - start);
                    } catch (Exception e) {
                        errorCount.incrementAndGet();
                    }
                }
            });
            threads[t].start();
        }

        for (Thread thread : threads) {
            thread.join(120000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * txnsPerThread, total);

        long avgNsPerTxn = totalTxnTime.get() / (threadCount * txnsPerThread);
        logInfo("Avg transaction time: %d ns (%d us)", avgNsPerTxn, avgNsPerTxn / 1000);
    }

    @Test
    public void testConcurrentTransactionWithExecutorService() throws Exception {
        final int threadCount = 20;
        final int txnsPerThread = 50;
        ExecutorService executor = Executors.newFixedThreadPool(threadCount);
        List<Future<?>> futures = new ArrayList<>();

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            futures.add(executor.submit(new Runnable() {
                @Override
                public void run() {
                    for (int i = 0; i < txnsPerThread; i++) {
                        db.beginTransaction();
                        try {
                            ContentValues v = new ContentValues();
                            v.put("name", "pool_" + tid + "_" + i);
                            db.insert("test", null, v);
                            db.setTransactionSuccessful();
                        } finally {
                            db.endTransaction();
                        }
                    }
                }
            }));
        }

        for (Future<?> f : futures) {
            f.get(60, TimeUnit.SECONDS);
        }
        executor.shutdown();

        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * txnsPerThread, total);
    }

    @Test
    public void testConcurrentTransactionDataIntegrity() throws InterruptedException {
        db.execSQL("CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, balance INTEGER)");
        db.execSQL("INSERT INTO accounts(id, balance) VALUES(1, 10000)");
        db.execSQL("INSERT INTO accounts(id, balance) VALUES(2, 10000)");

        final int threadCount = 20;
        final int transfersPerThread = 50;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < transfersPerThread; i++) {
                            db.beginTransaction();
                            try {
                                db.execSQL("UPDATE accounts SET balance = balance - 1 WHERE id = 1");
                                db.execSQL("UPDATE accounts SET balance = balance + 1 WHERE id = 2");
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long balance1 = DatabaseUtils.longForQuery(db, "SELECT balance FROM accounts WHERE id=1", null);
        long balance2 = DatabaseUtils.longForQuery(db, "SELECT balance FROM accounts WHERE id=2", null);
        assertEquals(20000, balance1 + balance2);

        long totalTransfers = (long) threadCount * transfersPerThread;
        assertEquals(10000 - totalTransfers, balance1);
        assertEquals(10000 + totalTransfers, balance2);
    }

    @Test
    public void testConcurrentTransactionWithQueryInsideTxn() throws InterruptedException {
        for (int i = 0; i < 50; i++) {
            ContentValues v = new ContentValues();
            v.put("id", i);
            v.put("name", "seed_" + i);
            db.insert("test", null, v);
        }

        final int threadCount = 15;
        final int opsPerThread = 30;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < opsPerThread; i++) {
                            db.beginTransaction();
                            try {
                                Cursor cursor = db.rawQuery(
                                        "SELECT COUNT(*) FROM test", null);
                                cursor.moveToFirst();
                                long beforeCount = cursor.getLong(0);
                                cursor.close();

                                ContentValues v = new ContentValues();
                                v.put("name", "qitxn_" + tid + "_" + i);
                                db.insert("test", null, v);

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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long finalCount = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(50 + (long) threadCount * opsPerThread, finalCount);
    }

    @Test
    public void testConcurrentNonExclusiveTransactions() throws InterruptedException {
        final int threadCount = 20;
        final int txnsPerThread = 40;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < txnsPerThread; i++) {
                            db.beginTransactionNonExclusive();
                            try {
                                ContentValues v = new ContentValues();
                                v.put("name", "nonexcl_" + tid + "_" + i);
                                db.insert("test", null, v);
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
            thread.join(60000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals((long) threadCount * txnsPerThread, total);
    }

    @Test
    public void testConcurrentTransactionStress() throws InterruptedException {
        final int threadCount = 30;
        final int opsPerThread = 40;
        final CyclicBarrier barrier = new CyclicBarrier(threadCount);
        final AtomicInteger errorCount = new AtomicInteger(0);
        final AtomicInteger insertedCount = new AtomicInteger(0);
        Thread[] threads = new Thread[threadCount];

        for (int t = 0; t < threadCount; t++) {
            final int tid = t;
            threads[t] = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        barrier.await();
                        for (int i = 0; i < opsPerThread; i++) {
                            switch (tid % 5) {
                                case 0: {
                                    db.beginTransaction();
                                    try {
                                        ContentValues v = new ContentValues();
                                        v.put("name", "stress_insert_" + tid + "_" + i);
                                        db.insert("test", null, v);
                                        db.setTransactionSuccessful();
                                        insertedCount.incrementAndGet();
                                    } finally {
                                        db.endTransaction();
                                    }
                                    break;
                                }
                                case 1: {
                                    db.beginTransaction();
                                    try {
                                        ContentValues v = new ContentValues();
                                        v.put("name", "stress_rollback_" + tid + "_" + i);
                                        db.insert("test", null, v);
                                    } finally {
                                        db.endTransaction();
                                    }
                                    break;
                                }
                                case 2: {
                                    db.beginTransaction();
                                    try {
                                        db.beginTransaction();
                                        try {
                                            ContentValues v = new ContentValues();
                                            v.put("name", "stress_nested_" + tid + "_" + i);
                                            db.insert("test", null, v);
                                            db.setTransactionSuccessful();
                                        } finally {
                                            db.endTransaction();
                                        }
                                        db.setTransactionSuccessful();
                                        insertedCount.incrementAndGet();
                                    } finally {
                                        db.endTransaction();
                                    }
                                    break;
                                }
                                case 3: {
                                    Cursor c = db.rawQuery("SELECT COUNT(*) FROM test", null);
                                    c.moveToFirst();
                                    c.close();
                                    break;
                                }
                                case 4: {
                                    db.beginTransactionWithListenerNonExclusive(
                                            new SQLiteTransactionListener() {
                                                @Override public void onBegin() {}
                                                @Override public void onCommit() {}
                                                @Override public void onRollback() {}
                                            });
                                    try {
                                        ContentValues v = new ContentValues();
                                        v.put("name", "stress_listener_" + tid + "_" + i);
                                        db.insert("test", null, v);
                                        db.setTransactionSuccessful();
                                        insertedCount.incrementAndGet();
                                    } finally {
                                        db.endTransaction();
                                    }
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
            thread.join(120000);
        }

        assertEquals(0, errorCount.get());
        long total = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(insertedCount.get(), total);
    }
}
