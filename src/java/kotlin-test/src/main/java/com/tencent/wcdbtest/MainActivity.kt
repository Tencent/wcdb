// Created by qiuwenchen on 2024/4/27.
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
package com.tencent.wcdbtest

import android.os.Bundle
import android.os.Process
import androidx.activity.ComponentActivity
import com.tencent.wcdb.base.WCDBException
import com.tencent.wcdb.core.Database
import com.tencent.wcdb.core.Database.SQLTracer
import com.tencent.wcdb.core.Table
import com.tencent.wcdb.winq.Pragma
import com.tencent.wcdb.winq.StatementPragma
import com.tencent.wcdb.winq.StatementSelect
import java.io.File
import java.lang.Thread.sleep
import kotlin.concurrent.thread

class MainActivity : ComponentActivity() {
    private var database: Database? = null
    private var tableName = "testTable"
    private var table: Table<TestObject>? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        database = Database(cacheDir.path + File.separator + "temp.db")
        table = database?.getTable(tableName, DBTestObject)
        thread(start = true) {
            println("start test proguard")
            execute { testBusyRetry() }
            execute { testReadOnly() }
            execute { testConfigFail() }
            execute { testGlobalTraceError() }
            execute { testGlobalTraceSQL() }
            execute { testZSTDNormalCompress() }
            println("all proguard tests are finished!")
        }
    }

    private fun execute(test: () -> Unit) {
        database?.removeFiles()
        database?.createTable(tableName, DBTestObject)
        test()
    }

    private fun testBusyRetry() {
        val testTid = WrappedValue()
        Database.globalTraceBusy({ tag, path, tid, sql ->
            require(tag == database?.tag)
            require(path == database?.path)
            require(sql == "INSERT INTO testTable(id, content) VALUES(?1, ?2)")
            testTid.intValue = tid
        }, 0.1)
        val objects = RandomTool.autoIncrementTestCaseObjects(100000)
        val dispatchTid = WrappedValue()
        val thread = Thread {
            dispatchTid.intValue = Process.myTid().toLong()
            database?.insertObjects(objects, DBTestObject.allFields(), tableName)
        }
        thread.start()
        sleep(100)
        database?.insertObject(
            RandomTool.testObjectWithId(100001),
            DBTestObject.allFields(),
            tableName
        )
        thread.join()
        require(testTid.intValue != 0L && testTid.intValue == dispatchTid.intValue)
        Database.globalTraceBusy(null, 0.0)
    }

    private fun testZSTDNormalCompress() {
        val preInsertObject = RandomTool.autoIncrementTestCaseObjects(2)
        table?.insertObjects(preInsertObject)
        database?.setCompression { info -> info.addZSTDNormalCompress(DBTestObject.content) }
        val tableCompressed = WrappedValue()
        val databaseCompressed = WrappedValue()
        database?.setNotificationWhenCompressed { notifiedDatabase, table ->
            require(database?.tag == notifiedDatabase.tag)
            require(database?.path == notifiedDatabase.path)
            if (table != null) {
                if (table == tableName) {
                    tableCompressed.boolValue = true
                }
            } else {
                databaseCompressed.boolValue = true
            }
        }
        require(!database?.isCompressed!!)
        database?.stepCompression()
        database?.stepCompression()
        require(database!!.isCompressed)
        require(tableCompressed.boolValue && databaseCompressed.boolValue)
    }

    private fun testReadOnly() {
        table!!.insertObject(TestObject.createAutoIncrementObject(RandomTool.string()))
        database!!.close {
            for (path in database!!.paths) {
                val file = File(path)
                if (file.exists()) {
                    file.setReadOnly()
                }
            }
        }
        require(database!!.canOpen())
        val objects = table!!.allObjects
        require(objects.size == 1)
        var failed = false
        try {
            table!!.insertObject(TestObject.createAutoIncrementObject(RandomTool.string()))
        } catch (e: WCDBException) {
            failed = true
        }
        require(failed)
    }

    private fun testConfigFail() {
        database!!.setConfig("testConfig") { handle ->
            handle.execute(StatementSelect().select("testColumn").from("testTable"))
        }
        require(!database!!.canOpen())
        database!!.setConfig("testConfig", null)
    }

    private fun testGlobalTraceError() {
        val tested = WrappedValue()
        Database.globalTraceException(null)
        Database.globalTraceException { exception ->
            if (exception.level == WCDBException.Level.Error &&
                exception.path() == database!!.path &&
                exception.tag() == database!!.tag &&
                exception.code == WCDBException.Code.Error &&
                exception.sql() == "SELECT 1 FROM dummy" &&
                exception.message() == "no such table: dummy"
            ) {
                tested.boolValue = true
            }
        }
        require(database!!.canOpen())
        try {
            database!!.execute(StatementSelect().select("1").from("dummy"))
        } catch (e: WCDBException) {
            require(tested.boolValue)
        } finally {
            require(tested.boolValue)
        }
    }

    private fun testGlobalTraceSQL() {
        val statement = StatementPragma().pragma(Pragma.userVersion)
        val tested = WrappedValue()
        database!!.close();
        Database.globalTraceSQL(null)
        Database.globalTraceSQL(SQLTracer { tag, path, handleId, sql, info ->
            if (database!!.path != path) {
                return@SQLTracer
            }
            require(tag == database!!.tag)
            if (sql == statement.description) {
                tested.boolValue = true
            }
        })
        database!!.execute(statement)
        require(tested.boolValue)
        Database.globalTraceSQL(null)
    }
}