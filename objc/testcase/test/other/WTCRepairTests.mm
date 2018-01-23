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

#import "Convenience.h"
#import "WTCBaseTestCase.h"
#import "WTCRepairObject+WCTTableCoding.h"
#import "WTCRepairObject.h"

@interface WTCRepairTests : WTCBaseTestCase
@property(nonatomic, readonly) WCTDatabase *database;
@property(nonatomic, readonly) int pageSize;
@property(nonatomic, readonly) NSArray<WTCRepairObject *> *preInsertedObjects;
@property(nonatomic, readonly) WCTCoreStatement *coreStatement;
@end

@implementation WTCRepairTests

- (void)setUp
{
    [super setUp];

    {
        NSMutableArray<WTCRepairObject *> *preInsertedObjects = [[NSMutableArray alloc] init];
        WTCRepairObject *object1 = [[WTCRepairObject alloc] init];
        object1.variable1 = 1;
        object1.variable2 = @"object1";
        [preInsertedObjects addObject:object1];
        WTCRepairObject *object2 = [[WTCRepairObject alloc] init];
        object2.variable1 = 2;
        object2.variable2 = @"object2";
        [preInsertedObjects addObject:object2];
        _preInsertedObjects = preInsertedObjects;
    }

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    WCDB::StatementPragma pragma = WCDB::StatementPragma().pragma(WCDB::Pragma::PageSize);
    _coreStatement = [self.database prepare:pragma];
    XCTAssertNotNil(_coreStatement);
    [self.coreStatement step];
    XCTAssertNil([self.coreStatement getError]);
    _pageSize = ((NSNumber *) [self.coreStatement valueAtIndex:0]).intValue;
    XCTAssertTrue([self.coreStatement finalize]);
    XCTAssertEqual(self.pageSize >> 1 & self.pageSize, 0);
    XCTAssertGreaterThan(_pageSize, 0);

    XCTAssertTrue([self.database createTableAndIndexesOfName:WTCRepairObject.Name withClass:WTCRepairObject.class]);

    XCTAssertTrue([self.database insertObjects:_preInsertedObjects into:WTCRepairObject.Name]);
}

- (void)tearDown
{
    if (_coreStatement) {
        XCTAssertTrue([_coreStatement finalize]);
        _coreStatement = nil;
    }
    [_database close:^{
      XCTAssertTrue([_database removeFilesWithError:nil]);
    }];
    _database = nil;
    [super tearDown];
}

- (void)corrupt:(WCTDatabase *)database
{
    [database close:^{
      NSMutableData *data = [[NSMutableData alloc] init];
      [data increaseLengthBy:_pageSize];
      int fd = open(self.database.path.UTF8String, O_RDWR);
      write(fd, data.bytes, data.length);
      close(fd);
    }];
}

- (void)testRepair
{
    //Give
    XCTAssertTrue([self.database backup]);
    //Then
    {
        NSArray<WTCRepairObject *> *results = [self.database getObjectsOfClass:WTCRepairObject.class fromTable:WTCRepairObject.Name];
        XCTAssertTrue([results.sorted isEqual:self.preInsertedObjects.sorted]);
    }
    //When
    [self corrupt:self.database];
    //Then
    XCTAssertNil([self.database getObjectsOfClass:WTCRepairObject.class fromTable:WTCRepairObject.Name]);
    //When
    WCTDatabase *recovered = [[WCTDatabase alloc] initWithPath:[self.database.path stringByAppendingString:@".recovered"]];
    [self.database close:^{
      XCTAssertTrue([recovered recoverFromPath:self.database.path withPageSize:self.pageSize backupCipher:nil databaseCipher:nil]);
    }];
    //Then
    {
        NSArray<WTCRepairObject *> *results = [recovered getObjectsOfClass:WTCRepairObject.class fromTable:WTCRepairObject.Name];
        XCTAssertTrue([results.sorted isEqual:self.preInsertedObjects.sorted]);
    }
}

@end
