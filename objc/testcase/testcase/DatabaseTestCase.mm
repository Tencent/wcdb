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

#import <TestCase/DatabaseTestCase.h>
#import <TestCase/NSObject+TestCase.h>
#import <TestCase/TestCaseAssertion.h>
#import <TestCase/TestCaseLog.h>

@implementation DatabaseTestCase {
    int _headerSize;
    int _walHeaderSize;
    int _walFrameHeaderSize;
    int _pageSize;
    int _walFrameSize;
    WCTDatabase* _database;
    NSString* _path;
    NSString* _walPath;
}

- (void)setUp
{
    [super setUp];

    WCTTag tag;
    do {
        tag = self.random.int32;
    } while (tag == 0);
    self.database.tag = tag;
}

- (void)tearDown
{
    if (_database.isValidated) {
        [_database close];
        [_database invalidate];
    }
    _database = nil;
    [super tearDown];
}

#pragma mark - File
- (NSString*)path
{
    if (!_path) {
        _path = [self.directory stringByAppendingPathComponent:@"testDatabase"];
    }
    return _path;
}

- (int)headerSize
{
    return 100;
}

- (int)pageSize
{
    return 4096;
}

#pragma mark - Database
- (WCTDatabase*)database
{
    if (!_database) {
        _database = [[WCTDatabase alloc] initWithPath:self.path];
    }
    return _database;
}

#pragma mark - WAL File
- (NSString*)walPath
{
    if (!_walPath) {
        _walPath = [self.path stringByAppendingString:@"-wal"];
    }
    return self.walPath;
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

- (int)getWalFrameCount
{
    NSInteger walSize = [[NSFileManager defaultManager] getFileSize:self.walPath];
    if (walSize < self.walHeaderSize) {
        return 0;
    }
    return (int) ((walSize - self.walHeaderSize) / (self.walFrameHeaderSize + self.pageSize));
}

+ (void)enableSQLTrace
{
    [WCTDatabase globalTraceSQL:^(NSString* sql) {
        NSThread* currentThread = [NSThread currentThread];
        if (currentThread.isMainThread) {
            static dispatch_once_t onceToken;
            dispatch_once(&onceToken, ^{
                pthread_setname_np("com.Tencent.WCDB.Queue.Main");
            });
        }
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

- (BOOL)checkAllSQLsInAllThreads:(NSArray<NSString*>*)expectedSQLs
           asExpectedInOperation:(BOOL (^)())block
{
    return [self checkAllSQLs:expectedSQLs asExpectedInOperation:block threaded:NO];
}

- (BOOL)checkAllSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedInOperation:(BOOL (^)())block
{
    return [self checkAllSQLs:expectedSQLs asExpectedInOperation:block threaded:YES];
}

- (BOOL)checkAllSQLs:(NSArray<NSString*>*)expectedSQLs
asExpectedInOperation:(BOOL (^)())block
             threaded:(BOOL)threaded
{
    BOOL result = NO;
    do {
        __block BOOL trace = NO;
        if (![expectedSQLs isKindOfClass:NSArray.class]
            || expectedSQLs.count == 0
            || block == nil) {
            TestCaseFailure();
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        NSThread* tracedThread = [NSThread currentThread];
        [self.database traceSQL:^(NSString* sql) {
            if (threaded && tracedThread != [NSThread currentThread]) {
                return;
            }
            if (!trace) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            } else {
                TestCaseLog(@"Failed: %@", TestCaseHint(sql, sqls.firstObject));
                trace = NO;
                TestCaseFailure();
            }
        }];
        if (![self.database canOpen]) {
            TestCaseFailure();
            break;
        }

        trace = YES;
        @autoreleasepool {
            if (!block()) {
                TestCaseFailure();
                break;
            }
        }
        if (sqls.count != 0) {
            TestCaseLog(@"Reminding: %@", sqls);
            TestCaseFailure();
            break;
        }
        trace = NO;
        result = YES;
    } while (false);
    [self.database traceSQL:nil];
    return result;
}

- (BOOL)checkBeginningSQLs:(NSArray<NSString*>*)expectedSQLs
     asExpectedInOperation:(BOOL (^)())block
{
    BOOL result = NO;
    do {
        __block BOOL trace = NO;
        if (![expectedSQLs isKindOfClass:NSArray.class]
            || expectedSQLs.count == 0
            || block == nil) {
            TestCaseFailure();
            break;
        }
        NSMutableArray<NSString*>* sqls = [NSMutableArray arrayWithArray:expectedSQLs];
        [self.database traceSQL:^(NSString* sql) {
            if (!trace || sqls.count == 0) {
                return;
            }
            //Test sql and expect exactly the same, including order and count
            if ([sqls.firstObject isEqualToString:sql]) {
                [sqls removeObjectAtIndex:0];
            }
        }];
        if (![self.database canOpen]) {
            TestCaseFailure();
            break;
        }

        trace = YES;
        @autoreleasepool {
            if (!block()) {
                TestCaseFailure();
                break;
            }
        }
        if (sqls.count != 0) {
            TestCaseLog(@"Reminding: %@", sqls);
            TestCaseFailure();
            break;
        }
        trace = NO;
        result = YES;
    } while (false);
    [self.database traceSQL:nil];
    return result;
}

@end
