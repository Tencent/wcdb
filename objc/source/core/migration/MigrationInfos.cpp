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
MigrationInfos::MigrationInfos() : m_tableFilter(nullptr)
{
}

bool MigrationInfos::shouldMigrate() const
{
    return m_tableFilter != nullptr || !m_userInfos.empty();
}

#pragma mark - Filter
void MigrationInfos::filterTable(const TableFilter& tableFilter)
{
    m_tableFilter = tableFilter;
}

#pragma mark - UserInfo
void MigrationInfos::addUserInfo(const MigrationUserInfo& info)
{
    WCTRemedialAssert(
    !info.shouldMigrate(), "Adding a unspecified migration info make no sense.", return;);
    auto iter = m_userInfos.emplace(info.getMigratedTable(), info);
    WCTRemedialAssert(
    iter.second,
    String::formatted("Migration user info: [%s] is duplicated with [%s].",
                      info.getDebugDescription().c_str(),
                      iter.first->second.getDebugDescription().c_str()),
    ;);
}

#pragma mark - Infos
bool MigrationInfos::initialize(MigrationInfosInitializer& initializer)
{
    WCTInnerAssert(shouldMigrate());
    WCTInnerAssert(m_infos.empty() && m_holder.empty());

    m_holder.clear();
    m_infos.clear();

    bool succeed;
    do {
        std::set<std::string> tables;
        std::tie(succeed, tables) = initializer.getAllExistingTables();
        if (!succeed) {
            break;
        }
        std::map<std::string, const MigrationUserInfo> uninitialized = m_userInfos;
        if (m_tableFilter) {
            for (const auto& table : tables) {
                MigrationUserInfo userInfo(table);
                if (uninitialized.find(userInfo.getMigratedTable())
                    == uninitialized.end()) {
                    // It's already set manually.
                    continue;
                }
                m_tableFilter(userInfo);
                if (userInfo.shouldMigrate()) {
                    uninitialized.emplace(userInfo.getMigratedTable(), userInfo);
                } else {
                    // Mark as ignored.
                    m_infos[table] = nullptr;
                }
            }
        }

        for (const auto& iter : uninitialized) {
            const MigrationUserInfo& userInfo = iter.second;
            WCTInnerAssert(userInfo.shouldMigrate());

            std::set<std::string> columns;
            std::tie(succeed, columns) = initializer.getAllColumns(
            userInfo.getSchemaForOriginDatabase(), userInfo.getOriginTable());
            if (!succeed) {
                break;
            }
            if (columns.empty()) {
                // Mark as ignored. It usually means that the origin table had been migrated.
                m_infos[userInfo.getMigratedTable()] = nullptr;
            } else {
                m_holder.push_back(MigrationInfo(userInfo, columns));
                const MigrationInfo* holdedInfo = &m_holder.back();
                WCTInnerAssert(m_infos.find(holdedInfo->getMigratedTable())
                               == m_infos.end());
                m_infos[holdedInfo->getMigratedTable()] = holdedInfo;
            }
        }
    } while (false);

    if (!succeed) {
        m_holder.clear();
        m_infos.clear();
    }
    return succeed;
}

std::map<std::string, const MigrationInfo*> MigrationInfos::getInfos() const
{
    return m_infos;
}

void MigrationInfos::markInfoAsMigrated(const MigrationInfo* info)
{
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_infos.find(info->getMigratedTable()) != m_infos.end());
    m_infos[info->getMigratedTable()] = nullptr;
}

} // namespace WCDB
