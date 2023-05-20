// Created by chenqiuwen on 2023/5/7.
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

package com.tencent.wcdbtest.base;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.Arrays;

public class DatabaseTestCase extends BaseTestCase {
    public enum Expect {
        AllSQLs, FirstFewSQLs, SomeSQLs
    }
    public String path;
    public String fileName;
    public Database database;
    public Expect expectMode;

    @Before
    public void setup() throws WCDBException {
        super.setup();
        expectMode = Expect.AllSQLs;
        fileName = "testDatabase";
        path = currentDirectory + File.separator + fileName;
        database = new Database(path);
        database.setTag(10001);
    }

    @After
    public void teardown() {
        database.close();
        database = null;
    }

    public interface TestOperation {
        void execute() throws WCDBException;
    }

    public void doTestSQL(String sql, TestOperation operation){
        doTestSQLs(new String[]{sql}, operation);
    }

    public void doTestSQLs(String[] sqls, TestOperation operation) {
        Assert.assertNotNull(sqls);
        Assert.assertNotNull(operation);
        do {
            final WrappedValue trace = new WrappedValue();
            trace.boolValue = false;
            final ArrayList<String> expectedSQLs = new ArrayList<String>(Arrays.asList(sqls));
            final Thread thread = Thread.currentThread();
            database.traceSQL(new Database.SQLTracer() {
                @Override
                public void onTrace(String path, long handleId, String sql) {
                    if(Thread.currentThread().getId() != thread.getId()) {
                        return;
                    }
                    if(!trace.boolValue) {
                        return;
                    }
                    doTestSQLAsExpected(expectedSQLs, sql);
                }
            });
            if(expectMode != Expect.SomeSQLs) {
                if( !database.canOpen()) {
                    Assert.fail();
                    break;
                }
            }
            trace.boolValue = true;
            try {
                operation.execute();
            } catch (WCDBException e) {
                throw new RuntimeException(e);
            }

            if(expectedSQLs.size() != 0) {
                logError("Reminding: " + expectedSQLs);
                Assert.fail();
                break;
            }
            trace.boolValue = false;
        } while (false);
        database.traceSQL(null);
    }

    private void doTestSQLAsExpected(ArrayList<String> expectedSQLs, String sql) {
        switch (expectMode) {
            case AllSQLs:{
                if(expectedSQLs.get(0).equals(sql)) {
                    expectedSQLs.remove(0);
                } else {
                    Assert.assertEquals(expectedSQLs.get(0), sql);
                }
            }break;
            case FirstFewSQLs:{

                if(expectedSQLs.size() > 0 && expectedSQLs.get(0).equals(sql)) {
                    expectedSQLs.remove(0);
                } else if(expectedSQLs.size() != 0){
                    Assert.assertEquals(expectedSQLs.get(0), sql);
                }
            }break;
            case SomeSQLs:{
                for(int i = 0; i < expectedSQLs.size(); i++) {
                    if(expectedSQLs.get(i).equals(sql)) {
                        expectedSQLs.remove(i);
                        break;
                    }
                }
            }break;
        }
    }

    public int headerSize()
    {
        return 100;
    }

    public int pageSize()
    {
        return 4096;
    }

    public int walHeaderSize()
    {
        return 32;
    }

    public int walFrameHeaderSize()
    {
        return 24;
    }

    public int walFrameSize()
    {
        return walFrameHeaderSize() + pageSize();
    }

    public String walPath()
    {
        return path + "-wal";
    }

    public String firstMaterialPath()
    {
        return path + "-first.material";
    }

    public String lastMaterialPath()
    {
        return path + "-last.material";
    }

    public String factoryPath()
    {
        return path + ".factory";
    }

    public String journalPath()
    {
        return path + "-journal";
    }

    public String shmPath()
    {
        return path + "-shm";
    }

    public void corruptHeader() {
        database.close(new Database.CloseCallBack() {
            @Override
            public void onClose() throws WCDBException {
                database.truncateCheckpoint();
                File file = new File(database.getPath());
                try {
                    RandomAccessFile accessFile = new RandomAccessFile(file, "rw");
                    accessFile.seek(0l);
                    accessFile.write(new byte[100]);
                } catch (FileNotFoundException e) {
                    logError("File not founded: " + e.toString());
                } catch (IOException e) {
                    logError("IO Error: " + e.toString());
                }
            }
        });
    }
}
