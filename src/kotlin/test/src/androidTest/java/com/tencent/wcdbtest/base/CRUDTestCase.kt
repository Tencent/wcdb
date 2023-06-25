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

import com.tencent.wcdb.base.Value
import com.tencent.wcdb.base.WCDBException
import com.tencent.wcdb.core.Handle
import com.tencent.wcdb.winq.Column
import org.junit.After
import org.junit.Before

open class CRUDTestCase : TableTestCase() {
    protected lateinit var object1: TestObject
    protected lateinit var object2: TestObject
    protected lateinit var objects: Array<TestObject>
    protected var nextId = 3
    protected var id = Column("id")
    protected var content = Column("content")
    protected lateinit var handle: Handle
    @Before
    @Throws(WCDBException::class)
    override fun setup() {
        super.setup()
        object1 = RandomTool.testObjectWithId(1)
        object2 = RandomTool.testObjectWithId(2)
        objects = arrayOf(object1, object2)
        handle = database.handle
        table = database.getTable(tableName, DBTestObject)
    }

    @After
    override fun teardown() {
        handle.invalidate()
        super.teardown()
    }

    @Throws(WCDBException::class)
    fun insertPresetObjects() {
        createTable()
        table.insertObjects(objects)
    }

    fun allRowsCount(): Long {
        return try {
            database.getValue(Column.all().count(), tableName).integer
        } catch (e: WCDBException) {
            throw RuntimeException(e)
        }
    }
}