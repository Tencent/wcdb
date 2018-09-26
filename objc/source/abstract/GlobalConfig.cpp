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

#include <WCDB/GlobalConfig.hpp>
#include <sqlcipher/sqlite3.h>

namespace WCDB {

void GlobalConfig::enableMultithread()
{
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
}

void GlobalConfig::setMemoryMapSize(int64_t defaultSizeLimit, int64_t maximumAllowedSizeLimit)
{
    sqlite3_config(SQLITE_CONFIG_MMAP_SIZE, defaultSizeLimit, maximumAllowedSizeLimit);
}

void GlobalConfig::enableMemoryStatus(bool enable)
{
    sqlite3_config(SQLITE_CONFIG_MEMSTATUS, enable);
}

void GlobalConfig::setNotificationForLog(const Log& log)
{
    sqlite3_config(SQLITE_CONFIG_LOG, log, nullptr);
}

void GlobalConfig::hookVFSOpen(const VFSOpen& vfsOpen)
{
    sqlite3_vfs* vfs = sqlite3_vfs_find(nullptr);
    vfs->xSetSystemCall(vfs, "open", (void (*)(void)) vfsOpen);
}

} // namespace WCDB
