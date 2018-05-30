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

#include <WCDB/Abstract.h>
#include <WCDB/String.hpp>
#include <fcntl.h>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

SQLiteGlobal *SQLiteGlobal::shared()
{
    static SQLiteGlobal *s_global(new SQLiteGlobal);
    return s_global;
}

SQLiteGlobal::SQLiteGlobal()
{
    sqlite3_config(SQLITE_CONFIG_LOG, SQLiteGlobal::log, nullptr);
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    sqlite3_config(SQLITE_CONFIG_MEMSTATUS, false);
    //TODO
    //    sqlite3_config(SQLITE_CONFIG_MMAP_SIZE, 0x7fff0000, 0x7fff0000);

    sqlite3_vfs *vfs = sqlite3_vfs_find(nullptr);
    vfs->xSetSystemCall(vfs, "open", (void (*)(void)) SQLiteGlobal::vfsOpen);
}

void SQLiteGlobal::hookVFSDidFileCreated(
    const std::function<void(const char *)> &didFileCreated)
{
    m_didFileCreated = didFileCreated;
}

int SQLiteGlobal::vfsOpen(const char *zFile, int flags, int mode)
{
    int fd = open(zFile, flags, mode);
    if (fd != -1 && (flags & O_CREAT)) {
        SQLiteGlobal *global = SQLiteGlobal::shared();
        if (global->m_didFileCreated != nullptr) {
            global->m_didFileCreated(zFile);
        }
    }
    return fd;
}

void SQLiteGlobal::log(void *userInfo, int code, const char *message)
{
    Error error;
    switch (code) {
        case SQLITE_WARNING:
            error.level = Error::Level::Warning;
            break;
        case SQLITE_NOTICE:
            error.level = Error::Level::Ignore;
            break;
        default:
            error.level = Error::Level::Debug;
            break;
    }
    error.setSQLiteCode(code);
    error.message = message ? message : String::empty();
    Reporter::shared()->report(error);
}

} //namespace WCDB
