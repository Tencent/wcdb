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
#include <WCDB/Error.hpp>
#include <WCDB/Migration.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initialize
Migration::Migration() : m_tableFilter(nullptr), m_initialized(true)
{
}

bool Migration::isInitialized() const
{
    return m_initialized;
}

void Migration::onStateChanged()
{
    m_initialized = m_tableFilter == nullptr && m_userInfos.empty();
    m_holder.clear();
    m_migrating.clear();
}

#pragma mark - Filter
void Migration::filterTable(const TableFilter& tableFilter)
{
    m_tableFilter = tableFilter;
    onStateChanged();
}

#pragma mark - UserInfo
void Migration::addUserInfo(const MigrationUserInfo& info)
{
    WCTRemedialAssert(
    !info.shouldMigrate(), "Adding a unspecified migration info makes no sense.", return;);
    auto iter = m_userInfos.emplace(info.getMigratedTable(), info);
    WCTRemedialAssert(
    iter.second,
    String::formatted("Migration user info: [%s] is duplicated with [%s].",
                      info.getDebugDescription().c_str(),
                      iter.first->second.getDebugDescription().c_str()),
    ;);
    onStateChanged();
}

#pragma mark - Infos
bool Migration::initialize(Initializer& initializer)
{
    if (m_initialized) {
        return true;
    }

    m_holder.clear();
    m_migrating.clear();

    bool succeed;

    std::set<String> tables;
    std::tie(succeed, tables) = initializer.getAllExistingTables();
    if (!succeed) {
        return false;
    }
    std::map<String, const MigrationUserInfo> uninitialized = m_userInfos;
    if (m_tableFilter) {
        for (const auto& table : tables) {
            MigrationUserInfo userInfo(table);
            if (uninitialized.find(userInfo.getMigratedTable()) == uninitialized.end()) {
                // It's already set manually.
                continue;
            }
            m_tableFilter(userInfo);
            if (userInfo.shouldMigrate()) {
                uninitialized.emplace(userInfo.getMigratedTable(), userInfo);
            }
        }
    }

    {
        // check if some of the infos contain same origin table
        std::map<std::pair<String, String>, const MigrationUserInfo*> origins;
        for (const auto& iter : uninitialized) {
            const MigrationUserInfo& userInfo = iter.second;
            auto key
            = std::make_pair(userInfo.getOriginDatabase(), userInfo.getOriginTable());
            auto origin = origins.find(key);
            if (origin != origins.end()) {
                WCTInnerAssert(origin->second != nullptr);
                succeed = false;
                Error error;
                error.setCode(Error::Code::Misuse);
                error.message = "Duplicated origin table and database are found";
                error.infos.set("Migrated_1", userInfo.getMigratedTable());
                error.infos.set("Migrated_2", origin->second->getMigratedTable());
                error.infos.set("OriginTable", userInfo.getOriginTable());
                error.infos.set("OriginDatabase", userInfo.getOriginDatabase());
                initializer.setError(error);
                return false;
            }
            origins.emplace(key, &userInfo);
        }
    }

    for (const auto& iter : uninitialized) {
        const MigrationUserInfo& userInfo = iter.second;
        WCTInnerAssert(userInfo.shouldMigrate());

        std::set<String> columns;
        std::tie(succeed, columns) = initializer.getAllColumns(
        userInfo.getOriginTable(), userInfo.getOriginDatabase());
        if (!succeed) {
            return false;
        }
        // When [columns] is empty, it usually means that the origin table had been migrated before the database is opened. At this case, it will be discarded but not marked as migrated.
        if (!columns.empty()) {
            auto inserted = m_holder.emplace(userInfo.getMigratedTable(),
                                             MigrationInfo(userInfo, columns));
            WCTInnerAssert(inserted.second);
            const MigrationInfo* info = &inserted.first->second;
            m_migrating.emplace(info);
        }
    }

    m_initialized = true;
    return m_initialized;
}

bool Migration::shouldMigrate() const
{
    WCTInnerAssert(isInitialized());
    return !m_holder.empty();
}

const std::set<const MigrationInfo*>& Migration::getMigratingInfos() const
{
    WCTInnerAssert(isInitialized());
    return m_migrating;
}

void Migration::markInfoAsMigrated(const MigrationInfo* info)
{
    WCTInnerAssert(isInitialized());
    WCTInnerAssert(info != nullptr);
    WCTInnerAssert(m_migrating.find(info) != m_migrating.end());
    m_migrating.erase(info);
}

} // namespace WCDB
