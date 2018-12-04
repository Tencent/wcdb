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

#pragma mark - Filter
void Migration::filterTable(const TableFilter& tableFilter)
{
    m_tableFilter = tableFilter;
    m_initialized = (m_tableFilter == nullptr);
    m_holder.clear();
    m_migrating.clear();
}

#pragma mark - Infos
Migration::Initializer::~Initializer()
{
}

bool Migration::initialize(Initializer& initializer)
{
    if (m_initialized) {
        return true;
    }

    m_holder.clear();
    m_migrating.clear();

    bool succeed;

    std::set<String> tables;
    std::tie(succeed, tables) = initializer.getTables();
    if (!succeed) {
        return false;
    }
    std::list<const MigrationUserInfo> userInfos;
    WCTInnerAssert(m_tableFilter != nullptr);
    for (const auto& table : tables) {
        MigrationUserInfo userInfo(table);
        m_tableFilter(userInfo);
        if (userInfo.shouldMigrate()) {
            userInfos.push_back(userInfo);
        }
    }

    {
        // check sanity
        // 1. duplicated {origin table, origin database} is not allowed.
        // 2. origin table == migrated table && origin database == current database is not allowed.
        String databasePath = initializer.getDatabasePath();
        std::map<std::pair<String, String>, const MigrationUserInfo*> origins;
        for (const auto& userInfo : userInfos) {
            if (userInfo.getOriginTable() == userInfo.getMigratedTable()
                && (userInfo.getOriginDatabase() == databasePath
                    || userInfo.isSameDatabaseMigration())) {
                Error error;
                error.setCode(Error::Code::Misuse);
                error.message = "Migrating to same table makes no sense.";
                error.infos.set("Info", userInfo.getDebugDescription());
                initializer.setError(error);
                return false;
            }

            auto key
            = std::make_pair(userInfo.getOriginDatabase(), userInfo.getOriginTable());
            auto iter = origins.find(key);
            if (iter != origins.end()) {
                Error error;
                error.setCode(Error::Code::Misuse);
                error.message = "Duplicated origin table and database are found";
                error.infos.set("Info1", userInfo.getDebugDescription());
                error.infos.set("Info2", iter->second->getDebugDescription());
                initializer.setError(error);
                return false;
            }
            origins.emplace(key, &userInfo);
        }
    }

    for (const auto& userInfo : userInfos) {
        WCTInnerAssert(userInfo.shouldMigrate());

        std::set<String> columns;
        std::tie(succeed, columns) = initializer.getColumns(
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
