// Created by chenqiuwen on 2023/5/11.
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
import com.tencent.wcdb.core.Table;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdbtest.base.BaseTestCase;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.RandomTool;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import static org.junit.Assert.*;
import org.junit.Test;

import java.io.File;
import java.util.List;
import java.util.Random;

public class MigrationTest extends BaseTestCase {
    @Test
    public void testMigration() throws WCDBException {
        TestObject oldObject1 = TestObject.createObject(1, "a");
        TestObject oldObject2 = TestObject.createObject(2, "b");
        TestObject oldObject3 = TestObject.createObject(3, "c");

        byte[] sourceCipher = RandomTool.bytes();
        byte[] targetCipher = RandomTool.bytes();

        final Database sourceDatabase = new Database(currentDirectory + File.separator + "sourceDatabase.sqlite3");
        sourceDatabase.setCipherKey(sourceCipher);

        final String sourceTableName = "sourceTable";
        sourceDatabase.createTable(sourceTableName, DBTestObject.INSTANCE);
        Table<TestObject> sourceTable = sourceDatabase.getTable(sourceTableName, DBTestObject.INSTANCE);
        sourceTable.insertObjects(new TestObject[]{oldObject1, oldObject2, oldObject3});

        Database targetDatabase = new Database(currentDirectory + File.separator + "targetDatabase.sqlite3");
        targetDatabase.setCipherKey(targetCipher);

        final String targetTableName = "targetTable";
        targetDatabase.addMigrationSource(sourceDatabase.getPath(), sourceCipher, new Database.MigrationFilter() {
            @Override
            public void filterMigrate(Database.MigrationInfo info) {
                if(info.table.equals(targetTableName)) {
                    info.sourceTable = sourceTableName;
                    info.filterCondition = DBTestObject.id.gt(2);
                }
            }
        });

        targetDatabase.createTable(targetTableName, DBTestObject.INSTANCE);
        Table<TestObject> targetTable = targetDatabase.getTable(targetTableName, DBTestObject.INSTANCE);

        assertEquals(targetTable.getValue(Column.all().count()).getInteger(), 1);
        assertEquals(sourceTable.getValue(Column.all().count()).getInteger(), 3);

        targetTable.updateValue("newContent", DBTestObject.content, DBTestObject.id.eq(3));
        assertEquals(targetTable.getValue(DBTestObject.content, DBTestObject.id.eq(3)).getText(), "newContent");

        targetTable.deleteObjects(DBTestObject.id.eq(3));
        assertEquals(targetTable.getValue(Column.all().count()).getInteger(), 0);
        assertEquals(sourceTable.getValue(Column.all().count()).getInteger(), 2);

        targetTable.insertObject(TestObject.createObject(4, "d"));
        assertEquals(targetTable.getValue(DBTestObject.content, DBTestObject.id.eq(4)).getText(), "d");
        assertEquals(targetTable.getValue(Column.all().count()).getInteger(), 1);
        assertEquals(sourceTable.getValue(Column.all().count()).getInteger(), 2);

        targetDatabase.close();
        final WrappedValue migrateTable = new WrappedValue();
        targetDatabase.setNotificationWhenMigrated(new Database.MigrationNotification() {
            @Override
            public void onMigrated(Database database, Database.MigrationInfo info) {
                if(info != null && info.table.equals(targetTableName)) {
                    migrateTable.stringValue = info.sourceTable;
                }
            }
        });

        do {
            targetDatabase.stepMigration();
        }while(!targetDatabase.isMigrated());

        assertEquals(migrateTable.stringValue, sourceTableName);
    }
}
