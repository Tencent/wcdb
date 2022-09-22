//
// Created by qiuwenchen on 2022/8/10.
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

#import "CPPDatabaseTestCase.h"
#import "NSObject+TestCase.h"
#import "Random+WCDB.h"
#import "Random.h"
#import "TestCaseAssertion.h"
#import "TestCaseLog.h"
#import "TestCaseResult.h"

@implementation CPPDatabaseTestCase {
    WCDB::Database *_database;
    NSString *_path;
}

- (void)setUp
{
    [super setUp];

    self.expectSQLsInAllThreads = NO;
    self.expectMode = DatabaseTestCaseExpectAllSQLs;
}

- (void)tearDown
{
    if (_database != nullptr) {
        _database->close();
        delete _database;
    }
    [super tearDown];
}

#pragma mark - Path
- (void)setPath:(NSString *)path
{
    @synchronized(self) {
        _path = path;
        if (_database != nullptr) {
            _database->close();
            delete _database;
        }
        _database = new WCDB::Database(path.UTF8String);
    }
}

- (NSString *)path
{
    @synchronized(self) {
        if (_path == nil) {
            _path = [self.directory stringByAppendingPathComponent:@"testDatabase"];
        }
        return _path;
    }
}

#pragma mark - Database
- (WCDB::Database *)database
{
    @synchronized(self) {
        if (_database == nullptr) {
            _database = new WCDB::Database(self.path.UTF8String);
            _database->setTag(Random.shared.tag);
        }
        return _database;
    }
}

- (NSString *)walPath
{
    return [self.path stringByAppendingString:@"-wal"];
}

- (NSString *)firstMaterialPath
{
    return [self.path stringByAppendingString:@"-first.material"];
}

- (NSString *)lastMaterialPath
{
    return [self.path stringByAppendingString:@"-last.material"];
}

- (NSString *)factoryPath
{
    return [self.path stringByAppendingString:@".factory"];
}

- (NSString *)journalPath
{
    return [self.path stringByAppendingString:@"-journal"];
}

- (NSString *)shmPath
{
    return [self.path stringByAppendingString:@"-shm"];
}

- (NSString *)factoryRestorePath
{
    return [self.factoryPath stringByAppendingPathComponent:@"restore"];
}

- (int)headerSize
{
    return 100;
}

- (int)pageSize
{
    return 4096;
}

- (int)walHeaderSize
{
    return 32;
}

- (int)walFrameHeaderSize
{
    return 24;
}

- (int)walFrameSize
{
    return self.walFrameHeaderSize + self.pageSize;
}

#pragma mark - SQL
// For debugging only
+ (void)enableSQLTrace
{
    WCDB::Database::globalTraceSQL([](const WCDB::UnsafeStringView &path, const WCDB::UnsafeStringView &sql, const void *handle) {
        WCDB_UNUSED(path);
        WCDB_UNUSED(handle);
        NSThread *currentThread = [NSThread currentThread];
        NSString *threadName = currentThread.name;
        if (threadName.length == 0) {
            threadName = [NSString stringWithFormat:@"%p", currentThread];
        }
        TestCaseLog(@"%@ Thread %@: %s", currentThread.isMainThread ? @"*" : @"-", threadName, sql.data());
    });
}

+ (void)disableSQLTrace
{
    WCDB::Database::globalTraceSQL(nullptr);
}

+ (void)enablePerformanceTrace
{
    WCDB::Database::globalTracePerformance([](const WCDB::UnsafeStringView &path, const WCDB::UnsafeStringView &sql, double cost, const void *handle) {
        WCDB_UNUSED(path);
        WCDB_UNUSED(handle);
        NSThread *currentThread = [NSThread currentThread];
        NSString *threadName = currentThread.name;
        if (threadName.length == 0) {
            threadName = [NSString stringWithFormat:@"%p", currentThread];
        }
        NSString *description = [NSString stringWithFormat:@"%s", sql.data()];
        TestCaseLog(@"%@ Thread %@: %@ %.2f", currentThread.isMainThread ? @"*" : @"-", threadName, description, cost);
    });
}

+ (void)disablePerformanceTrace
{
    WCDB::Database::globalTracePerformance(nullptr);
}

#pragma mark - Test
- (void)doTestSQLs:(NSArray<NSString *> *)testSQLs inOperation:(BOOL (^)())block
{
    TestCaseAssertTrue(testSQLs != nil);
    TestCaseAssertTrue(block != nil);
    TestCaseAssertTrue([testSQLs isKindOfClass:NSArray.class]);
    do {
        TestCaseResult *trace = [TestCaseResult no];
        NSMutableArray<NSString *> *expectedSQLs = [NSMutableArray arrayWithArray:testSQLs];
        NSThread *tracedThread = [NSThread currentThread];
        __weak CPPDatabaseTestCase *weakSelf = self;
        _database->traceSQL([weakSelf, tracedThread, expectedSQLs, trace](const WCDB::UnsafeStringView &, const WCDB::UnsafeStringView &sql, const void *) {
            CPPDatabaseTestCase *strongSelf = weakSelf;
            if (!strongSelf.expectSQLsInAllThreads && tracedThread != [NSThread currentThread]) {
                // skip other thread sqls due to the setting
                return;
            }
            if (trace.isNO) {
                return;
            }
            @synchronized(expectedSQLs) {
                [strongSelf doTestSQLAsExpected:expectedSQLs sql:sql];
            }
        });
        if (self.expectMode != DatabaseTestCaseExpectSomeSQLs) {
            if (!_database->canOpen()) {
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
    _database->traceSQL(nullptr);
}

- (void)doTestSQLAsExpected:(NSMutableArray<NSString *> *)expectedSQLs sql:(const WCDB::UnsafeStringView &)cppSql
{
    NSString *sql = [NSString stringWithUTF8String:cppSql.data()];
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

- (unsigned long)getNumberOfPages
{
    unsigned long result = 0;
    if ([[NSFileManager defaultManager] fileExistsAtPath:self.path]) {
        NSError *error;
        size_t size = (size_t) ((NSNumber *) [[NSFileManager defaultManager] attributesOfItemAtPath:self.path error:&error][NSFileSize]).unsignedLongLongValue;
        if (error == nil) {
            int numberOfPages = (int) (size / self.pageSize);
            result = numberOfPages > 0 ? numberOfPages : 0;
        } else {
            TestCaseLog(@"%@", error);
        }
    } else {
        result = 0;
    }
    return result;
}

- (unsigned long)getNumberOfWalFrames
{
    unsigned long result = 0;
    NSError *error;
    if ([[NSFileManager defaultManager] fileExistsAtPath:self.walPath]) {
        size_t size = (size_t) ((NSNumber *) [[NSFileManager defaultManager] attributesOfItemAtPath:self.walPath error:&error][NSFileSize]).unsignedLongLongValue;
        if (error == nil) {
            if (size == 0) {
                result = 0;
            } else if (size > self.walHeaderSize) {
                if ((size - self.walHeaderSize) % self.walFrameSize == 0) {
                    result = (size - self.walHeaderSize) / self.walFrameSize;
                }
            }
        } else {
            TestCaseLog(@"%@", error);
        }
    } else {
        result = 0;
    }
    return result;
}

@end
