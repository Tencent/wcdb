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

@interface StatementInsertTests : BaseTestCase

@end

@implementation StatementInsertTests {
    WCDB::With with;
    WCDB::Schema schema;
    NSString* table;
    NSString* alias;
    WCDB::Column column1;
    WCDB::Column column2;
    WCDB::Columns columns;
    WCDB::Expression expression1;
    WCDB::Expression expression2;
    WCDB::Expressions expressions1;
    WCDB::Expressions expressions2;
    WCDB::Upsert upsert;
    WCDB::StatementSelect select;
}

- (void)setUp
{
    [super setUp];
    with = WCDB::With().table(@"testTable").as(WCDB::StatementSelect().select(1));
    schema = @"testSchema";
    table = @"testTable";
    alias = @"testAliasTable";
    column1 = WCDB::Column(@"testColumn1");
    column2 = WCDB::Column(@"testColumn2");
    columns = {
        column1,
        column2,
    };
    expression1 = 1;
    expression2 = 2;
    expressions1 = {
        1,
        2,
    };
    expressions2 = {
        3,
        4,
    };
    upsert = WCDB::Upsert().conflict().doNothing();
    select = WCDB::StatementSelect().select(1);
}

- (void)test_default_constructible
{
    WCDB::StatementInsert constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementInsert().getType(), WCDB::SQL::Type::InsertSTMT);
    TestCaseAssertEqual(WCDB::StatementInsert::type, WCDB::SQL::Type::InsertSTMT);
}

- (void)test_insert
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_with_seperated_columns
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).column(column1).column(column2).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1, testColumn2) VALUES(1, 2)");
}

- (void)test_insert_with_columns
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).columns(columns).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1, testColumn2) VALUES(1, 2)");
}

- (void)test_insert_or_replace
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).orReplace().column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT OR REPLACE INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_or_rollback
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).orRollback().column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT OR ROLLBACK INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_or_abort
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).orAbort().column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT OR ABORT INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_or_fail
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).orFail().column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT OR FAIL INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_or_ignore
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).orIgnore().column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT OR IGNORE INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_with
{
    auto testingSQL = WCDB::StatementInsert().with(with).insertIntoTable(table).schema(schema).column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::WithClause, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"WITH testTable AS(SELECT 1) INSERT INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_without_schema
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO main.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_with_alias
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).as(alias).column(column1).values(expressions1);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable AS testAliasTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_with_multiple_values
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).column(column1).values(expressions1).values(expressions2);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1) VALUES(1, 2), (3, 4)");
}

- (void)test_insert_with_seperated_values
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).column(column1).value(expression1).value(expression2);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1) VALUES(1, 2)");
}

- (void)test_insert_with_select
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).column(column1).values(select);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1) SELECT 1");
}

- (void)test_insert_with_default_values
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).column(column1).defaultValues();

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1) DEFAULT VALUES");
}

- (void)test_insert_with_upsert
{
    auto testingSQL = WCDB::StatementInsert().insertIntoTable(table).schema(schema).column(column1).values(expressions1).upsert(upsert);

    auto testingTypes = { WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpsertClause };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"INSERT INTO testSchema.testTable(testColumn1) VALUES(1, 2) ON CONFLICT DO NOTHING");
}
@end
