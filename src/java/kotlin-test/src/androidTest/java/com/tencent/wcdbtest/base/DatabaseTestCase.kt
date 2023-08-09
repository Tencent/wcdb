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
import com.tencent.wcdb.core.Database
import org.junit.After
import org.junit.Assert
import org.junit.Before
import java.io.File
import java.util.*

typealias TestOperation = () -> Unit

open class DatabaseTestCase : BaseTestCase() {
    enum class Expect {
        AllSQLs, FirstFewSQLs, SomeSQLs
    }

    var path: String = ""
    var fileName: String = ""
    var database: Database = Database("")
    var expectMode: Expect = Expect.AllSQLs

    @Before
    @Throws(WCDBException::class)
    override fun setup() {
        super.setup()
        expectMode = Expect.AllSQLs
        fileName = "testDatabase"
        path = currentDirectory + File.separator + fileName
        database = Database(path)
        database.tag = 10001
    }

    @After
    open fun teardown() {
        database.close()
    }

    fun doTestSQL(sql: String, operation: TestOperation) {
        doTestSQLs(arrayOf(sql), operation)
    }

    fun doTestSQLs(sqls: Array<String>, operation: TestOperation) {
        Assert.assertNotNull(sqls)
        Assert.assertNotNull(operation)
        do {
            val trace = WrappedValue()
            trace.boolValue = false
            val expectedSQLs = mutableListOf(*sqls)
            val thread = Thread.currentThread()
            database.traceSQL { _, _, sql ->
                if (Thread.currentThread().id != thread.id) {
                    return@traceSQL
                }
                if (!trace.boolValue) {
                    return@traceSQL
                }
                doTestSQLAsExpected(expectedSQLs, sql)
            }
            if (expectMode != Expect.SomeSQLs) {
                if (!database.canOpen()) {
                    Assert.fail()
                    break
                }
            }
            trace.boolValue = true
            try {
                operation()
            } catch (e: WCDBException) {
                throw RuntimeException(e)
            }
            if (expectedSQLs.isNotEmpty()) {
                logError("Reminding: $expectedSQLs")
                Assert.fail()
                break
            }
            trace.boolValue = false
        } while (false)
        database.traceSQL(null)
    }

    private fun doTestSQLAsExpected(expectedSQLs: MutableList<String>, sql: String) {
        when (expectMode) {
            Expect.AllSQLs -> {
                if (expectedSQLs[0] == sql) {
                    expectedSQLs.removeAt(0)
                } else {
                    Assert.assertEquals(expectedSQLs[0], sql)
                }
            }
            Expect.FirstFewSQLs -> {
                if (expectedSQLs.size > 0 && expectedSQLs[0] == sql) {
                    expectedSQLs.removeAt(0)
                } else if (expectedSQLs.size != 0) {
                    Assert.assertEquals(expectedSQLs[0], sql)
                }
            }
            Expect.SomeSQLs -> {
                var i = 0
                while (i < expectedSQLs.size) {
                    if (expectedSQLs[i] == sql) {
                        expectedSQLs.removeAt(i)
                        break
                    }
                    i++
                }
            }
        }
    }
}