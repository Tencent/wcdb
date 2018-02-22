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

#import "WTCWINQTestCase.h"

@interface WTCStatementCreateTriggerTests : WTCWINQTestCase

@end

@implementation WTCStatementCreateTriggerTests

- (void)testStatementCreateTrigger
{
    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementInsert),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN INSERT INTO testTable VALUES(1); END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementDelete),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN DELETE FROM testTable; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementSelect),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN SELECT testColumn FROM testTable; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate)
                        .run(self.class.statementInsert)
                        .run(self.class.statementDelete)
                        .run(self.class.statementSelect),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; INSERT INTO testTable VALUES(1); DELETE FROM testTable; SELECT testColumn FROM testTable; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .insert()
                        .on(self.class.tableName)
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE INSERT ON testTable WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .update()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .updateOf(self.class.columnName)
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .updateOf(self.class.columnNames)
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn, testColumn2 ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .after()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger AFTER DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .insteadOf()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger INSTEAD OF DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .withSchema(self.class.schemaName)
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testSchema.testTrigger DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, false)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, true, false)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName, false, true)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TEMP TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");

    //Default
    WINQAssertEqual(WCDB::StatementCreateTrigger()
                        .createTrigger(self.class.triggerName)
                        .withSchema(self.class.schemaName)
                        .before()
                        .delete_()
                        .on(self.class.tableName)
                        .forEachRow()
                        .when(self.class.condition)
                        .run(self.class.statementUpdate),
                    @"CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN testColumn NOTNULL BEGIN UPDATE testTable SET testColumn = 1; END");
}

@end
