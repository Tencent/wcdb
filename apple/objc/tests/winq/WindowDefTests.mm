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

@interface WindowDefTests : BaseTestCase

@end

@implementation WindowDefTests {
    WCDB::Expressions expressions;
    WCDB::OrderingTerms orderingTerms;
    WCDB::FrameSpec frameSpec;
}

- (void)setUp
{
    [super setUp];
    expressions = {
        1,
        2,
    };
    orderingTerms = {
        1,
        2,
    };
    frameSpec = WCDB::FrameSpec().range().unboundedPreceding();
}

- (void)test_default_constructible
{
    WCDB::WindowDef constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertSQLEqual(constructible, @"");
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::WindowDef().getType(), WCDB::SQL::Type::WindowDef);
    TestCaseAssertEqual(WCDB::WindowDef::type, WCDB::SQL::Type::WindowDef);
}

- (void)test_partition
{
    auto testingSQL = WCDB::WindowDef().partition(expressions);

    auto testingTypes = { WCDB::SQL::Type::WindowDef, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"(PARTITION BY 1, 2)");
}

- (void)test_order
{
    auto testingSQL = WCDB::WindowDef().order(orderingTerms);

    auto testingTypes = { WCDB::SQL::Type::WindowDef, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"(ORDER BY 1, 2)");
}

- (void)test_framespec
{
    auto testingSQL = WCDB::WindowDef().framespec(frameSpec);

    auto testingTypes = { WCDB::SQL::Type::WindowDef, WCDB::SQL::Type::FrameSpec };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"(RANGE UNBOUNDED PRECEDING)");
}

- (void)test_long
{
    auto testingSQL = WCDB::WindowDef().partition(expressions).order(orderingTerms).framespec(frameSpec);

    auto testingTypes = { WCDB::SQL::Type::WindowDef, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::FrameSpec };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"(PARTITION BY 1, 2 ORDER BY 1, 2 RANGE UNBOUNDED PRECEDING)");
}

@end
