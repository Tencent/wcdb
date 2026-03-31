package com.tencent.wcdbtest.compat;

import static org.junit.Assert.*;

import android.database.Cursor;
import android.database.sqlite.SQLiteDoneException;

import com.tencent.wcdb.compat.SQLiteDatabase;
import com.tencent.wcdb.compat.SQLiteStatement;
import com.tencent.wcdbtest.base.BaseTestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.File;

public class CompatSQLiteStatementTest extends BaseTestCase {
    private SQLiteDatabase db;
    private String dbPath;

    @Before
    public void setup() {
        super.currentDirectory = tempFolder.getRoot().getPath() + File.separator + getClass().getSimpleName();
        new File(currentDirectory).mkdirs();
        dbPath = currentDirectory + File.separator + "stmt_test.db";
        db = SQLiteDatabase.openDatabase(dbPath);
        db.execSQL("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT, value REAL, data BLOB)");
    }

    @After
    public void teardown() {
        if (db != null) {
            db.close();
        }
    }

    @Test
    public void testCompileStatement() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");
        assertNotNull(stmt);
        stmt.close();
    }

    @Test
    public void testExecuteInsert() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");
        stmt.bindLong(1, 1);
        stmt.bindString(2, "hello");
        long rowId = stmt.executeInsert();
        assertTrue(rowId > 0);
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("hello", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testExecuteUpdateDelete() {
        db.execSQL("INSERT INTO test(id, name) VALUES(1, 'old')");

        SQLiteStatement stmt = db.compileStatement("UPDATE test SET name=? WHERE id=?");
        stmt.bindString(1, "new");
        stmt.bindLong(2, 1);
        int affected = stmt.executeUpdateDelete();
        assertEquals(1, affected);
        stmt.close();
    }

    @Test
    public void testExecute() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(1, 'exec_test')");
        stmt.execute();
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("exec_test", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testSimpleQueryForLong() {
        db.execSQL("INSERT INTO test(id, name) VALUES(1, 'a')");
        db.execSQL("INSERT INTO test(id, name) VALUES(2, 'b')");
        db.execSQL("INSERT INTO test(id, name) VALUES(3, 'c')");

        SQLiteStatement stmt = db.compileStatement("SELECT COUNT(*) FROM test");
        long count = stmt.simpleQueryForLong();
        assertEquals(3, count);
        stmt.close();
    }

    @Test
    public void testSimpleQueryForString() {
        db.execSQL("INSERT INTO test(id, name) VALUES(1, 'find_me')");

        SQLiteStatement stmt = db.compileStatement("SELECT name FROM test WHERE id=1");
        String result = stmt.simpleQueryForString();
        assertEquals("find_me", result);
        stmt.close();
    }

    @Test(expected = SQLiteDoneException.class)
    public void testSimpleQueryForLongNoResult() {
        SQLiteStatement stmt = db.compileStatement("SELECT id FROM test WHERE id=999999");
        try {
            stmt.simpleQueryForLong();
        } finally {
            stmt.close();
        }
    }

    @Test(expected = SQLiteDoneException.class)
    public void testSimpleQueryForStringNoResult() {
        SQLiteStatement stmt = db.compileStatement("SELECT name FROM test WHERE id=999999");
        try {
            stmt.simpleQueryForString();
        } finally {
            stmt.close();
        }
    }

    @Test
    public void testBindNull() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");
        stmt.bindLong(1, 1);
        stmt.bindNull(2);
        stmt.executeInsert();
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertTrue(cursor.isNull(0));
        cursor.close();
    }

    @Test
    public void testBindDouble() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, value) VALUES(?, ?)");
        stmt.bindLong(1, 1);
        stmt.bindDouble(2, 3.14159);
        stmt.executeInsert();
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT value FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(3.14159, cursor.getDouble(0), 0.00001);
        cursor.close();
    }

    @Test
    public void testBindBlob() {
        byte[] blob = new byte[]{10, 20, 30, 40, 50};
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, data) VALUES(?, ?)");
        stmt.bindLong(1, 1);
        stmt.bindBlob(2, blob);
        stmt.executeInsert();
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT data FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertArrayEquals(blob, cursor.getBlob(0));
        cursor.close();
    }

    @Test(expected = IllegalArgumentException.class)
    public void testBindStringNull() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");
        try {
            stmt.bindString(2, null);
        } finally {
            stmt.close();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testBindBlobNull() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, data) VALUES(?, ?)");
        try {
            stmt.bindBlob(2, null);
        } finally {
            stmt.close();
        }
    }

    @Test(expected = IllegalArgumentException.class)
    public void testBindInvalidIndex() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id) VALUES(?)");
        try {
            stmt.bindLong(0, 1);
        } finally {
            stmt.close();
        }
    }

    @Test
    public void testClearBindings() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");
        stmt.bindLong(1, 1);
        stmt.bindString(2, "first");
        stmt.executeInsert();

        stmt.clearBindings();
        stmt.bindLong(1, 2);
        stmt.bindString(2, "second");
        stmt.executeInsert();
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(2, cursor.getLong(0));
        cursor.close();
    }

    @Test
    public void testBindAllArgs() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");
        stmt.bindAllArgs(new Object[]{1L, "allargs"});
        stmt.executeInsert();
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("allargs", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testBindAllArgsNull() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id) VALUES(1)");
        stmt.bindAllArgs(null);
        stmt.executeInsert();
        stmt.close();
    }

    @Test
    public void testReuseStatementWithNewBindings() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");

        for (int i = 1; i <= 10; i++) {
            stmt.clearBindings();
            stmt.bindLong(1, i);
            stmt.bindString(2, "row" + i);
            stmt.executeInsert();
        }
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(10, cursor.getLong(0));
        cursor.close();
    }

    @Test
    public void testToString() {
        String sql = "SELECT * FROM test";
        SQLiteStatement stmt = db.compileStatement(sql);
        assertEquals("SQLiteProgram: " + sql, stmt.toString());
        stmt.close();
    }

    @Test
    public void testSimpleQueryForLongWithBind() {
        for (int i = 1; i <= 5; i++) {
            db.execSQL("INSERT INTO test(id, name) VALUES(" + i + ", 'groupA')");
        }
        for (int i = 6; i <= 8; i++) {
            db.execSQL("INSERT INTO test(id, name) VALUES(" + i + ", 'groupB')");
        }

        SQLiteStatement stmt = db.compileStatement("SELECT COUNT(*) FROM test WHERE name=?");
        stmt.bindString(1, "groupA");
        assertEquals(5, stmt.simpleQueryForLong());
        stmt.close();
    }

    @Test
    public void testStatementAutoClose() {
        try (SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(1, 'auto')")) {
            stmt.executeInsert();
        }

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("auto", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testBindOverwrite() {
        SQLiteStatement stmt = db.compileStatement("INSERT INTO test(id, name) VALUES(?, ?)");
        stmt.bindLong(1, 1);
        stmt.bindString(2, "original");
        stmt.bindString(2, "overwritten");
        stmt.executeInsert();
        stmt.close();

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=1", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("overwritten", cursor.getString(0));
        cursor.close();
    }
}
