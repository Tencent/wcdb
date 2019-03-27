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
#import <WCDB/SQLite.h>
#import <WCDB/WCTDatabase+TestCase.h>

static ssize_t illPwrite(int, const void *, size_t, off_t)
{
    return -1;
}

static ssize_t illPread(int, void *, size_t, off_t)
{
    return -1;
}

@implementation WCTDatabase (TestCase)

+ (void)resetGlobalErrorTracer
{
    WCDB::Console::shared()->setLogger(WCDB::Console::logger);
}

+ (void)simulateWriteIOError:(BOOL)enable
{
    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    sqlite3_mutex *mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MASTER);
    sqlite3_mutex_enter(mutex);
    static sqlite3_syscall_ptr s_originWrite = vfs->xGetSystemCall(vfs, "pwrite");
    vfs->xSetSystemCall(vfs, "pwrite", enable ? (sqlite3_syscall_ptr) illPwrite : s_originWrite);
    sqlite3_mutex_leave(mutex);
}

+ (void)simulateReadIOError:(BOOL)enable
{
    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    sqlite3_mutex *mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MASTER);
    sqlite3_mutex_enter(mutex);
    static sqlite3_syscall_ptr s_originRead = vfs->xGetSystemCall(vfs, "pread");
    vfs->xSetSystemCall(vfs, "pread", enable ? (sqlite3_syscall_ptr) illPread : s_originRead);
    sqlite3_mutex_leave(mutex);
}

@end
