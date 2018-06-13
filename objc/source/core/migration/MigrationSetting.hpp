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

#ifndef MigrationSetting_hpp
#define MigrationSetting_hpp

#include <WCDB/Lock.hpp>
#include <WCDB/MigrationInfo.hpp>
#include <map>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationHandlePool;

class MigrationSetting {
#pragma mark - Initialize
public:
    typedef std::function<void(const MigrationInfo *)> TableMigratedCallback;

    MigrationSetting(MigrationHandlePool *m_pool,
                     const std::list<std::shared_ptr<MigrationInfo>> &infos);

protected:
    MigrationSetting() = delete;
    MigrationSetting(const MigrationSetting &) = delete;
    MigrationSetting &operator=(const MigrationSetting &) = delete;

    MigrationHandlePool *m_pool;

#pragma mark - Basic
public:
    bool isSameDatabaseMigration() const;
    const std::map<std::string, std::pair<std::string, int>> &
    getSchemasForAttaching() const;
    const std::map<std::string, std::shared_ptr<MigrationInfo>> &
    getInfos() const;
    //return true to indicate schemas changed
    bool markAsMigrated(const std::string &table);
    bool isMigrated() const;
    const std::shared_ptr<MigrationInfo> &pickUpForMigration() const;

    SharedLock &getSharedLock();

    void setTableMigratedCallback(const TableMigratedCallback &onMigrated);

    void setMigrateRowPerStep(int row);
    int getMigrationRowPerStep() const;

    //info, rowid -> return true to replace and false to ignore
    typedef std::function<bool(const MigrationInfo *, const long long &)>
        ConflictCallback;
    void setConflictCallback(const ConflictCallback &callback);
    bool invokeConflictCallback(const MigrationInfo *, const long long &) const;

#ifdef DEBUG
    const int64_t hash;
    int64_t
    hashedInfos(const std::list<std::shared_ptr<MigrationInfo>> &infos) const;
#endif

protected:
    mutable SharedLock m_lock;
    //schema -> {path, reference}
    std::map<std::string, std::pair<std::string, int>> m_schemas;
    //target table -> infos
    std::map<std::string, std::shared_ptr<MigrationInfo>> m_infos;

    TableMigratedCallback m_onMigrated;
    ConflictCallback m_onConflict;
    int m_rowPerStep;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationSetting_hpp */
