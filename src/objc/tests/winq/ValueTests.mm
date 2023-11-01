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
#if TEST_WCDB_OBJC
#import <WCDBOBjc/Value.hpp>
#elif TEST_WCDB_CPP
#import <WCDBCpp/Value.hpp>
#else
#import <WCDB/Value.hpp>
#endif

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

- (void)test_shared_ptr_interger
{
    WCDB::Value value(std::make_shared<int>(0));
    TestCaseAssertTrue(value.intValue() == 0);
    TestCaseAssertTrue((int) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value.isNull());

    value = std::make_shared<int>(1);
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((int) value == 1);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1");

    value = std::shared_ptr<int>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_shared_ptr_float
{
    WCDB::Value value(std::make_shared<float>(0));
    TestCaseAssertTrue(value.floatValue() == 0);
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());

    value = std::make_shared<double>(1.2);
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1.2");

    value = std::shared_ptr<float>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_shared_ptr_string
{
    WCDB::Value value(std::make_shared<std::string>(""));
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);

    value = std::make_shared<std::string>("1.2");
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");

    value = std::shared_ptr<WCDB::StringView>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_shared_ptr_blob
{
    NSData* nsData = [@"" dataUsingEncoding:NSUTF8StringEncoding];
    WCDB::Value value(std::make_shared<WCDB::UnsafeData>((unsigned char*) nsData.bytes, nsData.length));
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value);

    NSData* nsData2 = [@"1.2" dataUsingEncoding:NSUTF8StringEncoding];
    value = std::make_shared<WCDB::UnsafeData>((unsigned char*) nsData2.bytes, nsData2.length);
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");

    value = std::shared_ptr<WCDB::UnsafeData>();
    TestCaseAssertTrue(value.isNull());
}

#if defined(__cplusplus) && __cplusplus > 201402L
- (void)test_std_optional_interger
{
    WCDB::Value value(std::make_optional<int>(0));
    TestCaseAssertTrue(value.intValue() == 0);
    TestCaseAssertTrue((int) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value.isNull());

    value = std::make_optional<int>(1);
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((int) value == 1);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1");

    value = std::optional<int>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_std_optional_float
{
    WCDB::Value value(std::make_optional<float>(0));
    TestCaseAssertTrue(value.floatValue() == 0);
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());

    value = std::make_optional<double>(1.2);
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1.2");

    value = std::optional<float>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_std_optional_string
{
    WCDB::Value value(std::make_optional<std::string>(""));
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);

    value = std::make_optional<std::string>("1.2");
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");

    value = std::optional<WCDB::StringView>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_std_optional_blob
{
    NSData* nsData = [@"" dataUsingEncoding:NSUTF8StringEncoding];
    WCDB::Value value(std::make_optional<WCDB::UnsafeData>((unsigned char*) nsData.bytes, nsData.length));
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value);

    NSData* nsData2 = [@"1.2" dataUsingEncoding:NSUTF8StringEncoding];
    value = std::make_optional<WCDB::UnsafeData>((unsigned char*) nsData2.bytes, nsData2.length);
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");

    value = std::optional<WCDB::UnsafeData>();
    TestCaseAssertTrue(value.isNull());
}
#endif

- (void)test_wcdb_optional_interger
{
    WCDB::Value value(WCDB::Optional<int>(0));
    TestCaseAssertTrue(value.intValue() == 0);
    TestCaseAssertTrue((int) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value.isNull());

    value = WCDB::Optional<int>(1);
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((int) value == 1);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1");

    value = WCDB::Optional<int>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_wcdb_optional_float
{
    WCDB::Value value(WCDB::Optional<float>(0));
    TestCaseAssertTrue(value.floatValue() == 0);
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);
    TestCaseAssertTrue(value.isEmpty());

    value = WCDB::Optional<double>(1.2);
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"1.2");

    value = WCDB::Optional<float>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_wcdb_optional_string
{
    WCDB::Value value(WCDB::Optional<std::string>(""));
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertTrue((double) value == 0);
    TestCaseAssertFalse(value);

    value = WCDB::Optional<std::string>("1.2");
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");

    value = WCDB::Optional<WCDB::StringView>();
    TestCaseAssertTrue(value.isNull());
}

- (void)test_wcdb_optional_blob
{
    NSData* nsData = [@"" dataUsingEncoding:NSUTF8StringEncoding];
    WCDB::Optional<WCDB::UnsafeData> data = WCDB::UnsafeData((unsigned char*) nsData.bytes, nsData.length);
    WCDB::Value value(data);
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertTrue(value.isEmpty());
    TestCaseAssertFalse(value);

    NSData* nsData2 = [@"1.2" dataUsingEncoding:NSUTF8StringEncoding];
    data = WCDB::UnsafeData((unsigned char*) nsData2.bytes, nsData2.length);
    value = WCDB::Optional<WCDB::UnsafeData>(data);
    TestCaseAssertStringEqual([NSString stringWithUTF8String:value.textValue().data()], @"1.2");
    TestCaseAssertFalse(value.isNull());
    TestCaseAssertFalse(value.isEmpty());
    TestCaseAssertTrue((double) value == 1.2);
    TestCaseAssertTrue(value);

    WCDB::LiteralValue literal = value;
    TestCaseAssertSQLEqual(literal, @"'1.2'");

    value = WCDB::Optional<WCDB::UnsafeData>();
    TestCaseAssertTrue(value.isNull());
}

@end
