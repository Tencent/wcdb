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

@interface WTCStatementCreateTableTests : XCTestCase

@end

@implementation WTCStatementCreateTableTests

- (void)testStatementCreateTable
{
    std::string schemaName = "testSchema";

    std::string tableName = "testTable";

    WCDB::ColumnDef columnDef1 = WCDB::ColumnDef("testColumn1").withType(WCDB::ColumnType::Integer32);
    WCDB::ColumnDef columnDef2 = WCDB::ColumnDef("testColumn2").withType(WCDB::ColumnType::Text);
    std::list<WCDB::ColumnDef> columnDefs = {columnDef1, columnDef2};

    WCDB::TableConstraint tableConstraint1 = WCDB::TableConstraint().withPrimaryKey("testColumn1");
    WCDB::TableConstraint tableConstraint2 = WCDB::TableConstraint().withUnique("testColumn2");
    std::list<WCDB::TableConstraint> tableConstraints = {tableConstraint1, tableConstraint2};

    WCDB::StatementSelect statementSelect = WCDB::StatementSelect().select(WCDB::ResultColumn(WCDB::Expression::ColumnNamed("testColumn1")));

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, false).withSchema(schemaName).define(columnDef1), @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, false).withSchema(schemaName).define(columnDef1).withoutRowID(), @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER) WITHOUT ROWID");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, false).withSchema(schemaName).define(columnDef1).addTableConstraint(tableConstraint1), @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, PRIMARY KEY(testColumn1))");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, false).withSchema(schemaName).define(columnDef1).addTableConstraints(tableConstraints), @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, PRIMARY KEY(testColumn1), UNIQUE(testColumn2))");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, false).withSchema(schemaName).define(columnDefs), @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, testColumn2 TEXT)");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, false).withSchema(schemaName).as(statementSelect), @"CREATE TABLE testSchema.testTable AS SELECT testColumn1");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, false).define(columnDef1), @"CREATE TABLE testTable(testColumn1 INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, true, false).withSchema(schemaName).define(columnDef1), @"CREATE TABLE IF NOT EXISTS testSchema.testTable(testColumn1 INTEGER)");

    WINQAssertEqual(WCDB::StatementCreateTable().createTable(tableName, false, true).withSchema(schemaName).define(columnDef1), @"CREATE TEMP TABLE testSchema.testTable(testColumn1 INTEGER)");
}

@end
