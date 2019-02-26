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

@interface FrameSpecTests : BaseTestCase

@end

@implementation FrameSpecTests {
    WCDB::Expression expression1;
    WCDB::Expression expression2;
}

- (void)setUp
{
    [super setUp];
    expression1 = 1;
    expression2 = 2;
}

- (void)test_default_constructible
{
    WCDB::FrameSpec constructible;
TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::FrameSpec().getType(), WCDB::SQL::Type::FrameSpec);
    TestCaseAssertEqual(WCDB::FrameSpec::type, WCDB::SQL::Type::FrameSpec);
}

- (void)test_range
{
    auto testingSQL = WCDB::FrameSpec().range().unboundedPreceding();

    auto testingTypes = { WCDB::SQL::Type::FrameSpec };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE UNBOUNDED PRECEDING");
}

- (void)test_rows
{
    auto testingSQL = WCDB::FrameSpec().rows().unboundedPreceding();

    auto testingTypes = { WCDB::SQL::Type::FrameSpec };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"ROWS UNBOUNDED PRECEDING");
}

- (void)test_preceding
{
    auto testingSQL = WCDB::FrameSpec().range().preceding(expression1);

    auto testingTypes = { WCDB::SQL::Type::FrameSpec, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE 1 PRECEDING");
}

- (void)test_current_row
{
    auto testingSQL = WCDB::FrameSpec().range().currentRow();

    auto testingTypes = { WCDB::SQL::Type::FrameSpec };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE CURRENT ROW");
}

- (void)test_following
{
    auto testingSQL = WCDB::FrameSpec().range().following(expression2);

    auto testingTypes = { WCDB::SQL::Type::FrameSpec, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE 2 FOLLOWING");
}

- (void)test_between_unbounded_preceding_and_preceding
{
    auto testingSQL = WCDB::FrameSpec().range().betweenUnboundedPreceding().andPreceding(expression2);

    auto testingTypes = { WCDB::SQL::Type::FrameSpec, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE BETWEEN UNBOUNDED PRECEDING AND 2 PRECEDING");
}

- (void)test_between_preceding_and_preceding
{
    auto testingSQL = WCDB::FrameSpec().range().betweenPreceding(expression1).andPreceding(expression2);

    auto testingTypes = { WCDB::SQL::Type::FrameSpec, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE BETWEEN 1 PRECEDING AND 2 PRECEDING");
}

- (void)test_between_current_row_and_preceding
{
    auto testingSQL = WCDB::FrameSpec().range().betweenCurrentRow().andPreceding(expression2);

    auto testingTypes = { WCDB::SQL::Type::FrameSpec, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE BETWEEN CURRENT ROW AND 2 PRECEDING");
}

- (void)test_between_following_and_preceding
{
    auto testingSQL = WCDB::FrameSpec().range().betweenFollowing(expression1).andPreceding(expression2);

    auto testingTypes = { WCDB::SQL::Type::FrameSpec, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE BETWEEN 1 FOLLOWING AND 2 PRECEDING");
}

- (void)test_between_unbounded_preceding_and_current_row
{
    auto testingSQL = WCDB::FrameSpec().range().betweenUnboundedPreceding().andCurrentRow();

    auto testingTypes = { WCDB::SQL::Type::FrameSpec };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW");
}

- (void)test_between_unbounded_preceding_and_following
{
    auto testingSQL = WCDB::FrameSpec().range().betweenUnboundedPreceding().andFollowing(expression2);

    auto testingTypes = { WCDB::SQL::Type::FrameSpec, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE BETWEEN UNBOUNDED PRECEDING AND 2 FOLLOWING");
}

- (void)test_between_unbounded_preceding_and_unbounded_preceding
{
    auto testingSQL = WCDB::FrameSpec().range().betweenUnboundedPreceding().andUnboundedFollowing();

    auto testingTypes = { WCDB::SQL::Type::FrameSpec };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING");
}

@end
