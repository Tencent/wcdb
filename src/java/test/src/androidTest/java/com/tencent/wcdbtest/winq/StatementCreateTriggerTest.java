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

import androidx.test.ext.junit.runners.AndroidJUnit4;
import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import com.tencent.wcdb.chaincall.Insert;
import com.tencent.wcdb.winq.Column;
import com.tencent.wcdb.winq.Expression;
import com.tencent.wcdb.winq.StatementCreateTrigger;
import com.tencent.wcdb.winq.StatementDelete;
import com.tencent.wcdb.winq.StatementInsert;
import com.tencent.wcdb.winq.StatementSelect;
import com.tencent.wcdb.winq.StatementUpdate;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class StatementCreateTriggerTest {
    @Test
    public void test() {
        String schema = "testSchema";
        String name = "testTrigger";
        Column column1 = new Column("column1");
        Column column2 = new Column("column2");
        String table = "testTable";
        Expression condition = column1.eq(1);
        StatementUpdate update = new StatementUpdate().update(table).set(column1).to(2);
        StatementInsert insert = new StatementInsert().insertInto(table).values(new Object[]{1});
        StatementSelect select = new StatementSelect().select(column1);
        StatementDelete delete = new StatementDelete().deleteFrom(table);

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().delete().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTempTrigger(name)
                        .before().delete().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TEMP TRIGGER testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema).ifNotExist()
                        .before().delete().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name)
                        .before().delete().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TRIGGER testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .after().delete().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TRIGGER testSchema.testTrigger AFTER DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .insteadOf().delete().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TRIGGER testSchema.testTrigger INSTEAD OF DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().insert().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TRIGGER testSchema.testTrigger BEFORE INSERT ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().update().onTable(table).forEachRow().when(condition).execute(update),
                "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().update().ofColumn(column1).onTable(table).execute(update),
                "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1 ON testTable " +
                "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().update().ofColumn("column1").onTable(table).execute(update),
                "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1 ON testTable " +
                        "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().update().ofColumns(new Column[]{column1, column2}).onTable(table).execute(update),
                "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1, column2 ON testTable " +
                        "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().update().ofColumns(new String[]{"column1", "column2"}).onTable(table).execute(update),
                "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1, column2 ON testTable " +
                        "BEGIN UPDATE testTable SET column1 = 2; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().delete().onTable(table).forEachRow().when(condition).execute(insert),
                "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                        "BEGIN INSERT INTO testTable VALUES(1); END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().delete().onTable(table).forEachRow().when(condition).execute(delete),
                "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                        "BEGIN DELETE FROM testTable; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().delete().onTable(table).forEachRow().when(condition).execute(select),
                "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                        "BEGIN SELECT column1; END");

        winqEqual(new StatementCreateTrigger().createTrigger(name).ofSchema(schema)
                        .before().delete().onTable(table).forEachRow().when(condition)
                        .execute(update).execute(insert).execute(delete).execute(select),
                "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 " +
                        "BEGIN UPDATE testTable SET column1 = 2; INSERT INTO testTable VALUES(1); DELETE FROM testTable; SELECT column1; END");
    }
}
