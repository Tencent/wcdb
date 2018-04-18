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

    _digitObject = [[FTSTestCaseObject alloc] initWithMessage:@"abc123efg456" andExtension:@"digit"];

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
    XCTAssertEqual(objects.count, 2);
}

- (void)searching_offset_test:(NSString *)keyword
         expectedColumnNumber:(int)expectedColumnNumber
                   termNumber:(int)expectedtermNumber
                       string:(NSString *)expectedString
                       object:(FTSTestCaseObject *)object
{
    WCTOneColumn *column = [_database getColumnOnResult:FTSTestCaseObject.ColumnNamed(_tableName).offsets() fromTable:_tableName where:FTSTestCaseObject.ColumnNamed(_tableName).match(keyword.UTF8String)];
    XCTAssertEqual(column.count, 1);

    NSArray<NSString *> *components = [column[0].stringValue componentsSeparatedByString:@" "];
    XCTAssertEqual(components.count, 4);

    int columnNumber = components[0].intValue;
    XCTAssertEqual(columnNumber, expectedColumnNumber);

    int termNumber = components[1].intValue;
    XCTAssertEqual(termNumber, expectedtermNumber);

    int offset = components[2].intValue;
    int size = components[3].intValue;

    NSString *expectedFullString;
    switch (expectedColumnNumber) {
        case 0:
            expectedFullString = object.message;
            break;
        case 1:
            expectedFullString = object.extension;
            break;
        default:
            XCTFail(@"");
            break;
    }

    NSData *data = [[expectedFullString dataUsingEncoding:NSUTF8StringEncoding] subdataWithRange:NSMakeRange(offset, size)];
    NSString *matchedString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    XCTAssertTrue([matchedString compare:expectedString options:NSCaseInsensitiveSearch] == NSOrderedSame);
}

- (void)test_offset_chinese
{
    [self searching_offset_test:@"果树"
           expectedColumnNumber:0
                     termNumber:0
                         string:@"果树"
                         object:_chineseObject];
}

- (void)test_offset_english
{
    [self searching_offset_test:@"framework*"
           expectedColumnNumber:0
                     termNumber:0
                         string:@"framework"
                         object:_englishObject];
}

- (void)test_offset_multiple_terms
{
    [self searching_offset_test:@"数字 OR DIGIT"
           expectedColumnNumber:1
                     termNumber:1
                         string:@"DIGIT"
                         object:_digitObject];
}

- (void)test_error_prone
{
    //TODO
    FTSTestCaseObject *errorProneObject1 = [[FTSTestCaseObject alloc] initWithMessage:@"error prone: benefit"];
    FTSTestCaseObject *errorProneObject2 = [[FTSTestCaseObject alloc] initWithMessage:@"error prone: island"];
    NSArray<FTSTestCaseObject *> *objects = @[ errorProneObject1, errorProneObject2 ];
    XCTAssertTrue([_database insertObjects:objects intoTable:_tableName]);

    [self searching_test_failed:"is"];
    [self searching_test:"is*" expectedObject:errorProneObject1];
    [self searching_test_failed:"be"];
    [self searching_test:"be*" expectedObject:errorProneObject2];
    [self searching_test_failed:"are"];
    [self searching_test_failed:"are*"];
}

@end
