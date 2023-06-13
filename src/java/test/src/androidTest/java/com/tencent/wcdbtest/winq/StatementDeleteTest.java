// Created by chenqiuwen on 2023/4/11.
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
public class StatementDeleteTest {
    @Test
    public void test() {
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");

        winqEqual(genStatementDelete().where(column1.gt(1)),
                "DELETE FROM testTable WHERE column1 > 1");

        winqEqual(new StatementDelete().deleteFrom(new QualifiedTable("testTable").of("testSchema"))
                .where(column1.gt(1)),
                "DELETE FROM testSchema.testTable WHERE column1 > 1");

        winqEqual(genStatementDelete().orderBy(
                new OrderingTerm[]{column1.order(Order.Asc), column2.order(Order.Desc)}),
                "DELETE FROM testTable ORDER BY column1 ASC, column2 DESC");

        winqEqual(genStatementDelete().limit(1), "DELETE FROM testTable LIMIT 1");

        winqEqual(genStatementDelete().limit(1, 2), "DELETE FROM testTable LIMIT 1, 2");

        winqEqual(genStatementDelete().limit(1).offset(3), "DELETE FROM testTable LIMIT 1 OFFSET 3");

        winqEqual(genStatementDelete().where(column1.gt(1))
                .orderBy(new OrderingTerm[]{column1.order(Order.Asc), column2.order(Order.Desc)})
                        .limit(1).offset(2),
                "DELETE FROM testTable WHERE column1 > 1 ORDER BY column1 ASC, column2 DESC LIMIT 1 OFFSET 2");

        CommonTableExpression cte = new CommonTableExpression("tempTable")
                .as(new StatementSelect().select("testColumn").from("table1"));
        winqEqual(new StatementDelete().with(cte).deleteFrom("table2"),
                "WITH tempTable AS(SELECT testColumn FROM table1) DELETE FROM table2");
        winqEqual(new StatementDelete().withRecursive(cte).deleteFrom("table2"),
                "WITH RECURSIVE tempTable AS(SELECT testColumn FROM table1) DELETE FROM table2");
    }

    private StatementDelete genStatementDelete() {
        return new StatementDelete().deleteFrom("testTable");
    }
}
