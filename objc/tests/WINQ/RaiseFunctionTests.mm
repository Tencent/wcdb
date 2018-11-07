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

@interface RaiseFunctionTests : WINQTestCase

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
    WCDB::RaiseFunction constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::RaiseFunction().getType(), WCDB::SQL::Type::RaiseFunction);
    XCTAssertEqual(WCDB::RaiseFunction::type, WCDB::SQL::Type::RaiseFunction);
}

- (void)test_ignore
{
    auto testingSQL = WCDB::RaiseFunction().ignore();

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"RAISE(IGNORE)");
}

- (void)test_rollback
{
    auto testingSQL = WCDB::RaiseFunction().rollback(errorMessage);

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"RAISE(ROLLBACK, testErrorMessage)");
}

- (void)test_abort
{
    auto testingSQL = WCDB::RaiseFunction().abort(errorMessage);

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"RAISE(ABORT, testErrorMessage)");
}

- (void)test_fail
{
    auto testingSQL = WCDB::RaiseFunction().fail(errorMessage);

    auto testingTypes = { WCDB::SQL::Type::RaiseFunction };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"RAISE(FAIL, testErrorMessage)");
}

- (void)test_raise_function_convertible
{
    WCDB::RaiseFunction sql = WCDB::RaiseFunction().ignore();

    NSString* expected = @"RAISE(IGNORE)";
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

@end
