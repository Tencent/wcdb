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

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.ObjectCRUDTestCase;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TestObject;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class ObjectInsertTest extends ObjectCRUDTestCase {
    TestObject renewObject1;
    TestObject renewObject2;

    TestObject renewedPartialObject1;
    TestObject renewedPartialObject2;

    TestObject object3;
    TestObject object4;

    TestObject partialObject3;
    TestObject partialObject4;

    @Before
    public void setup() {
        super.setup();
        renewObject1 = RandomTool.testObjectWithId(1);
        renewObject2 = RandomTool.testObjectWithId(2);
        renewedPartialObject1 = TestObject.createPartialObject(1);
        renewedPartialObject2 = TestObject.createPartialObject(2);
        object3 = RandomTool.testObjectWithId(3);
        object4 = RandomTool.testObjectWithId(4);
        partialObject3 = TestObject.createPartialObject(3);
        partialObject4 = TestObject.createPartialObject(4);
    }

    @Test
    public void testDatabaseAutoIncrement() {
        TestObject autoIncrementObject = TestObject.createAutoIncrementObject(object3.content);
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertObject(autoIncrementObject, DBTestObject.allFields(), tableName);
            }
        });
        assertTrue(autoIncrementObject.equals(object3));
    }

    @Test
    public void testDatabaseAutoIncrementWithPartialInsert() {
        TestObject autoIncrementObject = TestObject.createAutoIncrementObject(object3.content);
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(content) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertObject(autoIncrementObject, new Field[]{DBTestObject.content}, tableName);
            }
        });
        assertTrue(autoIncrementObject.equals(object3));
    }

    @Test
    public void testDatabaseInsertObject() {
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertObject(object3, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3, object4}, 2, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertObjects(new TestObject[]{object3, object4}, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrReplaceObject() {
        doTestObjectsAfterInsert(new Object[]{renewObject1, object2}, 1, new String[]{
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrReplaceObject(renewObject1, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrReplaceObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{renewObject1, renewObject2}, 2, new String[]{
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrReplaceObjects(new TestObject[]{renewObject1, renewObject2}, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrIgnoreObject() {
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 1, new String[]{
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrIgnoreObject(renewObject1, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrIgnoreObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 2, new String[]{
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrIgnoreObjects(new TestObject[]{renewObject1, renewObject2}, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{object1, object2, partialObject3}, 1, new String[]{
                "INSERT INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertObject(object3, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2, partialObject3, partialObject4}, 2, new String[]{
                "INSERT INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertObjects(new TestObject[]{object3, object4}, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrReplaceObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{renewedPartialObject1, object2}, 1, new String[]{
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrReplaceObject(object1, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrReplaceObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{renewedPartialObject1, renewedPartialObject2}, 2, new String[]{
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrReplaceObjects(new TestObject[]{object1, object2}, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrIgnoreObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 1, new String[]{
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrIgnoreObject(object1, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testDatabaseInsertOrIgnoreObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 2, new String[]{
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertOrIgnoreObjects(new TestObject[]{object1, object2}, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testTableAutoIncrement() {
        TestObject autoIncrementObject = TestObject.createAutoIncrementObject(object3.content);
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertObject(autoIncrementObject);
            }
        });
        assertTrue(autoIncrementObject.equals(object3));
    }

    @Test
    public void testTableAutoIncrementWithPartialInsert() {
        TestObject autoIncrementObject = TestObject.createAutoIncrementObject(object3.content);
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(content) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertObject(autoIncrementObject, new Field[]{DBTestObject.content});
            }
        });
        assertTrue(autoIncrementObject.equals(object3));
    }

    @Test
    public void testTableInsertObject() {
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertObject(object3);
            }
        });
    }

    @Test
    public void testTableInsertObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3, object4}, 2, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertObjects(new TestObject[]{object3, object4});
            }
        });
    }

    @Test
    public void testTableInsertOrReplaceObject() {
        doTestObjectsAfterInsert(new Object[]{renewObject1, object2}, 1, new String[]{
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrReplaceObject(renewObject1);
            }
        });
    }

    @Test
    public void testTableInsertOrReplaceObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{renewObject1, renewObject2}, 2, new String[]{
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrReplaceObjects(new TestObject[]{renewObject1, renewObject2});
            }
        });
    }

    @Test
    public void testTableInsertOrIgnoreObject() {
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 1, new String[]{
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrIgnoreObject(renewObject1);
            }
        });
    }

    @Test
    public void testTableInsertOrIgnoreObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 2, new String[]{
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrIgnoreObjects(new TestObject[]{renewObject1, renewObject2});
            }
        });
    }

    @Test
    public void testTableInsertObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{object1, object2, partialObject3}, 1, new String[]{
                "INSERT INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertObject(object3, new Field[]{DBTestObject.id});
            }
        });
    }

    @Test
    public void testTableInsertObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2, partialObject3, partialObject4}, 2, new String[]{
                "INSERT INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertObjects(new TestObject[]{object3, object4}, new Field[]{DBTestObject.id});
            }
        });
    }

    @Test
    public void testTableInsertOrReplaceObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{renewedPartialObject1, object2}, 1, new String[]{
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrReplaceObject(object1, new Field[]{DBTestObject.id});
            }
        });
    }

    @Test
    public void testTableInsertOrReplaceObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{renewedPartialObject1, renewedPartialObject2}, 2, new String[]{
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrReplaceObjects(new TestObject[]{object1, object2}, new Field[]{DBTestObject.id});
            }
        });
    }

    @Test
    public void testTableInsertOrIgnoreObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 1, new String[]{
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrIgnoreObject(object1, new Field[]{DBTestObject.id});
            }
        });
    }

    @Test
    public void testTableInsertOrIgnoreObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 2, new String[]{
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                table.insertOrIgnoreObjects(new TestObject[]{object1, object2}, new Field[]{DBTestObject.id});
            }
        });
    }

    @Test
    public void testHandleAutoIncrement() {
        TestObject autoIncrementObject = TestObject.createAutoIncrementObject(object3.content);
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertObject(autoIncrementObject, DBTestObject.allFields(), tableName);
            }
        });
        assertTrue(autoIncrementObject.equals(object3));
    }

    @Test
    public void testHandleAutoIncrementWithPartialInsert() {
        TestObject autoIncrementObject = TestObject.createAutoIncrementObject(object3.content);
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(content) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertObject(autoIncrementObject, new Field[]{DBTestObject.content}, tableName);
            }
        });
        assertTrue(autoIncrementObject.equals(object3));
    }

    @Test
    public void testHandleInsertObject() {
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3}, 1, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertObject(object3, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testHandleInsertObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2, object3, object4}, 2, new String[]{
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertObjects(new TestObject[]{object3, object4}, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrReplaceObject() {
        doTestObjectsAfterInsert(new Object[]{renewObject1, object2}, 1, new String[]{
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrReplaceObject(renewObject1, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrReplaceObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{renewObject1, renewObject2}, 2, new String[]{
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrReplaceObjects(new TestObject[]{renewObject1, renewObject2}, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrIgnoreObject() {
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 1, new String[]{
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrIgnoreObject(renewObject1, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrIgnoreObjects() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 2, new String[]{
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrIgnoreObjects(new TestObject[]{renewObject1, renewObject2}, DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testHandleInsertObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{object1, object2, partialObject3}, 1, new String[]{
                "INSERT INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertObject(object3, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testHandleInsertObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2, partialObject3, partialObject4}, 2, new String[]{
                "INSERT INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertObjects(new TestObject[]{object3, object4}, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrReplaceObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{renewedPartialObject1, object2}, 1, new String[]{
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrReplaceObject(object1, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrReplaceObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{renewedPartialObject1, renewedPartialObject2}, 2, new String[]{
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrReplaceObjects(new TestObject[]{object1, object2}, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrIgnoreObjectOnFields() {
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 1, new String[]{
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrIgnoreObject(object1, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testHandleInsertOrIgnoreObjectsOnFields() {
        expectMode = Expect.SomeSQLs;
        doTestObjectsAfterInsert(new Object[]{object1, object2}, 2, new String[]{
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                handle.insertOrIgnoreObjects(new TestObject[]{object1, object2}, new Field[]{DBTestObject.id}, tableName);
            }
        });
    }
}
