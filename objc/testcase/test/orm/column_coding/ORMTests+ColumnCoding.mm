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

#import "ORMColumnCoding+WCTTableCoding.h"
#import "ORMColumnCoding.h"
#import "TestCase.h"

@interface ORMTests_ColumnCoding : TestCase

@end

@implementation ORMTests_ColumnCoding {
    WCTDatabase *_database;
    NSString *_tableName;
    Class _cls;
}

- (void)setUp
{
    [super setUp];

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _tableName = self.className;

    _cls = ORMColumnCoding.class;

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:_cls]);
}

- (void)tearDown
{
    XCTAssertTrue([_database dropTable:_tableName]);

    [_database close:^{
      XCTAssertTrue([_database removeFiles]);
    }];

    _database = nil;

    _tableName = nil;

    _cls = nil;

    [super tearDown];
}

- (void)test_upper_bound
{
    ORMColumnCoding *object = [[ORMColumnCoding alloc] init];
    object.int32Value = std::numeric_limits<int32_t>::max();
    object.int64Value = std::numeric_limits<int64_t>::max();
    object.uint32Value = std::numeric_limits<uint32_t>::max();
    object.uint64Value = std::numeric_limits<uint64_t>::max();
    object.floatValue = std::numeric_limits<float>::max();
    object.doubleValue = std::numeric_limits<double>::max();
    object.dateValue = [NSDate distantFuture];
    object.numberValue = [NSNumber numberWithUnsignedLongLong:std::numeric_limits<uint64_t>::max()];
    XCTAssertTrue([_database insertObject:object intoTable:_tableName]);
    ORMColumnCoding *result = [_database getObjectOfClass:_cls fromTable:_tableName];
    XCTAssertTrue([object isEqualToObject:result]);
}

- (void)test_lower_bound
{
    ORMColumnCoding *object = [[ORMColumnCoding alloc] init];
    object.int32Value = std::numeric_limits<int32_t>::min();
    object.int64Value = std::numeric_limits<int64_t>::min();
    object.uint32Value = std::numeric_limits<uint32_t>::min();
    object.uint64Value = std::numeric_limits<uint64_t>::min();
    object.floatValue = std::numeric_limits<float>::min();
    object.doubleValue = std::numeric_limits<double>::min();
    object.dateValue = [NSDate distantPast];
    object.numberValue = [NSNumber numberWithUnsignedLongLong:std::numeric_limits<uint64_t>::min()];
    XCTAssertTrue([_database insertObject:object intoTable:_tableName]);
    ORMColumnCoding *result = [_database getObjectOfClass:_cls fromTable:_tableName];
    XCTAssertTrue([object isEqualToObject:result]);
}

- (void)test_empty
{
    ORMColumnCoding *object = [[ORMColumnCoding alloc] init];
    object.int32Value = 0;
    object.int64Value = 0;
    object.uint32Value = 0;
    object.uint64Value = 0;
    object.floatValue = 0;
    object.doubleValue = 0;
    object.dateValue = [NSDate date];
    object.numberValue = [NSNumber numberWithInt:0];
    object.dataValue = [@"" dataUsingEncoding:NSUTF8StringEncoding];
    object.stringValue = @"";
    object.arrayValue = [NSArray array];
    XCTAssertTrue([_database insertObject:object intoTable:_tableName]);
    ORMColumnCoding *result = [_database getObjectOfClass:_cls fromTable:_tableName];
    XCTAssertTrue([object isEqualToObject:result]);
}

- (void)test_nil
{
    ORMColumnCoding *object = [[ORMColumnCoding alloc] init];
    object.dateValue = nil;
    object.numberValue = nil;
    object.dataValue = nil;
    object.stringValue = nil;
    object.arrayValue = nil;
    XCTAssertTrue([_database insertObject:object intoTable:_tableName]);
    ORMColumnCoding *result = [_database getObjectOfClass:_cls fromTable:_tableName];
    XCTAssertTrue([object isEqualToObject:result]);
}

@end
