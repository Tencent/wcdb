// Created by qiuwenchen on 2023/5/8.
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
import com.tencent.wcdb.core.Table;
import com.tencent.wcdb.orm.TableBinding;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.ColumnDef;
import com.tencent.wcdb.winq.ColumnType;
import com.tencent.wcdb.winq.StatementCreateTable;

import org.junit.Assert;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class TableTestCase extends DatabaseTestCase {
    public String tableName = "testTable";
    public TableBinding<TestObject> tableBinding = DBTestObject.INSTANCE;
    public boolean isVirtualTable = false;
    public Table<TestObject> table;

    public void createTable() throws WCDBException {
        if (!isVirtualTable) {
            database.createTable(tableName, tableBinding);
        } else {
            database.createVirtualTable(tableName, tableBinding);
        }
        table = database.getTable(tableName, tableBinding);
    }

    public void createValueTable() {
        StatementCreateTable createTable = new StatementCreateTable().createTable(tableName);
        createTable.define(new ColumnDef("id", ColumnType.Integer).makePrimary(true));
        createTable.define(new ColumnDef("content", ColumnType.Text));
        database.execute(createTable);
    }

    public Column[] columns() {
        return new Column[]{new Column("id"), new Column("content")};
    }

    public void createVirtualTable() throws WCDBException {
        database.createVirtualTable(tableName, tableBinding);
        table = database.getTable(tableName, tableBinding);
    }

    public void dropTable() throws WCDBException {
        database.dropTable(tableName);
    }

    public void doTestObjectsAfterInsert(Object[] objects, int insertCount, String[] sqls, TestOperation operation) {
        if(insertCount > 1) {
            ArrayList<String> list = new ArrayList<>(Arrays.asList(sqls));
            list.add(0, "BEGIN IMMEDIATE");
            list.add("COMMIT");
            sqls = list.toArray(new String[list.size()]);
        }
        doTestObjectsAfterOperation(objects, sqls, operation);
    }

    public void doTestObjectAfterOperation(Object object, String sql, TestOperation operation) {
        doTestObjectsAfterOperation(new Object[]{object}, new String[]{sql}, operation);
    }

    public void doTestObjectsAfterOperation(Object[] objects, String sql, TestOperation operation) {
        doTestObjectsAfterOperation(objects, new String[]{sql}, operation);
    }

    public void doTestObjectsAfterOperation(Object[] objects, String[] sqls, TestOperation operation) {
        doTestSQLs(sqls, operation);
        ArrayList<TestObject> allObjects = getAllObjects();
        Assert.assertTrue(((objects == null || objects.length == 0) &&
                (allObjects == null || allObjects.size() == 0)) ||
                Arrays.equals(objects, allObjects.toArray()));
    }

    public interface SelectingObjectOperation {
        List execute();
    }

    public void doTestObjectBySelecting(Object object, String sql, SelectingObjectOperation operation) {
        doTestObjectBySelecting(new Object[]{object}, new String[]{sql}, operation);
    }

    public void doTestObjectBySelecting(Object[] objects, String sql, SelectingObjectOperation operation) {
        doTestObjectBySelecting(objects, new String[]{sql}, operation);
    }

    public void doTestObjectBySelecting(Object[] objects, String[] sqls, SelectingObjectOperation operation) {
        ArrayList selecting = new ArrayList();
        doTestSQLs(sqls, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                selecting.addAll(operation.execute());
            }
        });
        Assert.assertTrue(((objects == null || objects.length == 0) &&
                (selecting == null || selecting.size() == 0)) ||
                Arrays.equals(objects, selecting.toArray()));
    }

    public interface SelectingRowsOperation {
        ArrayList<Value[]> execute();
    }

    public void doTestValueBySelecting(Value value, String sql, SelectingRowsOperation operation) {
        doTestRowsBySelecting(Collections.singletonList(new Value[]{value}), sql, operation);
    }

    public void doTestColumnBySelecting(List<Value> column, String sql, SelectingRowsOperation operation) {
        ArrayList<Value[]> rows = new ArrayList<>();
        for(Value value : column) {
            rows.add(new Value[]{value});
        }
        doTestRowsBySelecting(rows, new String[]{sql}, operation);
    }

    public void doTestRowBySelecting(Value[] row, String sql, SelectingRowsOperation operation) {
        doTestRowsBySelecting(Collections.singletonList(row), new String[]{sql}, operation);
    }

    public void doTestRowsBySelecting(List<Value[]> rows, String sql, SelectingRowsOperation operation) {
        doTestRowsBySelecting(rows, new String[]{sql}, operation);
    }

    public void doTestRowsBySelecting(List<Value[]> rows, String[] sqls, SelectingRowsOperation operation) {
        ArrayList selecting = new ArrayList();
        doTestSQLs(sqls, new TestOperation() {
            @Override
            public void execute() throws WCDBException {
                selecting.addAll(Arrays.asList(operation.execute()));
            }
        });
        Assert.assertTrue(((rows == null || rows.size() == 0) &&
                (selecting == null || selecting.size() == 0)) ||
                Arrays.equals(rows.toArray(), selecting.toArray()));
    }

    public ArrayList<TestObject> getAllObjects() throws WCDBException {
        return table.getAllObjects();
    }
}
