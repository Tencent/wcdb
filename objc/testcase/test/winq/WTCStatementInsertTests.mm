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

@interface WTCStatementInsertTests : WTCWINQTestCase

@end

@implementation WTCStatementInsertTests

- (void)testStatementInsert
{
    WINQAssertEqual(WCDB::StatementInsert()
                        .insertInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"INSERT INTO testSchema.testTable(testColumn) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.values),
                    @"INSERT INTO testSchema.testTable(testColumn) VALUES(1, 'testValue')");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.statementSelect),
                    @"INSERT INTO testSchema.testTable(testColumn) SELECT testColumn FROM testTable");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .defaultValues(),
                    @"INSERT INTO testSchema.testTable(testColumn) DEFAULT VALUES");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnNames)
                        .values(self.class.value),
                    @"INSERT INTO testSchema.testTable(testColumn, testColumn2) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .values(self.class.value),
                    @"INSERT INTO testSchema.testTable VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertInto(self.class.tableName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"INSERT INTO testTable(testColumn) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertOrReplaceInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"INSERT OR REPLACE INTO testSchema.testTable(testColumn) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertOrRollbackInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"INSERT OR ROLLBACK INTO testSchema.testTable(testColumn) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertOrAbortInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"INSERT OR ABORT INTO testSchema.testTable(testColumn) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertOrFailInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"INSERT OR FAIL INTO testSchema.testTable(testColumn) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .insertOrIgnoreInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"INSERT OR IGNORE INTO testSchema.testTable(testColumn) VALUES(1)");

    WINQAssertEqual(WCDB::StatementInsert()
                        .with(self.class.withClause)
                        .insertInto(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .on(self.class.columnName)
                        .values(self.class.value),
                    @"WITH testTable AS(SELECT testColumn FROM testTable) INSERT INTO testSchema.testTable(testColumn) VALUES(1)");
}

@end
