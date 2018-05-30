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

#import "TestCaseCommon.h"

@interface DatabaseTests_RepairKit : TestCase
@end

@implementation DatabaseTests_RepairKit {
    WCTDatabase *_database;
    int _pageSize;
    NSArray<TestCaseObject *> *_preInserted;
    NSString *_tableName;
    Class _cls;
}

//- (void)setUp
//{
//    [super setUp];
//
//    _tableName = self.className;
//
//    _cls = TestCaseObject.class;
//
//    _preInserted = [TestCaseObject objectsWithCount:10000];
//
//    [self setUpDatabase:nil];
//}
//
//- (void)tearDown
//{
//    [self tearDownDatabase];
//    [super tearDown];
//}
//
//- (void)setUpDatabase:(NSData *)cipher
//{
//    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
//
//    if (cipher.length > 0) {
//        [_database setCipherKey:cipher];
//    }
//
//    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:_cls]);
//
//    XCTAssertTrue([_database insertObjects:_preInserted intoTable:_tableName]);
//
//    WCDB::StatementPragma pragma = WCDB::StatementPragma().pragma(WCDB::Pragma::pageSize());
//    WCTValue *value = [_database getValueFromStatement:pragma];
//    XCTAssertNotNil(value);
//    _pageSize = value.numberValue.intValue;
//    XCTAssertGreaterThan(_pageSize, 0);
//    XCTAssertEqual(_pageSize >> 1 & _pageSize, 0);
//}
//
//- (void)tearDownDatabase
//{
//    [_database close:^{
//      XCTAssertTrue([_database removeFiles]);
//    }];
//    _database = nil;
//}
//
//- (void)corrupt
//{
//    [_database close:^{
//      NSMutableData *data = [[NSMutableData alloc] init];
//      [data increaseLengthBy:_pageSize];
//      int fd = open(_database.path.UTF8String, O_RDWR);
//      write(fd, data.bytes, data.length);
//      close(fd);
//    }];
//}
//
//- (void)test_repair
//{
//    //Give
//    XCTAssertTrue([_database backup]);
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//    //When
//    [self corrupt];
//    //Then
//    XCTAssertNil([_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]);
//    //When
//    WCTDatabase *recovered = [[WCTDatabase alloc] initWithPath:[_database.path stringByAppendingString:@".recovered"]];
//    [_database close:^{
//      XCTAssertTrue([recovered recoverFromPath:_database.path withPageSize:_pageSize backupCipher:nil databaseCipher:nil]);
//    }];
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [recovered getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//}
//
//- (void)test_repair_crypted_backup
//{
//    NSData *backupCipher = [NSData randomData];
//    //Give
//    XCTAssertTrue([_database backupWithCipher:backupCipher]);
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//    //When
//    [self corrupt];
//    //Then
//    XCTAssertNil([_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]);
//    //When
//    WCTDatabase *recovered = [[WCTDatabase alloc] initWithPath:[_database.path stringByAppendingString:@".recovered"]];
//    [_database close:^{
//      XCTAssertTrue([recovered recoverFromPath:_database.path withPageSize:_pageSize backupCipher:backupCipher databaseCipher:nil]);
//    }];
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [recovered getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//}
//
//- (void)test_repair_crypted_database
//{
//    NSData *databaseCipher = [NSData randomData];
//    [self tearDownDatabase];
//    [self setUpDatabase:databaseCipher];
//    //Give
//    XCTAssertTrue([_database backup]);
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//    //When
//    [self corrupt];
//    //Then
//    XCTAssertNil([_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]);
//    //When
//    WCTDatabase *recovered = [[WCTDatabase alloc] initWithPath:[_database.path stringByAppendingString:@".recovered"]];
//    [_database close:^{
//      XCTAssertTrue([recovered recoverFromPath:_database.path withPageSize:_pageSize backupCipher:nil databaseCipher:databaseCipher]);
//    }];
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [recovered getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//}
//
//- (void)test_repair_crypted
//{
//    NSData *backupCipher = [NSData randomData];
//    NSData *databaseCipher = [NSData randomData];
//    [self tearDownDatabase];
//    [self setUpDatabase:databaseCipher];
//    //Give
//    XCTAssertTrue([_database backupWithCipher:backupCipher]);
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//    //When
//    [self corrupt];
//    //Then
//    XCTAssertNil([_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]);
//    //When
//    WCTDatabase *recovered = [[WCTDatabase alloc] initWithPath:[_database.path stringByAppendingString:@".recovered"]];
//    [_database close:^{
//      XCTAssertTrue([recovered recoverFromPath:_database.path withPageSize:_pageSize backupCipher:backupCipher databaseCipher:databaseCipher]);
//    }];
//    //Then
//    {
//        NSArray<TestCaseObject *> *results = [recovered getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1];
//        XCTAssertTrue([results isEqualToTestCaseObjects:_preInserted]);
//    }
//}

@end
