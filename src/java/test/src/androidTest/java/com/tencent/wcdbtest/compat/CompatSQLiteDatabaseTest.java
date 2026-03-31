package com.tencent.wcdbtest.compat;

import static org.junit.Assert.*;

import android.content.ContentValues;
import android.database.Cursor;

import com.tencent.wcdb.compat.SQLiteDatabase;
import com.tencent.wcdbtest.base.BaseTestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.File;

public class CompatSQLiteDatabaseTest extends BaseTestCase {
    private SQLiteDatabase db;
    private String dbPath;

    @Before
    public void setup() {
        super.currentDirectory = tempFolder.getRoot().getPath() + File.separator + getClass().getSimpleName();
        new File(currentDirectory).mkdirs();
        dbPath = currentDirectory + File.separator + "compat_test.db";
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
    public void testOpenAndClose() {
        assertTrue(db.isOpen());
        db.close();
        assertFalse(db.isOpen());
        db = SQLiteDatabase.openDatabase(dbPath);
        assertTrue(db.isOpen());
    }

    @Test
    public void testGetPath() {
        String actualPath = db.getPath();
        assertNotNull(actualPath);
        assertTrue(actualPath.endsWith("compat_test.db"));
    }

    @Test
    public void testToString() {
        String str = db.toString();
        assertTrue(str.startsWith("SQLiteDatabase: "));
        assertTrue(str.endsWith("compat_test.db"));
    }

    @Test
    public void testGetAndSetVersion() {
        assertEquals(0, db.getVersion());
        db.setVersion(42);
        assertEquals(42, db.getVersion());
        db.setVersion(0);
        assertEquals(0, db.getVersion());
    }

    @Test
    public void testGetPageSize() {
        long pageSize = db.getPageSize();
        assertTrue(pageSize > 0);
        assertTrue((pageSize & (pageSize - 1)) == 0);
    }

    @Test
    public void testGetMaximumSize() {
        long maxSize = db.getMaximumSize();
        assertTrue(maxSize > 0);
    }

    @Test
    public void testWrap() {
        SQLiteDatabase wrapped = SQLiteDatabase.wrap(db.getInnerDB());
        assertNotNull(wrapped);
        assertEquals(db.getPath(), wrapped.getPath());
    }

    @Test
    public void testGetInnerDB() {
        assertNotNull(db.getInnerDB());
    }

    @Test
    public void testInsert() {
        ContentValues values = new ContentValues();
        values.put("name", "Alice");
        values.put("value", 3.14);
        long rowId = db.insert("test", null, values);
        assertTrue(rowId > 0);
    }

    @Test
    public void testInsertAndQuery() {
        ContentValues values = new ContentValues();
        values.put("name", "Bob");
        values.put("value", 2.71);
        db.insert("test", null, values);

        Cursor cursor = db.query("test", null, "name=?", new String[]{"Bob"},
                null, null, null);
        assertNotNull(cursor);
        assertTrue(cursor.moveToFirst());
        assertEquals("Bob", cursor.getString(cursor.getColumnIndex("name")));
        assertEquals(2.71, cursor.getDouble(cursor.getColumnIndex("value")), 0.001);
        assertEquals(1, cursor.getCount());
        cursor.close();
    }

    @Test
    public void testInsertOrThrow() {
        ContentValues values = new ContentValues();
        values.put("name", "Charlie");
        long rowId = db.insertOrThrow("test", null, values);
        assertTrue(rowId > 0);
    }

    @Test
    public void testInsertWithNullColumnHack() {
        long rowId = db.insert("test", "name", new ContentValues());
        assertTrue(rowId > 0);

        Cursor cursor = db.rawQuery("SELECT * FROM test WHERE id=?",
                new Object[]{rowId});
        assertTrue(cursor.moveToFirst());
        assertTrue(cursor.isNull(cursor.getColumnIndex("name")));
        cursor.close();
    }

    @Test
    public void testInsertWithConflict() {
        ContentValues values = new ContentValues();
        values.put("id", 100);
        values.put("name", "Original");
        db.insertWithOnConflict("test", null, values, SQLiteDatabase.CONFLICT_NONE);

        values.put("name", "Replaced");
        db.insertWithOnConflict("test", null, values, SQLiteDatabase.CONFLICT_REPLACE);

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=100", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("Replaced", cursor.getString(0));
        assertEquals(1, cursor.getCount());
        cursor.close();
    }

    @Test
    public void testInsertWithConflictIgnore() {
        ContentValues values = new ContentValues();
        values.put("id", 200);
        values.put("name", "First");
        db.insertWithOnConflict("test", null, values, SQLiteDatabase.CONFLICT_NONE);

        values.put("name", "Second");
        db.insertWithOnConflict("test", null, values, SQLiteDatabase.CONFLICT_IGNORE);

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=200", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("First", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testReplace() {
        ContentValues values = new ContentValues();
        values.put("id", 300);
        values.put("name", "V1");
        db.insert("test", null, values);

        values.put("name", "V2");
        long rowId = db.replace("test", null, values);
        assertTrue(rowId > 0);

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=300", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("V2", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testUpdate() {
        ContentValues insert = new ContentValues();
        insert.put("id", 400);
        insert.put("name", "Before");
        db.insert("test", null, insert);

        ContentValues update = new ContentValues();
        update.put("name", "After");
        int affected = db.update("test", update, "id=?", new String[]{"400"});
        assertEquals(1, affected);

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=400", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("After", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testUpdateWithConflict() {
        ContentValues v1 = new ContentValues();
        v1.put("id", 500);
        v1.put("name", "Name500");
        db.insert("test", null, v1);

        ContentValues update = new ContentValues();
        update.put("name", "Updated500");
        int affected = db.updateWithOnConflict("test", update, "id=?",
                new String[]{"500"}, SQLiteDatabase.CONFLICT_ABORT);
        assertEquals(1, affected);
    }

    @Test
    public void testDelete() {
        for (int i = 1; i <= 5; i++) {
            ContentValues values = new ContentValues();
            values.put("id", 600 + i);
            values.put("name", "row" + i);
            db.insert("test", null, values);
        }

        int affected = db.delete("test", "id>?", new String[]{"603"});
        assertEquals(2, affected);

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test WHERE id>=601 AND id<=605", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(3, cursor.getLong(0));
        cursor.close();
    }

    @Test
    public void testDeleteAll() {
        ContentValues values = new ContentValues();
        values.put("name", "temp");
        db.insert("test", null, values);

        int affected = db.delete("test", null, null);
        assertTrue(affected >= 1);

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(0, cursor.getLong(0));
        cursor.close();
    }

    @Test
    public void testQueryWithAllParams() {
        for (int i = 0; i < 20; i++) {
            ContentValues values = new ContentValues();
            values.put("name", "group" + (i % 3));
            values.put("value", (double) i);
            db.insert("test", null, values);
        }

        Cursor cursor = db.query(true, "test",
                new String[]{"name", "COUNT(*) AS cnt"},
                "value>=?", new String[]{"5"},
                "name", "cnt>1", "name ASC", "2");
        assertNotNull(cursor);
        assertTrue(cursor.getCount() <= 2);
        cursor.close();
    }

    @Test
    public void testQueryDistinct() {
        for (int i = 0; i < 10; i++) {
            ContentValues values = new ContentValues();
            values.put("name", "dup");
            db.insert("test", null, values);
        }

        Cursor cursor = db.query(true, "test", new String[]{"name"},
                null, null, null, null, null, null);
        assertNotNull(cursor);
        assertTrue(cursor.moveToFirst());
        assertEquals(1, cursor.getCount());
        cursor.close();
    }

    @Test
    public void testRawQuery() {
        ContentValues values = new ContentValues();
        values.put("id", 700);
        values.put("name", "raw");
        db.insert("test", null, values);

        Cursor cursor = db.rawQuery("SELECT * FROM test WHERE id=?", new Object[]{700});
        assertNotNull(cursor);
        assertTrue(cursor.moveToFirst());
        assertEquals("raw", cursor.getString(cursor.getColumnIndex("name")));
        cursor.close();
    }

    @Test
    public void testRawQueryNoResult() {
        Cursor cursor = db.rawQuery("SELECT * FROM test WHERE id=999999", null);
        assertNotNull(cursor);
        assertFalse(cursor.moveToFirst());
        assertEquals(0, cursor.getCount());
        cursor.close();
    }

    @Test
    public void testExecSQL() {
        db.execSQL("INSERT INTO test(id, name) VALUES(800, 'exec')");
        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=800", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("exec", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testExecSQLWithBindArgs() {
        db.execSQL("INSERT INTO test(id, name) VALUES(?, ?)", new Object[]{801, "bound"});
        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=801", null);
        assertTrue(cursor.moveToFirst());
        assertEquals("bound", cursor.getString(0));
        cursor.close();
    }

    @Test
    public void testInsertBlob() {
        byte[] blob = new byte[]{1, 2, 3, 4, 5};
        ContentValues values = new ContentValues();
        values.put("id", 900);
        values.put("data", blob);
        db.insert("test", null, values);

        Cursor cursor = db.rawQuery("SELECT data FROM test WHERE id=900", null);
        assertTrue(cursor.moveToFirst());
        assertArrayEquals(blob, cursor.getBlob(0));
        cursor.close();
    }

    @Test
    public void testInsertNull() {
        ContentValues values = new ContentValues();
        values.put("id", 901);
        values.putNull("name");
        db.insert("test", null, values);

        Cursor cursor = db.rawQuery("SELECT name FROM test WHERE id=901", null);
        assertTrue(cursor.moveToFirst());
        assertTrue(cursor.isNull(0));
        cursor.close();
    }

    @Test
    public void testCursorIteration() {
        for (int i = 0; i < 50; i++) {
            ContentValues values = new ContentValues();
            values.put("id", 1000 + i);
            values.put("name", "iter" + i);
            db.insert("test", null, values);
        }

        Cursor cursor = db.rawQuery("SELECT id, name FROM test WHERE id>=1000 AND id<1050 ORDER BY id", null);
        assertEquals(50, cursor.getCount());
        int index = 0;
        while (cursor.moveToNext()) {
            assertEquals(1000 + index, cursor.getInt(0));
            assertEquals("iter" + index, cursor.getString(1));
            index++;
        }
        assertEquals(50, index);
        cursor.close();
    }

    @Test
    public void testCursorColumnNames() {
        ContentValues values = new ContentValues();
        values.put("id", 1100);
        values.put("name", "coltest");
        values.put("value", 1.0);
        db.insert("test", null, values);

        Cursor cursor = db.rawQuery("SELECT id, name, value FROM test WHERE id=1100", null);
        assertEquals(3, cursor.getColumnCount());
        assertEquals(0, cursor.getColumnIndex("id"));
        assertEquals(1, cursor.getColumnIndex("name"));
        assertEquals(2, cursor.getColumnIndex("value"));
        cursor.close();
    }

    @Test
    public void testReleaseMemory() {
        int released = SQLiteDatabase.releaseMemory();
        assertEquals(0, released);
    }

    @Test
    public void testBulkInsertPerformance() {
        db.execSQL("CREATE TABLE IF NOT EXISTS bulk_test (id INTEGER PRIMARY KEY, content TEXT)");
        db.beginTransaction();
        try {
            for (int i = 0; i < 1000; i++) {
                ContentValues values = new ContentValues();
                values.put("id", i);
                values.put("content", "content_" + i);
                db.insert("bulk_test", null, values);
            }
            db.setTransactionSuccessful();
        } finally {
            db.endTransaction();
        }

        Cursor cursor = db.rawQuery("SELECT COUNT(*) FROM bulk_test", null);
        assertTrue(cursor.moveToFirst());
        assertEquals(1000, cursor.getLong(0));
        cursor.close();
    }
}
