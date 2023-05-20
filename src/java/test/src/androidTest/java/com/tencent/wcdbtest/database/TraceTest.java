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

import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.winq.Pragma;
import com.tencent.wcdb.winq.StatementPragma;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdbtest.base.WrappedValue;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;

import static org.junit.Assert.*;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.util.ArrayList;

@RunWith(AndroidJUnit4.class)
public class TraceTest extends TableTestCase {
    @Test
    public void testTraceSQL() throws WCDBException {
        final StatementPragma statement = new StatementPragma().pragma(Pragma.userVersion());

        final WrappedValue tested = new WrappedValue();
        database.traceSQL(new Database.SQLTracer() {
            @Override
            public void onTrace(String path, long handleId, String sql) {
                if(sql.equals(statement.getDescription())) {
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
        final StatementPragma statement = new StatementPragma().pragma(Pragma.userVersion());

        final WrappedValue tested = new WrappedValue();
        Database.globalTraceSQL(null);
        Database.globalTraceSQL(new Database.SQLTracer() {
            @Override
            public void onTrace(String path, long handleId, String sql) {
                if(sql.equals(statement.getDescription())) {
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
        createTable();
        database.tracePerformance(null);
        final ArrayList<String> sqls = new ArrayList<String>(){{
            add("BEGIN IMMEDIATE");
            add("INSERT INTO testTable(id, content) VALUES(?1, ?2)");
            add("COMMIT");
        }};
        database.tracePerformance(new Database.PerformanceTracer() {
            @Override
            public void onTrace(String path, long handleId, String sql, double time) {
                if(sql.equals(sqls.get(0))) {
                    sqls.remove(0);
                }
            }
        });
        database.insertObjects(RandomTool.autoIncrementTestCaseObjects(10000), DBTestObject.allFields(), tableName);
        assertEquals(sqls.size(), 0);
        database.tracePerformance(null);
    }

    @Test
    public void testGlobalTracePerformance() throws WCDBException {
        Database.globalTracePerformance(null);
        final ArrayList<String> sqls = new ArrayList<String>(){{
            add("BEGIN IMMEDIATE");
            add("INSERT INTO testTable(id, content) VALUES(?1, ?2)");
            add("COMMIT");
        }};
        Database.globalTracePerformance(new Database.PerformanceTracer() {
            @Override
            public void onTrace(String path, long handleId, String sql, double time) {
                if(sqls.size() > 0 && sql.equals(sqls.get(0))) {
                    sqls.remove(0);
                }
            }
        });
        createTable();
        database.insertObjects(RandomTool.autoIncrementTestCaseObjects(10000), DBTestObject.allFields(), tableName);
        assertEquals(sqls.size(), 0);
        Database.globalTracePerformance(null);
    }

    @Test
    public void testTraceError() {
        final WrappedValue tested = new WrappedValue();
        database.traceException(new Database.ExceptionTracer() {
            @Override
            public void onTrace(WCDBException exception) {
                if(exception.level == WCDBException.Level.Error &&
                        exception.path().equals(path) &&
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
            public void onTrace(WCDBException exception) {
                if(exception.level == WCDBException.Level.Error &&
                        exception.path().equals(path) &&
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
        Database.globalTraceDatabaseOperation(new Database.OperationTracer() {
            @Override
            public void onTrace(Database database, Database.Operation operation) {
                switch (operation) {
                    case Create:
                        path.stringValue = database.getPath();
                        break;
                    case SetTag:
                        tag.intValue = database.getTag();
                        break;
                    case OpenHandle:
                        openHandleCount.intValue++;
                        break;
                }
            }
        });
        Database newDatabase = new Database(currentDirectory + File.separator + "testDatabase2");
        newDatabase.setTag(10000);
        newDatabase.createTable(tableName, DBTestObject.INSTANCE);
        newDatabase.insertObjects(RandomTool.autoIncrementTestCaseObjects(10), DBTestObject.allFields(), tableName);
        assertEquals(tag.intValue, newDatabase.getTag());
        assertEquals(path.stringValue, newDatabase.getPath());
        assertEquals(openHandleCount.intValue, 1);
        Database.globalTraceDatabaseOperation(null);
    }

}
