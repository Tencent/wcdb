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
MigrationHandlePool::MigrationHandlePool(
    const std::string &path,
    const Configs &configs,
    const std::shared_ptr<MigrationInfos> &migrationInfos)
    : HandlePool(path, configs), m_infos(migrationInfos)
{
#ifdef DEBUG
    WCTInnerAssert(debug_checkInfosLegal());
#endif
}

#ifdef DEBUG
bool MigrationHandlePool::debug_checkInfosLegal()
{
    if (!m_infos) {
        return false;
    }
    if (!m_infos->isSameDatabaseMigration()) {
        for (const auto &info : m_infos->getInfos()) {
            if (info.second->sourceDatabasePath == path) {
                return false;
            }
        }
    }
    return true;
}
#endif

#pragma mark - Migration
MigrationInfos *MigrationHandlePool::getMigrationInfos() const
{
    return m_infos.get();
}

#pragma mark - Override
std::shared_ptr<Handle> MigrationHandlePool::generateHandle()
{
    return MigrationHandle::handleWithPath(path, getTag(), m_infos);
}

bool MigrationHandlePool::willConfigurateHandle(Handle *handle)
{
    HandlePool::willConfigurateHandle(handle);
    MigrationHandle *migrationHandle = static_cast<MigrationHandle *>(handle);
    return migrationHandle->lazySetupVeryFirstMigratingInfo();
}

} //namespace WCDB
