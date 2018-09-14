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

#include <WCDB/Assertion.hpp>
#include <WCDB/MigrationInfos.hpp>

namespace WCDB {

#pragma mark - Initialize
MigrationInfos::MigrationInfos() : m_tableShouldBeMigratedCallback(nullptr)
{
}

#pragma mark - Notification
void MigrationInfos::setNotificaitionWhenTableShouldBeMigrated(const TableShouldBeMigratedCallback& callback)
{
    LockGuard lockGuard(m_lock);
    m_tableShouldBeMigratedCallback = callback;
}

#pragma mark - Infos
MigrationInfo* MigrationInfos::getOrIdentifyInfo(const std::string& migratedTable)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_ignored.find(migratedTable) != m_ignored.end()) {
            return nullptr;
        }
        auto iter = m_infos.find(migratedTable);
        if (iter != m_infos.end()) {
            return &iter->second;
        }
        if (m_tableShouldBeMigratedCallback == nullptr) {
            return nullptr;
        }
    }
    LockGuard lockGuard(m_lock);
    if (m_ignored.find(migratedTable) != m_ignored.end()) {
        return nullptr;
    }
    auto iter = m_infos.find(migratedTable);
    if (iter != m_infos.end()) {
        return &iter->second;
    }
    if (m_tableShouldBeMigratedCallback == nullptr) {
        return nullptr;
    }
    std::string originTable;
    std::string originDatabase;
    m_tableShouldBeMigratedCallback(migratedTable, originTable, originDatabase);
    if (!originTable.empty()) {
        m_infos.emplace(
        migratedTable, MigrationInfo(migratedTable, originTable, originDatabase));
    } else {
        m_ignored.emplace(migratedTable);
        return nullptr;
    }
    return nullptr;
}

void MigrationInfos::markInfoAsMigrated(MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    {
        LockGuard lockGuard(m_lock);
        m_ignored.emplace(info->migratedTable);
    }
    SharedLockGuard lockGuard(m_lock);
    auto iter = m_infos.find(info->migratedTable);
    WCTInnerAssert(iter != m_infos.end());
    iter->second.markAsMigrated();
}

std::list<MigrationInfo*>
MigrationInfos::resolveDetachableInfos(const std::map<std::string, MigrationInfo*>& attachedInfos)
{
    std::list<MigrationInfo*> detachableInfos;
    SharedLockGuard lockGuard(m_lock);
    for (const auto& iter : attachedInfos) {
        if (m_ignored.find(iter.first) != m_ignored.end()) {
            detachableInfos.push_back(iter.second);
        }
    }
    return detachableInfos;
}

} // namespace WCDB
