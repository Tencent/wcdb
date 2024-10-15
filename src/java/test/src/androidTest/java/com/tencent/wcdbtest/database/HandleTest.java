// Created by chenqiuwen on 2023/8/11.
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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import org.jetbrains.annotations.NotNull;

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.Transaction;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import org.junit.Test;

import java.util.HashMap;
import java.util.List;

public class HandleTest extends TableTestCase {
    @Test
    public void testCancellationSignal() throws WCDBException, InterruptedException {
        createTable();
        List<TestObject> objects = RandomTool.autoIncrementTestCaseObjects(10000);
        database.insertObjects(objects, DBTestObject.allFields(), tableName);

        WrappedValue hasTestInterrupt = new WrappedValue();
        Handle.CancellationSignal signal = new Handle.CancellationSignal();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                Handle handle = database.getHandle();
                try {
                    handle.attachCancellationSignal(signal);
                    List<TestObject> objects = handle.getAllObjects(DBTestObject.allFields(), tableName);
                } catch (WCDBException e) {
                    if (e.level == WCDBException.Level.Error && e.code == WCDBException.Code.Interrupt) {
                        hasTestInterrupt.boolValue = true;
                    }
                } finally {
                    handle.invalidate();
                }
            }
        });
        thread.start();
        sleep(10);
        signal.cancel();
        thread.join();
        assertTrue(hasTestInterrupt.boolValue);
    }

    @Test
    public void testWriteWithHandleCountLimit() throws WCDBException, InterruptedException {
        final WrappedValue maxHandleCount = new WrappedValue();
        Database.globalTraceDatabaseOperation(new Database.OperationTracer() {
            @Override
            public void onTrace(@NotNull Database database, Database.Operation operation, @NotNull HashMap<String, Value> info) {
                if (operation != Database.Operation.OpenHandle) {
                    return;
                }
                Value value = info.get(Database.OperationInfoKeyHandleCount);
                assert value != null;
                if (value.getLong() > maxHandleCount.intValue) {
                    maxHandleCount.intValue = value.getLong();
                }
            }
        });
        createTable();
        Thread[] threads = new Thread[80];
        for (int i = 0; i < 80; i++) {
            final int finalI = i;
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        switch (finalI % 8) {
                            case 0: {
                                List<TestObject> objects = RandomTool.autoIncrementTestCaseObjects(100);
                                table.insertObjects(objects);
                            }
                            break;
                            case 1: {
                                table.updateValue("abc", DBTestObject.content, DBTestObject.id.gt((finalI - 1) * 100).and(DBTestObject.id
                                        .lt(finalI * 100)));
                            }
                            break;
                            case 2: {
                                table.deleteObjects(DBTestObject.id.gt((finalI - 2) * 100 + 50).and(DBTestObject.id
                                        .lt(finalI * 100)));
                            }
                            break;
                            case 3: {
                                database.runTransaction(new Transaction() {
                                    @Override
                                    public boolean insideTransaction(@NotNull Handle handle) throws WCDBException {
                                        table.updateValue("abc2", DBTestObject.content, DBTestObject.id.gt((finalI - 1) * 100).and(DBTestObject.id
                                                .lt(finalI * 100)));
                                        return true;
                                    }
                                });
                            }
                            break;
                            case 4: {
                                List<TestObject> objects = RandomTool.autoIncrementTestCaseObjects(100);
                                database.insertObjects(objects, DBTestObject.allFields(), tableName);
                            }
                            break;
                            case 5: {
                                database.updateValue("abc", DBTestObject.content, tableName, DBTestObject.id.gt((finalI - 1) * 100).and(DBTestObject.id
                                        .lt(finalI * 100)));
                            }
                            break;
                            case 6: {
                                database.deleteObjects(tableName, DBTestObject.id.gt((finalI - 2) * 100 + 50).and(DBTestObject.id
                                        .lt(finalI * 100)));
                            }
                            break;
                            case 7: {
                                database.runTransaction(new Transaction() {
                                    @Override
                                    public boolean insideTransaction(@NotNull Handle handle) throws WCDBException {
                                        database.updateValue("abc2", DBTestObject.content, tableName, DBTestObject.id.gt((finalI - 1) * 100).and(DBTestObject.id
                                                .lt(finalI * 100)));
                                        return true;
                                    }
                                });
                            }
                            break;
                        }
                        sleep(100);
                    } catch (WCDBException e) {
                        throw new RuntimeException(e);
                    }
                }
            });
            thread.start();
            threads[i] = thread;
        }
        for (Thread thread : threads) {
            thread.join();
        }
        assertTrue(maxHandleCount.intValue <= 4);
        Database.globalTraceDatabaseOperation(null);
    }

    @Test
    public void testReadWithHandleCountLimit() throws WCDBException, InterruptedException {
        final WrappedValue maxHandleCount = new WrappedValue();
        Database.globalTraceDatabaseOperation(new Database.OperationTracer() {
            @Override
            public void onTrace(@NotNull Database database, Database.Operation operation, @NotNull HashMap<String, Value> info) {
                if (operation != Database.Operation.OpenHandle) {
                    return;
                }
                Value value = info.get(Database.OperationInfoKeyHandleCount);
                assert value != null;
                if (value.getLong() > maxHandleCount.intValue) {
                    maxHandleCount.intValue = value.getLong();
                }
            }
        });
        createTable();
        List<TestObject> objects = RandomTool.autoIncrementTestCaseObjects(64000);
        table.insertObjects(objects);
        Thread[] threads = new Thread[640];
        for (int i = 0; i < threads.length; i++) {
            final int finalI = i;
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        switch (finalI % 10) {
                            case 0: {
                                List<TestObject> objects = table.getAllObjects(DBTestObject.id.order(Order.Desc), 100, finalI * 100);
                                assertEquals(objects.size(), 100);
                            }
                            break;
                            case 1: {
                                Value count = database.getValueFromStatement(new StatementSelect().select(Column.all().count()).from(tableName));
                                assertEquals(count.getLong(), 64000);
                            }
                            break;
                            case 2: {
                                List<Value> column = table.getOneColumn(DBTestObject.content, DBTestObject.id.order(Order.Desc), 100, finalI * 100);
                                assertEquals(column.size(), 100);
                            }
                            break;
                            case 3: {
                                List<Value[]> rows = table.getAllRows(DBTestObject.allFields(), DBTestObject.id.order(Order.Desc), 100, finalI * 100);
                                assertEquals(rows.size(), 100);
                            }
                            break;
                            case 4: {
                                Value[] row = table.getOneRow(DBTestObject.allFields());
                                assertEquals(row.length, 2);
                            }
                            break;
                            case 5: {
                                List<TestObject> objects = database.getAllObjects(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 100, finalI * 100);
                                assertEquals(objects.size(), 100);
                            }
                            break;
                            case 6: {
                                List<Value> column = database.getOneColumn(DBTestObject.content, tableName, DBTestObject.id.order(Order.Desc), 100, finalI * 100);
                                assertEquals(column.size(), 100);
                            }
                            break;
                            case 7: {
                                List<Value[]> rows = database.getAllRows(DBTestObject.allFields(), tableName, DBTestObject.id.order(Order.Desc), 100, finalI * 100);
                                assertEquals(rows.size(), 100);
                            }
                            break;
                            case 8: {
                                Value[] row = database.getOneRow(DBTestObject.allFields(), tableName);
                                assertEquals(row.length, 2);
                            }
                            break;
                        }
                        sleep(100);
                    } catch (WCDBException e) {
                        throw new RuntimeException(e);
                    }
                }
            });
            thread.start();
            threads[i] = thread;
        }
        for (Thread thread : threads) {
            thread.join();
        }
        assertTrue(maxHandleCount.intValue > 4 && maxHandleCount.intValue <= 32);
        Database.globalTraceDatabaseOperation(null);
    }

    @Test
    public void testReadWriteWithHandleCountLimit() throws WCDBException, InterruptedException {
        final WrappedValue maxHandleCount = new WrappedValue();
        Database.globalTraceDatabaseOperation(new Database.OperationTracer() {
            @Override
            public void onTrace(@NotNull Database database, Database.Operation operation, @NotNull HashMap<String, Value> info) {
                if (operation != Database.Operation.OpenHandle) {
                    return;
                }
                Value value = info.get(Database.OperationInfoKeyHandleCount);
                assert value != null;
                if (value.getLong() > maxHandleCount.intValue) {
                    maxHandleCount.intValue = value.getLong();
                }
            }
        });
        createTable();
        List<TestObject> objects = RandomTool.autoIncrementTestCaseObjects(32000);
        table.insertObjects(objects);
        Thread[] threads = new Thread[320];
        for (int i = 0; i < threads.length; i++) {
            int finalI = i;
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        if (finalI % 8 == 0) {
                            List<TestObject> objects = RandomTool.autoIncrementTestCaseObjects(100);
                            table.insertObjects(objects);
                        } else {
                            List<TestObject> objects = table.getAllObjects(DBTestObject.id.order(Order.Desc), 100, finalI * 100);
                            assertEquals(objects.size(), 100);
                        }
                        sleep(100);
                    } catch (WCDBException e) {
                        throw new RuntimeException(e);
                    }
                }
            });
            thread.start();
            threads[i] = thread;
        }
        for (Thread thread : threads) {
            thread.join();
        }
        assertTrue(maxHandleCount.intValue > 4 && maxHandleCount.intValue <= 32);
        Database.globalTraceDatabaseOperation(null);
    }
}
