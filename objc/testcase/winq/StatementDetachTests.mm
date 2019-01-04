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

@interface StatementDetachTests : WINQTestCase

@end

@implementation StatementDetachTests {
    WCDB::Schema schema;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
}

- (void)test_default_constructible
{
    WCDB::StatementDetach constructible __attribute((unused));
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementDetach().getType(), WCDB::SQL::Type::DetachSTMT);
    TestCaseAssertEqual(WCDB::StatementDetach::type, WCDB::SQL::Type::DetachSTMT);
}

- (void)test_detach
{
    auto testingSQL = WCDB::StatementDetach().detach(schema);

    auto testingTypes = { WCDB::SQL::Type::DetachSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"DETACH testSchema");
}

@end
