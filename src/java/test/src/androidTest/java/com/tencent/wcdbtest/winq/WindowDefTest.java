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
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.ExpressionConvertible;
import com.tencent.wcdb.winq.FrameSpec;
import com.tencent.wcdb.winq.Order;
import com.tencent.wcdb.winq.WindowDef;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class WindowDefTest {
    @Test
    public void test() {
        winqEqual(new WindowDef(), "");
        winqEqual(new WindowDef().partitionBy("column1"),
                "(PARTITION BY column1)");
        winqEqual(new WindowDef().partitionBy(new String[]{"column1", "column2"}),
                "(PARTITION BY column1, column2)");
        winqEqual(new WindowDef().partitionBy(new Column("column1").add(1)),
                "(PARTITION BY column1 + 1)");
        winqEqual(new WindowDef().partitionBy(new ExpressionConvertible[]{
                new Column("column1").add(1), new Column("column2")}),
                "(PARTITION BY column1 + 1, column2)");
        winqEqual(new WindowDef().orderBy(new Column("column1").order(Order.Asc)),
                "(ORDER BY column1 ASC)");
        winqEqual(new WindowDef().frameSpec(new FrameSpec().range().unboundedPreceding()),
                "(RANGE UNBOUNDED PRECEDING)");
        winqEqual(new WindowDef().partitionBy(new ExpressionConvertible[]{
                new Column("column1").add(1), new Column("column2")})
                        .orderBy(new Column("column1").order(Order.Asc))
                        .frameSpec(new FrameSpec().range().unboundedPreceding()),
                "(PARTITION BY column1 + 1, column2 ORDER BY column1 ASC RANGE UNBOUNDED PRECEDING)");
    }
}
