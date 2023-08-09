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

import com.tencent.wcdb.winq.Order
import com.tencent.wcdbtest.base.DBTestObject
import com.tencent.wcdbtest.base.ObjectCRUDTestCase

class ObjectDeleteTest : ObjectCRUDTestCase() {
    @org.junit.Test
    fun testDatabaseDelete() {
        doTestSQL("DELETE FROM testTable") {
            database.deleteObjects(tableName)
        }
        org.junit.Assert.assertEquals(allRowsCount(), 0)
    }

    @org.junit.Test
    fun testDatabaseDeleteWhere() {
        doTestSQL("DELETE FROM testTable WHERE id == 1") {
            database.deleteObjects(
                tableName,
                DBTestObject.id.eq(1)
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testDatabaseDeleteOrderLimit() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1") {
            database.deleteObjects(
                tableName,
                DBTestObject.content.order(Order.Desc),
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testDatabaseDeleteOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1 OFFSET 1") {
            database.deleteObjects(
                tableName,
                DBTestObject.content.order(Order.Desc),
                1,
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testDatabaseDeleteWhereOrderLimit() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 3) ORDER BY content DESC LIMIT 1") {
            database.deleteObjects(
                tableName,
                DBTestObject.id.`in`(intArrayOf(1, 3)),
                DBTestObject.content.order(Order.Desc),
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testDatabaseDeleteWhereOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 2) ORDER BY content DESC LIMIT 1 OFFSET 1") {
            database.deleteObjects(
                tableName,
                DBTestObject.id.`in`(intArrayOf(1, 2)),
                DBTestObject.content.order(Order.Desc),
                1,
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testTableDelete() {
        doTestSQL("DELETE FROM testTable") { table.deleteObjects() }
        org.junit.Assert.assertEquals(allRowsCount(), 0)
    }

    @org.junit.Test
    fun testTableDeleteWhere() {
        doTestSQL("DELETE FROM testTable WHERE id == 1") { table.deleteObjects(DBTestObject.id.eq(1)) }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testTableDeleteOrderLimit() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1") {
            table.deleteObjects(
                DBTestObject.content.order(Order.Desc),
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testTableDeleteOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1 OFFSET 1") {
            table.deleteObjects(
                DBTestObject.content.order(Order.Desc),
                1,
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testTableDeleteWhereOrderLimit() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 3) ORDER BY content DESC LIMIT 1") {
            table.deleteObjects(
                DBTestObject.id.`in`(intArrayOf(1, 3)),
                DBTestObject.content.order(Order.Desc),
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testTableDeleteWhereOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 2) ORDER BY content DESC LIMIT 1 OFFSET 1") {
            table.deleteObjects(
                DBTestObject.id.`in`(intArrayOf(1, 2)),
                DBTestObject.content.order(Order.Desc),
                1,
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testHandleDelete() {
        doTestSQL("DELETE FROM testTable") { handle.deleteObjects(tableName) }
        org.junit.Assert.assertEquals(allRowsCount(), 0)
    }

    @org.junit.Test
    fun testHandleDeleteWhere() {
        doTestSQL("DELETE FROM testTable WHERE id == 1") {
            handle.deleteObjects(
                tableName,
                DBTestObject.id.eq(1)
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testHandleDeleteOrderLimit() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1") {
            handle.deleteObjects(
                tableName,
                DBTestObject.content.order(Order.Desc),
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testHandleDeleteOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable ORDER BY content DESC LIMIT 1 OFFSET 1") {
            handle.deleteObjects(
                tableName,
                DBTestObject.content.order(Order.Desc),
                1,
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testHandleDeleteWhereOrderLimit() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 3) ORDER BY content DESC LIMIT 1") {
            handle.deleteObjects(
                tableName,
                DBTestObject.id.`in`(intArrayOf(1, 3)),
                DBTestObject.content.order(Order.Desc),
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }

    @org.junit.Test
    fun testHandleDeleteWhereOrderLimitOffset() {
        doTestSQL("DELETE FROM testTable WHERE id IN(1, 2) ORDER BY content DESC LIMIT 1 OFFSET 1") {
            handle.deleteObjects(
                tableName,
                DBTestObject.id.`in`(intArrayOf(1, 2)),
                DBTestObject.content.order(Order.Desc),
                1,
                1
            )
        }
        org.junit.Assert.assertEquals(allRowsCount(), 1)
    }
}