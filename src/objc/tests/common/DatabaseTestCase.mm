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

#import "DatabaseTestCase.h"
#import "NSObject+TestCase.h"
#import "Random+WCDB.h"
#import "Random.h"
#import "TestCaseAssertion.h"
#import "TestCaseLog.h"
#import "TestCaseResult.h"
#if TEST_WCDB_OBJC
#import <WCDBOBjc/WCTDatabase+Test.h>
#elif TEST_WCDB_CPP
#import <WCDBCpp/WCTDatabase+Test.h>
#else
#import <WCDB/WCTDatabase+Test.h>
#endif

@implementation DatabaseTestCase {
    WCTDatabase* _database;
    NSString* _path;
}

- (void)setUp
{
    [super setUp];

    self.expectSQLsInAllThreads = NO;
    self.expectMode = DatabaseTestCaseExpectAllSQLs;
}

- (void)tearDown
{
    [_database close];
    _database = nil;
    [super tearDown];
}

#pragma mark - Path
- (void)setPath:(NSString*)path
{
    @synchronized(self) {
        _path = path;
        _database = nil;
    }
}

- (NSString*)path
{
    @synchronized(self) {
        if (_path == nil) {
            _path = [self.directory stringByAppendingPathComponent:@"testDatabase"];
        }
        return _path;
    }
}

#pragma mark - Database
- (WCTDatabase*)database
{
    @synchronized(self) {
        if (_database == nil) {
            _database = [[WCTDatabase alloc] initWithPath:self.path];
            _database.tag = Random.shared.tag;
        }
        return _database;
    }
}

#pragma mark - SQL
// For debugging only
+ (void)enableSQLTrace
{
    [WCTDatabase globalTraceSQL:^(NSString*, UInt64, NSString* sql) {
        NSThread* currentThread = [NSThread currentThread];
        NSString* threadName = currentThread.name;
        if (threadName.length == 0) {
            threadName = [NSString stringWithFormat:@"%p", currentThread];
        }
        TestCaseLog(@"%@ Thread %@: %@", currentThread.isMainThread ? @"*" : @"-", threadName, sql);
    }];
}

+ (void)disableSQLTrace
{
    [WCTDatabase globalTraceSQL:nil];
}

+ (void)enablePerformanceTrace
{
    [WCTDatabase globalTracePerformance:^(NSString*, UInt64, NSString* sql, double cost) {
        NSThread* currentThread = [NSThread currentThread];
        NSString* threadName = currentThread.name;
        if (threadName.length == 0) {
            threadName = [NSString stringWithFormat:@"%p", currentThread];
        }
        NSString* description = [NSString stringWithFormat:@"%@", sql];
        TestCaseLog(@"%@ Thread %@: %@ %.2f", currentThread.isMainThread ? @"*" : @"-", threadName, description, cost);
    }];
}

+ (void)disablePerformanceTrace
{
    [WCTDatabase globalTracePerformance:nil];
}

#pragma mark - Test
- (void)doTestSQLs:(NSArray<NSString*>*)testSQLs inOperation:(BOOL (^)())block
{
    TestCaseAssertTrue(testSQLs != nil);
    TestCaseAssertTrue(block != nil);
    TestCaseAssertTrue([testSQLs isKindOfClass:NSArray.class]);
    do {
        TestCaseResult* trace = [TestCaseResult no];
        NSMutableArray<NSString*>* expectedSQLs = [NSMutableArray arrayWithArray:testSQLs];
        NSThread* tracedThread = [NSThread currentThread];
        [self.database traceSQL:^(NSString*, UInt64, NSString* sql) {
            if (!self.expectSQLsInAllThreads && tracedThread != [NSThread currentThread]) {
                // skip other thread sqls due to the setting
                return;
            }
            if (trace.isNO) {
                return;
            }
            @synchronized(expectedSQLs) {
                [self doTestSQLAsExpected:expectedSQLs sql:sql];
            }
        }];
        if (self.expectMode != DatabaseTestCaseExpectSomeSQLs) {
            if (![self.database canOpen]) {
                TestCaseFailure();
                break;
            }
        }

        [trace makeYES];
        @autoreleasepool {
            if (!block()) {
                TestCaseFailure();
                break;
            }
        }
        @synchronized(expectedSQLs) {
            if (expectedSQLs.count != 0) {
                TestCaseLog(@"Reminding: %@", expectedSQLs);
                TestCaseFailure();
                break;
            }
        }
        [trace makeNO];
    } while (false);
    [self.database traceSQL:nil];
}

- (void)doTestSQLAsExpected:(NSMutableArray<NSString*>*)expectedSQLs sql:(NSString*)sql
{
    switch (self.expectMode) {
    case DatabaseTestCaseExpectAllSQLs:
        if ([expectedSQLs.firstObject isEqualToString:sql]) {
            [expectedSQLs removeObjectAtIndex:0];
        } else {
            TestCaseAssertStringEqual(expectedSQLs.firstObject, sql);
        }
        break;
    case DatabaseTestCaseExpectFirstFewSQLs:
        if ([expectedSQLs.firstObject isEqualToString:sql]) {
            [expectedSQLs removeObjectAtIndex:0];
        } else if (expectedSQLs.count != 0) {
            TestCaseAssertStringEqual(expectedSQLs.firstObject, sql);
        }
        break;
    case DatabaseTestCaseExpectSomeSQLs:
        for (NSUInteger i = 0; i < expectedSQLs.count; ++i) {
            if ([expectedSQLs[i] isEqualToString:sql]) {
                [expectedSQLs removeObjectAtIndex:i];
                break;
            }
        }
        break;
    }
}

- (NSTimeInterval)delayForTolerance
{
    return 2;
}

@end
