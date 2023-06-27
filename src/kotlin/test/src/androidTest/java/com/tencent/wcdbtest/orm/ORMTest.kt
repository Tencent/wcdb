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

package com.tencent.wcdbtest.orm

import com.tencent.wcdb.base.WCDBException
import com.tencent.wcdb.core.Database.ExceptionTracer
import com.tencent.wcdb.orm.Field
import com.tencent.wcdb.winq.ColumnDef
import com.tencent.wcdb.winq.ColumnType
import com.tencent.wcdb.winq.Order
import com.tencent.wcdb.winq.StatementCreateTable
import com.tencent.wcdbtest.base.DatabaseTestCase
import com.tencent.wcdbtest.base.TestOperation
import com.tencent.wcdbtest.base.WrappedValue
import com.tencent.wcdbtest.orm.testclass.*
import org.junit.Assert
import org.junit.Before
import org.junit.Test
import java.util.*

class ORMTest : DatabaseTestCase() {
    private var tableName = "testTable"
    @Before
    @Throws(WCDBException::class)
    override fun setup() {
        super.setup()
        expectMode = Expect.SomeSQLs
    }

    private fun doTestCreateTableAndIndexSQLsAsExpected(sqls: Array<String>, operation: TestOperation) {
        Assert.assertNotNull(sqls)
        Assert.assertNotNull(operation)
        val newSqls: MutableList<String> = ArrayList()
        newSqls.add("BEGIN IMMEDIATE")
        newSqls.addAll(listOf(*sqls))
        newSqls.add("COMMIT")
        doTestSQLs(newSqls.toTypedArray(), operation)
    }

    @Test
    fun testField() {
        doTestCreateTableAndIndexSQLsAsExpected(arrayOf("CREATE TABLE IF NOT EXISTS testTable(field INTEGER, differentName INTEGER)")) {
            database.createTable(
                tableName,
                DBFieldObject
            )
        }
        doTestCreateTableAndIndexSQLsAsExpected(arrayOf("CREATE TABLE IF NOT EXISTS newTable(field INTEGER, differentName INTEGER)")) {
            database.createTable(
                "newTable",
                DBFieldObject
            )
        }
    }

    @Test
    fun testAllField() {
        Assert.assertEquals(DBFieldObject.allFields().size.toLong(), 2)
        Assert.assertEquals(DBFieldObject.allFields()[0].description, "field")
        Assert.assertEquals(DBFieldObject.allFields()[1].description, "differentName")
    }

    @Test
    @Throws(WCDBException::class)
    fun testAllType() {
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(type TEXT, " +
                        "aBoolean INTEGER, aBoolean2 INTEGER, " +
                        "aChar INTEGER, character INTEGER, " +
                        "aByte INTEGER, aByte2 INTEGER, " +
                        "aShort INTEGER, aShort2 INTEGER, " +
                        "anInt INTEGER, integer INTEGER, " +
                        "aLong INTEGER, aLong2 INTEGER, " +
                        "aFloat REAL, aFloat2 REAL, " +
                        "aDouble REAL, aDouble2 REAL, " +
                        "string TEXT, bytes BLOB, bytes2 BLOB)"
            )
        ) { database.createTable(tableName, DBAllTypeObject) }
        val table = database.getTable(tableName, DBAllTypeObject)
        val max = AllTypeObject.maxObject()
        val min = AllTypeObject.minObject()
        val random = AllTypeObject.randomObject()
        val empty = AllTypeObject.emptyObject()
        table.insertObjects(arrayOf(max, min, random, empty))
        Assert.assertTrue(
            max == table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(max.type))
        )
        Assert.assertTrue(
            min == table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(min.type))
        )
        Assert.assertTrue(
            empty == table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(empty.type))
        )
        Assert.assertTrue(
            random == table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(random.type))
        )
    }

    @Test
    fun testColumnConstraint() {
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY)"
            )
        ) {
            database.createTable(tableName, DBColumnConstraintPrimaryObject)
            database.dropTable("testTable")
        }
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY AUTOINCREMENT)"
            )
        ) {
            database.createTable(tableName, DBColumnConstraintAutoIncrementObject)
            database.dropTable("testTable")
        }
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER UNIQUE)"
            )
        ) {
            database.createTable(tableName, DBColumnConstraintUniqueObject)
            database.dropTable("testTable")
        }
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(intValue INTEGER DEFAULT 0, intValue2 INTEGER DEFAULT 1, floatValue REAL DEFAULT 1.1000000000000001, stringValue TEXT DEFAULT 'abc')"
            )
        ) {
            database.createTable(tableName, DBColumnConstraintDefaultObject)
            database.dropTable("testTable")
        }
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(text TEXT NOT NULL)"
            )
        ) {
            database.createTable(tableName, DBColumnConstraintNotNullObject)
            database.dropTable("testTable")
        }
    }

    @Test
    fun testTableConstraint() {
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(multiPrimary1 INTEGER, multiPrimary2 INTEGER, multiPrimary3 INTEGER, " +
                        "multiUnique1 INTEGER, multiUnique2 INTEGER, multiUnique3 INTEGER, " +
                        "multiIndex1 INTEGER, multiIndex2 INTEGER, multiIndex3 INTEGER, " +
                        "PRIMARY KEY(multiPrimary1, multiPrimary2, multiPrimary3), " +
                        "UNIQUE(multiUnique1, multiUnique2, multiUnique3))",
                "CREATE INDEX IF NOT EXISTS testTable_multi_index ON testTable(multiIndex1, multiIndex2, multiIndex3)"
            )
        ) { database.createTable(tableName, DBTableConstraintObject) }
    }

    @Test
    fun testRemap() {
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER)"
            )
        ) { database.createTable("testTable", DBOldRemapObject) }
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "PRAGMA main.table_info('testTable')",
                "ALTER TABLE main.testTable ADD COLUMN newValue INTEGER",
                "CREATE INDEX IF NOT EXISTS testTable_index ON testTable(value)"
            )
        ) { database.createTable("testTable", DBNewRemapObject) }
    }

    @Test
    fun testIndex() {
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(index_ INTEGER, uniqueIndex INTEGER)",
                "CREATE INDEX IF NOT EXISTS testTable_index ON testTable(index_)",
                "CREATE UNIQUE INDEX IF NOT EXISTS testTable_unique_index ON testTable(uniqueIndex)"
            )
        ) { database.createTable(tableName, DBIndexObject) }
    }

    @Test
    fun testIndex2() {
        doTestCreateTableAndIndexSQLsAsExpected(
            arrayOf(
                "CREATE TABLE IF NOT EXISTS testTable(index_ INTEGER, uniqueIndex INTEGER)",
                "CREATE INDEX IF NOT EXISTS testTable_index ON testTable(index_)",
                "CREATE UNIQUE INDEX IF NOT EXISTS testTable_unique_index ON testTable(uniqueIndex)"
            )
        ) { database.createTable(tableName, DBDataIndexObject) }
    }

    @Test
    @Throws(WCDBException::class)
    fun testAutoAddColumn() {
        val fakeTable = "fakeTable"
        val fakeSchema = "notExistSchema"
        database.createTable(fakeTable, DBAutoAddColumnObject)
        doTestAutoAddColumn(DBAutoAddColumnObject.insertValue, true) {
            database.insertObject(
                AutoAddColumnObject(), DBAutoAddColumnObject.allFields(), tableName
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.updateValue, true) {
            database.updateValue(
                1,
                DBAutoAddColumnObject.updateValue,
                tableName
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, true) {
            database.deleteObjects(
                tableName,
                DBAutoAddColumnObject.deleteValue.eq(1)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, true) {
            database.deleteObjects(
                tableName,
                DBAutoAddColumnObject.deleteValue.table(tableName).eq(1)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, false) {
            database.deleteObjects(
                tableName,
                DBAutoAddColumnObject.deleteValue.table(fakeTable).eq(1)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, false) {
            database.deleteObjects(
                tableName,
                DBAutoAddColumnObject.deleteValue.table(tableName).of(fakeSchema).eq(1)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true) {
            database.getOneColumn(
                DBAutoAddColumnObject.selectValue,
                tableName
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true) {
            database.getOneColumn(
                DBAutoAddColumnObject.insertValue,
                tableName,
                DBAutoAddColumnObject.selectValue.eq(1)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true) {
            database.getOneColumn(
                DBAutoAddColumnObject.insertValue,
                tableName,
                DBAutoAddColumnObject.selectValue.order(Order.Asc)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true) {
            database.getOneColumn(
                DBAutoAddColumnObject.insertValue,
                tableName,
                DBAutoAddColumnObject.selectValue.table(tableName).order(Order.Asc)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, false) {
            database.getOneColumn(
                DBAutoAddColumnObject.insertValue,
                tableName,
                DBAutoAddColumnObject.selectValue.table(fakeTable).order(Order.Asc)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, false) {
            database.getOneColumn(
                DBAutoAddColumnObject.insertValue,
                tableName,
                DBAutoAddColumnObject.selectValue.table(tableName).of(fakeSchema).order(Order.Asc)
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.primaryValue, false) {
            database.getOneColumn(
                DBAutoAddColumnObject.primaryValue,
                tableName
            )
        }
        doTestAutoAddColumn(DBAutoAddColumnObject.uniqueValue, false) {
            database.getOneColumn(
                DBAutoAddColumnObject.uniqueValue,
                tableName
            )
        }
    }

    @Throws(WCDBException::class)
    fun doTestAutoAddColumn(
        removeField: Field<AutoAddColumnObject>,
        succeed: Boolean,
        operation: TestOperation
    ) {
        val columnName = removeField.name
        val createTable = StatementCreateTable().createTable(tableName)
        val columnDefs: MutableList<ColumnDef> = ArrayList()
        for (field in DBAutoAddColumnObject.allFields()) {
            if (field.description != columnName) {
                columnDefs.add(ColumnDef(field, ColumnType.Integer))
            }
        }
        createTable.define(columnDefs.toTypedArray())
        database.execute(createTable)
        val added = WrappedValue()
        database.traceException(ExceptionTracer { exception ->
            if (exception.message() != "Auto add column") {
                return@ExceptionTracer
            }
            added.boolValue = true
            Assert.assertEquals(exception.infos["Table"] as String?, tableName)
            Assert.assertEquals(exception.infos["Column"] as String?, columnName)
        })
        var hasError = false
        try {
            operation()
        } catch (e: WCDBException) {
            hasError = true
        }
        Assert.assertEquals(succeed, !hasError)
        Assert.assertEquals(succeed, added.boolValue)
        database.dropTable(tableName)
        database.traceException(null)
    }
}