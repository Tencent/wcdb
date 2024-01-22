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
    [self.database traceSQL:^(WCTTag tag, NSString* path, UInt64, NSString* sql, NSString*) {
        XCTAssertEqual(tag, self.database.tag);
        XCTAssertTrue([path isEqualToString:self.database.path]);
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

    NSMutableArray<TestCaseObject*>* objects = [[NSMutableArray alloc] init];
    for (int i = 0; i < 1000; i++) {
        TestCaseObject* object = [TestCaseObject objectWithIdentifier:0 andContent:[Random.shared stringWithLength:self.database.pageSize]];
        object.isAutoIncrement = YES;
        [objects addObject:object];
    }
    __block int testCount = 0;

    [self.database tracePerformance:^(WCTTag tag, NSString* path, UInt64, NSString* sql, WCTPerformanceInfo* info) {
        XCTAssertEqual(tag, self.database.tag);
        XCTAssertTrue([path isEqualToString:self.database.path]);
        XCTAssertTrue(info != nil);
        if ([sql hasPrefix:@"COMMIT"]) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount > 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount > 0);
            XCTAssertTrue(info.tablePageReadCount == 0);
            XCTAssertTrue(info.indexPageReadCount == 0);
            XCTAssertTrue(info.overflowPageReadCount == 0);
            testCount++;
        } else if ([sql hasPrefix:@"CREATE INDEX"]) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 1);
            XCTAssertTrue(info.indexPageWriteCount > 0);
            XCTAssertTrue(info.overflowPageWriteCount == objects.count);
            XCTAssertTrue(info.tablePageReadCount > 0);
            XCTAssertTrue(info.indexPageReadCount >= 0);
            XCTAssertTrue(info.overflowPageReadCount == objects.count);
            testCount++;
        } else if ([sql hasPrefix:@"SELECT"]) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount == 0);
            testCount++;
            if ([sql hasSuffix:@"ORDER BY content"]) {
                XCTAssertTrue(info.tablePageReadCount == 0);
                XCTAssertTrue(info.indexPageReadCount > 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.count);
            } else {
                XCTAssertTrue(info.tablePageReadCount > 0);
                XCTAssertTrue(info.indexPageReadCount == 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.count);
            }
        }
    }];
    TestCaseAssertTrue([self.database insertObjects:objects intoTable:self.tableName]);
    TestCaseAssertTrue([self.database execute:WCDB::StatementCreateIndex().createIndex("testIndex").table(self.tableName).indexed(TestCaseObject.content)]);
    TestCaseAssertTrue([self.table getObjects].count == objects.count);
    TestCaseAssertTrue([self.table getObjectsOrders:TestCaseObject.content].count == objects.count);
    TestCaseAssertTrue(testCount == 4);

    [self.database tracePerformance:nil];
}

- (void)test_global_trace_error
{
    self.tableClass = TestCaseObject.class;

    __block BOOL tested = NO;
    weakify(self);
    [WCTDatabase globalTraceError:nil];
    [WCTDatabase globalTraceError:^(WCTError* error) {
        strongify_or_return(self);
        if (error.level == WCTErrorLevelError
            && [error.path isEqualToString:self.path]
            && error.tag == self.database.tag
            && error.code == WCTErrorCodeError
            && [error.sql isEqualToString:@"SELECT 1 FROM dummy"]) {
            tested = YES;
        }
    }];

    TestCaseAssertTrue([self.database canOpen]);

    TestCaseAssertFalse([self.database execute:WCDB::StatementSelect().select(1).from(@"dummy")]);

    TestCaseAssertTrue(tested);
    [WCTDatabase globalTraceError:nil];
}

- (void)test_database_trace_error
{
    self.tableClass = TestCaseObject.class;

    __block BOOL tested = NO;
    weakify(self);
    [self.database traceError:^(WCTError* error) {
        strongify_or_return(self);
        TestCaseAssertTrue([error.path isEqualToString:self.path]);
        tested = YES;
    }];

    TestCaseAssertTrue([self.database canOpen]);

    TestCaseAssertFalse([self.database execute:WCDB::StatementSelect().select(1).from(@"dummy")]);
    TestCaseAssertTrue(tested);
    [self.database traceError:nil];
}

- (void)test_global_trace_sql
{
    WCDB::StatementPragma statement = WCDB::StatementPragma().pragma(WCDB::Pragma::userVersion());

    __block BOOL tested = NO;
    [WCTDatabase globalTraceSQL:^(WCTTag tag, NSString* path, UInt64, NSString* sql, NSString*) {
        if (![path isEqualToString:self.database.path]) {
            return;
        }
        XCTAssertEqual(tag, self.database.tag);
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
    NSMutableArray<TestCaseObject*>* objects = [[NSMutableArray alloc] init];
    for (int i = 0; i < 1000; i++) {
        TestCaseObject* object = [TestCaseObject objectWithIdentifier:0 andContent:[Random.shared stringWithLength:self.database.pageSize]];
        object.isAutoIncrement = YES;
        [objects addObject:object];
    }
    __block int testCount = 0;
    __block bool lastSQLIsInsert = false;
    [WCTDatabase globalTracePerformance:^(WCTTag tag, NSString* path, UInt64, NSString* sql, WCTPerformanceInfo* info) {
        if (![path isEqualToString:self.database.path]) {
            return;
        }
        XCTAssertEqual(tag, self.database.tag);
        XCTAssertTrue(info != nil);
        if ([sql hasPrefix:@"COMMIT"] && lastSQLIsInsert) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount > 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount > 0);
            XCTAssertTrue(info.tablePageReadCount == 0);
            XCTAssertTrue(info.indexPageReadCount == 0);
            XCTAssertTrue(info.overflowPageReadCount == 0);
            testCount++;
        } else if ([sql hasPrefix:@"CREATE INDEX"]) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 1);
            XCTAssertTrue(info.indexPageWriteCount > 0);
            XCTAssertTrue(info.overflowPageWriteCount == objects.count);
            XCTAssertTrue(info.tablePageReadCount > 0);
            XCTAssertTrue(info.indexPageReadCount >= 0);
            XCTAssertTrue(info.overflowPageReadCount == objects.count);
            testCount++;
        } else if ([sql hasPrefix:@"SELECT"]) {
            XCTAssertTrue(info.costInNanoseconds > 0);
            XCTAssertTrue(info.tablePageWriteCount == 0);
            XCTAssertTrue(info.indexPageWriteCount == 0);
            XCTAssertTrue(info.overflowPageWriteCount == 0);
            testCount++;
            if ([sql hasSuffix:@"ORDER BY content"]) {
                XCTAssertTrue(info.tablePageReadCount == 0);
                XCTAssertTrue(info.indexPageReadCount > 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.count);
            } else {
                XCTAssertTrue(info.tablePageReadCount > 0);
                XCTAssertTrue(info.indexPageReadCount == 0);
                XCTAssertTrue(info.overflowPageReadCount == objects.count);
            }
        }
        lastSQLIsInsert = [sql hasPrefix:@"INSERT"];
    }];
    [self createTable];
    TestCaseAssertTrue([self.database insertObjects:objects intoTable:self.tableName]);
    TestCaseAssertTrue([self.database execute:WCDB::StatementCreateIndex().createIndex("testIndex").table(self.tableName).indexed(TestCaseObject.content)]);
    TestCaseAssertTrue([self.table getObjects].count == objects.count);
    TestCaseAssertTrue([self.table getObjectsOrders:TestCaseObject.content].count == objects.count);
    TestCaseAssertTrue(testCount == 4);

    [WCTDatabase globalTracePerformance:nil];
}

- (void)test_global_trace_db_operation
{
    __block long tag = 0;
    __block NSString* path = nil;
    __block int openHandleCount = 0;
    __block int tableCount = 0;
    __block int indexCount = 0;
    [WCTDatabase globalTraceDatabaseOperation:^(WCTDatabase* database,
                                                WCTDatabaseOperation operation,
                                                NSDictionary* info) {
        switch (operation) {
        case WCTDatabaseOperation_Create:
            path = database.path;
            break;
        case WCTDatabaseOperation_SetTag:
            tag = database.tag;
            break;
        case WCTDatabaseOperation_OpenHandle: {
            openHandleCount++;
            TestCaseAssertTrue(((NSNumber*) info[WCTDatabaseMonitorInfoKeyHandleCount]).intValue == 1);
            TestCaseAssertTrue(((NSNumber*) info[WCTDatabaseMonitorInfoKeyHandleOpenTime]).intValue > 0);
            TestCaseAssertTrue(((NSNumber*) info[WCTDatabaseMonitorInfoKeyHandleOpenCPUTime]).intValue > 0);
            TestCaseAssertTrue(((NSNumber*) info[WCTDatabaseMonitorInfoKeySchemaUsage]).intValue > 0);
            TestCaseAssertTrue(((NSNumber*) info[WCTDatabaseMonitorInfoKeyTriggerCount]).intValue == 0);
            tableCount = ((NSNumber*) info[WCTDatabaseMonitorInfoKeyTableCount]).intValue;
            indexCount = ((NSNumber*) info[WCTDatabaseMonitorInfoKeyIndexCount]).intValue;
        } break;
        }
    }];

    TestCaseAssertTrue([self createTable]);
    TestCaseAssertTrue([self.database execute:WCDB::StatementCreateIndex()
                                              .createIndex("testIndex")
                                              .table(self.tableName)
                                              .indexed(TestCaseObject.content)]);

    TestCaseAssertTrue(tag == self.database.tag);
    TestCaseAssertStringEqual(path, self.database.path);
    TestCaseAssertTrue(openHandleCount == 1);

    [self.database close];
    TestCaseAssertTrue([self.database insertObjects:[Random.shared autoIncrementTestCaseObjectsWithCount:10] intoTable:self.tableName]);
    TestCaseAssertTrue(openHandleCount == 2);
    TestCaseAssertTrue(tableCount == 4);
    TestCaseAssertTrue(indexCount == 1);
    [WCTDatabase globalTraceDatabaseOperation:nil];
}

- (void)test_global_trace_busy
{
    __block uint64_t testTid = 0;
    [WCTDatabase
    globalTraceBusy:^(WCTTag tag, NSString* path, uint64_t tid, NSString* sql) {
        XCTAssertTrue(tag == self.database.tag);
        XCTAssertTrue([path isEqualToString:self.database.path]);
        XCTAssertTrue([sql isEqualToString:@"INSERT INTO testTable(identifier, content) VALUES(?1, ?2)"]);
        testTid = tid;
    }
        withTimeOut:0.1];

    XCTAssertTrue([self createTable]);

    __block uint64_t dispatchTid = 0;
    NSArray* objects = [Random.shared testCaseObjectsWithCount:100000 startingFromIdentifier:0];
    [self.dispatch async:^{
        pthread_threadid_np(nullptr, &dispatchTid);
        XCTAssertTrue([self.table insertObjects:objects]);
    }];
    usleep(10000);
    XCTAssertTrue([self.table insertObject:[Random.shared autoIncrementTestCaseObject]]);
    [self.dispatch waitUntilDone];
    XCTAssertTrue(testTid != 0 && testTid == dispatchTid);
    [WCTDatabase globalTraceBusy:nil withTimeOut:0];
}

@end
