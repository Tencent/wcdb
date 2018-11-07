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

@interface JoinConstraintTests : WINQTestCase

@end

@implementation JoinConstraintTests {
    WCDB::Expression expression;
    WCDB::Columns columns;
}

- (void)setUp
{
    [super setUp];
    expression = 1;
    columns = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
}

- (void)test_default_constructible
{
    WCDB::JoinConstraint constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::JoinConstraint().getType(), WCDB::SQL::Type::JoinConstraint);
    XCTAssertEqual(WCDB::JoinConstraint::type, WCDB::SQL::Type::JoinConstraint);
}

- (void)test_on
{
    auto testingSQL = WCDB::JoinConstraint().on(expression);

    auto testingTypes = { WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON 1");
}

- (void)test_using
{
    auto testingSQL = WCDB::JoinConstraint().usingColumns(columns);

    auto testingTypes = { WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"USING(testColumn1, testColumn2)");
}

@end
