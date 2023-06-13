// Created by chenqiuwen on 2023/6/11.
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

package com.tencent.wcdbtest.winq;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.ColumnDef;
import com.tencent.wcdb.winq.ColumnType;
import com.tencent.wcdb.winq.StatementAlterTable;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class StatementAlterTableTest {
    @Test
    public void test() {
        winqEqual(new StatementAlterTable().alterTable("table1").renameTo("table2"),
                "ALTER TABLE table1 RENAME TO table2");
        winqEqual(new StatementAlterTable().alterTable("table1")
                .of("testSchema").renameTo("table2"),
                "ALTER TABLE testSchema.table1 RENAME TO table2");
        winqEqual(new StatementAlterTable().alterTable("table1")
                .addColumn(new Column("column1").asDef(ColumnType.Float)),
                "ALTER TABLE table1 ADD COLUMN column1 REAL");
        winqEqual(new StatementAlterTable().alterTable("table1")
                .renameColumn("column1").toColumn("column2"),
                "ALTER TABLE table1 RENAME COLUMN column1 TO column2");
        winqEqual(new StatementAlterTable().alterTable("table1")
                  .renameColumn(new Column("column1")).toColumn(new Column("column2")),
                "ALTER TABLE table1 RENAME COLUMN column1 TO column2");
    }
}
