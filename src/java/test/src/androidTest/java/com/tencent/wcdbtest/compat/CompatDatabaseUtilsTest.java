package com.tencent.wcdbtest.compat;

import static org.junit.Assert.*;

import android.database.sqlite.SQLiteDoneException;

import com.tencent.wcdb.compat.DatabaseUtils;
import com.tencent.wcdb.compat.SQLiteDatabase;
import com.tencent.wcdbtest.base.BaseTestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.File;

public class CompatDatabaseUtilsTest extends BaseTestCase {
    private SQLiteDatabase db;
    private String dbPath;

    @Before
    public void setup() {
        super.currentDirectory = tempFolder.getRoot().getPath() + File.separator + getClass().getSimpleName();
        new File(currentDirectory).mkdirs();
        dbPath = currentDirectory + File.separator + "utils_test.db";
        db = SQLiteDatabase.openDatabase(dbPath);
        db.execSQL("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT, score REAL)");
    }

    @After
    public void teardown() {
        if (db != null) {
            db.close();
        }
    }

    @Test
    public void testLongForQuery() {
        db.execSQL("INSERT INTO test(id, name) VALUES(1, 'a')");
        db.execSQL("INSERT INTO test(id, name) VALUES(2, 'b')");
        db.execSQL("INSERT INTO test(id, name) VALUES(3, 'c')");

        long count = DatabaseUtils.longForQuery(db, "SELECT COUNT(*) FROM test", null);
        assertEquals(3, count);
    }

    @Test
    public void testLongForQueryWithArgs() {
        for (int i = 1; i <= 10; i++) {
            db.execSQL("INSERT INTO test(id, name) VALUES(?, ?)",
                    new Object[]{i, (i % 2 == 0) ? "even" : "odd"});
        }

        long count = DatabaseUtils.longForQuery(db,
                "SELECT COUNT(*) FROM test WHERE name=?",
                new Object[]{"even"});
        assertEquals(5, count);
    }

    @Test
    public void testStringForQuery() {
        db.execSQL("INSERT INTO test(id, name) VALUES(1, 'target')");

        String result = DatabaseUtils.stringForQuery(db,
                "SELECT name FROM test WHERE id=?",
                new Object[]{1});
        assertEquals("target", result);
    }

    @Test
    public void testStringForQueryNoArgs() {
        db.execSQL("INSERT INTO test(id, name) VALUES(1, 'only_one')");

        String result = DatabaseUtils.stringForQuery(db,
                "SELECT name FROM test LIMIT 1", null);
        assertEquals("only_one", result);
    }

    @Test(expected = SQLiteDoneException.class)
    public void testLongForQueryNoResult() {
        DatabaseUtils.longForQuery(db, "SELECT id FROM test WHERE id=999", null);
    }

    @Test(expected = SQLiteDoneException.class)
    public void testStringForQueryNoResult() {
        DatabaseUtils.stringForQuery(db, "SELECT name FROM test WHERE id=999", null);
    }

    @Test
    public void testLongForQueryMaxMin() {
        for (int i = 1; i <= 100; i++) {
            db.execSQL("INSERT INTO test(id, name) VALUES(?, ?)", new Object[]{i, "n" + i});
        }

        long max = DatabaseUtils.longForQuery(db, "SELECT MAX(id) FROM test", null);
        assertEquals(100, max);

        long min = DatabaseUtils.longForQuery(db, "SELECT MIN(id) FROM test", null);
        assertEquals(1, min);
    }

    @Test
    public void testLongForQuerySum() {
        db.execSQL("INSERT INTO test(id, score) VALUES(1, 10.5)");
        db.execSQL("INSERT INTO test(id, score) VALUES(2, 20.5)");

        long sum = DatabaseUtils.longForQuery(db, "SELECT CAST(SUM(score) AS INTEGER) FROM test", null);
        assertEquals(31, sum);
    }
}
