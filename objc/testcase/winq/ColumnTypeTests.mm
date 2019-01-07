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

@interface ColumnTypeTests : BaseTestCase

@end

@implementation ColumnTypeTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::ColumnType constructible __attribute((unused));
}

- (void)test_type
{
    TestCaseAssertWINQEnumEqual(WCDB::ColumnType::Integer32, @"INTEGER");
    TestCaseAssertWINQEnumEqual(WCDB::ColumnType::Integer64, @"INTEGER");
    TestCaseAssertWINQEnumEqual(WCDB::ColumnType::Text, @"TEXT");
    TestCaseAssertWINQEnumEqual(WCDB::ColumnType::BLOB, @"BLOB");
    TestCaseAssertWINQEnumEqual(WCDB::ColumnType::Float, @"REAL");
    TestCaseAssertWINQEnumEqual(WCDB::ColumnType::Null, @"NULL");
}

@end
