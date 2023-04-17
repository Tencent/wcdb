// Created by chenqiuwen on 2023/4/9.
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
import static com.tencent.wcdbtest.base.TestTool.winqEqual;
import com.tencent.wcdb.winq.*;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class TableConstraintTest {
    @Test
    public void test() {
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");
        winqEqual(column1, "column1");
        winqEqual(column2, "column2");
        winqEqual(new TableConstraint("testConstraint").primaryKey().indexedBy(new Column[]{column1, column2}),
                "CONSTRAINT testConstraint PRIMARY KEY(column1, column2)");
        winqEqual(new TableConstraint().primaryKey().indexedBy(new Column[]{column1, column2}),
                "PRIMARY KEY(column1, column2)");
        winqEqual(new TableConstraint().unique().indexedBy(new Column[]{column1, column2}),
                "UNIQUE(column1, column2)");
        winqEqual(new TableConstraint().unique().indexedBy(new Column[]{column1, column2}).onConflict(ConflictAction.Fail),
                "UNIQUE(column1, column2) ON CONFLICT FAIL");
        winqEqual(new TableConstraint().check(column1.gt(1)),
                "CHECK(column1 > 1)");

        String columnName1 = "column1";
        String columnName2 = "column2";
        winqEqual(new TableConstraint().primaryKey().indexedBy(new String[]{columnName1, columnName2}),
                "PRIMARY KEY(column1, column2)");
        winqEqual(new TableConstraint("testConstraint").primaryKey().indexedBy(new String[]{columnName1, columnName2}),
                "CONSTRAINT testConstraint PRIMARY KEY(column1, column2)");
        winqEqual(new TableConstraint().unique().indexedBy(new String[]{columnName1, columnName2}),
                "UNIQUE(column1, column2)");
        winqEqual(new TableConstraint().unique().indexedBy(new String[]{columnName1, columnName2}).onConflict(ConflictAction.Fail),
                "UNIQUE(column1, column2) ON CONFLICT FAIL");
    }
}
