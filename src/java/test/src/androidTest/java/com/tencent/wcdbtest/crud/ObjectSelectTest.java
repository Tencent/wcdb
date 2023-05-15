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

import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.ObjectCRUDTestCase;
import com.tencent.wcdbtest.base.TestObject;

import org.junit.Before;
import org.junit.Test;

import java.util.Arrays;
import java.util.List;

public class ObjectSelectTest extends ObjectCRUDTestCase {
    TestObject partialObject1;
    TestObject partialObject2;
    TestObject[] partialObjects;

    @Before
    public void setup() {
        super.setup();
        partialObject1 = TestObject.createPartialObject(1);
        partialObject2 = TestObject.createPartialObject(2);
        partialObjects = new TestObject[]{ partialObject1, partialObject2 };
    }

    @Test
    public void testDatabaseGetObject() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(DBTestObject.allFields(), tableName));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.eq(2)));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectWhereOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectWhereOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testDatabaseGetObjects() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.eq(1));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsWhereOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsWhereOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsWhereOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOnFields() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(new Field[]{DBTestObject.id}, tableName));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOnFieldsWhere() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id == 2", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.eq(2)));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOnFieldsWhereOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOnFieldsWhereOrderOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOnFieldsOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectOnFieldsOrderOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(database.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFields() {
        doTestObjectBySelecting(partialObjects, "SELECT id FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFieldsWhere() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id == 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.eq(1));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFieldsWhereOrder() {
        doTestObjectBySelecting(new TestObject[]{partialObject2, partialObject1}, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFieldsWhereOrderLimit() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFieldsOrder() {
        doTestObjectBySelecting(new TestObject[]{partialObject2, partialObject1}, "SELECT id FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFieldsOrderLimit() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testDatabaseGetObjectsOnFieldsOrderLimitOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return database.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testTableGetObject() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.allFields()));
            }
        });
    }

    @Test
    public void testTableGetObjectWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.allFields(), DBTestObject.id.eq(2)));
            }
        });
    }

    @Test
    public void testTableGetObjectWhereOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.allFields(), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testTableGetObjectWhereOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.allFields(), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testTableGetObjectOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.allFields(), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testTableGetObjectOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.allFields(), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testTableGetObjects() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields());
            }
        });
    }

    @Test
    public void testTableGetObjectsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields(), DBTestObject.id.eq(1));
            }
        });
    }

    @Test
    public void testTableGetObjectsWhereOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields(), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testTableGetObjectsWhereOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields(), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsWhereOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields(), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields(), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testTableGetObjectsOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields(), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.allFields(), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testTableGetObjectOnFields() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(new Field[]{DBTestObject.id}));
            }
        });
    }

    @Test
    public void testTableGetObjectOnFieldsWhere() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id == 2", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(new Field[]{DBTestObject.id}, DBTestObject.id.eq(2)));
            }
        });
    }

    @Test
    public void testTableGetObjectOnFieldsWhereOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(new Field[]{DBTestObject.id}, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testTableGetObjectOnFieldsWhereOrderOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(new Field[]{DBTestObject.id}, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testTableGetObjectOnFieldsOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(new Field[]{DBTestObject.id}, DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testTableGetObjectOnFieldsOrderOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(new Field[]{DBTestObject.id}, DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFields() {
        doTestObjectBySelecting(partialObjects, "SELECT id FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id});
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFieldsWhere() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id == 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id}, DBTestObject.id.eq(1));
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFieldsWhereOrder() {
        doTestObjectBySelecting(new TestObject[]{partialObject2, partialObject1}, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id}, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFieldsWhereOrderLimit() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id}, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id}, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFieldsOrder() {
        doTestObjectBySelecting(new TestObject[]{partialObject2, partialObject1}, "SELECT id FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id}, DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFieldsOrderLimit() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id}, DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsOnFieldsOrderLimitOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(new Field[]{DBTestObject.id}, DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testTableGetObjectWithoutField() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject());
            }
        });
    }

    @Test
    public void testTableGetObjectWithoutFieldWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.id.eq(2)));
            }
        });
    }

    @Test
    public void testTableGetObjectWithoutFieldWhereOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testTableGetObjectWithoutFieldWhereOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testTableGetObjectWithoutFieldOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testTableGetObjectWithoutFieldOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(table.getFirstObject(DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFields() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects();
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFieldsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.id.eq(1));
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFieldsWhereOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFieldsWhereOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFieldsOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFieldsOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testTableGetObjectsWithoutFieldsOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return table.getAllObjects(DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testHandleGetObject() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(DBTestObject.allFields(), tableName));
            }
        });
    }

    @Test
    public void testHandleGetObjectWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.eq(2)));
            }
        });
    }

    @Test
    public void testHandleGetObjectWhereOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testHandleGetObjectWhereOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testHandleGetObjectOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testHandleGetObjectOrderOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testHandleGetObjects() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName);
            }
        });
    }

    @Test
    public void testHandleGetObjectsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.eq(1));
            }
        });
    }

    @Test
    public void testHandleGetObjectsWhereOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testHandleGetObjectsWhereOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testHandleGetObjectsWhereOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testHandleGetObjectsOrder() {
        doTestObjectBySelecting(new TestObject[]{object2, object1}, "SELECT id, content FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testHandleGetObjectsOrderLimit() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testHandleGetObjectsOrderLimitOffset() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testHandleGetObjectOnFields() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(new Field[]{DBTestObject.id}, tableName));
            }
        });
    }

    @Test
    public void testHandleGetObjectOnFieldsWhere() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id == 2", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.eq(2)));
            }
        });
    }

    @Test
    public void testHandleGetObjectOnFieldsWhereOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testHandleGetObjectOnFieldsWhereOrderOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testHandleGetObjectOnFieldsOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc)));
            }
        });
    }

    @Test
    public void testHandleGetObjectOnFieldsOrderOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return Arrays.asList(handle.getFirstObject(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc), 1));
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFields() {
        doTestObjectBySelecting(partialObjects, "SELECT id FROM testTable", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName);
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFieldsWhere() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id == 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.eq(1));
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFieldsWhereOrder() {
        doTestObjectBySelecting(new TestObject[]{partialObject2, partialObject1}, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFieldsWhereOrderLimit() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFieldsOrder() {
        doTestObjectBySelecting(new TestObject[]{partialObject2, partialObject1}, "SELECT id FROM testTable ORDER BY id DESC", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc));
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFieldsOrderLimit() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc), 1);
            }
        });
    }

    @Test
    public void testHandleGetObjectsOnFieldsOrderLimitOffset() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1", new SelectingObjectOperation() {
            @Override
            public List execute() {
                return handle.getAllObjects(new Field[]{DBTestObject.id}, tableName, DBTestObject.id.order(Order.Desc), 1, 1);
            }
        });
    }
}
