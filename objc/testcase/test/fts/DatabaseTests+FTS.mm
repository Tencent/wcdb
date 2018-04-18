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

#import "FTSTestCaseObject+WCTTableCoding.h"
#import "FTSTestCaseObject.h"
#import "TestCase.h"

@interface DatabaseTests_FTS : TestCase

@end

@implementation DatabaseTests_FTS {
    WCTDatabase *_database;
    FTSTestCaseObject *_chineseObject;
    FTSTestCaseObject *_englishObject;
    FTSTestCaseObject *_digitObject;
    NSString *_tableName;
    Class _cls;
}

- (void)setUp
{
    [super setUp];

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _tableName = self.className;

    _cls = FTSTestCaseObject.class;

    _chineseObject = [[FTSTestCaseObject alloc] initWithMessage:@"苹果树" andExtension:@"WeChat"];

    _englishObject = [[FTSTestCaseObject alloc] initWithMessage:@"WCDB is a cross-platform database framework developed by WeChat."];

    _digitObject = [[FTSTestCaseObject alloc] initWithMessage:@"abc123efg456" andExtension:@"WeChat"];

    [_database setTokenizer:WCTTokenizer.name];

    XCTAssertTrue([_database createVirtualTable:_tableName withClass:_cls]);

    NSArray<FTSTestCaseObject *> *objects = @[ _chineseObject, _englishObject, _digitObject ];

    XCTAssertTrue([_database insertObjects:objects intoTable:_tableName]);
}

- (void)tearDown
{
    XCTAssertTrue([_database dropTable:_tableName]);

    [_database close:^{
      XCTAssertTrue([_database removeFiles]);
    }];

    _database = nil;

    [super tearDown];
}

- (void)searching_test:(const char *)keyword
        expectedObject:(FTSTestCaseObject *)expected
{
    NSArray<FTSTestCaseObject *> *objects = [_database getObjectsOfClass:_cls fromTable:_tableName where:FTSTestCaseObject.message.match(keyword)];
    XCTAssertEqual(objects.count, 1);
    XCTAssertTrue([objects[0] isEqualToObject:expected]);
}

- (void)searching_test_failed:(const char *)keyword
{
    NSArray<FTSTestCaseObject *> *objects = [_database getObjectsOfClass:_cls fromTable:_tableName where:FTSTestCaseObject.message.match(keyword)];
    XCTAssertNotNil(objects);
    XCTAssertEqual(objects.count, 0);
}

- (void)test_search_chinese
{
    [self searching_test:"苹" expectedObject:_chineseObject];
    [self searching_test:"苹果" expectedObject:_chineseObject];
    [self searching_test:"苹果树" expectedObject:_chineseObject];
    [self searching_test:"果" expectedObject:_chineseObject];
    [self searching_test:"果树" expectedObject:_chineseObject];
    [self searching_test:"树" expectedObject:_chineseObject];
    [self searching_test_failed:"苹树"];
}

- (void)test_english
{
    [self searching_test:"WeChat" expectedObject:_englishObject];
    [self searching_test_failed:"We Chat"];
}

- (void)test_english_case_insensive
{
    [self searching_test:"DATABASE" expectedObject:_englishObject];
}

- (void)test_english_lemmatization
{
    //"is" can be matched
    [self searching_test:"are" expectedObject:_englishObject];
    [self searching_test:"are*" expectedObject:_englishObject];
}

- (void)test_digit
{
    [self searching_test:"123" expectedObject:_digitObject];
    [self searching_test:"45*" expectedObject:_digitObject];
    [self searching_test_failed:"7*"];
}

- (void)test_table_search
{
    NSArray<FTSTestCaseObject *> *objects = [_database getObjectsOfClass:_cls fromTable:_tableName where:FTSTestCaseObject.ColumnNamed(_tableName).match("WeChat")];
    XCTAssertEqual(objects.count, 3);
}

@end
