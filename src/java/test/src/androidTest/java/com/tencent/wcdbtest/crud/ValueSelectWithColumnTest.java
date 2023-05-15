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

import static org.junit.Assert.assertTrue;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.ResultColumnConvertible;
import com.tencent.wcdbtest.base.ValueCRUDTestCase;

import org.junit.Test;

public class ValueSelectWithColumnTest extends ValueCRUDTestCase {
    @Test
    public void testDatabaseGetValue() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getValue(content, tableName).getText().length() > 0);
                assertTrue(database.getValue(content, tableName, id.eq(1)).getText().length() > 0);
                assertTrue(database.getValue(content.count(), tableName, content.order(Order.Asc)).getInteger() == 2);
                assertTrue(database.getValue(id.sum(), tableName, content.order(Order.Asc), 0).getInteger() > 0);
                assertTrue(database.getValue(content.count(), tableName, id.eq(2), content.order(Order.Asc)).getInteger() == 1);
                assertTrue(database.getValue(id.sum(), tableName, id.eq(2), content.order(Order.Asc), 0).getInteger() > 0);
            }
        });
    }

    @Test
    public void testDatabaseGetColumn() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT id FROM testTable ORDER BY content ASC",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getOneColumn(content, tableName).size() == 2);
                assertTrue(database.getOneColumn(content, tableName, id.eq(1)).get(0).getText().length() > 0);
                assertTrue(database.getOneColumn(id, tableName, content.order(Order.Asc)).size() == 2);
                assertTrue(database.getOneColumn(id, tableName, content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getOneColumn(id, tableName, content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(database.getOneColumn(content, tableName, id.eq(2), content.order(Order.Asc)).size() == 1);
                assertTrue(database.getOneColumn(content, tableName, id.eq(2), content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getOneColumn(content, tableName, id.eq(2), content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testDatabaseGetRow() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT id, COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getOneRow(columns(), tableName).length == 2);
                assertTrue(database.getOneRow(columns(), tableName, id.eq(1))[0].getInteger() == 1);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{id, content.count()}, tableName, content.order(Order.Asc))[1].getInteger() == 2);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{id.sum(), content}, tableName, content.order(Order.Asc), 0)[0].getInteger() > 0);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{id, content.count()}, tableName, id.eq(2), content.order(Order.Asc))[1].getInteger() == 1);
                assertTrue(database.getOneRow(new ResultColumnConvertible[]{id.sum(), content}, tableName, id.eq(2), content.order(Order.Asc), 0)[0].getInteger() > 0);
            }
        });
    }

    @Test
    public void testDatabaseGetRows() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, content FROM testTable ORDER BY content ASC",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(database.getAllRows(columns(), tableName).size() == 2);
                assertTrue(database.getAllRows(columns(), tableName, id.eq(1)).get(0)[1].getText().length() > 0);
                assertTrue(database.getAllRows(columns(), tableName, content.order(Order.Asc)).size() == 2);
                assertTrue(database.getAllRows(columns(), tableName, content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getAllRows(columns(), tableName, content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(database.getAllRows(columns(), tableName, id.eq(2), content.order(Order.Asc)).size() == 1);
                assertTrue(database.getAllRows(columns(), tableName, id.eq(2), content.order(Order.Asc), 1).size() == 1);
                assertTrue(database.getAllRows(columns(), tableName, id.eq(2), content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testHandleGetValue() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id) FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getValue(content, tableName).getText().length() > 0);
                assertTrue(handle.getValue(content, tableName, id.eq(1)).getText().length() > 0);
                assertTrue(handle.getValue(content.count(), tableName, content.order(Order.Asc)).getInteger() == 2);
                assertTrue(handle.getValue(id.sum(), tableName, content.order(Order.Asc), 0).getInteger() > 0);
                assertTrue(handle.getValue(content.count(), tableName, id.eq(2), content.order(Order.Asc)).getInteger() == 1);
                assertTrue(handle.getValue(id.sum(), tableName, id.eq(2), content.order(Order.Asc), 0).getInteger() > 0);
            }
        });
    }

    @Test
    public void testHandleGetColumn() {
        doTestSQLs(new String[]{
                "SELECT content FROM testTable",
                "SELECT content FROM testTable WHERE id == 1",
                "SELECT id FROM testTable ORDER BY content ASC",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getOneColumn(content, tableName).size() == 2);
                assertTrue(handle.getOneColumn(content, tableName, id.eq(1)).get(0).getText().length() > 0);
                assertTrue(handle.getOneColumn(id, tableName, content.order(Order.Asc)).size() == 2);
                assertTrue(handle.getOneColumn(id, tableName, content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getOneColumn(id, tableName, content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(handle.getOneColumn(content, tableName, id.eq(2), content.order(Order.Asc)).size() == 1);
                assertTrue(handle.getOneColumn(content, tableName, id.eq(2), content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getOneColumn(content, tableName, id.eq(2), content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }

    @Test
    public void testHandleGetRow() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, COUNT(content) FROM testTable ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 0",
                "SELECT id, COUNT(content) FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT SUM(id), content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 0"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getOneRow(columns(), tableName).length == 2);
                assertTrue(handle.getOneRow(columns(), tableName, id.eq(1))[0].getInteger() == 1);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{id, content.count()}, tableName, content.order(Order.Asc))[1].getInteger() == 2);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{id.sum(), content}, tableName, content.order(Order.Asc), 0)[0].getInteger() > 0);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{id, content.count()}, tableName, id.eq(2), content.order(Order.Asc))[1].getInteger() == 1);
                assertTrue(handle.getOneRow(new ResultColumnConvertible[]{id.sum(), content}, tableName, id.eq(2), content.order(Order.Asc), 0)[0].getInteger() > 0);
            }
        });
    }

    @Test
    public void testHandleGetRows() {
        doTestSQLs(new String[]{
                "SELECT id, content FROM testTable",
                "SELECT id, content FROM testTable WHERE id == 1",
                "SELECT id, content FROM testTable ORDER BY content ASC",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable ORDER BY content ASC LIMIT 1 OFFSET 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1",
                "SELECT id, content FROM testTable WHERE id == 2 ORDER BY content ASC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertTrue(handle.getAllRows(columns(), tableName).size() == 2);
                assertTrue(handle.getAllRows(columns(), tableName, id.eq(1)).get(0)[1].getText().length() > 0);
                assertTrue(handle.getAllRows(columns(), tableName, content.order(Order.Asc)).size() == 2);
                assertTrue(handle.getAllRows(columns(), tableName, content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getAllRows(columns(), tableName, content.order(Order.Asc), 1, 1).size() == 1);
                assertTrue(handle.getAllRows(columns(), tableName, id.eq(2), content.order(Order.Asc)).size() == 1);
                assertTrue(handle.getAllRows(columns(), tableName, id.eq(2), content.order(Order.Asc), 1).size() == 1);
                assertTrue(handle.getAllRows(columns(), tableName, id.eq(2), content.order(Order.Asc), 1, 1).size() == 0);
            }
        });
    }
}
