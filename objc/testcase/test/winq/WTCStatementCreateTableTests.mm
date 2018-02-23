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

@interface WTCStatementCreateTableTests : WTCWINQTestCase

@end

@implementation WTCStatementCreateTableTests

- (void)testStatementCreateTable
{
    XCTAssertEqual(WCDB::StatementCreateTable().getType(), WCDB::Statement::Type::CreateTable);

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, false, false)
                        .withSchema(self.class.schemaName)
                        .define(self.class.columnDef),
                    @"CREATE TABLE testSchema.testTable(testColumn INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, false, false)
                        .withSchema(self.class.schemaName)
                        .define(self.class.columnDef)
                        .withoutRowID(),
                    @"CREATE TABLE testSchema.testTable(testColumn INTEGER) WITHOUT ROWID");

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, false, false)
                        .withSchema(self.class.schemaName)
                        .define(self.class.columnDef)
                        .addTableConstraint(self.class.tableConstraint),
                    @"CREATE TABLE testSchema.testTable(testColumn INTEGER, CONSTRAINT testConstraint PRIMARY KEY(testColumn))")

        WINQAssertEqual(WCDB::StatementCreateTable()
                            .createTable(self.class.tableName, false, false)
                            .withSchema(self.class.schemaName)
                            .define(self.class.columnDef)
                            .addTableConstraints(self.class.tableConstraints),
                        @"CREATE TABLE testSchema.testTable(testColumn INTEGER, CONSTRAINT testConstraint PRIMARY KEY(testColumn), CONSTRAINT testConstraint2 UNIQUE(testColumn2))");

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, false, false)
                        .withSchema(self.class.schemaName)
                        .define(self.class.columnDefs),
                    @"CREATE TABLE testSchema.testTable(testColumn INTEGER, testColumn2 TEXT)");

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, false, false)
                        .withSchema(self.class.schemaName)
                        .as(self.class.statementSelect),
                    @"CREATE TABLE testSchema.testTable AS SELECT testColumn FROM testTable");

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, false, false)
                        .define(self.class.columnDef),
                    @"CREATE TABLE testTable(testColumn INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, true, false)
                        .withSchema(self.class.schemaName)
                        .define(self.class.columnDef),
                    @"CREATE TABLE IF NOT EXISTS testSchema.testTable(testColumn INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName, false, true)
                        .withSchema(self.class.schemaName)
                        .define(self.class.columnDef),
                    @"CREATE TEMP TABLE testSchema.testTable(testColumn INTEGER)");

    //Default
    WINQAssertEqual(WCDB::StatementCreateTable()
                        .createTable(self.class.tableName)
                        .withSchema(self.class.schemaName)
                        .define(self.class.columnDef),
                    @"CREATE TABLE IF NOT EXISTS testSchema.testTable(testColumn INTEGER)");
}

@end
