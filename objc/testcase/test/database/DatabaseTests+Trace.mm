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
#include <pwd.h>

@interface DatabaseTests_Trace : TestCase

@end

@implementation DatabaseTests_Trace {
    WCTDatabase *_database1;
    WCTDatabase *_database2;
    NSString *_errorTracerName;
}

- (void)setUp
{
    [super setUp];

    _database1 = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:@"_1"]];
    _database2 = [[WCTDatabase alloc] initWithPath:[self.recommendedPath stringByAppendingString:@"_2"]];
    _errorTracerName = self.testname;
}

- (void)tearDown
{
    XCTAssertTrue([_database1 removeFiles]);
    XCTAssertTrue([_database2 removeFiles]);

    _database1 = nil;
    _database2 = nil;

    [WCTDatabase globalTracePerformance:nil];
    [WCTDatabase globalTraceSQL:nil];
    [WCTDatabase globalTraceError:nil tracerNamed:_errorTracerName];

    [super tearDown];
}

- (void)test_sql_trace
{
    __block BOOL hit = NO;
    __block BOOL hit1 = NO;
    __block BOOL hit2 = NO;
    [WCTDatabase globalTraceSQL:^(NSString *sql) {
        if ([sql isEqualToString:@"PRAGMA main.user_version"]) {
            hit = YES;
        }
    }];
    [_database1 traceSQL:^(NSString *sql) {
        if ([sql isEqualToString:@"PRAGMA main.user_version"]) {
            hit1 = YES;
        }
    }];
    [_database2 traceSQL:^(NSString *sql) {
        if ([sql isEqualToString:@"PRAGMA main.user_version"]) {
            hit2 = YES;
        }
    }];

    XCTAssertTrue([_database1 execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);
    XCTAssertTrue(hit);
    XCTAssertTrue(hit1);
    XCTAssertFalse(hit2);

    hit = NO;
    hit1 = NO;
    hit2 = NO;

    XCTAssertTrue([_database2 execute:WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion())]);
    XCTAssertTrue(hit);
    XCTAssertFalse(hit1);
    XCTAssertTrue(hit2);
}

- (void)test_performance_trace
{
    int count = 12345;
    __block BOOL hit = NO;
    __block BOOL hit1 = NO;
    __block BOOL hit2 = NO;
    [WCTDatabase globalTracePerformance:^(NSArray<WCTPerformanceFootprint *> *footprints, NSInteger cost) {
        for (WCTPerformanceFootprint *footprint in footprints) {
            if (footprint.frequency == count && cost > 0) {
                hit = YES;
            }
        }
    }];

    [_database1 tracePerformance:^(NSArray<WCTPerformanceFootprint *> *footprints, NSInteger cost) {
        for (WCTPerformanceFootprint *footprint in footprints) {
            if (footprint.frequency == count && cost > 0) {
                hit1 = YES;
            }
        }
    }];
    [_database2 tracePerformance:^(NSArray<WCTPerformanceFootprint *> *footprints, NSInteger cost) {
        for (WCTPerformanceFootprint *footprint in footprints) {
            if (footprint.frequency == count && cost > 0) {
                hit2 = YES;
            }
        }
    }];

    NSArray<TestCaseObject *> *objects = [TestCaseObject objectsWithCount:count];
    NSString *tableName = NSStringFromSelector(_cmd);

    XCTAssertTrue([_database1 createTableAndIndexes:tableName withClass:TestCaseObject.class]);
    XCTAssertTrue([_database1 insertObjects:objects intoTable:tableName]);
    XCTAssertTrue(hit);
    XCTAssertTrue(hit1);
    XCTAssertFalse(hit2);

    hit = NO;
    hit1 = NO;
    hit2 = NO;

    XCTAssertTrue([_database2 createTableAndIndexes:tableName withClass:TestCaseObject.class]);
    XCTAssertTrue([_database2 insertObjects:objects intoTable:tableName]);
    XCTAssertTrue(hit);
    XCTAssertFalse(hit1);
    XCTAssertTrue(hit2);
}

- (void)test_trace_error
{
    __block BOOL hit = NO;
    _database1.tag = (int) self.hash;
    [WCTDatabase
    globalTraceError:^(WCTError *error) {
        if (error.level == WCTErrorLevelError) {
            XCTAssertEqual(error.tag, _database1.tag);
            hit = YES;
        }
    }
         tracerNamed:_errorTracerName];
    XCTAssertFalse([_database1 getObjectOfClass:TestCaseObject.class fromTable:@"non_existent_table"]);
    XCTAssertTrue(hit);
}

@end
