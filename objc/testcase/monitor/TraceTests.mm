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

#import <TestCase/TestCase.h>

@interface TraceTests : TableTestCase

@end

@implementation TraceTests

- (void)test_trace_sql
{
    [self.database removeSQLRelatedConfigs];
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());

    __block BOOL tested = NO;
    [self.database traceSQL:^(NSString* sql) {
        if ([sql isEqualToString:@(statement.getDescription().c_str())]) {
            tested = YES;
        }
    }];
    TestCaseAssertTrue([self.database execute:statement]);
    TestCaseAssertTrue(tested);

    [self.database traceSQL:nil];
}

- (void)test_trace_performance
{
    TestCaseAssertTrue([self createTable]);

    [self.database removeSQLRelatedConfigs];

    NSArray<TestCaseObject*>* objects = [self.random autoIncrementTestCaseObjectsWithCount:10000];

    NSArray<WCTPerformanceFootprint*>* expectedFootprints = @[
        [[WCTPerformanceFootprint alloc] initWithSQL:@"BEGIN IMMEDIATE"
                                        andFrequency:1],
        [[WCTPerformanceFootprint alloc] initWithSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                                        andFrequency:(unsigned int) objects.count],
        [[WCTPerformanceFootprint alloc] initWithSQL:@"COMMIT"
                                        andFrequency:1],
    ];

    __block BOOL tested = NO;
    [self.database tracePerformance:^(NSArray<WCTPerformanceFootprint*>* footprints, NSInteger cost) {
        if ([expectedFootprints isEqualToArray:footprints]
            && cost > 0) {
            tested = YES;
        }
    }];
    TestCaseAssertTrue([self.database insertObjects:objects intoTable:self.tableName]);
    TestCaseAssertTrue(tested);

    [self.database tracePerformance:nil];
}

- (void)test_global_trace_error
{
    self.tableClass = TestCaseObject.class;

    __block BOOL tested = NO;
    __block BOOL start = NO;
    __weak typeof(self) weakSelf = self;
    [WCTDatabase globalTraceError:^(WCTError* error) {
        TestCaseLog(@"%@", error);
        typeof(self) strongSelf = weakSelf; 
        if (strongSelf == nil) {
            return;
        }
        if (error.level == WCTErrorLevelError
            && [error.path isEqualToString:self.path]
            && error.tag == strongSelf.database.tag
            && [error.source isEqualToString:@"SQLite"]
            && error.code == WCTErrorCodeError
            && [error.sql isEqualToString:@"SELECT 1 FROM main.dummy"]) {
            tested = YES;
        }
    }];

    TestCaseAssertTrue([self.database canOpen]);

    start = YES;
    TestCaseAssertFalse([self.database execute:WCDB::StatementSelect().select(1).from(@"dummy")]);

    TestCaseAssertTrue(tested);
    [WCTDatabase resetGlobalErrorTracer];
}

- (void)test_global_trace_sql
{
    [self.database removeSQLRelatedConfigs];
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());

    __block BOOL tested = NO;
    [WCTDatabase globalTraceSQL:^(NSString* sql) {
        if ([sql isEqualToString:@(statement.getDescription().c_str())]) {
            tested = YES;
        }
    }];
    TestCaseAssertTrue([self.database execute:statement]);
    TestCaseAssertTrue(tested);

    [WCTDatabase globalTraceSQL:nil];
}

- (void)test_global_trace_performance
{
    [self.database removeSQLRelatedConfigs];

    NSArray<TestCaseObject*>* objects = [self.random autoIncrementTestCaseObjectsWithCount:10000];

    NSArray<WCTPerformanceFootprint*>* expectedFootprints = @[
        [[WCTPerformanceFootprint alloc] initWithSQL:@"BEGIN IMMEDIATE"
                                        andFrequency:1],
        [[WCTPerformanceFootprint alloc] initWithSQL:@"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)"
                                        andFrequency:(unsigned int) objects.count],
        [[WCTPerformanceFootprint alloc] initWithSQL:@"COMMIT"
                                        andFrequency:1],
    ];

    __block BOOL tested = NO;
    [WCTDatabase globalTracePerformance:^(NSArray<WCTPerformanceFootprint*>* footprints, NSInteger cost) {
        if ([expectedFootprints isEqualToArray:footprints]
            && cost > 0) {
            tested = YES;
        }
    }];

    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.database insertObjects:objects intoTable:self.tableName]);
    TestCaseAssertTrue(tested);

    [WCTDatabase globalTracePerformance:nil];
}

@end
