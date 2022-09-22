//
// Created by sanhuazhang on 2019/05/02
//

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

#import "TestCase.h"

@interface TraceTests : TableTestCase

@end

@implementation TraceTests

- (void)test_trace_sql
{
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());

    __block BOOL tested = NO;
    [self.database traceSQL:^(NSString*, UInt64, NSString* sql) {
        if ([sql isEqualToString:@(statement.getDescription().data())]) {
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

    NSArray<TestCaseObject*>* objects = [Random.shared autoIncrementTestCaseObjectsWithCount:10000];

    __block NSMutableArray* expectedFootprints = [[NSMutableArray alloc] initWithObjects:
                                                                         @"BEGIN IMMEDIATE",
                                                                         @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)",
                                                                         @"COMMIT",
                                                                         nil];
    [self.database tracePerformance:^(NSString*, UInt64, NSString* sql, double) {
        if ([sql isEqualToString:expectedFootprints.firstObject]) {
            [expectedFootprints removeObjectAtIndex:0];
        }
    }];
    TestCaseAssertTrue([self.database insertObjects:objects intoTable:self.tableName]);
    TestCaseAssertTrue(expectedFootprints.count == 0);

    [self.database tracePerformance:nil];
}

- (void)test_global_trace_error
{
    self.tableClass = TestCaseObject.class;

    __block BOOL tested = NO;
    __block BOOL start = NO;
    weakify(self);
    [WCTDatabase globalTraceError:^(WCTError* error) {
        strongify_or_return(self);
        if (error.level == WCTErrorLevelError
            && [error.path isEqualToString:self.path]
            && error.tag == self.database.tag
            && error.code == WCTErrorCodeError
            && [error.sql isEqualToString:@"SELECT 1 FROM main.dummy"]) {
            tested = YES;
        }
    }];

    TestCaseAssertTrue([self.database canOpen]);

    start = YES;
    TestCaseAssertFalse([self.database execute:WCDB::StatementSelect().select(1).from(@"dummy")]);

    TestCaseAssertTrue(tested);
    [WCTDatabase globalTraceError:nil];
}

- (void)test_global_trace_sql
{
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());

    __block BOOL tested = NO;
    [WCTDatabase globalTraceSQL:^(NSString*, UInt64, NSString* sql) {
        if ([sql isEqualToString:@(statement.getDescription().data())]) {
            tested = YES;
        }
    }];
    TestCaseAssertTrue([self.database execute:statement]);
    TestCaseAssertTrue(tested);

    [WCTDatabase globalTraceSQL:nil];
}

- (void)test_global_trace_performance
{
    NSArray<TestCaseObject*>* objects = [Random.shared autoIncrementTestCaseObjectsWithCount:10000];

    __block NSMutableArray* expectedFootprints = [[NSMutableArray alloc] initWithObjects:
                                                                         @"BEGIN IMMEDIATE",
                                                                         @"INSERT INTO main.testTable(identifier, content) VALUES(?1, ?2)",
                                                                         @"COMMIT",
                                                                         nil];
    [WCTDatabase globalTracePerformance:^(NSString*, UInt64, NSString* sql, double) {
        if ([sql isEqualToString:expectedFootprints.firstObject]) {
            [expectedFootprints removeObjectAtIndex:0];
        }
    }];

    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.database insertObjects:objects intoTable:self.tableName]);
    TestCaseAssertTrue(expectedFootprints.count == 0);

    [WCTDatabase globalTracePerformance:nil];
}

@end
