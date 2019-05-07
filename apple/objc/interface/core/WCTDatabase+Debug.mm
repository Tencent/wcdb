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

#import <WCDB/CoreConst.h>
#import <WCDB/Notifier.hpp>
#import <WCDB/SQLite.h>
#import <WCDB/WCTDatabase+Debug.h>
#import <WCDB/WCTError+Private.h>

static std::atomic<WCTSimulateIOErrorOptions> &simulateIOErrorOptions()
{
    static std::atomic<WCTSimulateIOErrorOptions> *s_simulateIOErrorOptions = new std::atomic<WCTSimulateIOErrorOptions>(WCTSimulateNoneIOError);
    return *s_simulateIOErrorOptions;
}

static ssize_t controllableWrite(int fd, const void *buf, size_t byte, off_t offset)
{
    if ((simulateIOErrorOptions().load() & WCTSimulateWriteIOError) != 0) {
        return -1;
    }
    return pwrite(fd, buf, byte, offset);
}

static ssize_t controllableRead(int fd, void *buf, size_t byte, off_t offset)
{
    if ((simulateIOErrorOptions().load() & WCTSimulateReadIOError) != 0) {
        return -1;
    }
    return pread(fd, buf, byte, offset);
}

static std::nullptr_t initialize()
{
    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    vfs->xSetSystemCall(vfs, "pwrite", (sqlite3_syscall_ptr) controllableWrite);
    vfs->xSetSystemCall(vfs, "pread", (sqlite3_syscall_ptr) controllableRead);
    return nullptr;
}

@implementation WCTDatabase (Debug)

+ (void)additionalGlobalTraceError:(WCTErrorTraceBlock)block
{
    WCDB::Notifier::Callback callback = nullptr;
    if (block != nullptr) {
        callback = [block](const WCDB::Error &error) {
            WCTError *nsError = [[WCTError alloc] initWithError:error];
            block(nsError);
        };
    }
    WCDB::Notifier::shared()->setNotification(std::numeric_limits<int>::min(), "com.Tencent.WCDB.Notifier.AdditionalLog", callback);
}

+ (void)simulateIOError:(WCTSimulateIOErrorOptions)options
{
    static std::nullptr_t _ = initialize();
    WCDB_UNUSED(_)

    simulateIOErrorOptions().store(options);
}

@end
