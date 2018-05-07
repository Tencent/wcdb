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

#import "CRUDTestCase.h"

//TODO
@interface HandleTests : CRUDTestCase

@end

@implementation HandleTests

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)test_value_number
{
    WCTValue *value = (WCTValue *) @123;

    XCTAssertTrue([value.stringValue isEqualToString:@"123"]);
    XCTAssertTrue([value.dataValue isEqualToData:[@"123" dataUsingEncoding:NSASCIIStringEncoding]]);
    XCTAssertTrue([value.numberValue isEqualToNumber:@123]);
}

- (void)test_value_string
{
    {
        WCTValue *value = (WCTValue *) @"123";

        XCTAssertTrue([value.stringValue isEqualToString:@"123"]);
        XCTAssertTrue([value.dataValue isEqualToData:[@"123" dataUsingEncoding:NSASCIIStringEncoding]]);
        XCTAssertTrue([value.numberValue isEqualToNumber:@123]);
    }
    {
        NSString *string = @"123.456";

        WCTValue *value = (WCTValue *) string;

        XCTAssertTrue([value.stringValue isEqualToString:@"123.456"]);
        XCTAssertTrue([value.dataValue isEqualToData:[@"123.456" dataUsingEncoding:NSASCIIStringEncoding]]);
        XCTAssertTrue([value.numberValue isEqualToNumber:@123.456]);
    }
    {
        NSString *string = @"not a number";

        WCTValue *value = (WCTValue *) string;

        XCTAssertTrue([value.stringValue isEqualToString:@"not a number"]);
        XCTAssertTrue([value.dataValue isEqualToData:[@"not a number" dataUsingEncoding:NSASCIIStringEncoding]]);
        XCTAssertNil(value.numberValue);
    }
}

- (void)test_value_data
{
    WCTValue *value = (WCTValue *) [@"not utf8 data" dataUsingEncoding:NSUTF32StringEncoding];

    XCTAssertNil(value.stringValue);
    XCTAssertTrue([value.dataValue isEqualToData:[@"not utf8 data" dataUsingEncoding:NSUTF32StringEncoding]]);
    XCTAssertNil(value.numberValue);
}

- (void)test_value_null
{
    WCTValue *value = (WCTValue *) [NSNull null];

    XCTAssertNil(value.stringValue);
    XCTAssertNil(value.numberValue);
    XCTAssertNil(value.dataValue);

    XCTAssertEqual(((NSNumber *) value).intValue, 0);
    XCTAssertEqual(((NSString *) value).length, 0);
    XCTAssertEqual(((NSData *) value).length, 0);
}

@end
