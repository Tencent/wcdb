// Created by chenqiuwen on 2023/5/14.
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

import static org.junit.Assert.assertEquals;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.ObjectCRUDTestCase;
import com.tencent.wcdbtest.base.RandomTool;

import org.junit.Test;

public class ValueInsertWithFieldTest extends ObjectCRUDTestCase {
    @Test
    public void testDatabaseInsertRow() {
        Value[] row = RandomTool.testRowWithId(nextId);
        doTestSQL("INSERT INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        database.insertRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 3);
    }

    @Test
    public void testDatabaseInsertOrReplaceRow() {
        Value[] row = RandomTool.testRowWithId(nextId - 1);
        doTestSQL("INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        database.insertOrReplaceRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testDatabaseInsertOrIgnoreRow() {
        Value[] row = RandomTool.testRowWithId(nextId - 1);
        doTestSQL("INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        database.insertOrIgnoreRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testDatabaseInsertRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId), RandomTool.testRowWithId(nextId + 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        database.insertRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 4);
    }

    @Test
    public void testDatabaseInsertOrReplaceRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId-2), RandomTool.testRowWithId(nextId - 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        database.insertOrReplaceRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testDatabaseInsertOrIgnoreRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId-2), RandomTool.testRowWithId(nextId - 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        database.insertOrIgnoreRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testTableInsertRow() {
        Value[] row = RandomTool.testRowWithId(nextId);
        doTestSQL("INSERT INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        table.insertRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 3);
    }

    @Test
    public void testTableInsertOrReplaceRow() {
        Value[] row = RandomTool.testRowWithId(nextId - 1);
        doTestSQL("INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        table.insertOrReplaceRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testTableInsertOrIgnoreRow() {
        Value[] row = RandomTool.testRowWithId(nextId - 1);
        doTestSQL("INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        table.insertOrIgnoreRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testTableInsertRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId), RandomTool.testRowWithId(nextId + 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        table.insertRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 4);
    }

    @Test
    public void testTableInsertOrReplaceRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId-2), RandomTool.testRowWithId(nextId - 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        table.insertOrReplaceRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testTableInsertOrIgnoreRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId-2), RandomTool.testRowWithId(nextId - 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        table.insertOrIgnoreRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testHandleInsertRow() {
        Value[] row = RandomTool.testRowWithId(nextId);
        doTestSQL("INSERT INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        handle.insertRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 3);
    }

    @Test
    public void testHandleInsertOrReplaceRow() {
        Value[] row = RandomTool.testRowWithId(nextId - 1);
        doTestSQL("INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        handle.insertOrReplaceRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testHandleInsertOrIgnoreRow() {
        Value[] row = RandomTool.testRowWithId(nextId - 1);
        doTestSQL("INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)",
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        handle.insertOrIgnoreRow(row, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testHandleInsertRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId), RandomTool.testRowWithId(nextId + 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        handle.insertRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 4);
    }

    @Test
    public void testHandleInsertOrReplaceRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId-2), RandomTool.testRowWithId(nextId - 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        handle.insertOrReplaceRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }

    @Test
    public void testHandleInsertOrIgnoreRows() {
        Value[][] rows = new Value[][]{RandomTool.testRowWithId(nextId-2), RandomTool.testRowWithId(nextId - 1)};
        expectMode = Expect.SomeSQLs;
        doTestSQLs(new String[]{
                        "BEGIN IMMEDIATE",
                        "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)",
                        "COMMIT"
                },
                new TestOperation() {
                    @Override
                    public void execute() throws WCDBException {
                        handle.insertOrIgnoreRows(rows, DBTestObject.allFields());
                    }
                });
        assertEquals(allRowsCount(), 2);
    }
}
