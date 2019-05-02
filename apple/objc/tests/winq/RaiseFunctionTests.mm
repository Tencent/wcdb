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

@interface RaiseFunctionTests : BaseTestCase

@end

@implementation RaiseFunctionTests {
    NSString* errorMessage;
}

- (void)setUp
{
    [super setUp];
    errorMessage = @"testErrorMessage";
}

- (void)test_default_constructible
{
    WCDB::RaiseFunction constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::RaiseFunction().getType(), WCDB::SQL::Type::RaiseFunction);
    TestCaseAssertEqual(WCDB::RaiseFunction::type, WCDB::SQL::Type::RaiseFunction);
}

- (void)test_ignore
{
    auto testingSQL = WCDB::RaiseFunction().ignore();

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RAISE(IGNORE)");
}

- (void)test_rollback
{
    auto testingSQL = WCDB::RaiseFunction().rollback(errorMessage);

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RAISE(ROLLBACK, testErrorMessage)");
}

- (void)test_abort
{
    auto testingSQL = WCDB::RaiseFunction().abort(errorMessage);

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RAISE(ABORT, testErrorMessage)");
}

- (void)test_fail
{
    auto testingSQL = WCDB::RaiseFunction().fail(errorMessage);

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RAISE(FAIL, testErrorMessage)");
}

- (void)test_raise_function_convertible
{
    WCDB::RaiseFunction sql = WCDB::RaiseFunction().ignore();

    NSString* expected = @"RAISE(IGNORE)";
    TestCaseAssertWINQConvertible(WCDB::Expression, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Expressions, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerm, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerms, sql, expected);
}

@end
