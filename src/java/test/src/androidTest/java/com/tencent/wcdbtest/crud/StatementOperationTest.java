// Created by chenqiuwen on 2023/5/12.
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

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.PreparedStatement;
import com.tencent.wcdb.winq.BindParameter;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Pragma;
import com.tencent.wcdb.winq.StatementDelete;
import com.tencent.wcdb.winq.StatementInsert;
import com.tencent.wcdb.winq.StatementPragma;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.ValueCRUDTestCase;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.util.Random;

public class StatementOperationTest extends ValueCRUDTestCase {
    StatementSelect select;
    StatementInsert insert;

    @Before
    public void setup() throws WCDBException {
        super.setup();
        insert = new StatementInsert().insertInto(tableName).columns(columns()).values(new Object[]{3, new BindParameter(1)});
        select = new StatementSelect().select(new Column("content")).from(tableName).where(new Column("id").eq(3));
    }

    @Test
    public void testExecute() {
        doTestSQL("PRAGMA user_version = 123", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle = database.getHandle();
                handle.execute(new StatementPragma().pragma(Pragma.userVersion()).toValue(123));
            }
        });
    }

    @Test
    public void testStep() {
        doTestSQLs(new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)",
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                StatementInsert insert = new StatementInsert().insertInto(tableName).columns(columns()).valuesWithBindParameters(2);
                PreparedStatement preparedStatement = handle.getOrCreatePreparedStatement(insert);
                preparedStatement.bindInteger(3, 1);
                preparedStatement.bindText(RandomTool.string(), 2);
                preparedStatement.step();
                assertTrue(preparedStatement.isDone());
                preparedStatement.reset();
                preparedStatement.bindRow(RandomTool.testRowWithId(4));
                preparedStatement.step();
                assertTrue(preparedStatement.isDone());
                preparedStatement.finalizeStatement();
            }
        });
    }

    @Test
    public void testReadOnly() throws WCDBException {
        PreparedStatement statement1 = handle.preparedWithMainStatement(new StatementPragma().pragma(Pragma.userVersion()));
        PreparedStatement statement2 = handle.getOrCreatePreparedStatement(new StatementPragma().pragma(Pragma.userVersion()).toValue(123));
        assertTrue(statement1.isReadOnly());
        assertFalse(statement2.isReadOnly());
        statement1.finalizeStatement();
    }

    @Test
    public void testGetChanges() throws WCDBException {
        handle.execute(new StatementDelete().deleteFrom(tableName));
        assertEquals(handle.getChanges(), rows.length);
    }

    @Test
    public void testGetLastInsertRowId() throws WCDBException {
        StatementInsert insert = new StatementInsert().insertInto(tableName).columns(columns()).values(new Object[]{null, RandomTool.string()});
        handle.execute(insert);
        assertEquals(handle.getLastInsertedRowId(), nextId);
    }

    @Test
    public void testInteger() throws WCDBException {
        long value = new Random().nextLong();
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindInteger(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(prepareSelect.getInteger(0), value);
    }

    @Test
    public void testIntegerValue() throws WCDBException {
        Value value = new Value(new Random().nextLong());
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindValue(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(prepareSelect.getValue(0), value);
    }

    @Test
    public void testDouble() throws WCDBException {
        double value = new Random().nextDouble();
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindDouble(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(prepareSelect.getDouble(0), value, 0.0001);
    }

    @Test
    public void testDoubleValue() throws WCDBException {
        Value value = new Value(1.2D);
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindValue(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(value, prepareSelect.getValue(0));
    }

    @Test
    public void testNull() throws WCDBException {
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindNull(1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertNull(prepareSelect.getValue(0));
    }

    @Test
    public void testNullValue() throws WCDBException {
        Value value = new Value();
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindValue(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(prepareSelect.getValue(0), value);
    }

    @Test
    public void testString() throws WCDBException {
        String value = RandomTool.string();
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindText(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(prepareSelect.getText(0), value);
    }

    @Test
    public void testStringValue() throws WCDBException {
        Value value = new Value(RandomTool.string());
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindValue(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(prepareSelect.getValue(0), value);
    }

    @Test
    public void testBLOB() throws WCDBException {
        byte[] value = RandomTool.bytes();
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindBLOB(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertArrayEquals(prepareSelect.getBLOB(0), value);
    }

    @Test
    public void testBLOBValue() throws WCDBException {
        Value value = new Value(RandomTool.bytes());
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindValue(value, 1);
        preparedInsert.step();
        assertTrue(preparedInsert.isDone());
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertFalse(prepareSelect.isDone());
        assertEquals(prepareSelect.getValue(0), value);
    }

    @Test
    public void testBindIndex() throws WCDBException {
        String value = RandomTool.string();
        BindParameter parameter = BindParameter.colon("data");
        StatementInsert insert = new StatementInsert().insertInto(tableName)
                .columns(new String[]{"id", "content"})
                .values(new Object[]{3, parameter});
        PreparedStatement preparedInsert = handle.getOrCreatePreparedStatement(insert);
        preparedInsert.bindText(value, preparedInsert.bindParameterIndex(parameter.getDescription()));
        preparedInsert.step();
        preparedInsert.finalizeStatement();

        PreparedStatement prepareSelect = handle.getOrCreatePreparedStatement(select);
        prepareSelect.step();
        assertEquals(prepareSelect.getText(0), value);
        prepareSelect.finalizeStatement();
    }
}
