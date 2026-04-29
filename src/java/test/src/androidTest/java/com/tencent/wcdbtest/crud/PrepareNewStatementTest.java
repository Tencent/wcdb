// Created by chenqiuwen on 2026/3/1.
//

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

package com.tencent.wcdbtest.crud;

import static org.junit.Assert.*;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.PreparedStatement;
import com.tencent.wcdb.winq.BindParameter;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.StatementDelete;
import com.tencent.wcdb.winq.StatementInsert;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdb.winq.StatementUpdate;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.ValueCRUDTestCase;

import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

public class PrepareNewStatementTest extends ValueCRUDTestCase {

    @Test
    public void testBasicPrepareAndFinalize() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns())
                .from(tableName);
        PreparedStatement stmt = handle.prepareNewStatement(select);
        assertNotNull(stmt);
        assertTrue(stmt.checkPrepared());

        stmt.step();
        assertFalse(stmt.isDone());
        assertEquals(stmt.getInt(0), 1);

        handle.finalizeAndReturnPreparedStatement(stmt);
    }

    @Test
    public void testBasicPrepareWithSQL() throws WCDBException {
        String sql = "SELECT id, content FROM testTable";
        PreparedStatement stmt = handle.prepareNewStatement(sql);
        assertNotNull(stmt);
        assertTrue(stmt.checkPrepared());

        stmt.step();
        assertFalse(stmt.isDone());

        handle.finalizeAndReturnPreparedStatement(stmt);
    }

    @Test
    public void testInterleavedWithMainStatement() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns())
                .from(tableName);
        PreparedStatement cursor = handle.prepareNewStatement(select);

        cursor.step();
        assertFalse(cursor.isDone());
        int firstId = cursor.getInt(0);
        assertEquals(firstId, 1);

        StatementInsert insert = new StatementInsert()
                .insertInto(tableName)
                .columns(columns())
                .valuesWithBindParameters(2);
        PreparedStatement mainStmt = handle.preparedWithMainStatement(insert);
        mainStmt.bindInteger(3, 1);
        mainStmt.bindText(RandomTool.string(), 2);
        mainStmt.step();
        mainStmt.finalizeStatement();

        cursor.step();
        assertFalse(cursor.isDone());
        int secondId = cursor.getInt(0);
        assertEquals(secondId, 2);

        handle.finalizeAndReturnPreparedStatement(cursor);

        assertEquals(allRowsCount(), 3);
    }

    @Test
    public void testMultipleUncachedStatements() throws WCDBException {
        StatementSelect selectAll = new StatementSelect()
                .select(columns())
                .from(tableName);
        StatementSelect selectById = new StatementSelect()
                .select(new Column("content"))
                .from(tableName)
                .where(new Column("id").eq(new BindParameter(1)));

        PreparedStatement cursor1 = handle.prepareNewStatement(selectAll);
        PreparedStatement cursor2 = handle.prepareNewStatement(selectById);

        cursor1.step();
        assertFalse(cursor1.isDone());
        int id = cursor1.getInt(0);

        cursor2.bindInteger(id, 1);
        cursor2.step();
        assertFalse(cursor2.isDone());
        String content = cursor2.getText(0);
        assertNotNull(content);

        cursor1.step();
        assertFalse(cursor1.isDone());

        handle.finalizeAndReturnPreparedStatement(cursor2);
        handle.finalizeAndReturnPreparedStatement(cursor1);
    }

    @Test
    public void testUncachedNotCached() throws WCDBException {
        String sql = "SELECT id FROM testTable WHERE id = 1";

        PreparedStatement stmt1 = handle.prepareNewStatement(sql);
        PreparedStatement stmt2 = handle.prepareNewStatement(sql);

        assertNotSame(stmt1, stmt2);

        stmt1.step();
        assertFalse(stmt1.isDone());
        assertEquals(stmt1.getInt(0), 1);

        stmt2.step();
        assertFalse(stmt2.isDone());
        assertEquals(stmt2.getInt(0), 1);

        handle.finalizeAndReturnPreparedStatement(stmt1);
        handle.finalizeAndReturnPreparedStatement(stmt2);
    }

    @Test
    public void testInterleavedReadWrite() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns())
                .from(tableName)
                .orderBy(new Column("id").order(Order.Asc));
        StatementInsert insert = new StatementInsert()
                .insertInto(tableName)
                .columns(columns())
                .valuesWithBindParameters(2);

        PreparedStatement readCursor = handle.prepareNewStatement(select);
        PreparedStatement writeCursor = handle.prepareNewStatement(insert);

        List<Integer> readIds = new ArrayList<>();

        readCursor.step();
        while (!readCursor.isDone()) {
            readIds.add(readCursor.getInt(0));
            readCursor.step();
        }
        assertEquals(readIds.size(), 2);
        assertTrue(readIds.contains(1));
        assertTrue(readIds.contains(2));

        for (int i = 3; i <= 5; i++) {
            writeCursor.reset();
            writeCursor.bindInteger(i, 1);
            writeCursor.bindText(RandomTool.string(), 2);
            writeCursor.step();
        }

        handle.finalizeAndReturnPreparedStatement(readCursor);
        handle.finalizeAndReturnPreparedStatement(writeCursor);

        assertEquals(allRowsCount(), 5);
    }

    @Test
    public void testCursorLikePattern() throws WCDBException {
        for (int i = 3; i <= 10; i++) {
            database.insertRow(new Value[]{new Value(i), new Value(RandomTool.string())}, columns(), tableName);
        }

        StatementSelect select = new StatementSelect()
                .select(columns())
                .from(tableName)
                .orderBy(new Column("id").order(Order.Asc));
        PreparedStatement cursor = handle.prepareNewStatement(select);

        List<Value[]> allRows = new ArrayList<>();
        cursor.step();
        while (!cursor.isDone()) {
            allRows.add(cursor.getOneRow());
            cursor.step();
        }

        assertEquals(allRows.size(), 10);
        for (int i = 0; i < allRows.size(); i++) {
            assertEquals(allRows.get(i)[0].getLong(), i + 1);
        }

        handle.finalizeAndReturnPreparedStatement(cursor);
    }

    @Test
    public void testMixWithCachedStatement() throws WCDBException {
        StatementSelect selectAll = new StatementSelect()
                .select(columns())
                .from(tableName)
                .orderBy(new Column("id").order(Order.Asc));

        PreparedStatement uncached = handle.prepareNewStatement(selectAll);

        StatementInsert insert = new StatementInsert()
                .insertInto(tableName)
                .columns(columns())
                .valuesWithBindParameters(2);
        PreparedStatement cached = handle.getOrCreatePreparedStatement(insert);

        uncached.step();
        assertFalse(uncached.isDone());
        assertEquals(uncached.getInt(0), 1);

        cached.bindInteger(3, 1);
        cached.bindText("cached_insert", 2);
        cached.step();
        cached.finalizeStatement();

        uncached.step();
        assertFalse(uncached.isDone());
        assertEquals(uncached.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(uncached);

        assertEquals(allRowsCount(), 3);
    }

    @Test
    public void testColumnInfo() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns())
                .from(tableName);
        PreparedStatement stmt = handle.prepareNewStatement(select);

        assertEquals(stmt.getColumnCount(), 2);
        assertEquals(stmt.getColumnName(0), "id");
        assertEquals(stmt.getColumnName(1), "content");

        handle.finalizeAndReturnPreparedStatement(stmt);
    }

    @Test
    public void testFinalizeAllStatementsCleanup() throws WCDBException {
        PreparedStatement stmt1 = handle.prepareNewStatement(
                new StatementSelect().select(columns()).from(tableName));
        PreparedStatement stmt2 = handle.prepareNewStatement(
                new StatementSelect().select(new Column("id")).from(tableName));

        stmt1.step();
        stmt2.step();

        handle.finalizeAllStatements();

        assertFalse(stmt1.checkPrepared());
        assertFalse(stmt2.checkPrepared());
    }

    @Test
    public void testGetHandleWithNewStatement() throws WCDBException {
        Handle newHandle = database.getHandle();
        try {
            StatementSelect select = new StatementSelect()
                    .select(columns())
                    .from(tableName);
            PreparedStatement stmt = newHandle.prepareNewStatement(select);

            stmt.step();
            assertFalse(stmt.isDone());
            assertEquals(stmt.getInt(0), 1);

            newHandle.finalizeAndReturnPreparedStatement(stmt);
        } finally {
            newHandle.invalidate();
        }
    }

    @Test
    public void testAllThreeTypesCoexist() throws WCDBException {
        StatementSelect selectAll = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        StatementInsert insert = new StatementInsert()
                .insertInto(tableName).columns(columns()).valuesWithBindParameters(2);
        StatementSelect selectCount = new StatementSelect()
                .select(Column.all().count()).from(tableName);

        PreparedStatement uncached = handle.prepareNewStatement(selectAll);
        PreparedStatement cached = handle.getOrCreatePreparedStatement(selectCount);
        PreparedStatement main = handle.preparedWithMainStatement(insert);

        uncached.step();
        assertFalse(uncached.isDone());
        assertEquals(uncached.getInt(0), 1);

        cached.step();
        assertFalse(cached.isDone());
        assertEquals(cached.getLong(0), 2);
        cached.finalizeStatement();

        main.bindInteger(3, 1);
        main.bindText("three", 2);
        main.step();
        main.finalizeStatement();

        uncached.step();
        assertFalse(uncached.isDone());
        assertEquals(uncached.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(uncached);
        assertEquals(allRowsCount(), 3);
    }

    @Test
    public void testUncachedSurvivesMainReprepareCycles() throws WCDBException {
        StatementSelect selectAll = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        PreparedStatement cursor = handle.prepareNewStatement(selectAll);

        cursor.step();
        assertFalse(cursor.isDone());
        assertEquals(cursor.getInt(0), 1);

        for (int i = 3; i <= 5; i++) {
            StatementInsert insert = new StatementInsert()
                    .insertInto(tableName).columns(columns()).valuesWithBindParameters(2);
            PreparedStatement main = handle.preparedWithMainStatement(insert);
            main.bindInteger(i, 1);
            main.bindText("val_" + i, 2);
            main.step();
            main.finalizeStatement();
        }

        cursor.step();
        assertFalse(cursor.isDone());
        assertEquals(cursor.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(cursor);
        assertEquals(allRowsCount(), 5);
    }

    @Test
    public void testCachedAndUncachedSameSQLIndependent() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));

        PreparedStatement cached = handle.getOrCreatePreparedStatement(select);
        PreparedStatement uncached = handle.prepareNewStatement(select);

        assertNotSame(cached, uncached);

        cached.step();
        assertFalse(cached.isDone());
        assertEquals(cached.getInt(0), 1);

        uncached.step();
        assertFalse(uncached.isDone());
        assertEquals(uncached.getInt(0), 1);

        cached.step();
        assertFalse(cached.isDone());
        assertEquals(cached.getInt(0), 2);

        uncached.step();
        assertFalse(uncached.isDone());
        assertEquals(uncached.getInt(0), 2);

        cached.step();
        assertTrue(cached.isDone());
        uncached.step();
        assertTrue(uncached.isDone());

        cached.finalizeStatement();
        handle.finalizeAndReturnPreparedStatement(uncached);
    }

    @Test
    public void testMainWriteWhileMultipleUncachedRead() throws WCDBException {
        for (int i = 3; i <= 6; i++) {
            database.insertRow(new Value[]{new Value(i), new Value("content_" + i)}, columns(), tableName);
        }

        StatementSelect selectOdd = new StatementSelect()
                .select(columns()).from(tableName)
                .where(new Column("id").mod(2).eq(1))
                .orderBy(new Column("id").order(Order.Asc));
        StatementSelect selectEven = new StatementSelect()
                .select(columns()).from(tableName)
                .where(new Column("id").mod(2).eq(0))
                .orderBy(new Column("id").order(Order.Asc));

        PreparedStatement cursorOdd = handle.prepareNewStatement(selectOdd);
        PreparedStatement cursorEven = handle.prepareNewStatement(selectEven);

        List<Integer> oddIds = new ArrayList<>();
        List<Integer> evenIds = new ArrayList<>();

        cursorOdd.step();
        cursorEven.step();

        while (!cursorOdd.isDone() || !cursorEven.isDone()) {
            if (!cursorOdd.isDone()) {
                oddIds.add(cursorOdd.getInt(0));
                cursorOdd.step();
            }
            if (!cursorEven.isDone()) {
                evenIds.add(cursorEven.getInt(0));
                cursorEven.step();
            }

            if (oddIds.size() == 1) {
                StatementInsert insert = new StatementInsert()
                        .insertInto(tableName).columns(columns()).valuesWithBindParameters(2);
                PreparedStatement main = handle.preparedWithMainStatement(insert);
                main.bindInteger(7, 1);
                main.bindText("inserted_during_read", 2);
                main.step();
                main.finalizeStatement();
            }
        }

        assertEquals(oddIds.size(), 4);
        assertEquals(evenIds.size(), 3);

        handle.finalizeAndReturnPreparedStatement(cursorOdd);
        handle.finalizeAndReturnPreparedStatement(cursorEven);

        assertEquals(allRowsCount(), 7);
    }

    @Test
    public void testUncachedResetAndReuse() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns()).from(tableName)
                .where(new Column("id").eq(new BindParameter(1)));

        PreparedStatement stmt = handle.prepareNewStatement(select);

        stmt.bindInteger(1, 1);
        stmt.step();
        assertFalse(stmt.isDone());
        assertEquals(stmt.getInt(0), 1);

        stmt.reset();
        stmt.clearBindings();

        stmt.bindInteger(2, 1);
        stmt.step();
        assertFalse(stmt.isDone());
        assertEquals(stmt.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(stmt);
    }

    @Test
    public void testExecuteInterleaveWithUncached() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        PreparedStatement cursor = handle.prepareNewStatement(select);

        cursor.step();
        assertFalse(cursor.isDone());
        assertEquals(cursor.getInt(0), 1);

        handle.execute(new StatementInsert()
                .insertInto(tableName).columns(columns())
                .values(new Object[]{3, "via_execute"}));

        cursor.step();
        assertFalse(cursor.isDone());
        assertEquals(cursor.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(cursor);
        assertEquals(allRowsCount(), 3);
    }

    @Test
    public void testTransactionWithMixedStatements() throws WCDBException {
        handle.beginTransaction();

        StatementSelect selectAll = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        PreparedStatement uncachedRead = handle.prepareNewStatement(selectAll);

        StatementInsert insert = new StatementInsert()
                .insertInto(tableName).columns(columns()).valuesWithBindParameters(2);
        PreparedStatement cachedInsert = handle.getOrCreatePreparedStatement(insert);

        uncachedRead.step();
        assertFalse(uncachedRead.isDone());
        assertEquals(uncachedRead.getInt(0), 1);

        cachedInsert.bindInteger(3, 1);
        cachedInsert.bindText("in_txn_cached", 2);
        cachedInsert.step();
        cachedInsert.reset();

        PreparedStatement main = handle.preparedWithMainStatement(
                new StatementInsert().insertInto(tableName).columns(columns())
                        .values(new Object[]{4, "in_txn_main"}));
        main.step();
        main.finalizeStatement();

        StatementInsert insertUncached = new StatementInsert()
                .insertInto(tableName).columns(columns()).valuesWithBindParameters(2);
        PreparedStatement uncachedWrite = handle.prepareNewStatement(insertUncached);
        uncachedWrite.bindInteger(5, 1);
        uncachedWrite.bindText("in_txn_uncached", 2);
        uncachedWrite.step();

        uncachedRead.step();
        assertFalse(uncachedRead.isDone());
        assertEquals(uncachedRead.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(uncachedWrite);
        handle.finalizeAndReturnPreparedStatement(uncachedRead);
        cachedInsert.finalizeStatement();

        handle.commitTransaction();
        assertEquals(allRowsCount(), 5);
    }

    @Test
    public void testReverseFinalizationOrder() throws WCDBException {
        PreparedStatement s1 = handle.prepareNewStatement(
                new StatementSelect().select(columns()).from(tableName));
        PreparedStatement s2 = handle.prepareNewStatement(
                new StatementSelect().select(new Column("id")).from(tableName));
        PreparedStatement s3 = handle.prepareNewStatement(
                new StatementSelect().select(new Column("content")).from(tableName));

        s1.step();
        s2.step();
        s3.step();

        assertFalse(s1.isDone());
        assertFalse(s2.isDone());
        assertFalse(s3.isDone());

        assertEquals(s1.getColumnCount(), 2);
        assertEquals(s2.getColumnCount(), 1);
        assertEquals(s3.getColumnCount(), 1);

        handle.finalizeAndReturnPreparedStatement(s3);
        handle.finalizeAndReturnPreparedStatement(s1);
        handle.finalizeAndReturnPreparedStatement(s2);
    }

    @Test
    public void testUncachedUpdateInterleaveWithUncachedRead() throws WCDBException {
        StatementSelect select = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        StatementUpdate update = new StatementUpdate()
                .update(tableName)
                .set(new Column("content")).to(new BindParameter(1))
                .where(new Column("id").eq(new BindParameter(2)));

        PreparedStatement readCursor = handle.prepareNewStatement(select);
        PreparedStatement updateStmt = handle.prepareNewStatement(update);

        readCursor.step();
        assertFalse(readCursor.isDone());
        assertEquals(readCursor.getInt(0), 1);

        updateStmt.bindText("updated_content", 1);
        updateStmt.bindInteger(2, 2);
        updateStmt.step();

        readCursor.step();
        assertFalse(readCursor.isDone());
        assertEquals(readCursor.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(updateStmt);
        handle.finalizeAndReturnPreparedStatement(readCursor);

        Value updatedContent = database.getValue(
                new Column("content"), tableName, new Column("id").eq(2));
        assertEquals(updatedContent.getText(), "updated_content");
    }

    @Test
    public void testUncachedDeleteInterleaveWithCachedRead() throws WCDBException {
        database.insertRow(new Value[]{new Value(3), new Value("c3")}, columns(), tableName);
        assertEquals(allRowsCount(), 3);

        StatementSelect select = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        StatementDelete delete = new StatementDelete()
                .deleteFrom(tableName)
                .where(new Column("id").eq(new BindParameter(1)));

        PreparedStatement cachedRead = handle.getOrCreatePreparedStatement(select);
        PreparedStatement uncachedDelete = handle.prepareNewStatement(delete);

        cachedRead.step();
        assertFalse(cachedRead.isDone());
        assertEquals(cachedRead.getInt(0), 1);

        uncachedDelete.bindInteger(3, 1);
        uncachedDelete.step();

        cachedRead.step();
        assertFalse(cachedRead.isDone());
        assertEquals(cachedRead.getInt(0), 2);

        cachedRead.finalizeStatement();
        handle.finalizeAndReturnPreparedStatement(uncachedDelete);

        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testHeavyMixFiveStatements() throws WCDBException {
        StatementSelect select1 = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        StatementSelect select2 = new StatementSelect()
                .select(new Column("content")).from(tableName)
                .where(new Column("id").eq(new BindParameter(1)));
        StatementInsert insertStmt = new StatementInsert()
                .insertInto(tableName).columns(columns()).valuesWithBindParameters(2);

        PreparedStatement uncachedA = handle.prepareNewStatement(select1);
        PreparedStatement uncachedB = handle.prepareNewStatement(select2);
        PreparedStatement cached = handle.getOrCreatePreparedStatement(insertStmt);

        uncachedA.step();
        assertFalse(uncachedA.isDone());
        int firstId = uncachedA.getInt(0);
        assertEquals(firstId, 1);

        uncachedB.bindInteger(firstId, 1);
        uncachedB.step();
        assertFalse(uncachedB.isDone());
        String contentOfFirst = uncachedB.getText(0);
        assertNotNull(contentOfFirst);

        cached.bindInteger(3, 1);
        cached.bindText("heavy_mix_cached", 2);
        cached.step();
        cached.reset();

        PreparedStatement main = handle.preparedWithMainStatement(
                new StatementInsert().insertInto(tableName).columns(columns())
                        .values(new Object[]{4, "heavy_mix_main"}));
        main.step();
        main.finalizeStatement();

        PreparedStatement uncachedC = handle.prepareNewStatement(
                new StatementSelect().select(Column.all().count()).from(tableName));
        uncachedC.step();
        assertEquals(uncachedC.getLong(0), 4);

        uncachedA.step();
        assertFalse(uncachedA.isDone());
        assertEquals(uncachedA.getInt(0), 2);

        handle.finalizeAndReturnPreparedStatement(uncachedC);
        handle.finalizeAndReturnPreparedStatement(uncachedB);
        handle.finalizeAndReturnPreparedStatement(uncachedA);
        cached.finalizeStatement();

        assertEquals(allRowsCount(), 4);
    }

    @Test
    public void testUncachedReadDuringCachedBatchInsert() throws WCDBException {
        StatementInsert insert = new StatementInsert()
                .insertInto(tableName).columns(columns()).valuesWithBindParameters(2);
        PreparedStatement cachedInsert = handle.getOrCreatePreparedStatement(insert);

        for (int i = 3; i <= 10; i++) {
            cachedInsert.reset();
            cachedInsert.bindInteger(i, 1);
            cachedInsert.bindText("batch_" + i, 2);
            cachedInsert.step();
        }

        StatementSelect select = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));
        PreparedStatement uncachedRead = handle.prepareNewStatement(select);

        int count = 0;
        uncachedRead.step();
        while (!uncachedRead.isDone()) {
            count++;
            int id = uncachedRead.getInt(0);
            assertTrue(id >= 1 && id <= 10);
            uncachedRead.step();
        }
        assertEquals(count, 10);

        cachedInsert.reset();
        cachedInsert.bindInteger(11, 1);
        cachedInsert.bindText("batch_11", 2);
        cachedInsert.step();
        cachedInsert.finalizeStatement();

        handle.finalizeAndReturnPreparedStatement(uncachedRead);
        assertEquals(allRowsCount(), 11);
    }

    @Test
    public void testMultipleMainRepreparesWithUncachedAndCached() throws WCDBException {
        StatementSelect selectAll = new StatementSelect()
                .select(columns()).from(tableName).orderBy(new Column("id").order(Order.Asc));

        PreparedStatement uncached = handle.prepareNewStatement(selectAll);
        PreparedStatement cached = handle.getOrCreatePreparedStatement(selectAll);

        uncached.step();
        cached.step();
        assertFalse(uncached.isDone());
        assertFalse(cached.isDone());
        assertEquals(uncached.getInt(0), 1);
        assertEquals(cached.getInt(0), 1);

        PreparedStatement main = handle.preparedWithMainStatement(
                new StatementInsert().insertInto(tableName).columns(columns())
                        .values(new Object[]{3, "main_1"}));
        main.step();
        main.finalizeStatement();

        main = handle.preparedWithMainStatement(
                new StatementInsert().insertInto(tableName).columns(columns())
                        .values(new Object[]{4, "main_2"}));
        main.step();
        main.finalizeStatement();

        main = handle.preparedWithMainStatement(
                new StatementUpdate().update(tableName)
                        .set(new Column("content")).to("updated")
                        .where(new Column("id").eq(4)));
        main.step();
        main.finalizeStatement();

        uncached.step();
        cached.step();
        assertFalse(uncached.isDone());
        assertFalse(cached.isDone());
        assertEquals(uncached.getInt(0), 2);
        assertEquals(cached.getInt(0), 2);

        cached.finalizeStatement();
        handle.finalizeAndReturnPreparedStatement(uncached);

        assertEquals(allRowsCount(), 4);
        Value val = database.getValue(new Column("content"), tableName, new Column("id").eq(4));
        assertEquals(val.getText(), "updated");
    }

    @Test
    public void testRollbackWithMixedStatements() throws WCDBException {
        long countBefore = allRowsCount();

        handle.beginTransaction();

        PreparedStatement uncached = handle.prepareNewStatement(
                new StatementInsert().insertInto(tableName).columns(columns())
                        .valuesWithBindParameters(2));
        uncached.bindInteger(3, 1);
        uncached.bindText("will_rollback", 2);
        uncached.step();

        PreparedStatement main = handle.preparedWithMainStatement(
                new StatementInsert().insertInto(tableName).columns(columns())
                        .values(new Object[]{4, "also_rollback"}));
        main.step();
        main.finalizeStatement();

        handle.finalizeAndReturnPreparedStatement(uncached);
        handle.rollbackTransaction();

        assertEquals(allRowsCount(), countBefore);
    }

    @Test
    public void testCreateAfterFinalizeAllStatements() throws WCDBException {
        PreparedStatement s1 = handle.prepareNewStatement(
                new StatementSelect().select(columns()).from(tableName));
        s1.step();

        handle.finalizeAllStatements();
        assertFalse(s1.checkPrepared());

        PreparedStatement s2 = handle.prepareNewStatement(
                new StatementSelect().select(columns()).from(tableName));
        assertNotNull(s2);
        assertTrue(s2.checkPrepared());
        s2.step();
        assertFalse(s2.isDone());

        handle.finalizeAndReturnPreparedStatement(s2);
    }
}
