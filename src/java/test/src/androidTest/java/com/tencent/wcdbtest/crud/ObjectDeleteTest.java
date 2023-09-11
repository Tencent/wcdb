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

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.ObjectCRUDTestCase;

import org.junit.Test;

public class ObjectDeleteTest extends ObjectCRUDTestCase {

    @Test
    public void testDatabaseDelete() {
        doTestSQL("DELETE FROM testTable", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName);
            }
        });
        assertEquals(allRowsCount(), 0);
    }

    @Test
    public void testDatabaseDeleteWhere() {
        doTestSQL("DELETE FROM testTable WHERE id == 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBTestObject.id.eq(1));
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testDatabaseDeleteOrderLimit() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBTestObject.content.order(Order.Desc), 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testDatabaseDeleteOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1 OFFSET 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBTestObject.content.order(Order.Desc), 1, 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testDatabaseDeleteWhereOrderLimit() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 3) ORDER BY content DESC LIMIT 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBTestObject.id.in(new int[]{1, 3}), DBTestObject.content.order(Order.Desc), 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testDatabaseDeleteWhereOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 2) ORDER BY content DESC LIMIT 1 OFFSET 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBTestObject.id.in(new int[]{1, 2}), DBTestObject.content.order(Order.Desc), 1, 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testTableDelete() {
        doTestSQL("DELETE FROM testTable", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.deleteObjects();
            }
        });
        assertEquals(allRowsCount(), 0);
    }

    @Test
    public void testTableDeleteWhere() {
        doTestSQL("DELETE FROM testTable WHERE id == 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.deleteObjects(DBTestObject.id.eq(1));
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testTableDeleteOrderLimit() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.deleteObjects(DBTestObject.content.order(Order.Desc), 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testTableDeleteOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1 OFFSET 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.deleteObjects(DBTestObject.content.order(Order.Desc), 1, 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testTableDeleteWhereOrderLimit() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 3) ORDER BY content DESC LIMIT 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.deleteObjects(DBTestObject.id.in(new int[]{1, 3}), DBTestObject.content.order(Order.Desc), 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testTableDeleteWhereOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 2) ORDER BY content DESC LIMIT 1 OFFSET 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.deleteObjects(DBTestObject.id.in(new int[]{1, 2}), DBTestObject.content.order(Order.Desc), 1, 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testHandleDelete() {
        doTestSQL("DELETE FROM testTable", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.deleteObjects(tableName);
            }
        });
        assertEquals(allRowsCount(), 0);
    }

    @Test
    public void testHandleDeleteWhere() {
        doTestSQL("DELETE FROM testTable WHERE id == 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.deleteObjects(tableName, DBTestObject.id.eq(1));
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testHandleDeleteOrderLimit() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.deleteObjects(tableName, DBTestObject.content.order(Order.Desc), 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testHandleDeleteOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1 OFFSET 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.deleteObjects(tableName, DBTestObject.content.order(Order.Desc), 1, 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testHandleDeleteWhereOrderLimit() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 3) ORDER BY content DESC LIMIT 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.deleteObjects(tableName, DBTestObject.id.in(new int[]{1, 3}), DBTestObject.content.order(Order.Desc), 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }

    @Test
    public void testHandleDeleteWhereOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 2) ORDER BY content DESC LIMIT 1 OFFSET 1", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.deleteObjects(tableName, DBTestObject.id.in(new int[]{1, 2}), DBTestObject.content.order(Order.Desc), 1, 1);
            }
        });
        assertEquals(allRowsCount(), 1);
    }
}
