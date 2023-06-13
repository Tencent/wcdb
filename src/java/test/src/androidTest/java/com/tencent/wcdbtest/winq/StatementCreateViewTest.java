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

import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.ResultColumnConvertible;
import com.tencent.wcdb.winq.StatementCreateView;
import com.tencent.wcdb.winq.StatementSelect;

import org.junit.Test;

public class StatementCreateViewTest {
    @Test
    public void test() {
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");
        StatementSelect select = new StatementSelect().select(new ResultColumnConvertible[]{column1, column2}).from("testTable");
        String view = "testView";

        winqEqual(new StatementCreateView().createView("testView").withColumns(new Column[]{column1, column2}).as(select),
                "CREATE VIEW testView(column1, column2) AS SELECT column1, column2 FROM testTable");

        winqEqual(new StatementCreateView().createTempView("testView").withColumns(new Column[]{column1, column2}).as(select),
                "CREATE TEMP VIEW testView(column1, column2) AS SELECT column1, column2 FROM testTable");

        winqEqual(new StatementCreateView().createView("testView").of("testSchema").withColumns(new Column[]{column1, column2}).as(select),
                "CREATE VIEW testSchema.testView(column1, column2) AS SELECT column1, column2 FROM testTable");

        winqEqual(new StatementCreateView().createView("testView").ifNotExist().withColumns(new Column[]{column1, column2}).as(select),
                "CREATE VIEW IF NOT EXISTS testView(column1, column2) AS SELECT column1, column2 FROM testTable");
    }
}
