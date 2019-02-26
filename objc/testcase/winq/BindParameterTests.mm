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

@interface BindParameterTests : BaseTestCase

@end

@implementation BindParameterTests {
    NSString* name;
}

- (void)setUp
{
    [super setUp];
    name = @"testName";
}

- (void)test_default_constructible
{
    WCDB::BindParameter constructible;
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::BindParameter().getType(), WCDB::SQL::Type::BindParameter);
    TestCaseAssertEqual(WCDB::BindParameter::type, WCDB::SQL::Type::BindParameter);
}

- (void)test_number
{
    auto testingSQL = WCDB::BindParameter(1);
    auto testingTypes = { WCDB::SQL::Type::BindParameter };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"?1");
}

- (void)test_name
{
    auto testingSQL = WCDB::BindParameter(name);
    auto testingTypes = { WCDB::SQL::Type::BindParameter };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"?testName");
}

- (void)test_colon
{
    auto testingSQL = WCDB::BindParameter::colon(name);
    auto testingTypes = { WCDB::SQL::Type::BindParameter };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @":testName");
}

- (void)test_at
{
    auto testingSQL = WCDB::BindParameter::at(name);
    auto testingTypes = { WCDB::SQL::Type::BindParameter };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"@testName");
}

- (void)test_dollar
{
    auto testingSQL = WCDB::BindParameter::dollar(name);
    auto testingTypes = { WCDB::SQL::Type::BindParameter };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"$testName");
}

- (void)test_numbers
{
    auto bindParameters = WCDB::BindParameter::bindParameters(2);
    TestCaseAssertEqual(bindParameters.size(), 2);
    TestCaseAssertSQLEqual(bindParameters[0], @"?1");
    TestCaseAssertSQLEqual(bindParameters[1], @"?2");
}

@end
