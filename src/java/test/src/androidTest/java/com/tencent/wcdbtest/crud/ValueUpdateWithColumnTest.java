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
import static org.junit.Assert.assertTrue;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdbtest.base.ValueCRUDTestCase;

import org.junit.Test;

public class ValueUpdateWithColumnTest extends ValueCRUDTestCase {
    @Test
    public void testDatabaseUpdateValue() throws WCDBException {
        doTestSQL("UPDATE testTable SET content = ?1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateValue("newContent", content, tableName);
            }
        });
        assertEquals(database.getValue(content, tableName).getText(), "newContent");
    }

    @Test
    public void testDatabaseUpdateValueWhere() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET content = ?1 WHERE id == 1",
                "UPDATE testTable SET id = ?1 WHERE content == 'newContent'"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateValue("newContent", content, tableName, id.eq(1));
                database.updateValue(nextId, id, tableName, content.eq("newContent"));
            }
        });
        assertEquals(database.getValue(content, tableName, id.eq(nextId)).getText(), "newContent");
        assertEquals(database.getValue(id, tableName, content.eq("newContent")).getInteger(), nextId);
    }

    @Test
    public void testDatabaseUpdateValueWhereOrderLimit() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id ASC LIMIT 1",
                "UPDATE testTable SET id = ?1 ORDER BY content ASC LIMIT 1"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateValue("newContent", content, tableName, id.gt(0), id.order(Order.Asc), 1);
                database.updateValue(nextId, id, tableName, content.order(Order.Asc), 1);
            }
        });
        assertEquals(database.getValue(content.count(), tableName, id.eq(nextId)).getInteger(), 1);
        assertEquals(database.getValue(id.count(), tableName, content.eq("newContent")).getInteger(), 1);
    }

    @Test
    public void testDatabaseUpdateValueWhereOrderLimitOffset() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id ASC LIMIT 1 OFFSET 1",
                "UPDATE testTable SET id = ?1 ORDER BY content ASC LIMIT 1 OFFSET 1"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateValue("newContent", content, tableName, id.gt(0), id.order(Order.Asc), 1, 1);
                database.updateValue(nextId, id, tableName, content.order(Order.Asc), 1, 1);
            }
        });
        assertEquals(database.getValue(content.count(), tableName, id.eq(nextId)).getInteger(), 1);
        assertEquals(database.getValue(id.count(), tableName, content.eq("newContent")).getInteger(), 1);
    }

    @Test
    public void testDatabaseUpdateRowWhere() throws WCDBException {
        doTestSQL("UPDATE testTable SET id = ?1, content = ?2 WHERE id != 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateRow(new Value[]{new Value(nextId), new Value("newContent")}, columns(), tableName, id.notEq(1));
            }
        });
        assertEquals(database.getValue(content, tableName, id.eq(nextId)).getText(), "newContent");
    }

    @Test
    public void testDatabaseUpdateRowWhereOrderLimit() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET id = ?1, content = ?2 WHERE id < 3 ORDER BY id DESC LIMIT 1",
                "UPDATE testTable SET id = ?1, content = ?2 ORDER BY content DESC LIMIT 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateRow(new Value[]{new Value(nextId), new Value("newContent")}, columns(), tableName, id.lt(nextId), id.order(Order.Desc), 1);
                database.updateRow(new Value[]{new Value(nextId + 1), new Value("newContent2")}, columns(), tableName, content.order(Order.Desc), 1);
            }
        });
        assertTrue(database.getValue(content.count(), tableName, id.eq(nextId)).getInteger() +
                database.getValue(content.count(), tableName, id.eq(nextId+1)).getInteger() >= 1);
        assertTrue(database.getValue(id.count(), tableName, content.eq("newContent")).getInteger() +
                database.getValue(id.count(), tableName, content.eq("newContent2")).getInteger() >= 1);
    }

    @Test
    public void testDatabaseUpdateRowWhereOrderLimitOffset() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET id = ?1, content = ?2 WHERE id < 3 ORDER BY id DESC LIMIT 1 OFFSET 1",
                "UPDATE testTable SET id = ?1, content = ?2 ORDER BY content DESC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateRow(new Value[]{new Value(nextId), new Value("newContent")}, columns(), tableName, id.lt(nextId), id.order(Order.Desc), 1, 1);
                database.updateRow(new Value[]{new Value(nextId + 1), new Value("newContent2")}, columns(), tableName, content.order(Order.Desc), 1, 1);
            }
        });
        assertTrue(database.getValue(content.count(), tableName, id.eq(nextId)).getInteger() +
                database.getValue(content.count(), tableName, id.eq(nextId+1)).getInteger() >= 1);
        assertTrue(database.getValue(id.count(), tableName, content.eq("newContent")).getInteger() +
                database.getValue(id.count(), tableName, content.eq("newContent2")).getInteger() >= 1);
    }

    @Test
    public void testHandleUpdateValue() throws WCDBException {
        doTestSQL("UPDATE testTable SET content = ?1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.updateValue("newContent", content, tableName);
            }
        });
        assertEquals(handle.getValue(content, tableName).getText(), "newContent");
    }

    @Test
    public void testHandleUpdateValueWhere() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET content = ?1 WHERE id == 1",
                "UPDATE testTable SET id = ?1 WHERE content == 'newContent'"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.updateValue("newContent", content, tableName, id.eq(1));
                handle.updateValue(nextId, id, tableName, content.eq("newContent"));
            }
        });
        assertEquals(handle.getValue(content, tableName, id.eq(nextId)).getText(), "newContent");
        assertEquals(handle.getValue(id, tableName, content.eq("newContent")).getInteger(), nextId);
    }

    @Test
    public void testHandleUpdateValueWhereOrderLimit() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id ASC LIMIT 1",
                "UPDATE testTable SET id = ?1 ORDER BY content ASC LIMIT 1"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.updateValue("newContent", content, tableName, id.gt(0), id.order(Order.Asc), 1);
                handle.updateValue(nextId, id, tableName, content.order(Order.Asc), 1);
            }
        });
        assertEquals(handle.getValue(content.count(), tableName, id.eq(nextId)).getInteger(), 1);
        assertEquals(handle.getValue(id.count(), tableName, content.eq("newContent")).getInteger(), 1);
    }

    @Test
    public void testHandleUpdateValueWhereOrderLimitOffset() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id ASC LIMIT 1 OFFSET 1",
                "UPDATE testTable SET id = ?1 ORDER BY content ASC LIMIT 1 OFFSET 1"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.updateValue("newContent", content, tableName, id.gt(0), id.order(Order.Asc), 1, 1);
                handle.updateValue(nextId, id, tableName, content.order(Order.Asc), 1, 1);
            }
        });
        assertEquals(handle.getValue(content.count(), tableName, id.eq(nextId)).getInteger(), 1);
        assertEquals(handle.getValue(id.count(), tableName, content.eq("newContent")).getInteger(), 1);
    }

    @Test
    public void testHandleUpdateRowWhere() throws WCDBException {
        doTestSQL("UPDATE testTable SET id = ?1, content = ?2 WHERE id != 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.updateRow(new Value[]{new Value(nextId), new Value("newContent")}, columns(), tableName, id.notEq(1));
            }
        });
        assertEquals(handle.getValue(content, tableName, id.eq(nextId)).getText(), "newContent");
    }

    @Test
    public void testHandleUpdateRowWhereOrderLimit() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET id = ?1, content = ?2 WHERE id < 3 ORDER BY id DESC LIMIT 1",
                "UPDATE testTable SET id = ?1, content = ?2 ORDER BY content DESC LIMIT 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.updateRow(new Value[]{new Value(nextId), new Value("newContent")}, columns(), tableName, id.lt(nextId), id.order(Order.Desc), 1);
                handle.updateRow(new Value[]{new Value(nextId + 1), new Value("newContent2")}, columns(), tableName, content.order(Order.Desc), 1);
            }
        });
        assertTrue(handle.getValue(content.count(), tableName, id.eq(nextId)).getInteger() +
                handle.getValue(content.count(), tableName, id.eq(nextId+1)).getInteger() >= 1);
        assertTrue(handle.getValue(id.count(), tableName, content.eq("newContent")).getInteger() +
                handle.getValue(id.count(), tableName, content.eq("newContent2")).getInteger() >= 1);
    }

    @Test
    public void testHandleUpdateRowWhereOrderLimitOffset() throws WCDBException {
        doTestSQLs(new String[] {
                "UPDATE testTable SET id = ?1, content = ?2 WHERE id < 3 ORDER BY id DESC LIMIT 1 OFFSET 1",
                "UPDATE testTable SET id = ?1, content = ?2 ORDER BY content DESC LIMIT 1 OFFSET 1",
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.updateRow(new Value[]{new Value(nextId), new Value("newContent")}, columns(), tableName, id.lt(nextId), id.order(Order.Desc), 1, 1);
                handle.updateRow(new Value[]{new Value(nextId + 1), new Value("newContent2")}, columns(), tableName, content.order(Order.Desc), 1, 1);
            }
        });
        assertTrue(handle.getValue(content.count(), tableName, id.eq(nextId)).getInteger() +
                handle.getValue(content.count(), tableName, id.eq(nextId+1)).getInteger() >= 1);
        assertTrue(handle.getValue(id.count(), tableName, content.eq("newContent")).getInteger() +
                handle.getValue(id.count(), tableName, content.eq("newContent2")).getInteger() >= 1);
    }
}
