// Created by qiuwenchen on 2023/5/12.
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
package com.tencent.wcdbtest.orm;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Table;
import com.tencent.wcdb.fts.BuiltinTokenizer;
import com.tencent.wcdb.orm.Field;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.ColumnDef;
import com.tencent.wcdb.winq.ColumnType;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.StatementCreateTable;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.DatabaseTestCase;
import com.tencent.wcdbtest.base.WrappedValue;
import com.tencent.wcdbtest.orm.testclass.*;

import static org.junit.Assert.*;

import org.jetbrains.annotations.NotNull;

import org.junit.Before;
import org.junit.Test;

import java.util.List;
import java.util.Arrays;
import java.util.ArrayList;

public class ORMTest extends DatabaseTestCase {
    String tableName = "testTable";

    @Before
    public void setup() throws WCDBException {
        super.setup();
        expectMode = Expect.SomeSQLs;
    }

    void doTestCreateTableAndIndexSQLsAsExpected(String[] sqls, TestOperation operation) {
        assertNotNull(sqls);
        assertNotNull(operation);
        List<String> newSqls = new ArrayList<String>();
        newSqls.add("BEGIN IMMEDIATE");
        newSqls.addAll(Arrays.asList(sqls));
        newSqls.add("COMMIT");
        doTestSQLs(newSqls.toArray(new String[0]), operation);
    }

    @Test
    public void testField() {
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{"CREATE TABLE IF NOT EXISTS testTable(field INTEGER, differentName INTEGER)"}, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBFieldObject.INSTANCE);
            }
        });
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{"CREATE TABLE IF NOT EXISTS newTable(field INTEGER, differentName INTEGER)"}, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable("newTable", DBFieldObject.INSTANCE);
            }
        });
    }

    @Test
    public void testAllField() {
        assertEquals(DBFieldObject.allFields().length, 2);
        assertEquals(DBFieldObject.allFields()[0].getDescription(), "field");
        assertEquals(DBFieldObject.allFields()[1].getDescription(), "differentName");
    }

    @Test
    public void testAllType() throws WCDBException {
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(type TEXT, " +
                        "aBoolean INTEGER, aBoolean2 INTEGER, " +
                        "aByte INTEGER, aByte2 INTEGER, " +
                        "aShort INTEGER, aShort2 INTEGER, " +
                        "anInt INTEGER, integer INTEGER, " +
                        "aLong INTEGER, aLong2 INTEGER, " +
                        "aFloat REAL, aFloat2 REAL, " +
                        "aDouble REAL, aDouble2 REAL, " +
                        "string TEXT, bytes BLOB)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBAllTypeObject.INSTANCE);
            }
        });
        Table<AllTypeObject> table = database.getTable(tableName, DBAllTypeObject.INSTANCE);
        AllTypeObject max = AllTypeObjectHelper.maxObject();
        AllTypeObject min = AllTypeObjectHelper.minObject();
        AllTypeObject random = AllTypeObjectHelper.randomObject();
        AllTypeObject empty = AllTypeObjectHelper.emptyObject();
        table.insertObjects(Arrays.asList(max, min, random, empty));

        assertEquals(max, table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(max.type)));
        assertEquals(min, table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(min.type)));
        assertEquals(empty, table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(empty.type)));
        assertEquals(random, table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq(random.type)));

        table.insertRow(new Value[]{new Value("null")}, new Column[]{DBAllTypeObject.type});
        assertEquals(empty, table.getFirstObject(DBAllTypeObject.allFields(), DBAllTypeObject.type.eq("null")));
    }

    @Test
    public void testColumnConstraint() {
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBColumnConstraintPrimaryObject.INSTANCE);
                database.dropTable("testTable");
            }
        });

        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER PRIMARY KEY AUTOINCREMENT)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBColumnConstraintAutoIncrementObject.INSTANCE);
                database.dropTable("testTable");
            }
        });

        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER UNIQUE)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBColumnConstraintUniqueObject.INSTANCE);
                database.dropTable("testTable");
            }
        });

        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(intValue INTEGER DEFAULT 1, floatValue REAL DEFAULT 1.1000000000000001, stringValue TEXT DEFAULT 'abc')"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBColumnConstraintDefaultObject.INSTANCE);
                database.dropTable("testTable");
            }
        });

        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(intValue INTEGER DEFAULT 0, floatValue REAL DEFAULT 0, stringValue TEXT DEFAULT '')"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBColumnConstraintDefaultZeroObject.INSTANCE);
                database.dropTable("testTable");
            }
        });

        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(text TEXT NOT NULL)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBColumnConstraintNotNullObject.INSTANCE);
                database.dropTable("testTable");
            }
        });
    }

    @Test
    public void testTableConstraint() {
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(multiPrimary1 INTEGER, multiPrimary2 INTEGER, multiPrimary3 INTEGER, " +
                        "multiUnique1 INTEGER, multiUnique2 INTEGER, multiUnique3 INTEGER, " +
                        "multiIndex1 INTEGER, multiIndex2 INTEGER, multiIndex3 INTEGER, " +
                        "PRIMARY KEY(multiPrimary1, multiPrimary2, multiPrimary3), " +
                        "UNIQUE(multiUnique1, multiUnique2, multiUnique3))",
                "CREATE INDEX IF NOT EXISTS specifiedNameIndex ON testTable(multiIndex1, multiIndex2, multiIndex3)",
                "CREATE INDEX IF NOT EXISTS testTable_multiIndex1_multiIndex2_index ON testTable(multiIndex1, multiIndex2)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBTableConstraintObject.INSTANCE);
            }
        });
    }

    @Test
    public void testRemap() {
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(value INTEGER)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable("testTable", DBOldRemapObject.INSTANCE);
            }
        });
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "PRAGMA main.table_info('testTable')",
                "ALTER TABLE main.testTable ADD COLUMN newValue INTEGER",
                "CREATE INDEX IF NOT EXISTS testTable_value_index ON testTable(value)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable("testTable", DBNewRemapObject.INSTANCE);
            }
        });
    }

    @Test
    public void testIndex() {
        doTestCreateTableAndIndexSQLsAsExpected(new String[]{
                "CREATE TABLE IF NOT EXISTS testTable(index_ INTEGER, uniqueIndex INTEGER)",
                "CREATE INDEX IF NOT EXISTS testTable_index__index ON testTable(index_)",
                "CREATE UNIQUE INDEX IF NOT EXISTS specifiedNameIndex ON testTable(uniqueIndex)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.createTable(tableName, DBIndexObject.INSTANCE);
            }
        });
    }

    @Test
    public void testAutoAddColumn() throws WCDBException {
        final String fakeTable = "fakeTable";
        final String fakeSchema = "notExistSchema";
        database.createTable(fakeTable, DBAutoAddColumnObject.INSTANCE);

        doTestAutoAddColumn(DBAutoAddColumnObject.insertValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.insertObject(new AutoAddColumnObject(), DBAutoAddColumnObject.allFields(), tableName);
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.updateValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.updateValue(1, DBAutoAddColumnObject.updateValue, tableName);
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBAutoAddColumnObject.deleteValue.eq(1));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBAutoAddColumnObject.deleteValue.table(tableName).eq(1));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, false, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBAutoAddColumnObject.deleteValue.table(fakeTable).eq(1));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.deleteValue, false, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deleteObjects(tableName, DBAutoAddColumnObject.deleteValue.table(tableName).of(fakeSchema).eq(1));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.selectValue, tableName);
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.insertValue, tableName, DBAutoAddColumnObject.selectValue.eq(1));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.insertValue, tableName, DBAutoAddColumnObject.selectValue.order(Order.Asc));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, true, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.insertValue, tableName, DBAutoAddColumnObject.selectValue.table(tableName).order(Order.Asc));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, false, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.insertValue, tableName, DBAutoAddColumnObject.selectValue.table(fakeTable).order(Order.Asc));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.selectValue, false, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.insertValue, tableName, DBAutoAddColumnObject.selectValue.table(tableName).of(fakeSchema).order(Order.Asc));
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.primaryValue, false, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.primaryValue, tableName);
            }
        });

        doTestAutoAddColumn(DBAutoAddColumnObject.uniqueValue, false, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.getOneColumn(DBAutoAddColumnObject.uniqueValue, tableName);
            }
        });
    }

    void doTestAutoAddColumn(Field<AutoAddColumnObject> removeField, boolean succeed, TestOperation operation) throws WCDBException {
        final String columnName = removeField.getName();
        final StatementCreateTable createTable = new StatementCreateTable().createTable(tableName);
        List<ColumnDef> columnDefs = new ArrayList<ColumnDef>();
        for (Field<AutoAddColumnObject> field : DBAutoAddColumnObject.allFields()) {
            if (!field.getDescription().equals(columnName)) {
                columnDefs.add(new ColumnDef(field, ColumnType.Integer));
            }
        }

        createTable.define(columnDefs.toArray(new ColumnDef[0]));
        database.execute(createTable);
        final WrappedValue added = new WrappedValue();
        database.traceException(new Database.ExceptionTracer() {
            @Override
            public void onTrace(@NotNull WCDBException exception) {
                if (!exception.message().equals("Auto add column")) {
                    return;
                }
                added.boolValue = true;
                assertEquals((String) exception.info.get("Table"), tableName);
                assertEquals((String) exception.info.get("Column"), columnName);
            }
        });
        boolean hasError = false;
        try {
            operation.execute();
        } catch (WCDBException e) {
            hasError = true;
        }
        assertEquals(succeed, !hasError);
        assertEquals(succeed, added.boolValue);
        database.dropTable(tableName);
        database.traceException(null);
    }

    @Test
    public void testPrimaryKeyEnableAutoIncrementForExistingTable() throws WCDBException {
        database.createTable(tableName, DBPrimaryNotAutoIncrementObject.INSTANCE);
        PrimaryNotAutoIncrementObject obj1 = new PrimaryNotAutoIncrementObject();
        obj1.id = 1;
        database.insertObject(obj1, DBPrimaryNotAutoIncrementObject.allFields(), tableName);

        database.createTable(tableName, DBPrimaryEnableAutoIncrementObject.INSTANCE);
        database.deleteObjects(tableName);

        PrimaryEnableAutoIncrementObject obj2 = new PrimaryEnableAutoIncrementObject();
        database.insertObject(obj2, DBPrimaryEnableAutoIncrementObject.allFields(), tableName);
        assertEquals(obj2.id, 2);
    }

    @Test
    public void testFTS3Object() {
        doTestSQLs(new String[]{
                "CREATE VIRTUAL TABLE IF NOT EXISTS testTable USING fts3(tokenize = wcdb_one_or_binary skip_stemming, id INTEGER, content TEXT, notindexed=id)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.addTokenizer(BuiltinTokenizer.OneOrBinary);
                database.createVirtualTable(tableName, DBFTS3TestObject.INSTANCE);
            }
        });
    }

    @Test
    public void testFTS5Object() {
        doTestSQLs(new String[]{
                "CREATE VIRTUAL TABLE IF NOT EXISTS testTable USING fts5(tokenize = 'wcdb_verbatim skip_stemming chinese_traditional_to_simplified', content='contentTable', id UNINDEXED, content)"
        }, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.addTokenizer(BuiltinTokenizer.Verbatim);
                database.createVirtualTable(tableName, DBFTS5TestObject.INSTANCE);
            }
        });
    }
}
