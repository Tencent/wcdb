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
import com.tencent.wcdbtest.base.DBTestObject
import com.tencent.wcdbtest.base.ObjectCRUDTestCase
import com.tencent.wcdbtest.base.RandomTool
import com.tencent.wcdbtest.base.TestObject
import org.junit.Assert
import org.junit.Before
import org.junit.Test

class ObjectInsertTest : ObjectCRUDTestCase() {
    private lateinit var renewObject1: TestObject
    private lateinit var renewObject2: TestObject
    private lateinit var renewedPartialObject1: TestObject
    private lateinit var renewedPartialObject2: TestObject
    private lateinit var object3: TestObject
    private lateinit var object4: TestObject
    private lateinit var partialObject3: TestObject
    private lateinit var partialObject4: TestObject
    @Before
    @Throws(WCDBException::class)
    override fun setup() {
        super.setup()
        renewObject1 = RandomTool.testObjectWithId(1)
        renewObject2 = RandomTool.testObjectWithId(2)
        renewedPartialObject1 = TestObject.createPartialObject(1)
        renewedPartialObject2 = TestObject.createPartialObject(2)
        object3 = RandomTool.testObjectWithId(3)
        object4 = RandomTool.testObjectWithId(4)
        partialObject3 = TestObject.createPartialObject(3)
        partialObject4 = TestObject.createPartialObject(4)
    }

    @Test
    fun testDatabaseAutoIncrement() {
        val autoIncrementObject = TestObject.createAutoIncrementObject(
            object3.content
        )
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { database.insertObject(autoIncrementObject, DBTestObject.allFields(), tableName) }
        Assert.assertEquals(autoIncrementObject, object3)
    }

    @Test
    fun testDatabaseAutoIncrementWithPartialInsert() {
        val autoIncrementObject = TestObject.createAutoIncrementObject(
            object3.content
        )
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(content) VALUES(?1)"
            )
        ) {
            database.insertObject(
                autoIncrementObject,
                arrayOf(DBTestObject.content),
                tableName
            )
        }
        Assert.assertEquals(autoIncrementObject, object3)
    }

    @Test
    fun testDatabaseInsertObject() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { database.insertObject(object3, DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testDatabaseInsertObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3, object4), 2, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { database.insertObjects(arrayOf(object3, object4), DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testDatabaseInsertOrReplaceObject() {
        doTestObjectsAfterInsert(
            arrayOf(renewObject1, object2), 1, arrayOf(
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { database.insertOrReplaceObject(renewObject1, DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testDatabaseInsertOrReplaceObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(renewObject1, renewObject2), 2, arrayOf(
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) {
            database.insertOrReplaceObjects(
                arrayOf(renewObject1, renewObject2),
                DBTestObject.allFields(),
                tableName
            )
        }
    }

    @Test
    fun testDatabaseInsertOrIgnoreObject() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 1, arrayOf(
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { database.insertOrIgnoreObject(renewObject1, DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testDatabaseInsertOrIgnoreObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 2, arrayOf(
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) {
            database.insertOrIgnoreObjects(
                arrayOf(renewObject1, renewObject2),
                DBTestObject.allFields(),
                tableName
            )
        }
    }

    @Test
    fun testDatabaseInsertObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, partialObject3), 1, arrayOf(
                "INSERT INTO testTable(id) VALUES(?1)"
            )
        ) { database.insertObject(object3, arrayOf(DBTestObject.id), tableName) }
    }

    @Test
    fun testDatabaseInsertObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, partialObject3, partialObject4), 2, arrayOf(
                "INSERT INTO testTable(id) VALUES(?1)"
            )
        ) {
            database.insertObjects(
                arrayOf(object3, object4),
                arrayOf(DBTestObject.id),
                tableName
            )
        }
    }

    @Test
    fun testDatabaseInsertOrReplaceObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(renewedPartialObject1, object2), 1, arrayOf(
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
            )
        ) { database.insertOrReplaceObject(object1, arrayOf(DBTestObject.id), tableName) }
    }

    @Test
    fun testDatabaseInsertOrReplaceObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(renewedPartialObject1, renewedPartialObject2), 2, arrayOf(
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
            )
        ) {
            database.insertOrReplaceObjects(
                arrayOf(object1, object2), arrayOf(
                    DBTestObject.id
                ), tableName
            )
        }
    }

    @Test
    fun testDatabaseInsertOrIgnoreObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 1, arrayOf(
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
            )
        ) { database.insertOrIgnoreObject(object1, arrayOf(DBTestObject.id), tableName) }
    }

    @Test
    fun testDatabaseInsertOrIgnoreObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 2, arrayOf(
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
            )
        ) {
            database.insertOrIgnoreObjects(
                arrayOf(object1, object2), arrayOf(
                    DBTestObject.id
                ), tableName
            )
        }
    }

    @Test
    fun testTableAutoIncrement() {
        val autoIncrementObject = TestObject.createAutoIncrementObject(
            object3.content
        )
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { table.insertObject(autoIncrementObject) }
        Assert.assertEquals(autoIncrementObject, object3)
    }

    @Test
    fun testTableAutoIncrementWithPartialInsert() {
        val autoIncrementObject = TestObject.createAutoIncrementObject(
            object3.content
        )
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(content) VALUES(?1)"
            )
        ) { table.insertObject(autoIncrementObject, arrayOf(DBTestObject.content)) }
        Assert.assertEquals(autoIncrementObject, object3)
    }

    @Test
    fun testTableInsertObject() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { table.insertObject(object3) }
    }

    @Test
    fun testTableInsertObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3, object4), 2, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { table.insertObjects(arrayOf(object3, object4)) }
    }

    @Test
    fun testTableInsertOrReplaceObject() {
        doTestObjectsAfterInsert(
            arrayOf(renewObject1, object2), 1, arrayOf(
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { table.insertOrReplaceObject(renewObject1) }
    }

    @Test
    fun testTableInsertOrReplaceObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(renewObject1, renewObject2), 2, arrayOf(
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { table.insertOrReplaceObjects(arrayOf(renewObject1, renewObject2)) }
    }

    @Test
    fun testTableInsertOrIgnoreObject() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 1, arrayOf(
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { table.insertOrIgnoreObject(renewObject1) }
    }

    @Test
    fun testTableInsertOrIgnoreObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 2, arrayOf(
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { table.insertOrIgnoreObjects(arrayOf(renewObject1, renewObject2)) }
    }

    @Test
    fun testTableInsertObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, partialObject3), 1, arrayOf(
                "INSERT INTO testTable(id) VALUES(?1)"
            )
        ) { table.insertObject(object3, arrayOf(DBTestObject.id)) }
    }

    @Test
    fun testTableInsertObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, partialObject3, partialObject4), 2, arrayOf(
                "INSERT INTO testTable(id) VALUES(?1)"
            )
        ) { table.insertObjects(arrayOf(object3, object4), arrayOf(DBTestObject.id)) }
    }

    @Test
    fun testTableInsertOrReplaceObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(renewedPartialObject1, object2), 1, arrayOf(
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
            )
        ) { table.insertOrReplaceObject(object1, arrayOf(DBTestObject.id)) }
    }

    @Test
    fun testTableInsertOrReplaceObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(renewedPartialObject1, renewedPartialObject2), 2, arrayOf(
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
            )
        ) {
            table.insertOrReplaceObjects(
                arrayOf(object1, object2),
                arrayOf(DBTestObject.id)
            )
        }
    }

    @Test
    fun testTableInsertOrIgnoreObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 1, arrayOf(
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
            )
        ) { table.insertOrIgnoreObject(object1, arrayOf(DBTestObject.id)) }
    }

    @Test
    fun testTableInsertOrIgnoreObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 2, arrayOf(
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
            )
        ) {
            table.insertOrIgnoreObjects(
                arrayOf(object1, object2),
                arrayOf(DBTestObject.id)
            )
        }
    }

    @Test
    fun testHandleAutoIncrement() {
        val autoIncrementObject = TestObject.createAutoIncrementObject(
            object3.content
        )
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { handle.insertObject(autoIncrementObject, DBTestObject.allFields(), tableName) }
        Assert.assertEquals(autoIncrementObject, object3)
    }

    @Test
    fun testHandleAutoIncrementWithPartialInsert() {
        val autoIncrementObject = TestObject.createAutoIncrementObject(
            object3.content
        )
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(content) VALUES(?1)"
            )
        ) {
            handle.insertObject(
                autoIncrementObject,
                arrayOf(DBTestObject.content),
                tableName
            )
        }
        Assert.assertEquals(autoIncrementObject, object3)
    }

    @Test
    fun testHandleInsertObject() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3), 1, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { handle.insertObject(object3, DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testHandleInsertObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, object3, object4), 2, arrayOf(
                "INSERT INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { handle.insertObjects(arrayOf(object3, object4), DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testHandleInsertOrReplaceObject() {
        doTestObjectsAfterInsert(
            arrayOf(renewObject1, object2), 1, arrayOf(
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { handle.insertOrReplaceObject(renewObject1, DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testHandleInsertOrReplaceObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(renewObject1, renewObject2), 2, arrayOf(
                "INSERT OR REPLACE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) {
            handle.insertOrReplaceObjects(
                arrayOf(renewObject1, renewObject2),
                DBTestObject.allFields(),
                tableName
            )
        }
    }

    @Test
    fun testHandleInsertOrIgnoreObject() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 1, arrayOf(
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) { handle.insertOrIgnoreObject(renewObject1, DBTestObject.allFields(), tableName) }
    }

    @Test
    fun testHandleInsertOrIgnoreObjects() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 2, arrayOf(
                "INSERT OR IGNORE INTO testTable(id, content) VALUES(?1, ?2)"
            )
        ) {
            handle.insertOrIgnoreObjects(
                arrayOf(renewObject1, renewObject2),
                DBTestObject.allFields(),
                tableName
            )
        }
    }

    @Test
    fun testHandleInsertObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, partialObject3), 1, arrayOf(
                "INSERT INTO testTable(id) VALUES(?1)"
            )
        ) { handle.insertObject(object3, arrayOf(DBTestObject.id), tableName) }
    }

    @Test
    fun testHandleInsertObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2, partialObject3, partialObject4), 2, arrayOf(
                "INSERT INTO testTable(id) VALUES(?1)"
            )
        ) {
            handle.insertObjects(
                arrayOf(object3, object4),
                arrayOf(DBTestObject.id),
                tableName
            )
        }
    }

    @Test
    fun testHandleInsertOrReplaceObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(renewedPartialObject1, object2), 1, arrayOf(
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
            )
        ) { handle.insertOrReplaceObject(object1, arrayOf(DBTestObject.id), tableName) }
    }

    @Test
    fun testHandleInsertOrReplaceObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(renewedPartialObject1, renewedPartialObject2), 2, arrayOf(
                "INSERT OR REPLACE INTO testTable(id) VALUES(?1)"
            )
        ) {
            handle.insertOrReplaceObjects(
                arrayOf(object1, object2), arrayOf(
                    DBTestObject.id
                ), tableName
            )
        }
    }

    @Test
    fun testHandleInsertOrIgnoreObjectOnFields() {
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 1, arrayOf(
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
            )
        ) { handle.insertOrIgnoreObject(object1, arrayOf(DBTestObject.id), tableName) }
    }

    @Test
    fun testHandleInsertOrIgnoreObjectsOnFields() {
        expectMode = Expect.SomeSQLs
        doTestObjectsAfterInsert(
            arrayOf(object1, object2), 2, arrayOf(
                "INSERT OR IGNORE INTO testTable(id) VALUES(?1)"
            )
        ) {
            handle.insertOrIgnoreObjects(
                arrayOf(object1, object2),
                arrayOf(DBTestObject.id),
                tableName
            )
        }
    }
}