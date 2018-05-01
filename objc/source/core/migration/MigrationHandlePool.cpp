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

#include <WCDB/Core.h>

namespace WCDB {
#pragma mark - Initialize
std::shared_ptr<HandlePool> MigrationHandlePool::pool(
    const std::string &path,
    const Configs &configs,
    const std::list<std::shared_ptr<MigrationInfo>> &infos)
{
    std::shared_ptr<HandlePool> pool(
        new MigrationHandlePool(path, configs, infos));
    if (pool) {
        static_cast<MigrationHandlePool *>(pool.get())->initialize();
    }
    return pool;
}

MigrationHandlePool::MigrationHandlePool(
    const std::string &path,
    const Configs &configs,
    const std::list<std::shared_ptr<MigrationInfo>> &infos)
    : HandlePool(path, configs), m_setting(infos)
{
}

void MigrationHandlePool::initialize()
{
#ifdef DEBUG
    debug_m_migratingThreadId = std::thread::id();
    WCTInnerAssert(debug_checkSettingLegal());
#endif
    m_setting.associate(this);
}

#ifdef DEBUG
bool MigrationHandlePool::debug_checkMigratingThread()
{
    if (debug_m_migratingThreadId == std::thread::id()) {
        debug_m_migratingThreadId = std::this_thread::get_id();
    }
    return debug_m_migratingThreadId == std::this_thread::get_id();
}

bool MigrationHandlePool::debug_checkSettingLegal()
{
    if (!m_setting.isSameDatabaseMigration()) {
        for (const auto &info : m_setting.getInfos()) {
            if (info.second->sourceDatabasePath == path) {
                return false;
            }
        }
    }
    return true;
}
#endif

#pragma mark - Migration
MigrationSetting *MigrationHandlePool::getMigrationSetting()
{
    return &m_setting;
}

#pragma mark - Override
std::shared_ptr<Handle> MigrationHandlePool::generateHandle()
{
    return MigrationHandle::handleWithPath(path, getTag(), m_setting);
}

} //namespace WCDB
