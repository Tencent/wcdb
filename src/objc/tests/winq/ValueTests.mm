//
// Created by qiuwenchen on 2022/8/12.
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

#import "TestCase.h"
#import "WINQAssertion.h"
#import <Foundation/Foundation.h>
#import <WCDB/Value.hpp>

@interface ValueTests : BaseTestCase

@end

@implementation ValueTests

- (void)test_null
{
    WCDB::Value value;
    TestCaseAssertTrue(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertTrue((int) value == 0);
    TestCaseAssertFalse(value);

    value = nullptr;
    TestCaseAssertTrue(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertTrue((int) value == 0);
    TestCaseAssertFalse(value);

    value = WCDB::Value(nullptr);
    TestCaseAssertTrue(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertTrue((int) value == 0);
    TestCaseAssertFalse(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"NULL");
}

- (void)test_interger
{
    WCDB::Value value(0);
    TestCaseAssertTrue(value.intValue() == 0);
    TestCaseAssertTrue((int) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value.isNull());

    value = 1;
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((int) value == 1);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1");
}

- (void)test_float
{
    WCDB::Value value(0);
    TestCaseAssertTrue(value.floatValue() == 0);
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());

    value = 1.2;
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1.2");
}

- (void)test_string
{
    WCDB::Value value("");
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);

    value = "1.2";
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");
}

- (void)test_blob
{
    NSData* nsData = [@"" dataUsingEncoding:NSUTF8StringEncoding];
    WCDB::Value value(WCDB::UnsafeData((unsigned char*) nsData.bytes, nsData.length));
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value);

    NSData* nsData2 = [@"1.2" dataUsingEncoding:NSUTF8StringEncoding];
    value = WCDB::UnsafeData((unsigned char*) nsData2.bytes, nsData2.length);
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");
}

@end
