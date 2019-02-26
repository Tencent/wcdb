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

@interface SyntaxListTests : BaseTestCase

@end

@implementation SyntaxListTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::SyntaxList<WCDB::Expression> constructible;
    TestCaseAssertTrue(constructible.getDescription().empty());
}

WCDB::SyntaxList<WCDB::Expression> acceptable(const WCDB::SyntaxList<WCDB::Expression>& expressions)
{
    return expressions;
}

- (void)test_single_construct
{
    TestCaseAssertSQLEqual(acceptable(WCDB::Expression(1)), @"1");
}

- (void)test_initial_list_construct
{
    std::initializer_list<WCDB::Expression> expressions = { 1, 2 };
    TestCaseAssertSQLEqual(acceptable(expressions), @"1, 2");
}

- (void)test_list_construct
{
    std::list<WCDB::Expression> expressions = { 1, 2 };
    TestCaseAssertSQLEqual(acceptable(expressions), @"1, 2");
}

- (void)test_construct_convertible
{
    TestCaseAssertSQLEqual(acceptable(1), @"1");
}

- (void)test_initial_list_construct_convertible
{
    std::initializer_list<WCDB::Expression> expressions = { 1, 2 };
    TestCaseAssertSQLEqual(acceptable(expressions), @"1, 2");
}

- (void)test_list_construct_convertible
{
    std::list<WCDB::Expression> expressions = { 1, 2 };
    TestCaseAssertSQLEqual(acceptable(expressions), @"1, 2");
}

- (void)test_syntax_list_construct_convertible
{
    WCDB::SyntaxList<WCDB::LiteralValue> values = { 1, 2 };
    TestCaseAssertSQLEqual(acceptable(values), @"1, 2");
}

@end
