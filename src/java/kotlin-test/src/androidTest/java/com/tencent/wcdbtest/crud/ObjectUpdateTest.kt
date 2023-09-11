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
import com.tencent.wcdbtest.base.RandomTool
import com.tencent.wcdbtest.base.TestObject
import org.junit.Before
import org.junit.Test

class ObjectUpdateTest : ObjectCRUDTestCase() {
    private lateinit var renewObject: TestObject
    private lateinit var renewedObject1: TestObject
    private lateinit var renewedObject2: TestObject
    @Before
    @Throws(WCDBException::class)
    override fun setup() {
        super.setup()
        val renewContent = RandomTool.string()
        renewObject = TestObject.createObject(0, renewContent)
        renewedObject1 = TestObject.createObject(1, renewContent)
        renewedObject2 = TestObject.createObject(2, renewContent)
    }

    @Test
    fun testDatabaseUpdateObject() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, renewedObject2),
            "UPDATE testTable SET content = ?1"
        ) { database.updateObject(renewObject, DBTestObject.content, tableName) }
    }

    @Test
    fun testDatabaseUpdateObjectWhere() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 WHERE id == 1"
        ) {
            database.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testDatabaseUpdateObjectWhereOrderLimit() {
        doTestObjectsAfterOperation(
            arrayOf(object1, renewedObject2),
            "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            database.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testDatabaseUpdateObjectWhereOrderLimitOffset() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            database.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testDatabaseUpdateObjectOrderLimit() {
        doTestObjectsAfterOperation(
            arrayOf(object1, renewedObject2),
            "UPDATE testTable SET content = ?1 ORDER BY id DESC LIMIT 1"
        ) {
            database.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testDatabaseUpdateObjectOrderLimitOffset() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            database.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testTableUpdateObject() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, renewedObject2),
            "UPDATE testTable SET content = ?1"
        ) { table.updateObject(renewObject, DBTestObject.content) }
    }

    @Test
    fun testTableUpdateObjectWhere() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 WHERE id == 1"
        ) { table.updateObject(renewObject, DBTestObject.content, DBTestObject.id.eq(1)) }
    }

    @Test
    fun testTableUpdateObjectWhereOrderLimit() {
        doTestObjectsAfterOperation(
            arrayOf(object1, renewedObject2),
            "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            table.updateObject(
                renewObject,
                DBTestObject.content,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testTableUpdateObjectWhereOrderLimitOffset() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.updateObject(
                renewObject,
                DBTestObject.content,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testTableUpdateObjectOrderLimit() {
        doTestObjectsAfterOperation(
            arrayOf(object1, renewedObject2),
            "UPDATE testTable SET content = ?1 ORDER BY id DESC LIMIT 1"
        ) {
            table.updateObject(
                renewObject,
                DBTestObject.content,
                DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testTableUpdateObjectOrderLimitOffset() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            table.updateObject(
                renewObject,
                DBTestObject.content,
                DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testHandleUpdateObject() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, renewedObject2),
            "UPDATE testTable SET content = ?1"
        ) { handle.updateObject(renewObject, DBTestObject.content, tableName) }
    }

    @Test
    fun testHandleUpdateObjectWhere() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 WHERE id == 1"
        ) {
            handle.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.eq(1)
            )
        }
    }

    @Test
    fun testHandleUpdateObjectWhereOrderLimit() {
        doTestObjectsAfterOperation(
            arrayOf(object1, renewedObject2),
            "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id DESC LIMIT 1"
        ) {
            handle.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testHandleUpdateObjectWhereOrderLimitOffset() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 WHERE id > 0 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            handle.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.gt(0),
                DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }

    @Test
    fun testHandleUpdateObjectOrderLimit() {
        doTestObjectsAfterOperation(
            arrayOf(object1, renewedObject2),
            "UPDATE testTable SET content = ?1 ORDER BY id DESC LIMIT 1"
        ) {
            handle.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.order(Order.Desc), 1
            )
        }
    }

    @Test
    fun testHandleUpdateObjectOrderLimitOffset() {
        doTestObjectsAfterOperation(
            arrayOf(renewedObject1, object2),
            "UPDATE testTable SET content = ?1 ORDER BY id DESC LIMIT 1 OFFSET 1"
        ) {
            handle.updateObject(
                renewObject,
                DBTestObject.content,
                tableName,
                DBTestObject.id.order(Order.Desc), 1, 1
            )
        }
    }
}