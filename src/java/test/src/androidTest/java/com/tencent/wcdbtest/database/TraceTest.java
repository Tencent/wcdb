// Created by chenqiuwen on 2023/5/10.
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

package com.tencent.wcdbtest.database;

import org.jetbrains.annotations.NotNull;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.Pragma;
import com.tencent.wcdb.winq.StatementCreateIndex;
import com.tencent.wcdb.winq.StatementPragma;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdbtest.base.FileTool;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;

import static org.junit.Assert.*;

import android.os.Process;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

@RunWith(AndroidJUnit4.class)
public class TraceTest extends TableTestCase {
    @Test
    public void testTraceSQL() throws WCDBException {
        final StatementPragma statement = new StatementPragma().pragma(Pragma.userVersion);

        final WrappedValue tested = new WrappedValue();
        database.traceSQL(new Database.SQLTracer() {
            @Override
            public void onTrace(long tag, @NotNull String path, long handleId, @NotNull String sql, @NotNull String info) {
                assertEquals(tag, database.getTag());
                assertEquals(path, database.getPath());
                if (sql.equals(statement.getDescription())) {
                    tested.boolValue = true;
                }
            }
        });
        database.execute(statement);
        assertTrue(tested.boolValue);
        database.traceSQL(null);
    }

    @Test
    public void testGlobalTraceSQL() throws WCDBException {
        final StatementPragma statement = new StatementPragma().pragma(Pragma.userVersion);

        final WrappedValue tested = new WrappedValue();
        Database.globalTraceSQL(null);
        Database.globalTraceSQL(new Database.SQLTracer() {
            @Override
            public void onTrace(long tag, @NotNull String path, long handleId, @NotNull String sql, @NotNull String info) {
                if (!database.getPath().equals(path)) {
                    return;
                }
                assertEquals(tag, database.getTag());
                if (sql.equals(statement.getDescription())) {
                    tested.boolValue = true;
                }
            }
        });
        database.execute(statement);
        assertTrue(tested.boolValue);
        Database.globalTraceSQL(null);
    }

    @Test
    public void testTracePerformance() throws WCDBException {
        database.removeFiles();
        createTable();
        database.tracePerformance(null);
        ArrayList<TestObject> objects = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            TestObject object = new TestObject();
            object.content = RandomTool.string(4096);
            objects.add(object);
        }
        WrappedValue testCount = new WrappedValue();
        database.tracePerformance(new Database.PerformanceTracer() {
            @Override
            public void onTrace(long tag, @NotNull String path, long handleId, @NotNull String sql, @NotNull Database.PerformanceInfo info) {
                assertEquals(tag, database.getTag());
                assertEquals(path, database.getPath());
                assertNotNull(info);
                if (sql.startsWith("COMMIT")) {
                    assertTrue(info.costInNanoseconds > 0);
                    assertTrue(info.tablePageWriteCount > 0);
                    assertEquals(0, info.indexPageWriteCount);
                    assertTrue(info.overflowPageWriteCount > 0);
                    assertEquals(0, info.tablePageReadCount);
                    assertEquals(0, info.indexPageReadCount);
                    assertEquals(0, info.overflowPageReadCount);
                    testCount.intValue++;
                } else if (sql.startsWith("CREATE INDEX")) {
                    assertTrue(info.costInNanoseconds > 0);
                    assertEquals(1, info.tablePageWriteCount);
                    assertTrue(info.indexPageWriteCount > 0);
                    assertEquals(info.overflowPageWriteCount, objects.size());
                    assertTrue(info.tablePageReadCount > 0);
                    assertTrue(info.indexPageReadCount >= 0);
                    assertTrue(info.overflowPageReadCount > objects.size() / 2);
                    testCount.intValue++;
                } else if (sql.startsWith("SELECT")) {
                    assertTrue(info.costInNanoseconds > 0);
                    assertEquals(0, info.tablePageWriteCount);
                    assertEquals(0, info.indexPageWriteCount);
                    assertEquals(0, info.overflowPageWriteCount);
                    testCount.intValue++;
                    if (sql.endsWith("ORDER BY content DESC")) {
                        assertEquals(0, info.tablePageReadCount);
                        assertTrue(info.indexPageReadCount > 0);
                        assertEquals(info.overflowPageReadCount, objects.size());
                    } else {
                        assertTrue(info.tablePageReadCount > 0);
                        assertEquals(0, info.indexPageReadCount);
                        assertEquals(info.overflowPageReadCount, objects.size());
                    }
                }
            }
        });
        database.insertObjects(objects, DBTestObject.allFields(), tableName);
        database.execute(new StatementCreateIndex().createIndex("testIndex").on(tableName).indexedBy(DBTestObject.content));
        assertEquals(database.getAllObjects(DBTestObject.allFields(), tableName).size(), objects.size());
        assertEquals(database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.content.order(Order.Desc)).size(), objects.size());
        assertEquals(testCount.intValue, 4);
        database.tracePerformance(null);
    }

    @Test
    public void testGlobalTracePerformance() throws WCDBException {
        database.removeFiles();
        ArrayList<TestObject> objects = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            TestObject object = new TestObject();
            object.content = RandomTool.string(4096);
            objects.add(object);
        }
        WrappedValue testCount = new WrappedValue();
        WrappedValue lastSQLIsInsert = new WrappedValue();
        Database.globalTracePerformance(null);
        Database.globalTracePerformance(new Database.PerformanceTracer() {
            @Override
            public void onTrace(long tag, @NotNull String path, long handleId, @NotNull String sql, @NotNull Database.PerformanceInfo info) {
                if (!database.getPath().equals(path)) {
                    return;
                }
                assertEquals(tag, database.getTag());
                if (sql.startsWith("COMMIT") && lastSQLIsInsert.boolValue) {
                    assertTrue(info.costInNanoseconds > 0);
                    assertTrue(info.tablePageWriteCount > 0);
                    assertEquals(0, info.indexPageWriteCount);
                    assertTrue(info.overflowPageWriteCount > 0);
                    assertEquals(0, info.tablePageReadCount);
                    assertEquals(0, info.indexPageReadCount);
                    assertEquals(0, info.overflowPageReadCount);
                    testCount.intValue++;
                } else if (sql.startsWith("CREATE INDEX")) {
                    assertTrue(info.costInNanoseconds > 0);
                    assertEquals(1, info.tablePageWriteCount);
                    assertTrue(info.indexPageWriteCount > 0);
                    assertEquals(info.overflowPageWriteCount, objects.size());
                    assertTrue(info.tablePageReadCount > 0);
                    assertTrue(info.indexPageReadCount >= 0);
                    assertTrue(info.overflowPageReadCount > objects.size() / 2);
                    testCount.intValue++;
                } else if (sql.startsWith("SELECT")) {
                    assertTrue(info.costInNanoseconds > 0);
                    assertEquals(0, info.tablePageWriteCount);
                    assertEquals(0, info.indexPageWriteCount);
                    assertEquals(0, info.overflowPageWriteCount);
                    testCount.intValue++;
                    if (sql.endsWith("ORDER BY content DESC")) {
                        assertEquals(0, info.tablePageReadCount);
                        assertTrue(info.indexPageReadCount > 0);
                        assertEquals(info.overflowPageReadCount, objects.size());
                    } else {
                        assertTrue(info.tablePageReadCount > 0);
                        assertEquals(0, info.indexPageReadCount);
                        assertEquals(info.overflowPageReadCount, objects.size());
                    }
                }
                lastSQLIsInsert.boolValue = sql.startsWith("INSERT");
            }
        });
        createTable();
        database.insertObjects(objects, DBTestObject.allFields(), tableName);
        database.execute(new StatementCreateIndex().createIndex("testIndex").on(tableName).indexedBy(DBTestObject.content));
        assertEquals(database.getAllObjects(DBTestObject.allFields(), tableName).size(), objects.size());
        assertEquals(database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.content.order(Order.Desc)).size(), objects.size());
        assertEquals(testCount.intValue, 4);
        Database.globalTracePerformance(null);
    }

    @Test
    public void testTraceError() {
        final WrappedValue tested = new WrappedValue();
        database.traceException(new Database.ExceptionTracer() {
            @Override
            public void onTrace(@NotNull WCDBException exception) {
                if (exception.level == WCDBException.Level.Error &&
                        exception.path().equals(FileTool.getRealPath(path)) &&
                        exception.tag() == database.getTag() &&
                        exception.code == WCDBException.Code.Error &&
                        exception.sql().equals("SELECT 1 FROM dummy") &&
                        exception.message().equals("no such table: dummy")) {
                    tested.boolValue = true;
                }
            }
        });
        assertTrue(database.canOpen());
        try {
            database.execute(new StatementSelect().select("1").from("dummy"));
        } catch (WCDBException e) {
            assertTrue(tested.boolValue);
        } finally {
            assertTrue(tested.boolValue);
        }
    }

    @Test
    public void testGlobalTraceError() {
        final WrappedValue tested = new WrappedValue();
        Database.globalTraceException(null);
        Database.globalTraceException(new Database.ExceptionTracer() {
            @Override
            public void onTrace(@NotNull WCDBException exception) {
                if (exception.level == WCDBException.Level.Error &&
                        exception.path().equals(FileTool.getRealPath(path)) &&
                        exception.tag() == database.getTag() &&
                        exception.code == WCDBException.Code.Error &&
                        exception.sql().equals("SELECT 1 FROM dummy") &&
                        exception.message().equals("no such table: dummy")) {
                    tested.boolValue = true;
                }
            }
        });
        assertTrue(database.canOpen());
        try {
            database.execute(new StatementSelect().select("1").from("dummy"));
        } catch (WCDBException e) {
            assertTrue(tested.boolValue);
        } finally {
            assertTrue(tested.boolValue);
        }
    }

    @Test
    public void testTraceDBOperation() throws WCDBException {
        final WrappedValue tag = new WrappedValue();
        final WrappedValue path = new WrappedValue();
        final WrappedValue openHandleCount = new WrappedValue();
        final WrappedValue tableCount = new WrappedValue();
        final WrappedValue indexCount = new WrappedValue();
        Database.globalTraceDatabaseOperation(new Database.OperationTracer() {
            @Override
            public void onTrace(@NotNull Database database, Database.Operation operation, @NotNull HashMap<String, Value> infos) {
                switch (operation) {
                    case Create:
                        path.stringValue = database.getPath();
                        break;
                    case SetTag:
                        tag.intValue = database.getTag();
                        break;
                    case OpenHandle:
                        openHandleCount.intValue++;
                        assertEquals(infos.get(Database.OperationInfoKeyHandleCount).getLong(), 1);
                        assertTrue(infos.get(Database.OperationInfoKeyOpenTime).getLong() > 0);
                        assertTrue(infos.get(Database.OperationInfoKeyOpenCPUTime).getLong() > 0);
                        assertTrue(infos.get(Database.OperationInfoKeySchemaUsage).getLong() > 0);
                        assertEquals(infos.get(Database.OperationInfoKeyTriggerCount).getLong(), 0);
                        tableCount.intValue = infos.get(Database.OperationInfoKeyTableCount).getLong();
                        indexCount.intValue = infos.get(Database.OperationInfoKeyIndexCount).getLong();
                        break;
                }
            }
        });
        Database newDatabase = new Database(currentDirectory + File.separator + "testDatabase2");
        newDatabase.setTag(10000);
        newDatabase.createTable(tableName, DBTestObject.INSTANCE);
        newDatabase.execute(new StatementCreateIndex().createIndex("testIndex").on(tableName).indexedBy(DBTestObject.content));

        assertEquals(tag.intValue, newDatabase.getTag());
        assertEquals(path.stringValue, newDatabase.getPath());
        assertEquals(openHandleCount.intValue, 1);

        newDatabase.close();

        newDatabase.insertObjects(RandomTool.autoIncrementTestCaseObjects(10), DBTestObject.allFields(), tableName);

        assertEquals(tableCount.intValue, 4);
        assertEquals(indexCount.intValue, 1);
        assertEquals(openHandleCount.intValue, 2);
        Database.globalTraceDatabaseOperation(null);
    }

    @Test
    public void testGlobalTraceBusy() throws InterruptedException, WCDBException {
        WrappedValue testTid = new WrappedValue();
        Database.globalTraceBusy(new Database.BusyTracer() {
            @Override
            public void onTrace(long tag, @NotNull String path, long tid, @NotNull String sql) {
                assertEquals(tag, database.getTag());
                assertEquals(path, database.getPath());
                assertEquals(sql, "INSERT INTO testTable(id, content) VALUES(?1, ?2)");
                testTid.intValue = tid;
            }
        }, 0.1);
        database.createTable(tableName, DBTestObject.INSTANCE);
        List<TestObject> objects = RandomTool.autoIncrementTestCaseObjects(100000);
        WrappedValue dispatchTid = new WrappedValue();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                dispatchTid.intValue = Process.myTid();
                database.insertObjects(objects, DBTestObject.allFields(), tableName);
            }
        });
        thread.start();
        sleep(100);
        database.insertObject(RandomTool.testObjectWithId(100001), DBTestObject.allFields(), tableName);
        thread.join();
        assertTrue(testTid.intValue != 0 && testTid.intValue == dispatchTid.intValue);
    }

}
