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

#import "WTCAssert.h"
#import <WINQ/abstract.h>

@interface WTCStatementCreateTriggerTests : XCTestCase

@end

@implementation WTCStatementCreateTriggerTests

- (void)testStatementCreateTrigger
{
    std::string schemaName = "testSchema";
    std::string triggerName = "testTrigger";
    std::string columnName1 = "testColumn1";
    std::list<std::string> columnNames = {columnName1, "testColumn2"};
    std::string tableName = "testTable";
    WCDB::Expression expr = WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn1")).notNull();

    WCDB::StatementUpdate statementUpdate = WCDB::StatementUpdate().update(WCDB::QualifiedTableName("testTable")).set("testColumn1", WCDB::Expression(WCDB::LiteralValue(1)));
    WCDB::StatementInsert statementInsert = WCDB::StatementInsert().insertInto("testTable").values(WCDB::Expression(WCDB::LiteralValue(1)));
    WCDB::StatementDelete statementDelete = WCDB::StatementDelete().deleteFrom(WCDB::QualifiedTableName("testTable"));
    WCDB::StatementSelect statementSelect = WCDB::StatementSelect().select(WCDB::ResultColumn(WCDB::Expression::ColumnNamed("testColumn1")));

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate).run(statementInsert).run(statementDelete).run(statementSelect), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; INSERT INTO testTable VALUES(1); DELETE FROM testTable; SELECT testColumn1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementInsert), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN INSERT INTO testTable VALUES(1); END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementDelete), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN DELETE FROM testTable; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementSelect), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN SELECT testColumn1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate).run(statementInsert).run(statementDelete).run(statementSelect), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; INSERT INTO testTable VALUES(1); DELETE FROM testTable; SELECT testColumn1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().delete_().on(tableName).when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().insert().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE INSERT ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().update().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().updateOf(columnName1).on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn1 ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).before().updateOf(columnNames).on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn1, testColumn2 ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).after().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger AFTER DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).insteadOf().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger INSTEAD OF DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).withSchema(schemaName).delete_().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testSchema.testTrigger DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, false).before().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, true, false).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger().createTrigger(triggerName, false, true).withSchema(schemaName).before().delete_().on(tableName).forEachRow().when(expr).run(statementUpdate), @"CREATE TEMP TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn1 NOTNULL BEGIN UPDATE testTable SET testColumn1 = 1; END");
}

@end
