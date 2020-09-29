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

@interface StatementCreateVirtualTableTests : BaseTestCase

@end

@implementation StatementCreateVirtualTableTests {
    WCDB::Schema schema;
    NSString* table;
    NSString* module;
    WCDB::StringView argument1;
    WCDB::ColumnDef argument2;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    table = @"testTable";
    module = @"testModule";
    argument1 = WCDB::StatementCreateVirtualTable::tokenize(@"testTokenize");
    argument2 = WCDB::ColumnDef(WCDB::Column(@"testColumn"), WCDB::ColumnType::Integer);
}

- (void)test_default_constructible
{
    WCDB::StatementCreateVirtualTable constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementCreateVirtualTable().getType(), WCDB::SQL::Type::CreateVirtualTableSTMT);
    TestCaseAssertEqual(WCDB::StatementCreateVirtualTable::type, WCDB::SQL::Type::CreateVirtualTableSTMT);
}

- (void)test_create_virtual_table
{
    auto testingSQL = WCDB::StatementCreateVirtualTable().createVirtualTable(table).schema(schema).usingModule(module).argument(argument1);

    auto testingTypes = { WCDB::SQL::Type::CreateVirtualTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE VIRTUAL TABLE testSchema.testTable USING testModule(tokenize = testTokenize)");
}

- (void)test_create_virtual_table_if_not_exists
{
    auto testingSQL = WCDB::StatementCreateVirtualTable().createVirtualTable(table).schema(schema).ifNotExists().usingModule(module).argument(argument1);

    auto testingTypes = { WCDB::SQL::Type::CreateVirtualTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE VIRTUAL TABLE IF NOT EXISTS testSchema.testTable USING testModule(tokenize = testTokenize)");
}

- (void)test_create_virtual_table_without_schema
{
    auto testingSQL = WCDB::StatementCreateVirtualTable().createVirtualTable(table).usingModule(module).argument(argument1);

    auto testingTypes = { WCDB::SQL::Type::CreateVirtualTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE VIRTUAL TABLE main.testTable USING testModule(tokenize = testTokenize)");
}

- (void)test_create_virtual_table_with_arguments
{
    auto testingSQL = WCDB::StatementCreateVirtualTable().createVirtualTable(table).schema(schema).usingModule(module).argument(argument1).argument(argument2);

    auto testingTypes = { WCDB::SQL::Type::CreateVirtualTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE VIRTUAL TABLE testSchema.testTable USING testModule(tokenize = testTokenize, testColumn INTEGER)");
}

- (void)test_create_virtual_table_without_argument
{
    auto testingSQL = WCDB::StatementCreateVirtualTable().createVirtualTable(table).schema(schema).usingModule(module);

    auto testingTypes = { WCDB::SQL::Type::CreateVirtualTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE VIRTUAL TABLE testSchema.testTable USING testModule");
}

@end
