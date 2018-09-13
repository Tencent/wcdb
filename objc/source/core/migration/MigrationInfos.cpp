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
const MigrationInfo* MigrationInfos::getInfo(const std::string& tableName)
{
    WCTInnerAssert(m_lock.readSafety());
    auto iter = m_tableInfos.find(tableName);
    if (iter != m_tableInfos.end()) {
        return iter->second;
    }
    return nullptr;
}

const MigrationInfo* MigrationInfos::getOrIdentifyInfo(const std::string& sourceTable)
{
    {
        SharedLockGuard lockGuard(m_lock);
        const MigrationInfo* info = getInfo(sourceTable);
        if (info != nullptr) {
            return info;
        }
    }
    LockGuard lockGuard(m_lock);
    const MigrationInfo* info = getInfo(sourceTable);
    if (info != nullptr) {
        return info;
    }
    std::string targetTable;
    std::string targetDatabase;
    m_tableShouldBeMigratedCallback(sourceTable, targetTable, targetDatabase);
    if (!targetTable.empty()) {
        m_infos.push_back(MigrationInfo(sourceTable, targetTable, targetDatabase));
        WCTInnerAssert(m_tableInfos.find(sourceTable) == m_tableInfos.end());
        m_tableInfos[sourceTable] = &m_infos.back();
        return &m_infos.back();
    } else {
        m_tableInfos[sourceTable] = nullptr;
        return nullptr;
    }
}

} // namespace WCDB
