// Created by chenqiuwen on 2023/12/31.
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
import com.tencent.wcdb.core.PausableTransaction;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TableTestCase;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import org.jetbrains.annotations.NotNull;
import org.junit.Assert;
import org.junit.Test;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class CompressionTest extends TableTestCase {
    @Test
    public void testZSTDNormalCompress() {
        RandomTool.useEnglishWord = true;
        createTable();
        List<TestObject> preInsertObject = RandomTool.testCaseObjects(1, 2);
        table.insertObjects(preInsertObject);

        database.setCompression(new Database.CompressionFilter() {
            @Override
            public void filterCompress(@NotNull Database.CompressionInfo info) {
                info.addZSTDNormalCompress(DBTestObject.content);
            }
        });

        WrappedValue tableCompressed = new WrappedValue();
        WrappedValue databaseCompressed = new WrappedValue();
        database.setNotificationWhenCompressed(new Database.CompressionNotification() {
            @Override
            public void onCompressed(@NotNull Database notifiedDatabase, String table) {
                Assert.assertEquals(database.getTag(), notifiedDatabase.getTag());
                Assert.assertEquals(database.getPath(), notifiedDatabase.getPath());
                if (table != null) {
                    if (table.equals(tableName)) {
                        tableCompressed.boolValue = true;
                    }
                } else {
                    databaseCompressed.boolValue = true;
                }
            }
        });

        Assert.assertFalse(database.isCompressed());
        database.stepCompression();
        database.stepCompression();
        Assert.assertTrue(database.isCompressed());
        Assert.assertTrue(tableCompressed.boolValue && databaseCompressed.boolValue);

        List<TestObject> newInsertObjects = RandomTool.testCaseObjects(3, 2);
        table.insertObjects(newInsertObjects);

        Value count = database.getValueFromStatement(new StatementSelect().select(Column.all().count()).from(tableName).where(new Column("WCDB_CT_content").eq(4)));
        Assert.assertEquals(count.getInt(), 4);

        List<TestObject> allObjects = table.getAllObjects();
        preInsertObject.addAll(newInsertObjects);
        Assert.assertEquals(allObjects, preInsertObject);

        RandomTool.useEnglishWord = false;
    }

    @Test
    public void testZSTDDictCompress() {
        RandomTool.useEnglishWord = true;
        createTable();
        List<TestObject> preInsertObject = RandomTool.testCaseObjects(1, 2);
        table.insertObjects(preInsertObject);

        ArrayList<byte[]> samples = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            String sample = RandomTool.englishString();
            samples.add(sample.getBytes(StandardCharsets.UTF_8));
        }
        byte[] dict = Database.trainDictWithData((List<byte[]>) samples, (byte) 1);
        Database.registerDict(dict, (byte) 1);

        database.setCompression(new Database.CompressionFilter() {
            @Override
            public void filterCompress(@NotNull Database.CompressionInfo info) {
                info.addZSTDDictCompress(DBTestObject.content, (byte) 1);
            }
        });

        Assert.assertFalse(database.isCompressed());
        database.stepCompression();
        database.stepCompression();
        Assert.assertTrue(database.isCompressed());

        List<TestObject> newInsertObjects = RandomTool.testCaseObjects(3, 2);
        table.insertObjects(newInsertObjects);

        Value count = database.getValueFromStatement(new StatementSelect().select(Column.all().count()).from(tableName).where(new Column("WCDB_CT_content").eq(2)));
        Assert.assertEquals(count.getInt(), 4);

        List<TestObject> allObjects = table.getAllObjects();
        preInsertObject.addAll(newInsertObjects);
        Assert.assertEquals(allObjects, preInsertObject);

        RandomTool.useEnglishWord = false;
    }

    @Test
    public void testZSTDMultiDictCompress() {
        RandomTool.useEnglishWord = true;
        createTable();
        List<TestObject> preInsertObject = RandomTool.testCaseObjects(1, 2);
        table.insertObjects(preInsertObject);

        ArrayList<String> samples = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            String sample = RandomTool.englishString();
            samples.add(sample);
        }
        byte[] dict1 = Database.trainDictWithString((List<String>) samples, (byte) 2);
        Database.registerDict(dict1, (byte) 2);

        samples.clear();
        for (int i = 0; i < 1000; i++) {
            String sample = RandomTool.englishString();
            samples.add(sample);
        }
        byte[] dict2 = Database.trainDictWithString((List<String>) samples, (byte) 3);
        Database.registerDict(dict2, (byte) 3);

        samples.clear();
        for (int i = 0; i < 1000; i++) {
            String sample = RandomTool.englishString();
            samples.add(sample);
        }
        byte[] dict3 = Database.trainDictWithString((List<String>) samples, (byte) 4);
        Database.registerDict(dict3, (byte) 4);

        database.setCompression(new Database.CompressionFilter() {
            @Override
            public void filterCompress(@NotNull Database.CompressionInfo info) {
                info.addZSTDMultiDictCompress(DBTestObject.content, DBTestObject.id, new HashMap<Long, Byte>() {{
                    put(1L, (byte) 2);
                    put(2L, (byte) 3);
                    put(Database.DictDefaultMatchValue, (byte) 3);
                }});
            }
        });

        Assert.assertFalse(database.isCompressed());
        database.stepCompression();
        database.stepCompression();
        Assert.assertTrue(database.isCompressed());

        List<TestObject> newInsertObjects = RandomTool.testCaseObjects(3, 2);
        table.insertObjects(newInsertObjects);

        Value count = database.getValueFromStatement(new StatementSelect().select(Column.all().count()).from(tableName).where(new Column("WCDB_CT_content").eq(2)));
        Assert.assertEquals(count.getInt(), 4);

        List<TestObject> allObjects = table.getAllObjects();
        preInsertObject.addAll(newInsertObjects);
        Assert.assertEquals(allObjects, preInsertObject);

        RandomTool.useEnglishWord = false;
    }
}
