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

#ifndef MigrationInfos_hpp
#define MigrationInfos_hpp

#include <WCDB/Lock.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <functional>
#include <map>
#include <set>

namespace WCDB {

class MigrationInfosInitializer {
protected:
    virtual std::pair<bool, std::set<std::string>> getAllExistingTables() = 0;
    // When succeed, if the empty columns means that table is already migrated.
    virtual std::pair<bool, std::set<std::string>>
    getAllColumns(const std::string& schema, const std::string& table) = 0;
    friend class MigrationInfos;
};

class MigrationInfos {
#pragma mark - Initialize
public:
    MigrationInfos();

protected:
    mutable SharedLock m_lock;

#pragma mark - Notification
public:
    typedef std::function<void(MigrationUserInfo&)> TableShouldBeMigratedCallback;
    void setNotificaitionWhenTableShouldBeMigrated(const TableShouldBeMigratedCallback& callback);

protected:
    TableShouldBeMigratedCallback m_tableShouldBeMigratedCallback;

#pragma mark - Infos
public:
    bool initialize(MigrationInfosInitializer& initializer);

    void addUserInfo(const MigrationUserInfo& info);

    //#warning TODO - cross database migration should be picked first
    //    const MigrationInfo* pickMigratingInfo();

    void markInfoAsMigrated(const MigrationInfo* info);

protected:
    void addInfo(const MigrationInfo& info);
    void markTableAsIgnorable(const std::string& table);

    //    std::map<std::string, const MigrationInfo*> m_crossDatabaseMigrating; // migrated table -> info
    //    std::map<std::string, const MigrationInfo*> m_sameDatabaseMigrating; // migrated table -> info
    std::map<std::string, const MigrationInfo*> m_infos; // migrated table -> info

    std::list<const MigrationInfo> m_holder; // infos will never be deleted.
    std::list<const MigrationUserInfo> m_uninitialized;
    bool m_firstInitialized;
};

} // namespace WCDB

#endif /* MigrationInfos_hpp */
