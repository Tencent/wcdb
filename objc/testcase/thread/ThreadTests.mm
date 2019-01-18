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
#import <WCDB/CoreConst.h>
#import <thread>

@interface ThreadTests : TableTestCase

@property (nonatomic, readonly) dispatch_group_t group;
@property (nonatomic, readonly) dispatch_queue_t queue;

@property (nonatomic, readonly) NSTimeInterval checkpointDelayForCritical;
@property (nonatomic, readonly) NSTimeInterval checkpointDelayForNonCritical;
@property (nonatomic, readonly) NSTimeInterval checkpointDelayForRetryingAfterFailure;

@property (nonatomic, readonly) int checkpointFramesThresholdForTruncating;
@property (nonatomic, readonly) int checkpointFramesThresholdForCritical;

@property (nonatomic, readonly) NSTimeInterval delayForTolerance;

@property (nonatomic, readonly) int maxConcurrency;

@end

@implementation ThreadTests {
    dispatch_group_t _group;
    dispatch_queue_t _queue;
}

- (void)setUp
{
    [super setUp];
    _group = dispatch_group_create();
    _queue = dispatch_queue_create([[NSBundle mainBundle].bundleIdentifier stringByAppendingFormat:@".%@.%@", self.className, self.testName].UTF8String, DISPATCH_QUEUE_CONCURRENT);
}

- (NSTimeInterval)checkpointDelayForCritical
{
    return WCDB::CheckpointConfigDelayForCritical;
}

- (NSTimeInterval)checkpointDelayForNonCritical
{
    return WCDB::CheckpointConfigDelayForNonCritical;
}

- (NSTimeInterval)checkpointDelayForRetryingAfterFailure
{
    return WCDB::CheckpointQueueDelayForRetryingAfterFailure;
}

- (int)checkpointFramesThresholdForTruncating
{
    return WCDB::CheckpointQueueFramesThresholdForTruncating;
}

- (int)checkpointFramesThresholdForCritical
{
    return WCDB::CheckpointConfigFramesThresholdForCritical;
}

- (NSTimeInterval)delayForTolerance
{
    return 2;
}

- (int)maxConcurrency
{
    return std::max<int>(WCDB::HandlePooMaxAllowedNumberOfHandles, std::thread::hardware_concurrency());
}

- (void)test_feature_read_concurrency
{
    // trigger table initialization
    TestCaseAssertTrue([self createTable]);

    __block NSDate* read1Begin;
    __block NSDate* read2Begin;
    __block NSDate* read1End;
    __block NSDate* read2End;
    dispatch_group_async(self.group, self.queue, ^{
        // Acquire read transaction. Note that it's not recommended in production environment.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginDeferred()]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementSelect().select(1)]);
        read1Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        read1End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    });
    dispatch_group_async(self.group, self.queue, ^{
        // Acquire read transaction. Note that it's not recommended in production environment.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginDeferred()]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementSelect().select(1)]);
        read2Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        read2End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    });
    dispatch_group_wait(self.group, DISPATCH_TIME_FOREVER);

    // Contains intersection
    TestCaseAssertTrue([read1End compare:read2Begin] == NSOrderedDescending
                       && [read2End compare:read1Begin] == NSOrderedDescending);
}

- (void)test_feature_read_and_write_concurrency
{
    // trigger table initialization
    TestCaseAssertTrue([self createTable]);

    __block NSDate* readBegin;
    __block NSDate* writeBegin;
    __block NSDate* readEnd;
    __block NSDate* writeEnd;
    dispatch_group_async(self.group, self.queue, ^{
        // Acquire read transaction. Note that it's not recommended in production environment.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginDeferred()]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementSelect().select(1)]);
        readBegin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        readEnd = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    });
    dispatch_group_async(self.group, self.queue, ^{
        // Acquire write transaction.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        writeBegin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        writeEnd = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    });
    dispatch_group_wait(self.group, DISPATCH_TIME_FOREVER);

    // Contains intersection
    TestCaseAssertTrue([readEnd compare:writeBegin] == NSOrderedDescending
                       && [writeEnd compare:readBegin] == NSOrderedDescending);
}

- (void)test_feature_write_serial
{
    // trigger table initialization
    TestCaseAssertTrue([self createTable]);

    __block NSDate* write1Begin;
    __block NSDate* write2Begin;
    __block NSDate* write1End;
    __block NSDate* write2End;
    dispatch_group_async(self.group, self.queue, ^{
        // Acquire write transaction.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        write1Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        write1End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    });
    dispatch_group_async(self.group, self.queue, ^{
        // Acquire write transaction.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        write2Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        write2End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    });
    dispatch_group_wait(self.group, DISPATCH_TIME_FOREVER);

    // No intersection
    TestCaseAssertTrue([write1Begin compare:write2End] == NSOrderedDescending
                       || [write2Begin compare:write1End] == NSOrderedDescending);
}

- (void)test_feature_subthread_checkpoint_when_idled
{
    // trigger subthread checkpoint
    TestCaseAssertTrue([self createTable]);

    self.expectSQLsInAllThreads = YES;
    [self doTestSQLs:@[ @"PRAGMA main.wal_checkpoint('PASSIVE')" ]
         inOperation:^BOOL {
             [NSThread sleepForTimeInterval:self.checkpointDelayForNonCritical + self.delayForTolerance];
             return YES;
         }];
}

- (void)test_feature_subthread_checkpoint_when_meet_frames_threshold
{
    TestCaseAssertTrue([self createTable]);

    TestCaseObject* object = [[TestCaseObject alloc] init];
    object.isAutoIncrement = YES;
    object.content = self.random.string;

    while ([self getWalFrameCount] < self.checkpointFramesThresholdForCritical) {
        TestCaseAssertTrue([self.table insertObject:object]);
    }

    self.expectSQLsInAllThreads = YES;
    [self doTestSQLs:@[ @"PRAGMA main.wal_checkpoint('PASSIVE')" ]
         inOperation:^BOOL {
             [NSThread sleepForTimeInterval:self.checkpointDelayForCritical + self.delayForTolerance];
             return YES;
         }];
}

- (void)test_feature_subthread_checkpoint_when_meet_truncate_threshold
{
    TestCaseAssertTrue([self createTable]);

    TestCaseObject* object = [[TestCaseObject alloc] init];
    object.isAutoIncrement = YES;
    object.content = self.random.string;

    while ([self getWalFrameCount] < self.checkpointFramesThresholdForTruncating) {
        TestCaseAssertTrue([self.table insertObject:object]);
    }

    self.expectSQLsInAllThreads = YES;
    [self doTestSQLs:@[ @"PRAGMA main.wal_checkpoint('TRUNCATE')" ]
         inOperation:^BOOL {
             [NSThread sleepForTimeInterval:self.checkpointDelayForCritical + self.delayForTolerance];
             return YES;
         }];
}

- (void)test_feature_retry_subthread_checkpoint_when_failed
{
    TestCaseAssertTrue([self createTable]);

    TestCaseObject* object = [[TestCaseObject alloc] init];
    object.isAutoIncrement = YES;
    object.content = self.random.string;

    TestCaseAssertTrue([self.table insertObject:object]);
    TestCaseAssertTrue([self getWalFrameCount] < self.checkpointFramesThresholdForCritical)

    self.expectSQLsInAllThreads
    = YES;

    [self doTestSQLs:@[ @"PRAGMA main.wal_checkpoint('PASSIVE')", @"PRAGMA main.wal_checkpoint('PASSIVE')" ]
         inOperation:^BOOL {
             [self.database blockade];
             [WCTDatabase disableSQLiteWrite];
             [self.database unblockade];

             [NSThread sleepForTimeInterval:self.checkpointDelayForNonCritical + self.delayForTolerance];

             [self.database blockade];
             [WCTDatabase enableSQLiteWrite];
             [self.database unblockade];

             [NSThread sleepForTimeInterval:self.checkpointDelayForRetryingAfterFailure + self.delayForTolerance];
             return YES;
         }];
}

- (void)test_feature_stop_subthread_checkpoint_when_manual_checkpoint
{
    // trigger subthread checkpoint
    TestCaseAssertTrue([self createTable]);

    self.expectSQLsInAllThreads = YES;
    [self doTestSQLs:@[ @"PRAGMA main.wal_checkpoint('PASSIVE')" ]
         inOperation:^BOOL {
             if (![self.database execute:WCDB::StatementPragma().pragma(WCDB::Pragma::walCheckpoint()).with(@"PASSIVE")]) {
                 return NO;
             }
             [NSThread sleepForTimeInterval:self.checkpointDelayForNonCritical + self.delayForTolerance];
             return YES;
         }];
}

- (void)test_feature_threaded_handle
{
    __block int handleCount = 0;
    [self.database
           setConfig:^BOOL(WCTHandle*) {
               ++handleCount;
               return YES;
           }
    withUninvocation:nil
             forName:self.testName];
    {
        WCTHandle* handle1 = [self.database getHandle];
        TestCaseAssertTrue([handle1 validate]);
        WCTHandle* handle2 = [self.database getHandle];
        TestCaseAssertTrue([handle2 validate]);
        WCTHandle* handle3 = [self.database getHandle];
        TestCaseAssertTrue([handle3 validate]);

        [handle1 invalidate];
        [handle2 invalidate];
        [handle3 invalidate];
    }
    TestCaseAssertEqual(handleCount, 3);

    BOOL result = [self.database runTransaction:^BOOL(WCTHandle* _Nonnull) {
        TestCaseAssertTrue([self.database isInTransaction]);
        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertTrue([handle isInTransaction]);
        return YES;
    }];
    TestCaseAssertTrue(result);
}

- (void)test_feature_max_concurrency
{
    NSCondition* condition = [[NSCondition alloc] init];
    __block int currentConcurrency = 0;
    for (int i = 0; i < self.maxConcurrency; ++i) {
        dispatch_group_async(self.group, self.queue, ^{
            WCTHandle* handle = nil;
            @synchronized(self) {
                handle = [self.database getHandle];
            }
            TestCaseAssertTrue([handle validate]);
            [condition lock];
            ++currentConcurrency;
            [condition wait];
            [condition unlock];
            [handle invalidate];
        });
    }

    do {
        BOOL prepared = NO;
        [condition lock];
        prepared = currentConcurrency == self.maxConcurrency;
        [condition unlock];
        if (prepared) {
            break;
        }
    } while (YES);

    TestCaseAssertFalse([[self.database getHandle] validate]);
    [condition broadcast];
    dispatch_group_wait(self.group, DISPATCH_TIME_FOREVER);
}

#warning TODO - add tests for busy retry

@end
