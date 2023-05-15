// Created by chenqiuwen on 2023/5/12.
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

import com.tencent.wcdb.base.Value;
import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.winq.Column;

import org.junit.After;
import org.junit.Before;

import java.util.ArrayList;

public class CRUDTestCase extends TableTestCase {
    protected TestObject object1;
    protected TestObject object2;
    protected TestObject[] objects;
    protected Value[] row1;
    protected Value[] row2;
    protected Value[][] rows;
    protected int nextId = 3;
    protected Column id = new Column("id");
    protected Column content = new Column("content");
    protected Handle handle;

    @Before
    public void setup() {
        super.setup();
        object1 = RandomTool.testObjectWithId(1);
        object2 = RandomTool.testObjectWithId(2);
        objects = new TestObject[]{object1, object2};
        row1 = RandomTool.testRowWithId(1);
        row2 = RandomTool.testRowWithId(2);
        rows = new Value[][]{row1, row2};
        handle = database.getHandle();
        table = database.getTable(tableName, DBTestObject.INSTANCE);
    }

    @After
    public void teardown() {
        if(handle != null) {
            handle.invalidate();
            handle = null;
        }
        super.teardown();
    }

    public void insertPresetObjects() throws WCDBException {
        createTable();
        table.insertObjects(objects);
    }

    public void insertPresetRows() throws WCDBException {
        createValueTable();
        database.insertRows(rows, columns(), tableName);
    }

    public long allRowsCount() {
        return database.getValue(Column.all().count(), tableName).getInteger();
    }
}
