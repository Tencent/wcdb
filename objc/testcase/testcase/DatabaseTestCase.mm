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
#import <TestCase/Random+WCDB.h>
#import <TestCase/Random.h>
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
    NSString* _factoryPath;
    NSString* _firstMaterialPath;
    NSString* _lastMaterialPath;
    NSArray<NSString*>* _paths;
    ReusableFactory* _factory;
}

- (void)setUp
{
    [super setUp];

    self.expectSQLsInAllThreads = NO;
    self.expectFirstFewSQLsOnly = NO;
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

#pragma mark - Path
- (void)setPath:(NSString*)path
{
    _path = path;
    _walPath = nil;
    _factoryPath = nil;
    _firstMaterialPath = nil;
    _lastMaterialPath = nil;
    _paths = nil;
    _database = nil;
}

- (NSString*)path
{
    if (!_path) {
        _path = [self.directory stringByAppendingPathComponent:@"testDatabase"];
    }
    return _path;
}

- (NSString*)walPath
{
    if (!_walPath) {
        _walPath = [self.path stringByAppendingString:@"-wal"];
    }
    return _walPath;
}

- (NSString*)firstMaterialPath
{
    if (!_firstMaterialPath) {
        _firstMaterialPath = [self.path stringByAppendingString:@"-first.material"];
    }
    return _firstMaterialPath;
}

- (NSString*)lastMaterialPath
{
    if (!_lastMaterialPath) {
        _lastMaterialPath = [self.path stringByAppendingString:@"-last.material"];
    }
    return _lastMaterialPath;
}

- (NSString*)factoryPath
{
    if (!_factoryPath) {
        _factoryPath = [self.path stringByAppendingString:@".factory"];
    }
    return _factoryPath;
}

- (NSArray<NSString*>*)paths
{
    if (!_paths) {
        _paths = @[
            self.path,
            self.walPath,
            self.firstMaterialPath,
            self.lastMaterialPath,
            self.factoryPath,
            [self.path stringByAppendingString:@"-journal"],
            [self.path stringByAppendingString:@"-shm"],
        ];
    }
    return _paths;
}

#pragma mark - Database
- (WCTDatabase*)database
{
    if (!_database) {
        _database = [[WCTDatabase alloc] initWithPath:self.path];
        _database.tag = self.random.tag;
    }
    return _database;
}

#pragma mark - File
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

- (int)getWalFrameCount
{
    NSInteger walSize = [[NSFileManager defaultManager] getFileSizeIfExists:self.walPath];
    if (walSize < self.walHeaderSize) {
        return 0;
    }
    return (int) ((walSize - self.walHeaderSize) / (self.walFrameHeaderSize + self.pageSize));
}

#pragma mark - Factory
- (ReusableFactory*)factory
{
    if (!_factory) {
        _factory = [[ReusableFactory alloc] initWithDirectory:self.class.cacheRoot];
        _factory.delegate = self;
        [self log:@"cache at %@", self.class.cacheRoot];
    }
    return _factory;
}

- (BOOL)stepPreparePrototype:(NSString*)path
{
    TestCaseFailure();
}

- (double)getQuality:(NSString*)path
{
    TestCaseFailure();
}

- (NSString*)category
{
    TestCaseFailure();
}

- (NSArray<NSString*>*)additionalPrototypes:(NSString*)prototype
{
    return @[
        [prototype stringByAppendingString:@"-wal"],
        [prototype stringByAppendingString:@"-first.material"],
        [prototype stringByAppendingString:@"-last.material"],
        [prototype stringByAppendingString:@"-factory"],
        [prototype stringByAppendingString:@"-journal"],
        [prototype stringByAppendingString:@"-shm"],
    ];
}

#pragma mark - SQL
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

#pragma mark - Test
- (void)doTestSQLs:(NSArray<NSString*>*)testSQLs inOperation:(BOOL (^)())block
{
    TestCaseAssertTrue(testSQLs != nil);
    TestCaseAssertTrue(block != nil);
    TestCaseAssertTrue([testSQLs isKindOfClass:NSArray.class]);
    do {
        __block BOOL trace = NO;
        NSMutableArray<NSString*>* expectedSQLs = [NSMutableArray arrayWithArray:testSQLs];
        NSThread* tracedThread = [NSThread currentThread];
        [self.database traceSQL:^(NSString* sql) {
            if (!self.expectSQLsInAllThreads && tracedThread != [NSThread currentThread]) {
                // skip other thread sqls due to the setting
                return;
            }
            if (!trace) {
                return;
            }
            NSString* expectedSQL = expectedSQLs.firstObject;
            if ([expectedSQL isEqualToString:sql]) {
                [expectedSQLs removeObjectAtIndex:0];
            } else {
                trace = NO;
                if (expectedSQL == nil) {
                    if (self.expectFirstFewSQLsOnly) {
                        return;
                    }
                    expectedSQL = @"";
                }
                TestCaseAssertStringEqual(sql, expectedSQL);
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
        if (expectedSQLs.count != 0) {
            TestCaseLog(@"Reminding: %@", expectedSQLs);
            TestCaseFailure();
            break;
        }
        trace = NO;
    } while (false);
    [self.database traceSQL:nil];
}

@end
