// Created by chenqiuwen on 2023/4/10.
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
public class StatementCreateIndexTest {
    @Test
    public void test() {
        IndexedColumn index1 = new IndexedColumn("column1");
        IndexedColumn index2 = new IndexedColumn("column2").order(Order.Asc);
        String indexName = "index1";
        String tableName = "table1";

        winqEqual(new StatementCreateIndex().createIndex(indexName).on(tableName)
                        .indexedBy(new IndexedColumn[]{index1, index2}),
                "CREATE INDEX index1 ON table1(column1, column2 ASC)");

        winqEqual(new StatementCreateIndex().createIndex(indexName).of("testSchema").on(tableName)
                        .indexedBy(new IndexedColumn[]{index1, index2}),
                "CREATE INDEX testSchema.index1 ON table1(column1, column2 ASC)");

        winqEqual(new StatementCreateIndex().createIndex(indexName).unique().on(tableName).indexedBy(index2),
                "CREATE UNIQUE INDEX index1 ON table1(column2 ASC)");

        winqEqual(new StatementCreateIndex().createIndex(indexName).ifNotExist().on(tableName).indexedBy("newColumn"),
                "CREATE INDEX IF NOT EXISTS index1 ON table1(newColumn)");

        winqEqual(new StatementCreateIndex().createIndex(indexName).on(tableName)
                .indexedBy(new String[]{"column1", "column2"}).where(new Column("column1").ge(1)),
                "CREATE INDEX index1 ON table1(column1, column2) WHERE column1 >= 1");
    }
}
