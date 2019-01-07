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

@interface FilterTests : WINQTestCase

@end

@implementation FilterTests {
    WCDB::Expression condition;
}

- (void)setUp
{
    [super setUp];
    condition = 1;
}

- (void)test_default_constructible
{
    WCDB::Filter constructible __attribute((unused));
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::Filter().getType(), WCDB::SQL::Type::Filter);
    TestCaseAssertEqual(WCDB::Filter::type, WCDB::SQL::Type::Filter);
}

- (void)test_filter
{
    auto testingSQL = WCDB::Filter().where(condition);

    auto testingTypes = { WCDB::SQL::Type::Filter, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"FILTER(WHERE 1)");
}

@end
