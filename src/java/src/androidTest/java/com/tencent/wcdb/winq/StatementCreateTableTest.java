// Created by qiuwenchen on 2023/4/11.
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

package com.tencent.wcdb.winq;

import static com.tencent.wcdb.TestUtility.TestTool.winqEqual;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class StatementCreateTableTest {
    @Test
    public void test() {
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");

        ColumnDef def1 = column1.asDef(ColumnType.Integer);
        ColumnDef def2 = column2.asDef(ColumnType.Text);

        TableConstraint constraint1 = new TableConstraint("constraint1").primaryKey().indexedBy(column1);
        TableConstraint constraint2 = new TableConstraint("constraint2").unique().indexedBy(column2);

        String table1 = "table1";
        String table2 = "table2";

        winqEqual(new StatementCreateTable().createTable(table1).define(new ColumnDef[]{def1, def2}),
                "CREATE TABLE table1(column1 INTEGER, column2 TEXT)");

        winqEqual(new StatementCreateTable().createTable(table1).temp().define(new ColumnDef[]{def1, def2}),
                "CREATE TEMP TABLE table1(column1 INTEGER, column2 TEXT)");

        winqEqual(new StatementCreateTable().createTable(table1).define(new ColumnDef[]{def1, def2}).withoutRowid(),
                "CREATE TABLE table1(column1 INTEGER, column2 TEXT) WITHOUT ROWID");

        winqEqual(new StatementCreateTable().createTable(table1).of("testSchema").define(new ColumnDef[]{def1, def2}),
                "CREATE TABLE testSchema.table1(column1 INTEGER, column2 TEXT)");

        winqEqual(new StatementCreateTable().createTable(table1).ifNotExist().define(new ColumnDef[]{def1, def2}),
                "CREATE TABLE IF NOT EXISTS table1(column1 INTEGER, column2 TEXT)");

        winqEqual(new StatementCreateTable().createTable(table1).define(new ColumnDef[]{def1, def2})
                .constraint(new TableConstraint[]{constraint1, constraint2}),
                "CREATE TABLE table1(column1 INTEGER, column2 TEXT, " +
                        "CONSTRAINT constraint1 PRIMARY KEY(column1), " +
                        "CONSTRAINT constraint2 UNIQUE(column2))");
    }
}
