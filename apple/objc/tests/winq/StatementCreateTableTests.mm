//
// Created by sanhuazhang on 2019/05/02
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

#import "WINQAssertion.h"

@interface StatementCreateTableTests : BaseTestCase

@end

@implementation StatementCreateTableTests {
    WCDB::Schema schema;
    NSString* table;
    WCDB::ColumnDef columnDef1;
    WCDB::ColumnDef columnDef2;
    WCDB::TableConstraint tableConstraint1;
    WCDB::TableConstraint tableConstraint2;
    WCDB::StatementSelect select;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    table = @"testTable";
    columnDef1 = WCDB::ColumnDef(WCDB::Column(@"testColumn1"), WCDB::ColumnType::Integer);
    columnDef2 = WCDB::ColumnDef(WCDB::Column(@"testColumn2"), WCDB::ColumnType::Text);
    tableConstraint1 = WCDB::TableConstraint().check(1);
    tableConstraint2 = WCDB::TableConstraint().check(2);
    select = WCDB::StatementSelect().select(1);
}

- (void)test_default_constructible
{
    WCDB::StatementCreateTable constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementCreateTable().getType(), WCDB::SQL::Type::CreateTableSTMT);
    TestCaseAssertEqual(WCDB::StatementCreateTable::type, WCDB::SQL::Type::CreateTableSTMT);
}

- (void)test_create_table
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDef1);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER)");
}

- (void)test_create_table_with_multi_defines
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDef1).define(columnDef2);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, testColumn2 TEXT)");
}

- (void)test_create_temp_table
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).temp().define(columnDef1);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TEMP TABLE testTable(testColumn1 INTEGER)");
}

- (void)test_create_table_if_not_exists
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).ifNotExists().define(columnDef1);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE IF NOT EXISTS testSchema.testTable(testColumn1 INTEGER)");
}

- (void)test_create_table_without_schema
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).define(columnDef1);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE testTable(testColumn1 INTEGER)");
}

- (void)test_create_table_with_constraint
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDef1).constraint(tableConstraint1);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, CHECK(1))");
}

- (void)test_create_table_with_constraints
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDef1).constraint(tableConstraint1).constraint(tableConstraint2);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, CHECK(1), CHECK(2))");
}

- (void)test_create_table_without_rowid
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDef1).withoutRowID();

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER) WITHOUT ROWID");
}

- (void)test_create_table_as_select
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).as(select);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE TABLE testSchema.testTable AS SELECT 1");
}

@end
