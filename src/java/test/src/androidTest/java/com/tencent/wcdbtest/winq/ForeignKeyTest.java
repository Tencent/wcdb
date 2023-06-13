// Created by qiuwenchen on 2023/6/8.
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
import com.tencent.wcdb.winq.ForeignKey;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class ForeignKeyTest {
    @Test
    public void test() {
        winqEqual(genForeignKey().onDelete(ForeignKey.Action.SetNull),
                "REFERENCES testForeignKeyTable(column1, column2) ON DELETE SET NULL");
        winqEqual(genForeignKey().onUpdate(ForeignKey.Action.SetDefault),
                "REFERENCES testForeignKeyTable(column1, column2) ON UPDATE SET DEFAULT");
        winqEqual(genForeignKey().onDelete(ForeignKey.Action.Cascade),
                "REFERENCES testForeignKeyTable(column1, column2) ON DELETE CASCADE");
        winqEqual(genForeignKey().onUpdate(ForeignKey.Action.Restrict),
                "REFERENCES testForeignKeyTable(column1, column2) ON UPDATE RESTRICT");
        winqEqual(genForeignKey().onDelete(ForeignKey.Action.NoAction),
                "REFERENCES testForeignKeyTable(column1, column2) ON DELETE NO ACTION");
        winqEqual(genForeignKey().deferrable(ForeignKey.Initially.Deferred),
                "REFERENCES testForeignKeyTable(column1, column2) DEFERRABLE INITIALLY DEFERRED");
        winqEqual(genForeignKey().notDeferrable(ForeignKey.Initially.Immediate),
                "REFERENCES testForeignKeyTable(column1, column2) NOT DEFERRABLE INITIALLY IMMEDIATE");
        winqEqual(genForeignKey().match(ForeignKey.Match.Simple),
                "REFERENCES testForeignKeyTable(column1, column2) MATCH SIMPLE");
    }

    ForeignKey genForeignKey() {
        return new ForeignKey().references("testForeignKeyTable")
                .columns(new Column[]{new Column("column1"), new Column("column2")});
    }
}
