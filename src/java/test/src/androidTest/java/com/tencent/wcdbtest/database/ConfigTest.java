// Created by qiuwenchen on 2023/5/10.
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

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.Table;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Pragma;
import com.tencent.wcdb.winq.StatementPragma;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.FileTool;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import org.jetbrains.annotations.NotNull;
import org.junit.After;

import static org.junit.Assert.*;

import org.junit.Test;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ConfigTest extends TableTestCase {
    String configName = "testConfig";

    @After
    public void teardown() {
        database.setConfig(configName, null);
        super.teardown();
    }

    @Test
    public void testConfig() throws WCDBException {
        final StatementPragma setSecureDelete = new StatementPragma().pragma(Pragma.secureDelete).toValue(true);
        final StatementPragma unsetSecureDelete = new StatementPragma().pragma(Pragma.secureDelete).toValue(false);
        final StatementPragma getSecureDelete = new StatementPragma().pragma(Pragma.secureDelete);
        final WrappedValue uninvoked = new WrappedValue();
        database.setConfig(configName, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                handle.execute(setSecureDelete);
            }
        }, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                uninvoked.boolValue = true;
                handle.execute(unsetSecureDelete);
            }
        }, Database.ConfigPriority.low);
        expectMode = Expect.SomeSQLs;
        doTestSQL("PRAGMA secure_delete = TRUE", new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.close();
                assertTrue(database.canOpen());
            }
        });
        assertTrue(database.getValueFromStatement(getSecureDelete).getBool());
        database.setConfig(configName, null);
        assertTrue(database.canOpen());
        assertTrue(uninvoked.boolValue);
        assertFalse(database.getValueFromStatement(getSecureDelete).getBool());
    }

    @Test
    public void testConfigFail() {
        database.setConfig(configName, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                handle.execute(new StatementSelect().select("testColumn").from("testTable"));
            }
        });
        assertFalse(database.canOpen());
    }

    @Test
    public void tesUnInvokeFail() {
        database.setConfig(configName, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
            }
        }, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                handle.execute(new StatementSelect().select("testColumn").from("testTable"));
            }
        }, Database.ConfigPriority.low);
        assertTrue(database.canOpen());
        database.setConfig(configName, null);
        assertFalse(database.canOpen());
    }

    @Test
    public void testOrderConfig() {
        String config1 = configName + "_1";
        String config2 = configName + "_2";
        String config3 = configName + "_3";
        final WrappedValue step = new WrappedValue();
        database.setConfig(config1, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                assertEquals(step.intValue, 1);
                step.intValue++;
            }
        });

        database.setConfig(config2, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                assertEquals(step.intValue, 2);
                step.intValue++;
            }
        }, null, Database.ConfigPriority.low);

        database.setConfig(config3, new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                assertEquals(step.intValue, 0);
                step.intValue++;
            }
        }, null, Database.ConfigPriority.high);
        assertTrue(database.canOpen());
        assertEquals(step.intValue, 3);
    }

    @Test
    public void testCipher() {
        byte[] cipher = "123".getBytes();
        byte[] wrongCipher = "456".getBytes();
        database.setCipherKey(cipher);
        assertTrue(database.canOpen());
        database.close();
        database.setCipherKey(wrongCipher);
        assertFalse(database.canOpen());
    }

    @Test
    public void testDatabasePageSize() {
        byte[] cipher = "123".getBytes();
        byte[] wrongCipher = "456".getBytes();
        database.setCipherKey(cipher);
        database.setConfig("page_size", new Database.Config() {
            @Override
            public void onInvocation(@NotNull Handle handle) throws WCDBException {
                handle.execute(new StatementPragma().pragma(Pragma.pageSize).toValue(16 * 1024));
            };
        }, Database.ConfigPriority.highest);
        assertTrue(database.canOpen());
        assertTrue(database.getValueFromStatement(new StatementPragma().pragma(Pragma.pageSize)).getInt() == 16 * 1024);
    }

    @Test
    public void testCipherWithPageSize() {
        byte[] cipher = "123".getBytes();
        int pageSize = 8 * 1024;
        int wrongPageSize = 16 * 1024;
        database.setCipherKey(cipher, pageSize);
        assertTrue(database.canOpen());
        database.close();
        database.setCipherKey(cipher, wrongPageSize);
        assertFalse(database.canOpen());
    }

    @Test
    public void testCipherWithDifferentVersion() throws WCDBException {
        byte[] cipher = "123".getBytes();
        int pageSize = 4096;
        database.setCipherKey(cipher, pageSize, Database.CipherVersion.version3);
        assertTrue(database.canOpen());

        database.close();
        database.setCipherKey(cipher, pageSize);
        assertFalse(database.canOpen());

        Database.setDefaultCipherVersion(Database.CipherVersion.version3);
        assertTrue(database.canOpen());

        database.removeFiles();
        database.setCipherKey(cipher, pageSize, Database.CipherVersion.version4);
        assertTrue(database.canOpen());
        database.close();

        database.setCipherKey(cipher, pageSize);
        assertFalse(database.canOpen());
        Database.setDefaultCipherVersion(Database.CipherVersion.version4);
        assertTrue(database.canOpen());
    }

    @Test
    public void testAutoVacuum() throws WCDBException {
        database.enableAutoVacuum(false);
        Value vacuumMode = database.getValueFromStatement(new StatementPragma().pragma(Pragma.autoVacuum));
        assertTrue(vacuumMode != null && vacuumMode.getInt() == 1);

        database.enableAutoVacuum(true);
        vacuumMode = database.getValueFromStatement(new StatementPragma().pragma(Pragma.autoVacuum));
        assertTrue(vacuumMode != null && vacuumMode.getInt() == 2);
    }

    @Test
    public void testIncrementalVacuum() throws WCDBException {
        database.enableAutoVacuum(true);
        createTable();
        table.insertObjects(RandomTool.autoIncrementTestCaseObjects(2));
        database.truncateCheckpoint();

        database.dropTable(tableName);
        database.truncateCheckpoint();

        Value freelist = database.getValueFromStatement(new StatementPragma().pragma(Pragma.freelistCount));
        assertTrue(freelist != null && freelist.getInt() > 0);

        database.incrementalVacuum(0);

        freelist = database.getValueFromStatement(new StatementPragma().pragma(Pragma.freelistCount));
        assertTrue(freelist != null && freelist.getInt() == 0);
    }

    @Test
    public void testLiteMode() throws WCDBException {
        database.enableLiteMode(true);
        database.createTable("testTable", DBTestObject.INSTANCE);
        Table<TestObject> table = database.getTable("testTable", DBTestObject.INSTANCE);
        int numberOfTasks = 300;
        ExecutorService executorService = Executors.newFixedThreadPool(numberOfTasks);
        CountDownLatch latch = new CountDownLatch(numberOfTasks);

        for (int i = 0; i < numberOfTasks; i++) {
            executorService.submit(() -> {
                try {
                    table.insertObjects(RandomTool.autoIncrementTestCaseObjects(100));
                } finally {
                    latch.countDown();
                }
            });
        }

        try {
            latch.await();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        } finally {
            assertEquals(table.getValue(Column.all().count()).getInt(), 30000);
            assertFalse(FileTool.fileExist(walPath()));
            executorService.shutdown();
        }
    }
}
