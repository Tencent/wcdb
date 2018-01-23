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
#import "WTCTracerObject.h"
#import "WTCTracerObject+WCTTableCoding.h"

@interface WTCTracerTests : WTCBaseTestCase

@end

@implementation WTCTracerTests

- (void)reset
{
    [WCTStatistics SetGlobalSQLTrace:nil];
    [WCTStatistics SetGlobalErrorReport:nil];
    [WCTStatistics SetGlobalPerformanceTrace:nil];
    [WCTStatistics ResetGlobalErrorReport];
}

- (void)setUp {
    [super setUp];
    [self reset];
}

- (void)tearDown {
    [self reset];
    [super tearDown];
}

- (void)testTraceSQL {
    //Give
    NSString* expectedSQL = @"SELECT * FROM sqlite_master";
    
    //Then
    __block BOOL pass = NO;
    [WCTStatistics SetGlobalSQLTrace:^(NSString * sql) {
        if ([sql isEqualToString:expectedSQL]) {
            pass = YES;
        }
    }];
    
    //Give
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    
    //When
    XCTAssertNotNil([database getColumnOnResult:WCTMaster.AllColumns fromTable:WCTMaster.TableName]);
    
    XCTAssertTrue(pass);
}

- (void)testTraceError {
    //Give
    NSString* tableName = @"nonexistentTable";
    WCTTag expectedTag = self.recommendedTag;
    int expectedErrorCode = 1;
    NSString* expectedErrorMessage = [NSString stringWithFormat:@"no such table: %@", tableName];
    int expectedOperation = 3;
    NSString* expectedSQL = [NSString stringWithFormat:@"SELECT * FROM %@", tableName];
    NSString* expectedPath = self.recommendedPath;
    int expectedExtendedErrorCode = 1;
    
    //Then
    __block BOOL didCatch = NO;
    [WCTStatistics SetGlobalErrorReport:^(WCTError *error) {
        if (error.type == WCTErrorTypeSQLite) {
            NSNumber* tag = error.tag;
            XCTAssertNotNil(tag);
            XCTAssertEqual(tag.intValue, expectedTag);
            
            XCTAssertEqual(error.code, expectedErrorCode);
            
            NSString* message = error.message;
            XCTAssertNotNil(message);
            XCTAssertTrue([message isEqualToString:expectedErrorMessage]);
            
            NSNumber* operation = error.operation;
            XCTAssertNotNil(operation);
            XCTAssertEqual(operation.intValue, expectedOperation);
            
            NSString* sql = error.sql;
            XCTAssertNotNil(sql);
            XCTAssertTrue([sql isEqualToString:expectedSQL]);
            
            NSString* path = error.path;
            XCTAssertNotNil(path);
            XCTAssertTrue([path isEqualToString:expectedPath]);
            
            NSNumber* extendedCode = error.extendedCode;
            XCTAssertNotNil(extendedCode);
            XCTAssertEqual(extendedCode.intValue, expectedExtendedErrorCode);
            
            didCatch = YES;
        }
    }];
    
    //Give
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:expectedPath];
    database.tag = expectedTag;
    
    //When
    XCTAssertNil([database getColumnOnResult:WCDB::Column::All fromTable:tableName]);
    
    XCTAssertTrue(didCatch);
}

- (void)testGlobalTracePerformanceCommit
{
    //Give
    NSString* tableName = WTCTracerObject.Name;
    WCTTag expectedTag = self.recommendedTag;
    NSString* expectedSQL = [NSString stringWithFormat:@"INSERT INTO %@(variable) VALUES(?)", tableName];
    
    //Then
    __block BOOL didCatch = NO;
    [WCTStatistics SetGlobalPerformanceTrace:^(WCTTag tag, NSDictionary<NSString *,NSNumber *> * sqls, NSInteger cost) {
        if (tag == expectedTag) {
            __block BOOL contains = NO;
            [sqls enumerateKeysAndObjectsUsingBlock:^(NSString * key, NSNumber * obj, BOOL * stop) {
                if ([key isEqualToString:expectedSQL]) {
                    contains = YES;
                    *stop = YES;
                }
            }];
            if (contains) {
                XCTAssertGreaterThan(cost, 0);
                didCatch = YES;
            }
        }
    }];
    
    //Give
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    [database close:^{
        XCTAssertTrue([database removeFilesWithError:nil]);
    }];
    database.tag = expectedTag;
    
    //When
    XCTAssertTrue([database createTableAndIndexesOfName:WTCTracerObject.Name withClass:WTCTracerObject.class]);
    WTCTracerObject* templateObject = [[WTCTracerObject alloc] init];
    templateObject.isAutoIncrement = YES;
    NSMutableArray* objects = [NSMutableArray array] ;
    for (int i = 0; i < 1000000; ++i) {
        [objects addObject:templateObject];
    }
    XCTAssertTrue([database insertObjects:objects into:tableName]);
    [database close];
    
    XCTAssertTrue(didCatch);
}

- (void)testTraceRollback
{
    //Give
    NSString* tableName = WTCTracerObject.Name;
    int expectedTag = self.recommendedTag;
    NSString* expectedSQL = [NSString stringWithFormat:@"INSERT INTO %@(variable) VALUES(?)", tableName];
    NSString* expectedRollback = @"ROLLBACK";
    
    //Then
    __block BOOL didCatch = NO;
    [WCTStatistics SetGlobalPerformanceTrace:^(WCTTag tag, NSDictionary<NSString *,NSNumber *> *sqls, NSInteger cost) {
        if (tag == expectedTag) {
            __block BOOL contains = NO;
            [sqls enumerateKeysAndObjectsUsingBlock:^(NSString * key, NSNumber * obj, BOOL * stop) {
                if ([key isEqualToString:expectedSQL]) {
                    contains = YES;
                    *stop = YES;
                }
            }];
            if (contains) {
                [sqls enumerateKeysAndObjectsUsingBlock:^(NSString * key, NSNumber * obj, BOOL * stop) {
                    if ([key isEqualToString:expectedRollback]) {
                        didCatch = YES;
                        *stop = YES;
                    }
                }];    
            }
        }
    }];
    
    //Give
    WCTDatabase* database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    [database close:^{
        XCTAssertTrue([database removeFilesWithError:nil]);
    }];
    database.tag = expectedTag;
    
    //When
    XCTAssertTrue([database createTableAndIndexesOfName:WTCTracerObject.Name withClass:WTCTracerObject.class]);
    WTCTracerObject* templateObject = [[WTCTracerObject alloc] init];
    templateObject.isAutoIncrement = YES;
    NSMutableArray* objects = [NSMutableArray array] ;
    for (int i = 0; i < 1000000; ++i) {
        [objects addObject:templateObject];
    }
    XCTAssertFalse([database runControllableTransaction:^BOOL{
        [database insertObjects:objects into:tableName];
        return NO;
    }]);
    [database close];
    
    XCTAssertTrue(didCatch);
}

@end
