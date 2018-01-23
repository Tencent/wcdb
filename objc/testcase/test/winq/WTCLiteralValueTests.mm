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

#import "WTCAssert.h"
#import "WTCBaseTestCase.h"

@interface WTCLiteralValueTests : WTCBaseTestCase

@end

@implementation WTCLiteralValueTests

- (void)testLiteralValue
{
    WCDB::LiteralValue boolLiteralValue = false;
    WINQAssertEqual(boolLiteralValue, @"0");

    WCDB::LiteralValue intLiteralValue = 1;
    WINQAssertEqual(intLiteralValue, @"1");

    WCDB::LiteralValue floatLiteralValue = 0.1;
    WINQAssertEqual(floatLiteralValue, @"0.100000");

    WCDB::LiteralValue stringLiteralValue = "test";
    WINQAssertEqual(stringLiteralValue, @"'test'");

    WCDB::LiteralValue nullLiteralValue = nil;
    WINQAssertEqual(nullLiteralValue, @"NULL");

    WINQAssertEqual(WCDB::LiteralValue(true), @"1");

    WINQAssertEqual(WCDB::LiteralValue(1), @"1");

    int64_t int64 = 17626545782784;
    WINQAssertEqual(WCDB::LiteralValue(int64), @"17626545782784");

    WINQAssertEqual(WCDB::LiteralValue(0.1), @"0.100000");

    WINQAssertEqual(WCDB::LiteralValue("test"), @"'test'");

    std::string str = "test";
    std::vector<unsigned char> data(str.begin(), str.end());
    WINQAssertEqual(WCDB::LiteralValue(data), @"'test'");

    WINQAssertEqual(WCDB::LiteralValue(nullptr), @"NULL");
}

@end
