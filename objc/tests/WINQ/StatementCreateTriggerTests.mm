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

@interface StatementCreateTriggerTests : WINQTestCase

@end

@implementation StatementCreateTriggerTests {
    WCDB::Schema schema;
    NSString* name;
    WCDB::Column column1;
    WCDB::Column column2;
    NSString* table;
    WCDB::Expression condition;
    WCDB::StatementUpdate update;
    WCDB::StatementSelect select;
    WCDB::StatementInsert insert;
    WCDB::StatementDelete delete_;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    name = @"testTrigger";
    column1 = WCDB::Column(@"testColumn1");
    column2 = WCDB::Column(@"testColumn2");
    table = @"testTable";
    condition = 1;
    update = WCDB::StatementUpdate().update(table).set(WCDB::Column(@"testColumn")).to(1);
    select = WCDB::StatementSelect().select(1);
    insert = WCDB::StatementInsert().insertIntoTable(table).values(1);
    delete_ = WCDB::StatementDelete().deleteFrom(table);
}

- (void)test_default_constructible
{
    WCDB::StatementCreateTrigger constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::StatementCreateTrigger().getType(), WCDB::SQL::Type::CreateTriggerSTMT);
    XCTAssertEqual(WCDB::StatementCreateTrigger::type, WCDB::SQL::Type::CreateTriggerSTMT);
}

- (void)test_create_trigger
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().delete_().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_temp_trigger
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).temp().before().delete_().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TEMP TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_if_not_exists
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).ifNotExists().before().delete_().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_without_schema
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).before().delete_().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER main.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_after_delete
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).after().delete_().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger AFTER DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_instead_of_delete
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).insteadOf().delete_().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger INSTEAD OF DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_before_insert
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().insert().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE INSERT ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_before_update
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().update().on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_before_update_of_column
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().update().column(column1).on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn1 ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_before_update_of_columns
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().update().column(column1).column(column2).on(table).forEachRow().when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF testColumn1, testColumn2 ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_without_for_each_row
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().delete_().on(table).when(condition).execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_without_condition
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().delete_().on(table).forEachRow().execute(update);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW BEGIN UPDATE main.testTable SET testColumn = 1; END");
}

- (void)test_create_trigger_with_insert
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().delete_().on(table).forEachRow().when(condition).execute(insert);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN INSERT INTO main.testTable VALUES(1); END");
}

- (void)test_create_trigger_with_delete
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().delete_().on(table).forEachRow().when(condition).execute(delete_);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN DELETE FROM main.testTable; END");
}

- (void)test_create_trigger_with_select
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().delete_().on(table).forEachRow().when(condition).execute(select);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN SELECT 1; END");
}

- (void)test_create_trigger_with_multiple_stmts
{
    auto testingSQL = WCDB::StatementCreateTrigger().createTrigger(name).schema(schema).before().delete_().on(table).forEachRow().when(condition).execute(update).execute(insert).execute(delete_).execute(select);

    auto testingTypes = { WCDB::SQL::Type::CreateTriggerSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::InsertSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN 1 BEGIN UPDATE main.testTable SET testColumn = 1; INSERT INTO main.testTable VALUES(1); DELETE FROM main.testTable; SELECT 1; END");
}

@end
