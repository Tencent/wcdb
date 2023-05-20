// Created by qiuwenchen on 2023/3/31.
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
public class ColumnTest {
    @Test
    public void test() {
        winqEqual(Column.all(), "*");
        winqEqual(Column.rowId(), "rowid");
        winqEqual(new Column("testColumn"), "testColumn");
        winqEqual(new Column("testColumn").table("testTable"), "testTable.testColumn");
        winqEqual(new Column("testColumn").table("testTable").of("testSchema"), "testSchema.testTable.testColumn");
        winqEqual(new Column("testColumn").table("testTable").of(new Schema("testSchema")), "testSchema.testTable.testColumn");
    }
}
