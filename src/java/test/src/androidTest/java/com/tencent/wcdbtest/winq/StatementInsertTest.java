// Created by chenqiuwen on 2023/4/12.
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
import com.tencent.wcdb.winq.*;
import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class StatementInsertTest {
    @Test
    public void test() {
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");

        winqEqual(new StatementInsert().insertInto("testTable")
                        .values(new Object[]{1, "value"}),
                "INSERT INTO testTable VALUES(1, 'value')");

        winqEqual(new StatementInsert().insertInto("testTable")
                        .values(new Object[]{null, true, 'a', 1, 1L, 1.1f, 1.1, "value", new BindParameter(1)}),
                "INSERT INTO testTable VALUES(NULL, TRUE, 97, 1, 1, 1.1000000238418579, 1.1000000000000001, 'value', ?1)");

        winqEqual(new StatementInsert().insertInto("testTable")
                .of(new Schema("testSchema")).values(new Object[]{1, "value"}),
                "INSERT INTO testSchema.testTable VALUES(1, 'value')");

        winqEqual(new StatementInsert().insertInto("testTable").of("testSchema")
                .as("newTable").valuesWithBindParameters(2),
                "INSERT INTO testSchema.testTable AS newTable VALUES(?1, ?2)");

        winqEqual(new StatementInsert().insertInto("testTable")
                        .columns(new Column[]{column1, column2}).values(new Object[]{1, 1.1f}),
                "INSERT INTO testTable(column1, column2) VALUES(1, 1.1000000238418579)");

        winqEqual(new StatementInsert().insertInto("testTable").orReplace()
                        .values(new Object[]{1, new BindParameter(1)}),
                "INSERT OR REPLACE INTO testTable VALUES(1, ?1)");

        CommonTableExpression cte = new CommonTableExpression("tempTable")
                .as(new StatementSelect().select("testColumn").from("table1"));
        winqEqual(new StatementInsert().with(cte).insertInto("table2")
                        .column(new Column("testColumn")).value(1),
                "WITH tempTable AS(SELECT testColumn FROM table1) INSERT INTO table2(testColumn) VALUES(1)");
        winqEqual(new StatementInsert().withRecursive(cte).insertInto("table2")
                        .column(new Column("testColumn")).value(1),
                "WITH RECURSIVE tempTable AS(SELECT testColumn FROM table1) INSERT INTO table2(testColumn) VALUES(1)");

        winqEqual(new StatementInsert().insertInto("testTable").column(new Column("testColumn")).defaultValues(),
                "INSERT INTO testTable(testColumn) DEFAULT VALUES");

        winqEqual(new StatementInsert().insertInto("testTable").column(new Column("testColumn")).value(1)
                .upsert(new Upsert().onConflict().doNoThing()),
                "INSERT INTO testTable(testColumn) VALUES(1) ON CONFLICT DO NOTHING");
    }
}
