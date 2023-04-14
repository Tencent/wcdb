// Created by qiuwenchen on 2023/4/7.
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
public class ColumnDefTest {
    @Test
    public void test() {
        winqEqual(new ColumnDef("testColumn"), "testColumn");
        winqEqual(new ColumnDef("testColumn", ColumnType.Integer), "testColumn INTEGER");
        winqEqual(new ColumnDef(new Column("testColumn")), "testColumn");
        winqEqual(new ColumnDef(new Column("testColumn"), ColumnType.Integer), "testColumn INTEGER");
        winqEqual(new ColumnDef("testColumn", ColumnType.Integer), "testColumn INTEGER");
        winqEqual(genColumnDef().makePrimary(), "testColumn INTEGER PRIMARY KEY");
        winqEqual(genColumnDef().makePrimary(true), "testColumn INTEGER PRIMARY KEY AUTOINCREMENT");
        winqEqual(genColumnDef().makeNotNull(), "testColumn INTEGER NOT NULL");
        winqEqual(genColumnDef().makeDefaultTo(LiteralValue.currentDate()), "testColumn INTEGER DEFAULT CURRENT_DATE");
        winqEqual(genColumnDef().makeDefaultTo(LiteralValue.currentTime()), "testColumn INTEGER DEFAULT CURRENT_TIME");
        winqEqual(genColumnDef().makeDefaultTo(LiteralValue.currentTimeStamp()), "testColumn INTEGER DEFAULT CURRENT_TIMESTAMP");
    }

    ColumnDef genColumnDef() {
        return new ColumnDef("testColumn", ColumnType.Integer);
    }
}
