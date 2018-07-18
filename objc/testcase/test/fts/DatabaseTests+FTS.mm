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
#import "TestCaseCommon.h"

@interface DatabaseTests_FTS : TestCase

@end

@implementation DatabaseTests_FTS {
    WCTDatabase *_database;
    FTSTestCaseObject *_chineseObject;
    FTSTestCaseObject *_englishObject;
    FTSTestCaseObject *_digitObject;
    NSString *_tableName;
    WCDB::Column _tableColumn;
    Class _cls;
}

- (void)setUp
{
    [super setUp];

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _tableName = self.className;

    _cls = FTSTestCaseObject.class;

    _tableColumn = FTSTestCaseObject.columnNamed(_tableName);

    [_database setTokenizer:WCTTokenizerWCDB];

    XCTAssertTrue([_database createVirtualTable:_tableName withClass:_cls]);
}

- (void)tearDown
{
    XCTAssertTrue([_database dropTable:_tableName]);

    XCTAssertTrue([_database removeFiles]);

    _database = nil;

    [super tearDown];
}

- (BOOL)expectObject:(FTSTestCaseObject *)expected
       withCondition:(const WCDB::Expression &)condition
{
    NSArray<FTSTestCaseObject *> *objects = [_database getObjectsOfClass:_cls fromTable:_tableName where:condition];
    return objects.count == 1 && [objects[0] isEqualToObject:expected];
}

- (BOOL)expectNilWithCondition:(const WCDB::Expression)condition
{
    NSArray<FTSTestCaseObject *> *objects = [_database getObjectsOfClass:_cls fromTable:_tableName where:condition];
    return objects != nil && objects.count == 0;
}

- (BOOL)expectObjects:(NSArray<FTSTestCaseObject *> *)expected
        withCondition:(const WCDB::Expression &)condition
{
    NSArray<FTSTestCaseObject *> *objects = [_database getObjectsOfClass:_cls fromTable:_tableName where:condition];
    if (objects.count != expected.count) {
        return NO;
    }
    expected = expected.sorted;
    objects = objects.sorted;
    for (NSUInteger i = 0; i < expected.count; ++i) {
        if (![expected[i] isEqualToObject:objects[i]]) {
            return NO;
        }
    }
    return YES;
}

- (BOOL)expectObject:(FTSTestCaseObject *)expected
whileSearchingByKeyword:(const char *)keyword
{
    return [self expectObject:expected withCondition:_tableColumn.match(keyword)];
}

- (BOOL)expectNilWhileSearchingByKeyword:(const char *)keyword
{
    return [self expectNilWithCondition:_tableColumn.match(keyword)];
}

- (BOOL)expectObjects:(NSArray<FTSTestCaseObject *> *)expected
whileSearchingByKeyword:(const char *)keyword
{
    return [self expectObjects:expected withCondition:_tableColumn.match(keyword)];
}

- (BOOL)insertDummyObject
{
    FTSTestCaseObject *dummy = [[FTSTestCaseObject alloc] initWithMessage:@"dummy" andExtension:@"dummy"];
    return [_database insertObject:dummy intoTable:_tableName];
}

- (void)test_chinese
{
    FTSTestCaseObject *chinese = [[FTSTestCaseObject alloc] initWithMessage:@"苹果树"];
    XCTAssertTrue([_database insertObject:chinese intoTable:_tableName]);

    XCTAssertTrue([self insertDummyObject]);

    XCTAssertTrue([self expectObject:chinese whileSearchingByKeyword:"苹"]);
    XCTAssertTrue([self expectObject:chinese whileSearchingByKeyword:"苹果"]);
    XCTAssertTrue([self expectObject:chinese whileSearchingByKeyword:"苹果树"]);
    XCTAssertTrue([self expectObject:chinese whileSearchingByKeyword:"果树"]);
    XCTAssertTrue([self expectObject:chinese whileSearchingByKeyword:"树"]);
    XCTAssertTrue([self expectNilWhileSearchingByKeyword:"苹树"]);
}

- (void)test_english
{
    FTSTestCaseObject *english = [[FTSTestCaseObject alloc] initWithMessage:@"WCDB is a cross-platform database framework developed by WeChat."];
    XCTAssertTrue([_database insertObject:english intoTable:_tableName]);

    XCTAssertTrue([self insertDummyObject]);

    XCTAssertTrue([self expectObject:english whileSearchingByKeyword:"WCDB"]);
    XCTAssertTrue([self expectObject:english whileSearchingByKeyword:"WeChat"]);
    //case insensive
    XCTAssertTrue([self expectObject:english whileSearchingByKeyword:"DATABASE"]);
    XCTAssertTrue([self expectNilWhileSearchingByKeyword:"Hello"]);
}

- (void)test_number
{
    FTSTestCaseObject *number = [[FTSTestCaseObject alloc] initWithMessage:@"123 456 789"];
    XCTAssertTrue([_database insertObject:number intoTable:_tableName]);

    XCTAssertTrue([self insertDummyObject]);

    XCTAssertTrue([self expectObject:number whileSearchingByKeyword:"123"]);
    XCTAssertTrue([self expectObject:number whileSearchingByKeyword:"456"]);
    XCTAssertTrue([self expectObject:number whileSearchingByKeyword:"789"]);
    XCTAssertTrue([self expectNilWhileSearchingByKeyword:"567"]);
}

- (void)test_stemming
{
    FTSTestCaseObject *developer = [[FTSTestCaseObject alloc] initWithMessage:@"developer"];
    XCTAssertTrue([_database insertObject:developer intoTable:_tableName]);

    XCTAssertTrue([self insertDummyObject]);

    XCTAssertTrue([self expectObject:developer whileSearchingByKeyword:"developed"]);
}

- (void)test_mixed
{
    FTSTestCaseObject *mixed = [[FTSTestCaseObject alloc] initWithMessage:@"中文English日本語123"];
    XCTAssertTrue([_database insertObject:mixed intoTable:_tableName]);
    XCTAssertTrue([self insertDummyObject]);
    XCTAssertTrue([self expectObject:mixed whileSearchingByKeyword:"中"]);
    XCTAssertTrue([self expectObject:mixed whileSearchingByKeyword:"English"]);
    XCTAssertTrue([self expectObject:mixed whileSearchingByKeyword:"語"]);
    XCTAssertTrue([self expectObject:mixed whileSearchingByKeyword:"123"]);
}

- (void)test_column_match
{
    FTSTestCaseObject *message = [[FTSTestCaseObject alloc] initWithMessage:@"error_prone"];
    FTSTestCaseObject *extension = [[FTSTestCaseObject alloc] initWithExtension:@"error_prone"];
    NSArray<FTSTestCaseObject *> *objects = @[ message, extension ];
    XCTAssertTrue([_database insertObjects:objects intoTable:_tableName]);

    XCTAssertTrue([self expectObject:message withCondition:FTSTestCaseObject.message.match("error_prone")]);
    XCTAssertTrue([self expectObject:extension withCondition:FTSTestCaseObject.extension.match("error_prone")]);
    XCTAssertTrue([self expectObjects:objects whileSearchingByKeyword:"error_prone"]);

    XCTAssertTrue([self expectObject:message whileSearchingByKeyword:"message: error_prone"]);
    XCTAssertTrue([self expectObject:extension whileSearchingByKeyword:"extension: error_prone"]);
    XCTAssertTrue([self expectNilWhileSearchingByKeyword:"empty: error_prone"]);
}

- (void)test_token_prefix_queries
{
    FTSTestCaseObject *tokenPrefix = [[FTSTestCaseObject alloc] initWithMessage:@"token prefix queries"];
    XCTAssertTrue([_database insertObject:tokenPrefix intoTable:_tableName]);
    XCTAssertTrue([self insertDummyObject]);

    XCTAssertTrue([self expectObject:tokenPrefix whileSearchingByKeyword:"tok*"]);
    XCTAssertTrue([self expectNilWhileSearchingByKeyword:"ok*"]);
}

- (void)test_phrase_queries
{
    FTSTestCaseObject *phraseQueries = [[FTSTestCaseObject alloc] initWithMessage:@"prefix phrase queries subfix"];
    XCTAssertTrue([_database insertObject:phraseQueries intoTable:_tableName]);
    XCTAssertTrue([self insertDummyObject]);

    XCTAssertTrue([self expectObject:phraseQueries whileSearchingByKeyword:"phrase subfix"]);
    XCTAssertTrue([self expectNilWhileSearchingByKeyword:"\"phrase subfix\""]);
    XCTAssertTrue([self expectObject:phraseQueries whileSearchingByKeyword:"\"phrase queries\""]);
}

- (void)test_near_queries
{
    FTSTestCaseObject *objectWithOneGaps = [[FTSTestCaseObject alloc] initWithMessage:@"Cowherd one WeaverGirl"];
    FTSTestCaseObject *objectWithTenGaps = [[FTSTestCaseObject alloc] initWithMessage:@"Cowherd one two three four five six seven eight nine ten WeaverGirl"];
    FTSTestCaseObject *objectWithEleventGaps = [[FTSTestCaseObject alloc] initWithMessage:@"Cowherd one two three four five six seven eight nine ten eleven WeaverGirl"];
    NSArray<FTSTestCaseObject *> *objects = @[ objectWithOneGaps, objectWithTenGaps, objectWithEleventGaps ];
    XCTAssertTrue([_database insertObjects:objects intoTable:_tableName]);

    NSArray<FTSTestCaseObject *> *objectsLessThanTenGaps = @[ objectWithOneGaps, objectWithTenGaps ];

    XCTAssertTrue([self expectObject:objectWithOneGaps whileSearchingByKeyword:"Cowherd NEAR/1 WeaverGirl"]);
    XCTAssertTrue([self expectObjects:objectsLessThanTenGaps whileSearchingByKeyword:"Cowherd NEAR/10 WeaverGirl"]);
    XCTAssertTrue([self expectObjects:objects whileSearchingByKeyword:"Cowherd NEAR/11 WeaverGirl"]);

    XCTAssertTrue([self expectObjects:objectsLessThanTenGaps whileSearchingByKeyword:"Cowherd NEAR WeaverGirl"]);
}

- (void)test_operation
{
    FTSTestCaseObject *AB = [[FTSTestCaseObject alloc] initWithMessage:@"A B"];
    FTSTestCaseObject *BC = [[FTSTestCaseObject alloc] initWithMessage:@"B C"];
    FTSTestCaseObject *CD = [[FTSTestCaseObject alloc] initWithMessage:@"C D"];

    NSArray<FTSTestCaseObject *> *objects = @[ AB, BC, CD ];
    XCTAssertTrue([_database insertObjects:objects intoTable:_tableName]);

    NSArray<FTSTestCaseObject *> *ABBC = @[ AB, BC ];
    XCTAssertTrue([self expectObject:AB whileSearchingByKeyword:"A AND B"]);
    XCTAssertTrue([self expectObject:AB whileSearchingByKeyword:"A B"]);
    XCTAssertTrue([self expectObjects:ABBC whileSearchingByKeyword:"A OR B"]);

    XCTAssertTrue([self expectObject:AB whileSearchingByKeyword:"B NOT C"]);
}

- (void)test_offsets
{
    FTSTestCaseObject *sentence = [[FTSTestCaseObject alloc] initWithMessage:@"one two three four" andExtension:@"five six seven eight nine ten"];
    XCTAssertTrue([_database insertObject:sentence intoTable:_tableName]);

    WCTValue *result = [_database getValueOnResult:_tableColumn.offsets() fromTable:_tableName where:_tableColumn.match("f* t*")];

    NSArray<NSString *> *offsets = [result.stringValue componentsSeparatedByString:@" "];
    XCTAssertGreaterThan(offsets.count, 0);
    XCTAssertEqual(offsets.count % 4, 0);

    //"f*" match "five"
    XCTAssertEqual(offsets[0].intValue, 0); // 0 for first column in database, which is "extension" here.
    XCTAssertEqual(offsets[1].intValue, 0); // 0 for first matched term in query, which is "f*" here.
    XCTAssertEqual(offsets[2].intValue, 0); // 0 for the byte offset matched term.
    XCTAssertEqual(offsets[3].intValue, 4); // 4 for the byte length matched term.

    //"t*" match "ten"
    XCTAssertEqual(offsets[4].intValue, 0);  // 0 for first column in database, which is "extension" here.
    XCTAssertEqual(offsets[5].intValue, 1);  // 1 for second matched term in query, which is "t*" here.
    XCTAssertEqual(offsets[6].intValue, 26); // 26 for the byte offset matched term.
    XCTAssertEqual(offsets[7].intValue, 3);  // 4 for the byte length matched term.

    //"t*" match "two"
    XCTAssertEqual(offsets[8].intValue, 1);  // 1 for second column in database, which is "message" here.
    XCTAssertEqual(offsets[9].intValue, 1);  // 1 for second matched term in query, which is "t*" here.
    XCTAssertEqual(offsets[10].intValue, 4); // 4 for the byte offset matched term.
    XCTAssertEqual(offsets[11].intValue, 3); // 3 for the byte length matched term.

    //"t*" match "three"
    XCTAssertEqual(offsets[12].intValue, 1); // 1 for second column in database, which is "message" here.
    XCTAssertEqual(offsets[13].intValue, 1); // 1 for second matched term in query, which is "t*" here.
    XCTAssertEqual(offsets[14].intValue, 8); // 8 for the byte offset matched term.
    XCTAssertEqual(offsets[15].intValue, 5); // 5 for the byte length matched term.

    //"f*" match "four"
    XCTAssertEqual(offsets[16].intValue, 1);  // 1 for second column in database, which is "message" here.
    XCTAssertEqual(offsets[17].intValue, 0);  // 0 for first matched term in query, which is "f*" here.
    XCTAssertEqual(offsets[18].intValue, 14); // 14 for the byte offset matched term.
    XCTAssertEqual(offsets[19].intValue, 4);  // 4 for the byte length matched term.
}

- (void)test_snippet
{
    FTSTestCaseObject *easyToUse = [[FTSTestCaseObject alloc] initWithMessage:@"Easy-to-use" andExtension:@"Through WCDB, you can get objects from database in one line code."];
    FTSTestCaseObject *efficient = [[FTSTestCaseObject alloc] initWithMessage:@"Efficient" andExtension:@"Through the framework layer and sqlcipher source optimization, WCDB have more efficient performance."];
    FTSTestCaseObject *complete = [[FTSTestCaseObject alloc] initWithMessage:@"Complete" andExtension:@"Encryption Support, Corruption recovery, Anti-injection."];
    XCTAssertTrue([_database insertObject:easyToUse intoTable:_tableName]);
    XCTAssertTrue([_database insertObject:efficient intoTable:_tableName]);
    XCTAssertTrue([_database insertObject:complete intoTable:_tableName]);

    const char *startMatchText = "/* ";
    const char *endMatchText = " */";
    const char *ellipses = "...";
    int columnNumber = -1;
    int approximateNumberOfTokens = 5;

    WCTOneColumn *results = [_database getColumnOnResult:_tableColumn.snippet(startMatchText, endMatchText, ellipses, columnNumber, approximateNumberOfTokens) fromTable:_tableName where:FTSTestCaseObject.extension.match("e*")];

    XCTAssertEqual(results.count, 2);
    XCTAssertTrue([results[0].stringValue isEqualToString:@"...WCDB have more /* efficient */ performance."]);
    XCTAssertTrue([results[1].stringValue isEqualToString:@"/* Encryption */ Support, Corruption recovery, Anti..."]);
}

@end
