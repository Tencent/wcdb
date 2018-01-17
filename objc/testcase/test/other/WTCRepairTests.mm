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

#import "WTCBaseTestCase.h"
#import "Convenience.h"
#import "WTCRepairObject.h"
#import "WTCRepairObject+WCTTableCoding.h"

@interface WTCRepairTests : WTCBaseTestCase
@property(nonatomic, readonly) WCTDatabase* database;
@property(nonatomic, readonly) int pageSize;
@property(nonatomic, readonly) NSArray<WTCRepairObject*>* preInsertedObjects;
@end

@implementation WTCRepairTests

- (void)setUp {
    [super setUp];
    
    {
        NSMutableArray<WTCRepairObject*> *preInsertedObjects = [[NSMutableArray alloc] init];
        WTCRepairObject* object1 = [[WTCRepairObject alloc] init];
        object1.variable1 = 1;
        object1.variable2 = @"object1";
        [preInsertedObjects addObject:object1];
        WTCRepairObject* object2 = [[WTCRepairObject alloc] init];
        object2.variable1 = 2;
        object2.variable2 = @"object2";
        [preInsertedObjects addObject:object2];
        _preInsertedObjects = preInsertedObjects;
    }    
    
    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    
    WCDB::StatementPragma pragma = WCDB::StatementPragma().pragma(WCDB::Pragma::PageSize);
    WCTStatement * coreStatement = [_database prepare:pragma];
    XCTAssertNotNil(coreStatement);
    XCTAssertTrue([coreStatement step]);
    _pageSize = ((NSNumber*)[coreStatement getValueAtIndex:0]).intValue;
    [coreStatement finalize];
    XCTAssertEqual(_pageSize >> 1 & _pageSize, 0);
    XCTAssertGreaterThan(_pageSize, 0);
    
    XCTAssertTrue([_database createTableAndIndexesOfName:WTCRepairObject.Name withClass:WTCRepairObject.class]);
    
    XCTAssertTrue([_database insertObjects:_preInsertedObjects into:WTCRepairObject.Name]);
}

- (void)corrupt:(WCTDatabase*)database
{
    [database close:^{
        NSMutableData* data = [[NSMutableData alloc] init];
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
        NSArray<WTCRepairObject*>* results = [self.database getObjectsOfClass:WTCRepairObject.class fromTable:WTCRepairObject.Name];
        NSArray<WTCRepairObject*>* sortedResults = [results sortedArrayUsingComparator:WTCRepairObject.Comparator];
        NSArray<WTCRepairObject*>* sortedPreInsertedObjects = [self.preInsertedObjects sortedArrayUsingComparator:WTCRepairObject.Comparator];
        XCTAssertTrue([sortedResults isEqualToArray:sortedPreInsertedObjects]);
    }
    //When
    [self corrupt:self.database];
    //Then
    XCTAssertNil([self.database getObjectsOfClass:WTCRepairObject.class fromTable:WTCRepairObject.Name]);
    //When
    WCTDatabase* recovered = [[WCTDatabase alloc] initWithPath:[self.database.path stringByAppendingString:@".recovered"]];
    [self.database close:^{
        XCTAssertTrue([recovered recoverFromPath:self.database.path withPageSize:self.pageSize backupCipher:nil databaseCipher:nil]);
    }];
    //Then
    {
        NSArray<WTCRepairObject*>* results = [recovered getObjectsOfClass:WTCRepairObject.class fromTable:WTCRepairObject.Name];
        NSArray<WTCRepairObject*>* sortedResults = [results sortedArrayUsingComparator:WTCRepairObject.Comparator];
        NSArray<WTCRepairObject*>* sortedPreInsertedObjects = [self.preInsertedObjects sortedArrayUsingComparator:WTCRepairObject.Comparator];
        XCTAssertTrue([sortedResults isEqualToArray:sortedPreInsertedObjects]);
    }
}

@end
