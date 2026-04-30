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

#import "CoreConst.h"
#import "TestCase.h"
#include "ThreadLocal.hpp"
#include <mach/mach.h>
#include <pthread.h>

// ============================================================
// Reproduce the original crash scenario in-process.
//
// On a worker thread, we exploit thread_local destruction order:
//   - A "hook" thread_local is initialized FIRST → destroyed LAST
//   - threadedStorage()'s thread_local is initialized SECOND → destroyed FIRST
// When the thread exits, the hook's destructor accesses ThreadLocal
// after the underlying storage is already destroyed — exactly the
// original crash path (SharedLockGuard → lockShared → getOrCreate).
//
// With the old unique_ptr code, this crashes (SIGSEGV).
// With the fix (ThreadedStorage + nullptr guard), this succeeds.
// ============================================================

namespace ThreadLocalDestructionTest {

static std::atomic<bool> s_hookSucceeded{false};
static WCDB::ThreadLocal<int> s_tl;

struct DestructionHook {
    bool active = false;
    ~DestructionHook()
    {
        if (active) {
            s_tl.getOrCreate() = 42;
            s_hookSucceeded.store(true);
        }
    }
};

static void* threadEntry(void*)
{
    // Hook initialized FIRST → its destructor registered first → destroyed LAST
    thread_local DestructionHook hook;
    hook.active = true;

    // threadedStorage() initialized SECOND → registered second → destroyed FIRST
    s_tl.getOrCreate() = 10;

    return nullptr;
}

} // namespace ThreadLocalDestructionTest

// ============================================================
// Memory leak stress test helpers
// ============================================================
namespace ThreadLocalStressTest {

static WCDB::ThreadLocal<int> s_tls[50];

static void* threadEntry(void*)
{
    for (int j = 0; j < 50; j++) {
        s_tls[j].getOrCreate() = j;
    }
    return nullptr;
}

} // namespace ThreadLocalStressTest

static size_t getPhysicalFootprint()
{
    task_vm_info_data_t vmInfo;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    kern_return_t kr = task_info(mach_task_self(), TASK_VM_INFO,
                                (task_info_t) &vmInfo, &count);
    return kr == KERN_SUCCESS ? vmInfo.phys_footprint : 0;
}

@interface ThreadTests : TableTestCase

@end

@implementation ThreadTests

- (void)test_feature_read_concurrency
{
    // trigger table initialization
    TestCaseAssertTrue([self createTable]);

    __block NSDate* read1Begin;
    __block NSDate* read2Begin;
    __block NSDate* read1End;
    __block NSDate* read2End;
    [self.dispatch async:^{
        // Acquire read transaction. Note that it's not recommended in production environment.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginDeferred()]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementSelect().select(1)]);
        read1Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        read1End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    }];
    [self.dispatch async:^{
        // Acquire read transaction. Note that it's not recommended in production environment.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginDeferred()]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementSelect().select(1)]);
        read2Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        read2End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    }];
    [self.dispatch waitUntilDone];

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
    [self.dispatch async:^{
        // Acquire read transaction. Note that it's not recommended in production environment.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginDeferred()]);
        TestCaseAssertTrue([self.database execute:WCDB::StatementSelect().select(1)]);
        readBegin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        readEnd = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    }];
    [self.dispatch async:^{
        // Acquire write transaction.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        writeBegin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        writeEnd = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    }];
    [self.dispatch waitUntilDone];

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
    [self.dispatch async:^{
        // Acquire write transaction.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        write1Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        write1End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    }];
    [self.dispatch async:^{
        // Acquire write transaction.
        TestCaseAssertTrue([self.database execute:WCDB::StatementBegin().beginImmediate()]);
        write2Begin = [NSDate date];
        [NSThread sleepForTimeInterval:self.delayForTolerance];
        write2End = [NSDate date];
        TestCaseAssertTrue([self.database execute:WCDB::StatementRollback().rollback()]);
    }];
    [self.dispatch waitUntilDone];

    // No intersection
    TestCaseAssertTrue([write1Begin compare:write2End] == NSOrderedDescending
                       || [write2Begin compare:write1End] == NSOrderedDescending);
}

- (void)test_feature_subthread_checkpoint
{
    // trigger subthread checkpoint
    TestCaseAssertTrue([self createTable]);

    TestCaseAssertOptionalFalse([self.database isAlreadyCheckpointed]);

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForCheckpoint + self.delayForTolerance];

    TestCaseAssertOptionalFalse([self.database isAlreadyTruncateCheckpointed]);
    TestCaseAssertOptionalTrue([self.database isAlreadyCheckpointed]);
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
    TestCaseAssertEqual(handleCount, 1);
}

- (void)test_feature_transactioned_handle
{
    BOOL result = [self.database runTransaction:^BOOL(WCTHandle* _Nonnull) {
        TestCaseAssertTrue([self.database isInTransaction]);
        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertTrue([handle isInTransaction]);
        return YES;
    }];
    TestCaseAssertTrue(result);
}

- (void)test_feature_threaded_transactioned_handle
{
    TestCaseAssertTrue([self.database beginTransaction]);

    TestCaseResult* began = [TestCaseResult no];
    TestCaseResult* tested = [TestCaseResult no];
    [self.dispatch async:^{
        WCTHandle* handle = [self.database getHandle];
        TestCaseAssertFalse([handle isInTransaction]);
        // it will not reuse transactioned handle.
        TestCaseAssertTrue([handle validate]);
        [began makeYES];
        while ([tested isNO]);
        [handle invalidate];
    }];

    while ([began isNO]);

    // it will reuse transactioned handle.
    TestCaseAssertTrue([self.database isInTransaction]);
    [tested makeYES];
    [self.database rollbackTransaction];
}

- (void)test_feature_max_concurrency
{
    [self.database enableAutoCheckpoint:NO];

    NSCondition* condition = [[NSCondition alloc] init];
    TestCaseCounter* counter = [TestCaseCounter value:0];
    for (int i = 0; i < WCDB::HandlePoolMaxAllowedNumberOfHandles; ++i) {
        [self.dispatch async:^{
            WCTHandle* handle = nil;
            @synchronized(self) {
                handle = [self.database getHandle];
            }

            while (counter.value < i) {
            }

            TestCaseAssertTrue([handle validate]);

            [counter increment];
            [condition lock];
            [condition wait];
            [condition unlock];
            [handle invalidate];
            [counter decrement];
        }];
    }

    while (counter.value != WCDB::HandlePoolMaxAllowedNumberOfHandles) {
    }
    [condition broadcast];
    [self.dispatch waitUntilDone];
    TestCaseAssertEqual(counter.value, 0);
}

- (void)test_feature_checkpoint_while_notice_recover_wal
{
    TestCaseAssertTrue([self.database truncateCheckpoint]);

    // trigger subthread checkpoint
    TestCaseAssertTrue([self createTable]);

    [self.database close];

    // trigger notice recover wal
    TestCaseAssertTrue([self.database canOpen]);

    TestCaseAssertOptionalFalse(self.database.isAlreadyCheckpointed);

    [NSThread sleepForTimeInterval:WCDB::OperationQueueTimeIntervalForCheckpoint + self.delayForTolerance];

    TestCaseAssertOptionalTrue(self.database.isAlreadyCheckpointed);
}

- (void)test_feature_busy_retry
{
    TestCaseResult* began = [TestCaseResult no];
    TestCaseResult* rollbacked = [TestCaseResult no];
    TestCaseResult* tested = [TestCaseResult no];
    [self.dispatch async:^{
        TestCaseAssertTrue([self.database beginTransaction]);
        [began makeYES];

        [NSThread sleepForTimeInterval:1];
        TestCaseAssertResultNO(tested);
        [rollbacked makeYES];
        [self.database rollbackTransaction];
    }];

    while ([began isNO]) {
    }

    TestCaseAssertTrue([self.database beginTransaction]);
    TestCaseAssertResultYES(rollbacked);
    [tested makeYES];
    [self.database rollbackTransaction];
    [self.dispatch waitUntilDone];
}

- (void)test_write_with_handle_count_limit
{
    __block int maxHandleCount = 0;
    [WCTDatabase globalTraceDatabaseOperation:^(WCTDatabase*, WCTDatabaseOperation operation, NSDictionary* info) {
        if (operation != WCTDatabaseOperation_OpenHandle) {
            return;
        }
        maxHandleCount = MAX(maxHandleCount, ((NSNumber*) info[WCTDatabaseMonitorInfoKeyHandleCount]).intValue);
    }];
    TestCaseAssertTrue([self createTable]);
    for (int i = 0; i < 40; i++) {
        [self.dispatch async:^{
            switch (i % 4) {
            case 0: {
                NSArray* objects = [[Random shared] testCaseObjectsWithCount:100 startingFromIdentifier:i * 100];
                TestCaseAssertTrue([self.table insertObjects:objects]);
            } break;
            case 1: {
                TestCaseAssertTrue([self.table updateProperty:TestCaseObject.content toValue:@"abc" where:TestCaseObject.identifier > (i - 1) * 100 && TestCaseObject.identifier <= i * 100]);
            } break;
            case 2: {
                TestCaseAssertTrue([self.table deleteObjectsWhere:TestCaseObject.identifier > ((i - 2) * 100 + 50) && TestCaseObject.identifier <= i * 100]);
            } break;
            case 3: {
                TestCaseAssertTrue([self.database runTransaction:^BOOL(WCTHandle*) {
                    return [self.table updateProperty:TestCaseObject.content toValue:@"abc2" where:TestCaseObject.identifier > (i - 1) * 100 && TestCaseObject.identifier <= i * 100];
                }]);
            } break;

            default:
                break;
            }
            usleep(100000);
        }];
    }
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(maxHandleCount <= 4);
    [WCTDatabase globalTraceDatabaseOperation:nil];
}

- (void)test_read_with_handle_count_limit
{
    __block int maxHandleCount = 0;
    [WCTDatabase globalTraceDatabaseOperation:^(WCTDatabase*, WCTDatabaseOperation operation, NSDictionary* info) {
        if (operation != WCTDatabaseOperation_OpenHandle) {
            return;
        }
        maxHandleCount = MAX(maxHandleCount, ((NSNumber*) info[WCTDatabaseMonitorInfoKeyHandleCount]).intValue);
    }];

    TestCaseAssertTrue([self createTable]);
    NSArray* objects = [Random.shared testCaseObjectsWithCount:32000 startingFromIdentifier:0];
    TestCaseAssertTrue([self.table insertObjects:objects]);

    for (int i = 0; i < 320; i++) {
        [self.dispatch async:^{
            switch (i % 5) {
            case 0: {
                NSArray* ret = [self.table getObjectsOffset:i * 100 limit:100];
                TestCaseAssertTrue([ret count] == 100);
            } break;
            case 1: {
                WCTValue* value = [self.database getValueFromStatement:WCDB::StatementSelect().select(TestCaseObject.allProperties.count()).from(self.tableName)];
                TestCaseAssertTrue(value.numberValue.intValue == 32000);
            } break;
            case 2: {
                WCTOneColumn* colums = [self.table getColumnOnResultColumn:TestCaseObject.content limit:100 offset:i * 100];
                TestCaseAssertTrue(colums.count == 100);
            } break;
            case 3: {
                WCTColumnsXRows* rows = [self.table getRowsOnResultColumns:TestCaseObject.allProperties limit:100 offset:i * 100];
                TestCaseAssertTrue(rows.count == 100);
            } break;
            case 4: {
                WCTOneRow* row = [self.table getRowOnResultColumns:TestCaseObject.allProperties orders:TestCaseObject.identifier.asOrder(WCTOrderedDescending)];
                TestCaseAssertTrue(row.count == 2);
            } break;

            default:
                break;
            }

            usleep(100000);
        }];
    }
    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(maxHandleCount > 4 && maxHandleCount <= 32);
    [WCTDatabase globalTraceDatabaseOperation:nil];
}

- (void)test_read_write_with_handle_count_limit
{
    __block int maxHandleCount = 0;
    [WCTDatabase globalTraceDatabaseOperation:^(WCTDatabase*, WCTDatabaseOperation operation, NSDictionary* info) {
        if (operation != WCTDatabaseOperation_OpenHandle) {
            return;
        }
        maxHandleCount = MAX(maxHandleCount, ((NSNumber*) info[WCTDatabaseMonitorInfoKeyHandleCount]).intValue);
    }];

    TestCaseAssertTrue([self createTable]);
    NSArray* objects = [Random.shared testCaseObjectsWithCount:32000 startingFromIdentifier:0];
    TestCaseAssertTrue([self.table insertObjects:objects]);

    for (int i = 0; i < 320; i++) {
        if (i % 8 == 0) {
            [self.dispatch async:^{
                NSArray* newObjects = [[Random shared] testCaseObjectsWithCount:100 startingFromIdentifier:i * 100 + 32000];
                TestCaseAssertTrue([self.table insertObjects:newObjects]);
                usleep(100000);
            }];
        } else {
            [self.dispatch async:^{
                NSArray* ret = [self.table getObjectsOffset:i * 10 limit:1000];
                TestCaseAssertTrue([ret count] == 1000);
                usleep(100000);
            }];
        }
    }

    [self.dispatch waitUntilDone];
    TestCaseAssertTrue(maxHandleCount > 4 && maxHandleCount <= 32);
    [WCTDatabase globalTraceDatabaseOperation:nil];
}

// Reproduce the original crash: a thread_local destructor accesses ThreadLocal
// after threadedStorage is already destroyed — the same code path as the crash:
//   Database::~Database → flowBack → SharedLockGuard → lockShared → getOrCreate
//
// With the old code (unique_ptr), this crashes with SIGSEGV.
// With the fix (ThreadedStorage + nullptr guard), the hook succeeds.
- (void)test_threadLocal_access_after_storage_destruction
{
    ThreadLocalDestructionTest::s_hookSucceeded = false;

    pthread_t thread;
    pthread_create(&thread, nullptr, ThreadLocalDestructionTest::threadEntry, nullptr);
    pthread_join(thread, nullptr);

    TestCaseAssertTrue(ThreadLocalDestructionTest::s_hookSucceeded.load());
}

// Stress test: create & destroy many threads, each touching 50 ThreadLocal<int>.
// Verifies thread_local storage is properly cleaned up on thread exit (no leak).
//
// Per-thread map with 50 entries ≈ 3.5 KB.
// 2000 threads × 3.5 KB ≈ 7 MB if leaked; ~0 if properly cleaned up.
- (void)test_threadLocal_no_memory_leak_under_stress
{
    // Warm up: let the runtime/allocator settle
    for (int i = 0; i < 200; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, ThreadLocalStressTest::threadEntry, nullptr);
        pthread_join(t, nullptr);
    }

    size_t memBefore = getPhysicalFootprint();

    for (int i = 0; i < 2000; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, ThreadLocalStressTest::threadEntry, nullptr);
        pthread_join(t, nullptr);
    }

    size_t memAfter = getPhysicalFootprint();
    size_t growth = memAfter > memBefore ? memAfter - memBefore : 0;
    TestCaseAssertTrue(growth < 2 * 1024 * 1024);
}

- (void)test_multithread_with_error
{
    for (int i = 0; i < 10000; i++) {
        [self.dispatch async:^{
            [self.database execute:WCDB::StatementSelect().select(WCDB::Column::all()).from(@"abc")];
            [self.database runTransaction:^BOOL(WCTHandle* handle) {
                return [handle execute:WCDB::StatementSelect().select(WCDB::Column::all()).from(@"abc")];
            }];
        }];
    }
    [self.dispatch waitUntilDone];
}

@end
