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

#import "WINQTestCase.h"
#import <WCDB/WCDB.h>

@interface StatementCreateTableTests : WINQTestCase

@end

@implementation StatementCreateTableTests {
    WCDB::Schema schema;
    NSString* table;
    WCDB::ColumnDefs columnDefs;
    WCDB::TableConstraints tableConstraints;
    WCDB::StatementSelect select;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    table = @"testTable";
    columnDefs = {
        WCDB::ColumnDef(WCDB::Column(@"testColumn1"), WCDB::ColumnType::Integer32),
        WCDB::ColumnDef(WCDB::Column(@"testColumn2"), WCDB::ColumnType::Text),
    };
    tableConstraints = { WCDB::TableConstraint().check(1), WCDB::TableConstraint().check(2) };
    select = WCDB::StatementSelect().select(1);
}

- (void)test_default_constructible
{
    WCDB::StatementCreateTable constructible __attribute((unused));
}

- (void)test_create_table
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDefs);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, testColumn2 TEXT)");
}

- (void)test_create_temp_table
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).temp().define(columnDefs);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TEMP TABLE testSchema.testTable(testColumn1 INTEGER, testColumn2 TEXT)");
}

- (void)test_create_table_if_not_exists
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).ifNotExists().define(columnDefs);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TABLE IF NOT EXISTS testSchema.testTable(testColumn1 INTEGER, testColumn2 TEXT)");
}

- (void)test_create_table_without_schema
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).define(columnDefs);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TABLE main.testTable(testColumn1 INTEGER, testColumn2 TEXT)");
}

- (void)test_create_table_with_constraints
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDefs).constraint(tableConstraints);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, testColumn2 TEXT, CHECK(1), CHECK(2))");
}

- (void)test_create_table_without_rowid
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).define(columnDefs).withoutRowID();

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TABLE testSchema.testTable(testColumn1 INTEGER, testColumn2 TEXT) WITHOUT ROWID");
}

- (void)test_create_table_as_select
{
    auto testingSQL = WCDB::StatementCreateTable().createTable(table).schema(schema).as(select);

    auto testingTypes = { WCDB::SQL::Type::CreateTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TABLE testSchema.testTable AS SELECT 1");
}

@end
