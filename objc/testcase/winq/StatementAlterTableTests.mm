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

@interface StatementAlterTableTests : BaseTestCase

@end

@implementation StatementAlterTableTests {
    WCDB::Schema schema;
    NSString* table;
    NSString* newTable;
    WCDB::Column column;
    WCDB::Column newColumn;
    WCDB::ColumnDef columnDef;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    table = @"testTable";
    newTable = @"testNewTable";
    column = WCDB::Column(@"testColumn");
    newColumn = WCDB::Column(@"testNewColumn");
    columnDef = WCDB::ColumnDef(newColumn);
}

- (void)test_default_constructible
{
    WCDB::StatementAlterTable constructible;
TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementAlterTable().getType(), WCDB::SQL::Type::AlterTableSTMT);
    TestCaseAssertEqual(WCDB::StatementAlterTable::type, WCDB::SQL::Type::AlterTableSTMT);
}

- (void)test_alter_table_rename_to
{
    auto testingSQL = WCDB::StatementAlterTable().alterTable(table).schema(schema).renameToTable(newTable);
    auto testingTypes = { WCDB::SQL::Type::AlterTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"ALTER TABLE testSchema.testTable RENAME TO testNewTable");
}

- (void)test_alter_table_rename_to_without_schema
{
    auto testingSQL = WCDB::StatementAlterTable()
                      .alterTable(table)
                      .renameToTable(newTable);
    auto testingTypes = { WCDB::SQL::Type::AlterTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL,
                           @"ALTER TABLE main.testTable RENAME TO testNewTable");
}

- (void)test_alter_table_rename_column
{
    auto testingSQL = WCDB::StatementAlterTable()
                      .alterTable(table)
                      .schema(schema)
                      .renameColumn(column)
                      .toColumn(newColumn);
    auto testingTypes = { WCDB::SQL::Type::AlterTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"ALTER TABLE testSchema.testTable RENAME COLUMN testColumn TO testNewColumn");
}

- (void)test_alter_table_add_column
{
    auto testingSQL = WCDB::StatementAlterTable()
                      .alterTable(table)
                      .schema(schema)
                      .addColumn(columnDef);
    auto testingTypes = { WCDB::SQL::Type::AlterTableSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL,
                           @"ALTER TABLE testSchema.testTable ADD COLUMN testNewColumn");
}

@end
