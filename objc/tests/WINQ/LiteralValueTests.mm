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

@interface LiteralValueTests : WINQTestCase

@end

@implementation LiteralValueTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::LiteralValue constructible __attribute((unused));
}

- (void)test_uint
{
    auto testingSQL = WCDB::LiteralValue(1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_int
{
    auto testingSQL = WCDB::LiteralValue(-1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"-1");
}

- (void)test_ns_number
{
    auto testingSQL = WCDB::LiteralValue(@(1));
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_true
{
    auto testingSQL = WCDB::LiteralValue(true);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_false
{
    auto testingSQL = WCDB::LiteralValue(false);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0");
}

- (void)test_yes
{
    auto testingSQL = WCDB::LiteralValue(YES);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"1");
}

- (void)test_no
{
    auto testingSQL = WCDB::LiteralValue(NO);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0");
}

- (void)test_int32_max
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int32_t>::max());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"2147483647");
}

- (void)test_int32_min
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int32_t>::min());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"-2147483648");
}

- (void)test_int64_max
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int64_t>::max());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"9223372036854775807");
}

- (void)test_int64_min
{
    auto testingSQL = WCDB::LiteralValue(std::numeric_limits<int64_t>::min());
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"-9223372036854775808");
}

- (void)test_float
{
    auto testingSQL = WCDB::LiteralValue((float) 0.1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0.1");
}

- (void)test_double
{
    auto testingSQL = WCDB::LiteralValue((float) 0.1);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"0.1");
}

- (void)test_string
{
    auto testingSQL = WCDB::LiteralValue("test");
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"'test'");
}

- (void)test_cpp_string
{
    auto testingSQL = WCDB::LiteralValue(std::string("test"));
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"'test'");
}

- (void)test_ns_string
{
    auto testingSQL = WCDB::LiteralValue(@"test");
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"'test'");
}

- (void)test_null
{
    auto testingSQL = WCDB::LiteralValue(nullptr);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"NULL");
}

- (void)test_nil
{
    auto testingSQL = WCDB::LiteralValue(nil);
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"NULL");
}

- (void)test_current_time
{
    auto testingSQL = WCDB::LiteralValue::currentTime();
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CURRENT_TIME");
}

- (void)test_current_date
{
    auto testingSQL = WCDB::LiteralValue::currentDate();
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CURRENT_DATE");
}

- (void)test_current_timestamp
{
    auto testingSQL = WCDB::LiteralValue::currentTimestamp();
    auto testingTypes = { WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CURRENT_TIMESTAMP");
}

@end
