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

@interface OrderingTermTests : WINQTestCase

@end

@implementation OrderingTermTests {
    WCDB::Expression expression;
    NSString* collation;
}

- (void)setUp
{
    [super setUp];
    expression = WCDB::Column(@"testColumn");
    collation = @"testCollation";
}

- (void)test_default_constructible
{
    WCDB::OrderingTerm constructible __attribute((unused));
}

- (void)test
{
    auto testingSQL = WCDB::OrderingTerm(expression);

    auto testingTypes = { WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testColumn");
}

- (void)test_with_collation
{
    auto testingSQL = WCDB::OrderingTerm(expression).collate(collation);

    auto testingTypes = { WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testColumn COLLATE testCollation");
}

- (void)test_with_order
{
    auto testingSQL = WCDB::OrderingTerm(expression).order(WCDB::Order::ASC);

    auto testingTypes = { WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testColumn ASC");
}

@end
