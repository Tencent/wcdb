// Created by chenqiuwen on 2023/6/24.
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

package com.tencent.wcdbtest.base

import com.tencent.wcdb.base.WCDBException
import com.tencent.wcdb.core.Table
import com.tencent.wcdb.orm.TableBinding
import org.junit.Assert
import java.util.*

typealias SelectingObjectOperation = () -> List<TestObject>

open class TableTestCase : DatabaseTestCase() {
    var tableName = "testTable"
    var tableBinding: TableBinding<TestObject> = DBTestObject
    var isVirtualTable = false
    lateinit var table: Table<TestObject>

    @Throws(WCDBException::class)
    fun createTable() {
        if (!isVirtualTable) {
            database.createTable(tableName, tableBinding)
        } else {
            database.createVirtualTable(tableName, tableBinding)
        }
        table = database.getTable(tableName, tableBinding)
    }

    @Throws(WCDBException::class)
    fun createVirtualTable() {
        database.createVirtualTable(tableName, tableBinding)
        table = database.getTable(tableName, tableBinding)
    }

    @Throws(WCDBException::class)
    fun dropTable() {
        database.dropTable(tableName)
    }

    fun doTestObjectsAfterInsert(
        objects: Array<Any>,
        insertCount: Int,
        sqls: Array<String>,
        operation: TestOperation
    ) {
        var newSqls = sqls
        if (insertCount > 1) {
            val list: MutableList<String> = mutableListOf(*sqls)
            list.add(0, "BEGIN IMMEDIATE")
            list.add("COMMIT")
            newSqls = list.toTypedArray()
        }
        doTestObjectsAfterOperation(objects, newSqls, operation)
    }

    fun doTestObjectsAfterOperation(
        objects: Array<Any>,
        sql: String,
        operation: TestOperation
    ) {
        doTestObjectsAfterOperation(objects, arrayOf(sql), operation)
    }

    fun doTestObjectsAfterOperation(
        objects: Array<Any>,
        sqls: Array<String>,
        operation: TestOperation
    ) {
        doTestSQLs(sqls, operation)
        val allObjects: List<TestObject> = try {
            this.allObjects
        } catch (e: WCDBException) {
            throw RuntimeException(e)
        }
        Assert.assertTrue(
            (objects.isEmpty() && allObjects.isEmpty()) ||
                    objects.contentEquals(allObjects.toTypedArray())
        )
    }

    fun doTestObjectBySelecting(
        `object`: TestObject,
        sql: String,
        operation: SelectingObjectOperation
    ) {
        doTestObjectBySelecting(arrayOf(`object`), arrayOf(sql), operation)
    }

    fun doTestObjectBySelecting(
        objects: Array<TestObject>,
        sql: String,
        operation: SelectingObjectOperation
    ) {
        doTestObjectBySelecting(objects, arrayOf(sql), operation)
    }

    fun doTestObjectBySelecting(
        objects: Array<TestObject>,
        sqls: Array<String>,
        operation: SelectingObjectOperation
    ) {
        val selecting = mutableListOf<TestObject>()
        doTestSQLs(sqls) {
            selecting.addAll(operation())
        }
        Assert.assertTrue(
            (objects.isEmpty() && selecting.isEmpty()) ||
                    objects.contentEquals(selecting.toTypedArray())
        )
    }

    @get:Throws(WCDBException::class)
    val allObjects: List<TestObject>
        get() = table.allObjects
}