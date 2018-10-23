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

@interface FunctionInvocationTests : WINQTestCase

@end

@implementation FunctionInvocationTests {
    NSString* function;
    WCDB::Expressions expressions;
}

- (void)setUp
{
    [super setUp];
    function = @"testFunction";
    expressions = {
        1,
        2,
    };
}

- (void)test_default_constructible
{
    WCDB::FunctionInvocation constructible __attribute((unused));
}

- (void)test_function
{
    auto testingSQL = WCDB::FunctionInvocation(function).distinct().invoke(expressions);

    auto testingTypes = { WCDB::SQL::Type::FunctionInvocation, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testFunction(DISTINCT 1, 2)");
}

- (void)test_function_without_distinct
{
    auto testingSQL = WCDB::FunctionInvocation(function).invoke(expressions);

    auto testingTypes = { WCDB::SQL::Type::FunctionInvocation, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testFunction(1, 2)");
}

- (void)test_function_without_parameters
{
    auto testingSQL = WCDB::FunctionInvocation(function).invoke();

    auto testingTypes = { WCDB::SQL::Type::FunctionInvocation };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testFunction()");
}

- (void)test_function_all
{
    auto testingSQL = WCDB::FunctionInvocation(function).invokeAll();

    auto testingTypes = { WCDB::SQL::Type::FunctionInvocation };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testFunction(*)");
}

@end
