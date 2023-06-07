// Created by qiuwenchen on 2023/5/11.
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

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.FileTool;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import static org.junit.Assert.*;
import org.junit.Test;

import java.util.List;

public class RepairTest extends TableTestCase {
    TestObject[] preInsertObjects = RandomTool.autoIncrementTestCaseObjects(2);

    void executeTest(TestOperation operation) {
        database.setCipherKey(null);
        try {
            createTable();
            table.insertObjects(preInsertObjects);
            operation.execute();
            database.removeFiles();
            database.setCipherKey("123".getBytes());
            createTable();
            table.insertObjects(preInsertObjects);
            operation.execute();
        } catch (WCDBException e) {
            throw new RuntimeException(e);
        }
    }

    @Test
    public void testBackup() {
        executeTest(new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                assertFalse(FileTool.fileExist(firstMaterialPath()));
                assertFalse(FileTool.fileExist(lastMaterialPath()));

                database.backup();
                assertTrue(FileTool.fileExist(firstMaterialPath()));
                assertFalse(FileTool.fileExist(lastMaterialPath()));

                sleep(1000);

                database.backup();
                assertTrue(FileTool.fileExist(firstMaterialPath()));
                assertTrue(FileTool.fileExist(lastMaterialPath()));
            }
        });
    }

    @Test
    public void testBackupWithFilter() throws WCDBException {
        createTable();
        table.insertObjects(preInsertObjects);
        database.filterBackup(null);

        database.backup();
        assertTrue(FileTool.fileExist(firstMaterialPath()));
        assertFalse(FileTool.fileExist(lastMaterialPath()));

        database.filterBackup(new Database.BackupFilter() {
            @Override
            public boolean tableShouldBeBackup(String tableName) {
                return false;
            }
        });

        sleep(1000);
        database.backup();
        assertTrue(FileTool.fileExist(firstMaterialPath()));
        assertTrue(FileTool.fileExist(lastMaterialPath()));
        assertTrue(FileTool.getFileSize(firstMaterialPath()) > FileTool.getFileSize(lastMaterialPath()));
    }

//    @Test
    public void testAutoBackup() {
        executeTest(new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.enableAutoBackup(true);
                TestObject newObject = TestObject.createObject(3, "testContent");
                assertFalse(FileTool.fileExist(firstMaterialPath()));
                table.insertObject(newObject);
                database.passiveCheckpoint();
                sleep(606000);
                assertTrue(FileTool.fileExist(firstMaterialPath()));
            }
        });
    }

    @Test
    public void testDeposit() {
        executeTest(new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                // 0.
                long num0 = table.getValue(Column.all().count()).getInteger();
                assertTrue(num0 > 0);
                long rowId = num0;

                // 1.
                database.backup();
                database.deposit();

                long num1 = table.getValue(Column.all().count()).getInteger();
                assertEquals(num1, 0);

                TestObject newObject = TestObject.createAutoIncrementObject("newObject");
                table.insertObject(newObject);
                rowId += 1;
                assertEquals(rowId, newObject.id);

                // 2.
                database.backup();
                database.deposit();

                long num2 = table.getValue(Column.all().count()).getInteger();
                assertEquals(num2, 0);

                newObject.id = 0;
                table.insertObject(newObject);
                rowId += 1;
                assertEquals(rowId, newObject.id);

                assertTrue(FileTool.fileExist(factoryPath()));
                assertTrue(database.containDepositedFiles());

                database.removeDepositedFiles();
                assertFalse(database.containDepositedFiles());
                assertFalse(FileTool.fileExist(factoryPath()));
            }
        });
    }

    void doTestRetrieve(boolean success) throws WCDBException {
        final WrappedValue lastPercentage = new WrappedValue();
        double score = database.retrieve(new Database.RetrieveProgressMonitor() {
            @Override
            public void onProgressUpdate(double percentage, double increment) {
                assertTrue(percentage - lastPercentage.doubleValue == increment && increment > 0);
                lastPercentage.doubleValue = percentage;
            }
        });
        assertTrue((success && score == 1.0) || (!success && score < 1.0));
        assertEquals(lastPercentage.doubleValue, 1.0, 0.00001);
    }

    void doTestObjectsRetrieved(boolean success) {
        List<TestObject> objects = null;
        try {
            objects = database.getAllObjects(DBTestObject.allFields(), tableName);
        } catch (WCDBException e) {

        }
        if( success ) {
            assertNotNull(objects);
            assertArrayEquals(objects.toArray(), preInsertObjects);
        } else {
            assertTrue(objects == null || objects.size() == 0);
        }
    }

    @Test
    public void testRetrieveWithBackupAndDeposit() {
        executeTest(new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.backup();
                database.deposit();
                corruptHeader();
                doTestRetrieve(true);
                doTestObjectsRetrieved(true);
            }
        });
    }

    @Test
    public void testRetrieveWithBackupAndWithoutDeposit() {
        executeTest(new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.backup();
                corruptHeader();
                doTestRetrieve(true);
                doTestObjectsRetrieved(true);
            }
        });
    }

    @Test
    public void testRetrieveWithoutBackupAndWithDeposit() {
        executeTest(new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                database.deposit();
                corruptHeader();
                doTestRetrieve(false);
                doTestObjectsRetrieved(true);
            }
        });
    }

    @Test
    public void testRetrieveWithoutBackupAndDeposit() {
        executeTest(new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                corruptHeader();
                database.deposit();
                doTestRetrieve(false);
                doTestObjectsRetrieved(false);
            }
        });
    }
}
