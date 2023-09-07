// Created by qiuwenchen on 2023/6/9.
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
import com.tencent.wcdb.winq.Join;
import com.tencent.wcdb.winq.StatementSelect;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class JoinTest {
    @Test
    public void test() {
        winqEqual(new Join("testTable1").join("testTable2"),
                "testTable1 JOIN testTable2");

        winqEqual(new Join(select1()).join(select2()),
                select1Sql() + " JOIN " + select2Sql());

        winqEqual(new Join("testTable1").with("testTable2"),
                "testTable1, testTable2");

        winqEqual(new Join(select1()).with(select2()),
                select1Sql() + ", " + select2Sql());

        winqEqual(new Join("testTable1").leftJoin("testTable2"),
                "testTable1 LEFT JOIN testTable2");

        winqEqual(new Join(select1()).leftJoin(select2()),
                select1Sql() + " LEFT JOIN " + select2Sql());

        winqEqual(new Join("testTable1").leftOuterJoin("testTable2"),
                "testTable1 LEFT OUTER JOIN testTable2");

        winqEqual(new Join(select1()).leftOuterJoin(select2()),
                select1Sql() + " LEFT OUTER JOIN " + select2Sql());

        winqEqual(new Join("testTable1").innerJoin("testTable2"),
                "testTable1 INNER JOIN testTable2");

        winqEqual(new Join(select1()).innerJoin(select2()),
                select1Sql() + " INNER JOIN " + select2Sql());

        winqEqual(new Join("testTable1").crossJoin("testTable2"),
                "testTable1 CROSS JOIN testTable2");

        winqEqual(new Join(select1()).crossJoin(select2()),
                select1Sql() + " CROSS JOIN " + select2Sql());

        winqEqual(new Join("testTable1").naturalJoin("testTable2"),
                "testTable1 NATURAL JOIN testTable2");

        winqEqual(new Join(select1()).naturalJoin(select2()),
                select1Sql() + " NATURAL JOIN " + select2Sql());

        winqEqual(new Join("testTable1").naturalLeftJoin("testTable2"),
                "testTable1 NATURAL LEFT JOIN testTable2");

        winqEqual(new Join(select1()).naturalLeftJoin(select2()),
                select1Sql() + " NATURAL LEFT JOIN " + select2Sql());

        winqEqual(new Join("testTable1").naturalLeftOuterJoin("testTable2"),
                "testTable1 NATURAL LEFT OUTER JOIN testTable2");

        winqEqual(new Join(select1()).naturalLeftOuterJoin(select2()),
                select1Sql() + " NATURAL LEFT OUTER JOIN " + select2Sql());

        winqEqual(new Join("testTable1").naturalInnerJoin("testTable2"),
                "testTable1 NATURAL INNER JOIN testTable2");

        winqEqual(new Join(select1()).naturalInnerJoin(select2()),
                select1Sql() + " NATURAL INNER JOIN " + select2Sql());

        winqEqual(new Join("testTable1").naturalCrossJoin("testTable2"),
                "testTable1 NATURAL CROSS JOIN testTable2");

        winqEqual(new Join(select1()).naturalCrossJoin(select2()),
                select1Sql() + " NATURAL CROSS JOIN " + select2Sql());

        winqEqual(new Join("testTable1").join("testTable2").on(new Column("column1").eq(new Column("column2"))),
                "testTable1 JOIN testTable2 ON column1 == column2");

        winqEqual(new Join(select1()).join(select2()).on(new Column("column1").eq(new Column("column2"))),
                select1Sql() + " JOIN " + select2Sql() + " ON column1 == column2");

        winqEqual(new Join("testTable1").join("testTable2").using(new Column[]{new Column("column1"), new Column("column2")}),
                "testTable1 JOIN testTable2 USING(column1, column2)");

        winqEqual(new Join(select1()).join(select2()).using(new Column[]{new Column("column1"), new Column("column2")}),
                select1Sql() + " JOIN " + select2Sql() + " USING(column1, column2)");
    }

    StatementSelect select1() {
        return new StatementSelect().select(new Column("column1")).from("testTable1");
    }

    String select1Sql() {
        return "(" + select1().getDescription() + ")";
    }

    StatementSelect select2() {
        return new StatementSelect().select(new Column("column2")).from("testTable2");
    }

    String select2Sql() {
        return "(" + select2().getDescription() + ")";
    }
}
