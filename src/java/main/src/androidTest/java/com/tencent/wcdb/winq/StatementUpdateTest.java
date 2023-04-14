// Created by qiuwenchen on 2023/4/14.
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

import static com.tencent.wcdb.base.TestTool.winqEqual;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class StatementUpdateTest {
    @Test
    public void test() {
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");

        winqEqual(new StatementUpdate().update("testTable").set(column1).to(1),
                "UPDATE testTable SET column1 = 1");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to(true),
                "UPDATE testTable SET column1 = TRUE");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to("abc"),
                "UPDATE testTable SET column1 = 'abc'");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to(1.1),
                "UPDATE testTable SET column1 = 1.1000000000000001");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to((String) null),
                "UPDATE testTable SET column1 = NULL");
        winqEqual(new StatementUpdate().update("testTable").orReplace().set(column1).to(1).set(column2).to("abc"),
                "UPDATE OR REPLACE testTable SET column1 = 1, column2 = 'abc'");
        winqEqual(new StatementUpdate().update("testTable").setColumnsToBindParameters(new Column[]{column1, column2}),
                "UPDATE testTable SET column1 = ?1, column2 = ?2");
        winqEqual(new StatementUpdate().update("testTable").setColumnsToValues(new Column[]{column1, column2}, new Object[]{1, "abc"}),
                "UPDATE testTable SET column1 = 1, column2 = 'abc'");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to(1).where(column1.gt(1)),
                "UPDATE testTable SET column1 = 1 WHERE column1 > 1");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to(1)
                .orderBy(new OrderingTerm[]{column1.order(Order.Asc), column2.order(Order.Desc)}),
                "UPDATE testTable SET column1 = 1 ORDER BY column1 ASC, column2 DESC");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to(1).limit(1),
                "UPDATE testTable SET column1 = 1 LIMIT 1");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to(1).limit(1, 2),
                "UPDATE testTable SET column1 = 1 LIMIT 1, 2");
        winqEqual(new StatementUpdate().update("testTable").set(column1).to(1).limit(1).offset(3),
                "UPDATE testTable SET column1 = 1 LIMIT 1 OFFSET 3");
    }
}
