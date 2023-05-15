// Created by qiuwenchen on 2023/4/13.
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
public class StatementSelectTest {
    @Test
    public void test() {
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");
        String table1 = "table1";
        String table2 = "table2";

        winqEqual(new StatementSelect().select(column1).from(table1),
                "SELECT column1 FROM table1");

        winqEqual(new StatementSelect().select(new Column[]{column1, column2}).distinct().from(table1),
                "SELECT DISTINCT column1, column2 FROM table1");

        winqEqual(new StatementSelect().select(new Object[]{"column1", column2}).distinct().from(table1),
                "SELECT DISTINCT column1, column2 FROM table1");

        winqEqual(new StatementSelect().select(new String[]{"column1", "column2"}).distinct().from(table1),
                "SELECT DISTINCT column1, column2 FROM table1");

        winqEqual(new StatementSelect().select(column1).from(new String[]{table1, table2}),
                "SELECT column1 FROM table1, table2");

        winqEqual(new StatementSelect().select(column1).from(new TableOrSubqueryConvertible[]{
                new TableOrSubquery(table1), new TableOrSubquery(table2)}),
                "SELECT column1 FROM table1, table2");

        winqEqual(new StatementSelect().select(column1).from(new Object[]{table1, new TableOrSubquery(table2)}),
                "SELECT column1 FROM table1, table2");

        winqEqual(new StatementSelect().select(column1).from(table1).where(column1.gt(1)),
                "SELECT column1 FROM table1 WHERE column1 > 1");

        winqEqual(new StatementSelect().select(column1).from(table1)
                .orderBy(new OrderingTerm[]{column1.order(Order.Asc), column2.order(Order.Desc)}),
                "SELECT column1 FROM table1 ORDER BY column1 ASC, column2 DESC");

        winqEqual(new StatementSelect().select(column1).from(table1).limit(1),
                "SELECT column1 FROM table1 LIMIT 1");

        winqEqual(new StatementSelect().select(column1).from(table1).limit(new BindParameter(1)),
                "SELECT column1 FROM table1 LIMIT ?1");

        winqEqual(new StatementSelect().select(column1).from(table1).limit(1, 2),
                "SELECT column1 FROM table1 LIMIT 1, 2");

        winqEqual(new StatementSelect().select(column1).from(table1).limit(1).offset(3),
                "SELECT column1 FROM table1 LIMIT 1 OFFSET 3");

        winqEqual(new StatementSelect().select(column1).from(table1).groupBy(column1),
                "SELECT column1 FROM table1 GROUP BY column1");

        winqEqual(new StatementSelect().select(column1).from(table1)
                        .groupBy(new Column[]{column1, column2}).having(column1.gt(1)),
                "SELECT column1 FROM table1 GROUP BY column1, column2 HAVING column1 > 1");

        winqEqual(new StatementSelect().select(column1).from(
                new StatementSelect().select(new Column[]{column1, column2}).from(table2)),
                "SELECT column1 FROM (SELECT column1, column2 FROM table2)");

        winqEqual(new StatementSelect().select(column1).from(table1).union().select(column2).from(table2),
                "SELECT column1 FROM table1 UNION SELECT column2 FROM table2");

        winqEqual(new StatementSelect().select(column1).from(table1).unionAll().select(column2).from(table2),
                "SELECT column1 FROM table1 UNION ALL SELECT column2 FROM table2");

        winqEqual(new StatementSelect().select(column1).from(table1).intersect().select(column2).from(table2),
                "SELECT column1 FROM table1 INTERSECT SELECT column2 FROM table2");

        winqEqual(new StatementSelect().select(column1).from(table1).except().select(column2).from(table2),
                "SELECT column1 FROM table1 EXCEPT SELECT column2 FROM table2");


    }
}
