//
// Created by qiuwenchen on 2022/7/15.
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

import XCTest
import WCDB

class StatementCreateTriggerTests: BaseTestCase {

    func testCreateTrigger() {
        // Give
        let schema = "testSchema"
        let name = "testTrigger"
        let column1 = Column(named: "testColumn1")
        let column2 = Column(named: "testColumn2")
        let table = "testTable"
        let condition = 1
        let update = StatementUpdate().update(table: table).set(Column(named: "testColumn")).to(1)
        let select = StatementSelect().select(1)
        let insert = StatementInsert().insert(intoTable: table).values(1)
        let delete_ = StatementDelete().delete(from: table)

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().delete().on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name, isTemp: true).before().delete().on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TEMP TRIGGER testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).ifNotExists().before().delete().on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).before().delete().on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TRIGGER main.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).after().delete().on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TRIGGER testSchema.testTrigger AFTER DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).insteadOf().delete().on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TRIGGER testSchema.testTrigger INSTEAD OF DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).ifNotExists().before().insert().on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE INSERT ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().update().of(columns: column1, column2).on(table: table).forEachRow().when(condition).execute(update),
            "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn1, testColumn2 ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().update().of(columns: column1, column2).on(table: table).when(condition).execute(update),
            "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn1, testColumn2 ON testTable WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().update().of(columns: column1, column2).on(table: table).execute(update),
            "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn1, testColumn2 ON testTable BEGIN UPDATE testTable SET testColumn = 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().delete().on(table: table).forEachRow().when(condition).execute(insert),
            "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN INSERT INTO testTable VALUES(1); END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().delete().on(table: table).forEachRow().when(condition).execute(delete_),
            "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN DELETE FROM testTable; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().delete().on(table: table).forEachRow().when(condition).execute(select),
            "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN SELECT 1; END"
        )

        WINQAssertEqual(
            StatementCreateTrigger().create(trigger: name).of(schema: schema).before().delete().on(table: table).forEachRow().when(condition).execute(update).execute(insert).execute(delete_).execute(select),
            "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE testTable SET testColumn = 1; INSERT INTO testTable VALUES(1); DELETE FROM testTable; SELECT 1; END"
        )
    }
}
