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
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
MigrationInfos::MigrationInfos()
: m_tableShouldBeMigratedCallback(nullptr), m_firstInitialized(false)
{
}

#pragma mark - Notification
void MigrationInfos::setNotificaitionWhenTableShouldBeMigrated(const TableShouldBeMigratedCallback& callback)
{
    LockGuard lockGuard(m_lock);
    WCTRemedialAssert(!m_firstInitialized,
                      "Migration method must be set before the very first operation.",
                      return;);
    m_tableShouldBeMigratedCallback = callback;
}

#pragma mark - Infos
bool MigrationInfos::initialize(MigrationInfosInitializer& initializer)
{
    {
        SharedLockGuard lockGuard(m_lock);
        if (m_firstInitialized && m_uninitialized.empty()) {
            return true;
        }
    }
    LockGuard lockGuard(m_lock);
    if (m_firstInitialized && m_uninitialized.empty()) {
        return true;
    }

    bool succeed;
    if (!m_firstInitialized) {
        std::set<std::string> tables;
        std::tie(succeed, tables) = initializer.getAllExistingTables();
        if (!succeed) {
            return false;
        }
        if (m_tableShouldBeMigratedCallback) {
            for (const auto& table : tables) {
                MigrationUserInfo userInfo(table);
                m_tableShouldBeMigratedCallback(userInfo);
                if (userInfo.shouldMigrate()) {
                    m_uninitialized.push_back(userInfo);
                } else {
                    markTableAsIgnorable(table);
                }
            }
        }
    }

    while (!m_uninitialized.empty()) {
        const MigrationUserInfo& userInfo = m_uninitialized.back();
        WCTInnerAssert(userInfo.shouldMigrate());

        auto iter = m_infos.find(userInfo.getMigratedTable());
        if (iter != m_infos.end()) {
            Error error;
            error.level = Error::Level::Warning;
            error.setCode(Error::Code::Conflict);
            if (iter->second == nullptr) {
                error.message = "Table is migrated or mark as ignorable.";
            } else {
                error.message = "Conflict info is found.";
            }
            error.infos.set("Conflict", userInfo.getDebugDescription());
            Notifier::shared()->notify(error);
            m_uninitialized.pop_back();
            continue;
        }

        std::set<std::string> columns;
        std::tie(succeed, columns) = initializer.getAllColumns(
        userInfo.getSchemaForOriginDatabase(), userInfo.getOriginTable());
        if (!succeed) {
            return false;
        }
        if (columns.empty()) {
            markTableAsIgnorable(userInfo.getMigratedTable());
        } else {
            addInfo(MigrationInfo(userInfo, columns));
        }
        m_uninitialized.pop_back();
    }
    return true;
}

void MigrationInfos::markInfoAsMigrated(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    LockGuard lockGuard(m_lock);
    WCTInnerAssert(m_infos.find(info->getMigratedTable()) != m_infos.end());
    m_infos[info->getMigratedTable()] = nullptr;
}

void MigrationInfos::addUserInfo(const MigrationUserInfo& info)
{
    WCTRemedialAssert(
    !info.shouldMigrate(), "Adding a unspecified migration info make no sense.", return;);
    LockGuard lockGuard(m_lock);
    m_uninitialized.push_back(info);
}

void MigrationInfos::addInfo(const MigrationInfo& info)
{
    WCTInnerAssert(info.shouldMigrate());
    WCTInnerAssert(m_lock.writeSafety());

    WCTInnerAssert(m_infos.find(info.getMigratedTable()) == m_infos.end());

    m_holder.push_back(info);
    const MigrationInfo* holdedInfo = &m_holder.back();
    WCTInnerAssert(m_infos.find(holdedInfo->getMigratedTable()) == m_infos.end());
    m_infos[holdedInfo->getMigratedTable()] = holdedInfo;
}

void MigrationInfos::markTableAsIgnorable(const std::string& table)
{
    WCTInnerAssert(m_lock.writeSafety());
    WCTInnerAssert(!table.empty());
    m_infos[table] = nullptr;
}

} // namespace WCDB
