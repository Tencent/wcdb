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

import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Pragma;
import com.tencent.wcdb.winq.StatementPragma;
import com.tencent.wcdbtest.base.FileTool;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import static org.junit.Assert.*;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.util.ArrayList;

@RunWith(AndroidJUnit4.class)
public class DatabaseTest extends TableTestCase {
    @Test
    public void testTag() {
        assertNotEquals(database.getTag(), 0);
        Database newDatabase = new Database(database.getPath());
        assertEquals(database.getTag(), newDatabase.getTag());
    }

    @Test
    public void testPath() {
        assertEquals(database.getPath(), path);
    }

    @Test
    public void testOpenAndClose() {
        assertFalse(database.isOpened());
        assertTrue(database.canOpen());
        assertTrue(database.isOpened());
        database.close();
        assertFalse(database.isOpened());
    }

    @Test
    public void testBlockade() throws InterruptedException {
        database.blockade();
        final WrappedValue time = new WrappedValue();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                assertTrue(database.canOpen());
                time.intValue = System.currentTimeMillis();
            }
        });
        thread.start();
        sleep(1000);
        long newTime = System.currentTimeMillis();
        database.unblockade();
        thread.join();
        assertTrue(newTime < time.intValue);
    }

    @Test
    public void testBlockadeAndClose() throws InterruptedException {
        final WrappedValue main = WrappedValue.currentTime();
        final WrappedValue subThread = WrappedValue.currentTime();
        final Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                assertTrue(database.canOpen());
                subThread.intValue = System.currentTimeMillis();
            }
        });
        database.close(new Database.CloseCallBack() {
            @Override
            public void onClose() throws WCDBException {
                thread.start();
                sleep(1000);
                main.intValue = System.currentTimeMillis();
            }
        });
        thread.join();
        assertTrue(main.intValue < subThread.intValue);
    }

    @Test
    public void testReadOnly() throws WCDBException {
        createTable();
        table.insertObject(TestObject.createAutoIncrementObject(RandomTool.string()));

        database.close(new Database.CloseCallBack() {
            @Override
            public void onClose() throws WCDBException {
                for(String path: database.getPaths()) {
                    File file = new File(path);
                    if(file.exists()) {
                        file.setReadOnly();
                    }
                }
            }
        });

        assertTrue(database.canOpen());
        ArrayList<TestObject> objects = table.getAllObjects();
        assertEquals(objects.size(), 1);
        boolean failed = false;
        try {
            table.insertObject(TestObject.createAutoIncrementObject(RandomTool.string()));
        } catch (WCDBException e) {
            failed = true;
        }
        assertTrue(failed);
        for(String path: database.getPaths()) {
            File file = new File(path);
            if(file.exists()) {
                file.setReadOnly();
            }
        }
    }

    @Test
    public void testRunWhileClose() {
        assertTrue(database.canOpen());
        assertTrue(database.isOpened());
        database.close(new Database.CloseCallBack() {
            @Override
            public void onClose() throws WCDBException {
                database.execute(new StatementPragma().pragma(Pragma.userVersion()).toValue(123));
            }
        });
        assertFalse(database.isOpened());
    }

    @Test
    public void testPurge() throws WCDBException {
        database.execute(new StatementPragma().pragma(Pragma.userVersion()).toValue(123));
        assertTrue(database.isOpened());
        database.purge();
        assertFalse(database.isOpened());
    }

    @Test
    public void testPurgeAll() throws WCDBException {
        database.execute(new StatementPragma().pragma(Pragma.userVersion()).toValue(123));
        assertTrue(database.isOpened());
        Database.purgeAll();
        assertFalse(database.isOpened());
    }

    @Test
    public void testCheckPoint() throws WCDBException {
        createTable();
        TestObject[] objects = RandomTool.autoIncrementTestCaseObjects(100);
        final WrappedValue walFrameNum = new WrappedValue();
        for(int i = 0; i < 100; i++) {
            assertEquals(table.getValue(Column.all().count()).getInteger(), i);
            table.insertObject(objects[i]);
            database.passiveCheckpoint();
            database.close(new Database.CloseCallBack() {
                @Override
                public void onClose() throws WCDBException {
                    if( walFrameNum.intValue != 0 ) {
                        assertEquals(walFrameNum.intValue, getNumberOfWalFrames());
                    } else {
                        walFrameNum.intValue = getNumberOfWalFrames();
                    }
                    assertTrue(walFrameNum.intValue > 0);
                    for( String path: database.getPaths() ) {
                        if( path.endsWith("wal") ) {
                            assertTrue(FileTool.getFileSize(path) > 32);
                        } else if( path.endsWith("shm") ) {
                            assertTrue(FileTool.getFileSize(path) >= 32 * 1024);
                        }
                    }
                }
            });
        }
    }

    @Test
    public void testOpenFail() {
        File folder = new File(currentDirectory);
        folder.mkdirs();
        Database newDatabase = new Database(currentDirectory);
        assertFalse(newDatabase.canOpen());
    }

    private int getNumberOfWalFrames() {
        long size = FileTool.getFileSize(walPath());
        if(size > walHeaderSize() && (size - walHeaderSize()) % walFrameSize() == 0) {
            return (int) ((size - walHeaderSize()) / walFrameSize());
        }
        return 0;
    }
}
