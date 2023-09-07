// Created by qiuwenchen on 2023/6/25.
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

package com.tencent.wcdbtest.crud

import com.tencent.wcdb.base.WCDBException
import com.tencent.wcdb.winq.Order
import com.tencent.wcdbtest.base.DBTestObject
import com.tencent.wcdbtest.base.ObjectCRUDTestCase
import com.tencent.wcdbtest.base.TestObject
import org.junit.Before
import org.junit.Test

class ObjectSelectTest : ObjectCRUDTestCase() {
    private lateinit var partialObject1: TestObject
    private lateinit var partialObject2: TestObject
    private lateinit var partialObjects: Array<TestObject>

    @Before
    @Throws(WCDBException::class)
    override fun setup() {
        super.setup()
        partialObject1 = TestObject.createPartialObject(1)
        partialObject2 = TestObject.createPartialObject(2)
        partialObjects = arrayOf(partialObject1, partialObject2)
    }

    @Test
    fun testDatabaseGetObject() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable") {
            listOf(
                database.getFirstObject(
                    DBTestObject.allFields(), tableName
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2") {
            listOf(
                database.getFirstObject(
                    DBTestObject.allFields(), tableName, DBTestObject.id.eq(2)
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectWhereOrder() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            listOf(
                database.getFirstObject(
                    DBTestObject.allFields(),
                    tableName,
                    DBTestObject.id.gt(0),
                    DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectWhereOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                database.getFirstObject(
                    DBTestObject.allFields(),
                    tableName,
                    DBTestObject.id.gt(0),
                    DBTestObject.id.order(Order.Desc),
                    1
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC") {
            listOf(
                database.getFirstObject(
                    DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                database.getFirstObject(
                    DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjects() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable") {
            database.getAllObjects(
                DBTestObject.allFields(), tableName
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1") {
            database.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsWhereOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            database.getAllObjects(
                DBTestObject.allFields(),
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsWhereOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            database.getAllObjects(
                DBTestObject.allFields(),
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc),
                1
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsWhereOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            database.getAllObjects(
                DBTestObject.allFields(),
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc),
                1,
                1
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable ORDER BY id DESC"
        ) {
            database.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1"
        ) {
            database.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            database.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOnFields() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable") {
            listOf(
                database.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOnFieldsWhere() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id == 2") {
            listOf(
                database.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.eq(2)
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOnFieldsWhereOrder() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            listOf(
                database.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOnFieldsWhereOrderOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                database.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOnFieldsOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC") {
            listOf(
                database.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectOnFieldsOrderOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                database.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFields() {
        doTestObjectBySelecting(
            partialObjects,
            "SELECT id FROM testTable"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFieldsWhere() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id == 1"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFieldsWhereOrder() {
        doTestObjectBySelecting(
            arrayOf(partialObject2, partialObject1),
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFieldsWhereOrderLimit() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFieldsOrder() {
        doTestObjectBySelecting(
            arrayOf(partialObject2, partialObject1),
            "SELECT id FROM testTable ORDER BY id DESC"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFieldsOrderLimit() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testDatabaseGetObjectsOnFieldsOrderLimitOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            database.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testTableGetObject() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable") {
            listOf(
                table.getFirstObject(
                    DBTestObject.allFields()
                )
            )
        }
    }

    @Test
    fun testTableGetObjectWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2") {
            listOf(
                table.getFirstObject(
                    DBTestObject.allFields(), DBTestObject.id.eq(2)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectWhereOrder() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            listOf(
                table.getFirstObject(
                    DBTestObject.allFields(),
                    DBTestObject.id.gt(0),
                    DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectWhereOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                table.getFirstObject(
                    DBTestObject.allFields(),
                    DBTestObject.id.gt(0),
                    DBTestObject.id.order(Order.Desc),
                    1
                )
            )
        }
    }

    @Test
    fun testTableGetObjectOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC") {
            listOf(
                table.getFirstObject(
                    DBTestObject.allFields(), DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                table.getFirstObject(
                    DBTestObject.allFields(), DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testTableGetObjects() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable") {
            table.getAllObjects(
                DBTestObject.allFields()
            )
        }
    }

    @Test
    fun testTableGetObjectsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1") {
            table.getAllObjects(
                DBTestObject.allFields(), DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testTableGetObjectsWhereOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            table.getAllObjects(
                DBTestObject.allFields(), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testTableGetObjectsWhereOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            table.getAllObjects(
                DBTestObject.allFields(),
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc),
                1
            )
        }
    }

    @Test
    fun testTableGetObjectsWhereOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.getAllObjects(
                DBTestObject.allFields(),
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc),
                1,
                1
            )
        }
    }

    @Test
    fun testTableGetObjectsOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable ORDER BY id DESC"
        ) {
            table.getAllObjects(
                DBTestObject.allFields(), DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testTableGetObjectsOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1"
        ) {
            table.getAllObjects(
                DBTestObject.allFields(), DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testTableGetObjectsOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.getAllObjects(
                DBTestObject.allFields(), DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testTableGetObjectOnFields() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable") {
            listOf(
                table.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    )
                )
            )
        }
    }

    @Test
    fun testTableGetObjectOnFieldsWhere() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id == 2") {
            listOf(
                table.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), DBTestObject.id.eq(2)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectOnFieldsWhereOrder() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            listOf(
                table.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectOnFieldsWhereOrderOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                table.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testTableGetObjectOnFieldsOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC") {
            listOf(
                table.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectOnFieldsOrderOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                table.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFields() {
        doTestObjectBySelecting(
            partialObjects,
            "SELECT id FROM testTable"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                )
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFieldsWhere() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id == 1"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFieldsWhereOrder() {
        doTestObjectBySelecting(
            arrayOf(partialObject2, partialObject1),
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFieldsWhereOrderLimit() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFieldsOrder() {
        doTestObjectBySelecting(
            arrayOf(partialObject2, partialObject1),
            "SELECT id FROM testTable ORDER BY id DESC"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFieldsOrderLimit() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testTableGetObjectsOnFieldsOrderLimitOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testTableGetObjectWithoutField() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable"
        ) { listOf(table.firstObject) }
    }

    @Test
    fun testTableGetObjectWithoutFieldWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2") {
            listOf(
                table.getFirstObject(
                    DBTestObject.id.eq(2)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectWithoutFieldWhereOrder() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            listOf(
                table.getFirstObject(
                    DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectWithoutFieldWhereOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                table.getFirstObject(
                    DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testTableGetObjectWithoutFieldOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC") {
            listOf(
                table.getFirstObject(
                    DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testTableGetObjectWithoutFieldOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                table.getFirstObject(
                    DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testTableGetObjectsWithoutFields() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable") { table.allObjects }
    }

    @Test
    fun testTableGetObjectsWithoutFieldsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1") {
            table.getAllObjects(
                DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testTableGetObjectsWithoutFieldsWhereOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            table.getAllObjects(
                DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testTableGetObjectsWithoutFieldsWhereOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            table.getAllObjects(
                DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testTableGetObjectsWithoutFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.getAllObjects(
                DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testTableGetObjectsWithoutFieldsOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable ORDER BY id DESC"
        ) {
            table.getAllObjects(
                DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testTableGetObjectsWithoutFieldsOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1"
        ) {
            table.getAllObjects(
                DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testTableGetObjectsWithoutFieldsOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.getAllObjects(
                DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testHandleGetObject() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable") {
            listOf(
                handle.getFirstObject(
                    DBTestObject.allFields(), tableName
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectWhere() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable WHERE id == 2") {
            listOf(
                handle.getFirstObject(
                    DBTestObject.allFields(), tableName, DBTestObject.id.eq(2)
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectWhereOrder() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            listOf(
                handle.getFirstObject(
                    DBTestObject.allFields(),
                    tableName,
                    DBTestObject.id.gt(0),
                    DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectWhereOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                handle.getFirstObject(
                    DBTestObject.allFields(),
                    tableName,
                    DBTestObject.id.gt(0),
                    DBTestObject.id.order(Order.Desc),
                    1
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectOrder() {
        doTestObjectBySelecting(object2, "SELECT id, content FROM testTable ORDER BY id DESC") {
            listOf(
                handle.getFirstObject(
                    DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectOrderOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                handle.getFirstObject(
                    DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testHandleGetObjects() {
        doTestObjectBySelecting(objects, "SELECT id, content FROM testTable") {
            handle.getAllObjects(
                DBTestObject.allFields(), tableName
            )
        }
    }

    @Test
    fun testHandleGetObjectsWhere() {
        doTestObjectBySelecting(object1, "SELECT id, content FROM testTable WHERE id == 1") {
            handle.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testHandleGetObjectsWhereOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            handle.getAllObjects(
                DBTestObject.allFields(),
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testHandleGetObjectsWhereOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            handle.getAllObjects(
                DBTestObject.allFields(),
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc),
                1
            )
        }
    }

    @Test
    fun testHandleGetObjectsWhereOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            handle.getAllObjects(
                DBTestObject.allFields(),
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc),
                1,
                1
            )
        }
    }

    @Test
    fun testHandleGetObjectsOrder() {
        doTestObjectBySelecting(
            arrayOf(object2, object1),
            "SELECT id, content FROM testTable ORDER BY id DESC"
        ) {
            handle.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testHandleGetObjectsOrderLimit() {
        doTestObjectBySelecting(
            object2,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1"
        ) {
            handle.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testHandleGetObjectsOrderLimitOffset() {
        doTestObjectBySelecting(
            object1,
            "SELECT id, content FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            handle.getAllObjects(
                DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testHandleGetObjectOnFields() {
        doTestObjectBySelecting(partialObject1, "SELECT id FROM testTable") {
            listOf(
                handle.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectOnFieldsWhere() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable WHERE id == 2") {
            listOf(
                handle.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.eq(2)
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectOnFieldsWhereOrder() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            listOf(
                handle.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectOnFieldsWhereOrderOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                handle.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectOnFieldsOrder() {
        doTestObjectBySelecting(partialObject2, "SELECT id FROM testTable ORDER BY id DESC") {
            listOf(
                handle.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.order(Order.Desc)
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectOnFieldsOrderOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            listOf(
                handle.getFirstObject(
                    arrayOf(
                        DBTestObject.id
                    ), tableName, DBTestObject.id.order(Order.Desc), 1
                )
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFields() {
        doTestObjectBySelecting(
            partialObjects,
            "SELECT id FROM testTable"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFieldsWhere() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id == 1"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFieldsWhereOrder() {
        doTestObjectBySelecting(
            arrayOf(partialObject2, partialObject1),
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFieldsWhereOrderLimit() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFieldsWhereOrderLimitOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.gt(0), DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFieldsOrder() {
        doTestObjectBySelecting(
            arrayOf(partialObject2, partialObject1),
            "SELECT id FROM testTable ORDER BY id DESC"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.order(Order.Desc)
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFieldsOrderLimit() {
        doTestObjectBySelecting(
            partialObject2,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testHandleGetObjectsOnFieldsOrderLimitOffset() {
        doTestObjectBySelecting(
            partialObject1,
            "SELECT id FROM testTable ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            handle.getAllObjects(
                arrayOf(
                    DBTestObject.id
                ), tableName, DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }
}