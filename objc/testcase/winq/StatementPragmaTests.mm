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

@interface StatementPragmaTests : BaseTestCase

@end

@implementation StatementPragmaTests {
    WCDB::Pragma pragma;
    WCDB::Schema schema;
    WCDB::LiteralValue value;
}

- (void)setUp
{
    [super setUp];
    pragma = @"testPragma";
    schema = @"testSchema";
    value = 1;
}

- (void)test_default_constructible
{
    WCDB::StatementPragma constructible;
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementPragma().getType(), WCDB::SQL::Type::PragmaSTMT);
    TestCaseAssertEqual(WCDB::StatementPragma::type, WCDB::SQL::Type::PragmaSTMT);
}

- (void)test_pragma
{
    auto testingSQL = WCDB::StatementPragma().pragma(pragma).schema(schema);

    auto testingTypes = { WCDB::SQL::Type::PragmaSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Pragma };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"PRAGMA testSchema.testPragma");
}

- (void)test_pragma_to
{
    auto testingSQL = WCDB::StatementPragma().pragma(pragma).schema(schema).to(value);

    auto testingTypes = { WCDB::SQL::Type::PragmaSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Pragma, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"PRAGMA testSchema.testPragma = 1");
}

- (void)test_pragma_to_without_schema
{
    auto testingSQL = WCDB::StatementPragma().pragma(pragma).to(value);

    auto testingTypes = { WCDB::SQL::Type::PragmaSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Pragma, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"PRAGMA main.testPragma = 1");
}

- (void)test_pragma_with
{
    auto testingSQL = WCDB::StatementPragma().pragma(pragma).schema(schema).with(value);

    auto testingTypes = { WCDB::SQL::Type::PragmaSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Pragma, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"PRAGMA testSchema.testPragma(1)");
}

@end
