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

@interface OrderTests : BaseTestCase

@end

@implementation OrderTests {
    WCDB::Column column;
}

- (void)setUp
{
    [super setUp];
    column = WCDB::Column(@"testColumn");
}

- (void)test_default_constructible
{
    WCDB::Order constructible;
    WCDB_UNUSED(constructible);
}

- (void)test_asc
{
    auto testingSQL = WCDB::IndexedColumn(column).order(WCDB::Order::ASC);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn ASC");
}

- (void)test_desc
{
    auto testingSQL = WCDB::IndexedColumn(column).order(WCDB::Order::DESC);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn DESC");
}

@end
