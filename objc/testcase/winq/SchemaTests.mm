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

@interface SchemaTests : BaseTestCase

@end

@implementation SchemaTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::Schema constructible __attribute((unused));
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::Schema().getType(), WCDB::SQL::Type::Schema);
    TestCaseAssertEqual(WCDB::Schema::type, WCDB::SQL::Type::Schema);
}

- (void)test
{
    auto testingSQL = WCDB::Schema(@"testSchema");

    auto testingTypes = { WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testSchema");
}

- (void)test_default
{
    TestCaseAssertSQLEqual(WCDB::Schema(), @"main");
}

- (void)test_main
{
    TestCaseAssertSQLEqual(WCDB::Schema::main(), @"main");
}

- (void)test_temp
{
    TestCaseAssertSQLEqual(WCDB::Schema::temp(), @"temp");
}

@end
