// Created by chenqiuwen on 2023/6/10.
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
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.Upsert;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class UpsertTest {
    @Test
    public void test() {
        Column column = new Column("column1");
        Column[] columns = new Column[]{new Column("column2"), new Column("column3")};

        winqEqual(new Upsert().onConflict().doNoThing(),
                "ON CONFLICT DO NOTHING");
        winqEqual(new Upsert().onConflict().indexedBy(column).doNoThing(),
                "ON CONFLICT(column1) DO NOTHING");
        winqEqual(new Upsert().onConflict().indexedBy(column).where(column.eq(1)).doNoThing(),
                "ON CONFLICT(column1) WHERE column1 == 1 DO NOTHING");
        winqEqual(new Upsert().onConflict().doUpdate().setColumn(column).to((ExpressionConvertible) null),
                "ON CONFLICT DO UPDATE SET column1 = NULL");
        winqEqual(new Upsert().onConflict().doUpdate().setColumn(column).to(true),
                "ON CONFLICT DO UPDATE SET column1 = TRUE");
        winqEqual(new Upsert().onConflict().doUpdate().setColumn(column).to(1),
                "ON CONFLICT DO UPDATE SET column1 = 1");
        winqEqual(new Upsert().onConflict().doUpdate().setColumn(column).to("abc"),
                "ON CONFLICT DO UPDATE SET column1 = 'abc'");
        winqEqual(new Upsert().onConflict().doUpdate().setColumn(column).to(1).setColumns(columns).to(2),
                "ON CONFLICT DO UPDATE SET column1 = 1, (column2, column3) = 2");
        winqEqual(new Upsert().onConflict().doUpdate().setColumn(column).to(1).where(column.eq(2)),
                "ON CONFLICT DO UPDATE SET column1 = 1 WHERE column1 == 2");
    }
}
