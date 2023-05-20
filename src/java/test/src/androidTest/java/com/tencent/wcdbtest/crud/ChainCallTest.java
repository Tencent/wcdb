// Created by qiuwenchen on 2023/5/15.
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

package com.tencent.wcdbtest.crud;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.chaincall.Delete;
import com.tencent.wcdb.chaincall.Insert;
import com.tencent.wcdb.chaincall.Select;
import com.tencent.wcdb.chaincall.Update;
import com.tencent.wcdbtest.base.DBTestObject;
import com.tencent.wcdbtest.base.ObjectCRUDTestCase;
import com.tencent.wcdbtest.base.TestObject;

import static com.tencent.wcdbtest.base.WinqTool.*;

import org.junit.Test;

public class ChainCallTest extends ObjectCRUDTestCase {
    @Test
    public void testDatabaseDelete() throws WCDBException {
        Delete delete = database.prepareDelete().fromTable(tableName);
        winqEqual(delete.getStatement(), "DELETE FROM testTable");
    }

    @Test
    public void testTableDelete() throws WCDBException {
        Delete delete = table.prepareDelete();
        winqEqual(delete.getStatement(), "DELETE FROM testTable");
    }

    @Test
    public void testHandleDelete() throws WCDBException {
        Delete delete = handle.prepareDelete().fromTable(tableName);
        winqEqual(delete.getStatement(), "DELETE FROM testTable");
    }

    @Test
    public void testDatabaseInsert() throws WCDBException {
        Insert<TestObject> insert = database.<TestObject>prepareInsert().intoTable(tableName).onFields(DBTestObject.allFields());
        winqEqual(insert.getStatement(), "INSERT INTO testTable(id, content) VALUES(?1, ?2)");
    }

    @Test
    public void testTableInsert() throws WCDBException {
        Insert<TestObject> insert = table.prepareInsert().onFields(DBTestObject.allFields());
        winqEqual(insert.getStatement(), "INSERT INTO testTable(id, content) VALUES(?1, ?2)");
    }

    @Test
    public void testHandleInsert() throws WCDBException {
        Insert<TestObject> insert = handle.<TestObject>prepareInsert().intoTable(tableName).onFields(DBTestObject.allFields());
        winqEqual(insert.getStatement(), "INSERT INTO testTable(id, content) VALUES(?1, ?2)");
    }

    @Test
    public void testDatabaseUpdate() throws WCDBException {
        Update<TestObject> update = database.<TestObject>prepareUpdate().table(tableName).set(DBTestObject.content);
        winqEqual(update.getStatement(), "UPDATE testTable SET content = ?1");
    }

    @Test
    public void testTableUpdate() throws WCDBException {
        Update<TestObject> update = table.prepareUpdate().set(DBTestObject.content);
        winqEqual(update.getStatement(), "UPDATE testTable SET content = ?1");
    }

    @Test
    public void testHandleUpdate() throws WCDBException {
        Update<TestObject> update = handle.<TestObject>prepareUpdate().table(tableName).set(DBTestObject.content);
        winqEqual(update.getStatement(), "UPDATE testTable SET content = ?1");
    }

    @Test
    public void testDatabaseSelect() throws WCDBException {
        Select<TestObject> select = database.<TestObject>prepareSelect().select(DBTestObject.allFields()).from(tableName).where(DBTestObject.id.eq(1));
        winqEqual(select.getStatement(), "SELECT id, content FROM testTable WHERE id == 1");
    }

    @Test
    public void testTableSelect() throws WCDBException {
        Select<TestObject> select = table.prepareSelect().select(DBTestObject.allFields()).where(DBTestObject.id.eq(1));
        winqEqual(select.getStatement(), "SELECT id, content FROM testTable WHERE id == 1");
    }

    @Test
    public void testHandleSelect() throws WCDBException {
        Select<TestObject> select = handle.<TestObject>prepareSelect().select(DBTestObject.allFields()).from(tableName).where(DBTestObject.id.eq(1));
        winqEqual(select.getStatement(), "SELECT id, content FROM testTable WHERE id == 1");
    }
}
