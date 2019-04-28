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

#import <WCDB/Console.hpp>
#import <WCDB/CoreConst.h>
#import <WCDB/SQLite.h>
#import <WCDB/WCTDatabase+TestCase.h>

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

@implementation WCTDatabase (TestCase)

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

+ (void)resetGlobalErrorTracer
{
    WCDB::Console::shared()->setLogger(WCDB::Console::logger);
}

+ (void)simulateIOError:(WCTSimulateIOErrorOptions)options
{
    static std::nullptr_t _ = initialize();
    WCDB_UNUSED(_)

    simulateIOErrorOptions().store(options);
}

- (void)removeCheckpointConfig
{
    [self removeConfigForName:@(WCDB::CheckpointConfigName)];
}

- (WCTOptionalSize)getNumberOfWalFrames
{
    WCTOptionalSize size = nullptr;
    NSError *error;
    NSNumber *nsSize = [[NSFileManager defaultManager] attributesOfItemAtPath:self.walPath error:&error][NSFileSize];
    if (nsSize != nil && error == nil) {
        int numberOfFrames = (int) ((nsSize.integerValue - self.walHeaderSize) / (self.walFrameHeaderSize + self.pageSize));
        size = numberOfFrames > 0 ? numberOfFrames : 0;
    }
    return size;
}

@end
