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

@interface LiteralValueTests : WINQTestCase

@end

@implementation LiteralValueTests

- (void)testLiteralValue
{
    int numericLiteral = 1;
    float floatLitearl = 2.1;
    std::string stringLiteral = "3";
    std::vector<unsigned char> blobLiteral;
    blobLiteral.push_back('4');

    WINQAssertEqual(WCDB::LiteralValue(numericLiteral), @"1");
    WINQAssertEqual(WCDB::LiteralValue(floatLitearl), @"2.1");
    WINQAssertEqual(WCDB::LiteralValue(stringLiteral), @"'3'");
    WINQAssertEqual(WCDB::LiteralValue(blobLiteral), @"'4'");

    WINQAssertEqual(WCDB::LiteralValue("3"), @"'3'");
    WINQAssertEqual(WCDB::LiteralValue(nullptr), @"NULL");

    WINQAssertEqual(WCDB::LiteralValue::CurrentTime, @"CURRENT_TIME");
    WINQAssertEqual(WCDB::LiteralValue::CurrentDate, @"CURRENT_DATE");
    WINQAssertEqual(WCDB::LiteralValue::CurrentTimestamp, @"CURRENT_TIMESTAMP");
}

- (void)testAnitInjection
{
    WINQAssertEqual(WCDB::LiteralValue("'SELECT'"), @"'''SELECT'''")
}

@end
