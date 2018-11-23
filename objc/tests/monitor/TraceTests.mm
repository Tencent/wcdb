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

#import "SingleTableTestCase.h"
#import "TestCaseObject+WCTTableCoding.h"
#import "TestCaseObject.h"

@interface TraceTests : SingleTableTestCase

@end

@implementation TraceTests

- (void)setUp
{
    [super setUp];
    self.tableClass = TestCaseObject.class;
}

- (void)test_trace_sql
{
    [self removeSQLRelatedConfigs];
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

    [self removeSQLRelatedConfigs];

    NSMutableArray<TestCaseObject*>* objects = [[NSMutableArray<TestCaseObject*> alloc] init];
    for (int i = 0; i < 10000; ++i) {
        TestCaseObject* object = [[TestCaseObject alloc] init];
        object.isAutoIncrement = YES;
        object.content = [NSString randomString];
        [objects addObject:object];
    }

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
    [WCTDatabase globalTraceError:^(WCTError* error) {
        if (error.level == WCTErrorLevelError
            && [error.path isEqualToString:self.path]
            && error.tag == self.database.tag
            && [error.source isEqualToString:@"SQLite"]
            && error.code == WCTErrorCodeIOError
            && [error.sql isEqualToString:@"BEGIN IMMEDIATE"]) {
            tested = YES;
        }
    }];

    TestCaseAssertTrue([self.database canOpen]);

    start = YES;
    [self.console disableSQLiteWrite];
    TestCaseAssertFalse([self createTable]);
    [self.console enableSQLiteWrite];

    TestCaseAssertTrue(tested);
    [WCTDatabase resetGlobalErrorTracer];
}

- (void)test_global_trace_sql
{
    [self removeSQLRelatedConfigs];
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
    [self removeSQLRelatedConfigs];

    NSMutableArray<TestCaseObject*>* objects = [[NSMutableArray<TestCaseObject*> alloc] init];
    for (int i = 0; i < 10000; ++i) {
        TestCaseObject* object = [[TestCaseObject alloc] init];
        object.isAutoIncrement = YES;
        object.content = [NSString randomString];
        [objects addObject:object];
    }

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
