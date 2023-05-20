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
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdbtest.base.BaseTestCase;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.DatabaseTestCase;
import com.tencent.wcdbtest.base.TestObject;
import com.tencent.wcdbtest.base.WrappedValue;

import static org.junit.Assert.*;
import org.junit.Test;

import java.io.File;
import java.nio.file.Paths;
import java.util.ArrayList;

public class MigrationTest extends BaseTestCase {
    @Test
    public void testMigration() {
        final Database sourceDatabase = new Database(currentDirectory + File.separator + "sourceDatabase.sqlite3");
        sourceDatabase.createTable("sourceTable", DBTestObject.INSTANCE);

        TestObject oldObject1 = TestObject.createObject(1, "oldContent1");
        TestObject oldObject2 = TestObject.createObject(2, "oldContent2");
        sourceDatabase.insertObjects(new TestObject[]{oldObject1, oldObject2}, DBTestObject.allFields(), "sourceTable");

        Database targetDatabase = new Database(currentDirectory + File.separator + "targetDatabase.sqlite3");
        final String tableName = "targetTable";
        targetDatabase.filterMigration(new Database.MigrationFilter() {
            @Override
            public void filterMigrate(Database.MigrationInfo info) {
                if(info.table.equals(tableName)) {
                    info.sourceTable = "sourceTable";
                    info.sourceDatabase = sourceDatabase.getPath();
                }
            }
        });

        targetDatabase.createTable(tableName, DBTestObject.INSTANCE);
        targetDatabase.updateValue("newContent2", DBTestObject.content, tableName, DBTestObject.id.eq(2));

        ArrayList<TestObject> objects = targetDatabase.getAllObjects(DBTestObject.allFields(), tableName);
        assertNotNull(objects);
        assertEquals(objects.size(), 2);
        assertEquals(objects.get(1).content, "newContent2");

        targetDatabase.deleteObjects(tableName, DBTestObject.id.eq(2));
        assertEquals(targetDatabase.getValue(Column.all().count(), tableName).getInteger(), 1);

        TestObject newObject = TestObject.createObject(3, "newContent3");
        targetDatabase.insertObject(newObject, DBTestObject.allFields(), tableName);

        ArrayList<Value> contents = targetDatabase.getOneColumn(DBTestObject.content, tableName, DBTestObject.id.order(Order.Asc));
        assertNotNull(contents);
        assertEquals(contents.size(), 2);
        assertEquals(contents.get(0).getText(), "oldContent1");
        assertEquals(contents.get(1).getText(), "newContent3");

        targetDatabase.close();
        assertFalse(targetDatabase.isMigrated());

        final WrappedValue migratedTable = new WrappedValue();
        targetDatabase.setNotificationWhenMigrated(new Database.MigrationNotification() {
            @Override
            public void onMigrated(Database database, Database.MigrationInfo info) {
                if(info != null && info.sourceTable.equals("sourceTable")) {
                    migratedTable.stringValue = info.sourceTable;
                }
            }
        });

        do {
            targetDatabase.stepMigration();
        } while (!targetDatabase.isMigrated());

        assertTrue(targetDatabase.isMigrated());
        assertEquals(migratedTable.stringValue, "sourceTable");

    }
}
