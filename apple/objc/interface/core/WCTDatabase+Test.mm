//
// Created by sanhuazhang on 2019/06/03
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

#import <WCDB/Core.hpp>
#import <WCDB/CoreConst.h>
#import <WCDB/Notifier.hpp>
#import <WCDB/SQLite.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTDatabase+Test.h>

static std::atomic<WCTSimulateIOErrorOptions> &simulateIOErrorOptions()
{
    WCDB_STATIC_VARIABLE std::atomic<WCTSimulateIOErrorOptions> s_simulateIOErrorOptions(WCTSimulateNoneIOError);
    return s_simulateIOErrorOptions;
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

@implementation WCTDatabase (Test)

+ (void)simulateIOError:(WCTSimulateIOErrorOptions)options
{
    WCDB_ONCE(initialize());

    simulateIOErrorOptions().store(options);
}

- (void)enableAutoCheckpoint:(BOOL)flag
{
    WCDB::Core::shared().enableAutoCheckpoint(_database, flag);
}

- (BOOL)isOpened
{
    return _database->isOpened();
}

- (BOOL)canOpen
{
    return _database->canOpen();
}

- (void)close
{
    _database->close(nullptr);
}

- (BOOL)isBlockaded
{
    return _database->isBlockaded();
}

- (void)blockade
{
    _database->blockade();
}

- (void)unblockade
{
    _database->unblockade();
}

@end
