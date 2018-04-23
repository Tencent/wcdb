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
#include <set>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationInfos {
#pragma mark - Initialize
public:
    static std::shared_ptr<MigrationInfos>
    infos(const std::list<std::shared_ptr<MigrationInfo>> &infos);

protected:
    MigrationInfos(const std::list<std::shared_ptr<MigrationInfo>> &infos);
    MigrationInfos() = delete;
    MigrationInfos(const MigrationInfos &) = delete;
    MigrationInfos &operator=(const MigrationInfos &) = delete;

#pragma mark - Basic
public:
    bool isSameDatabaseMigration() const;
    const std::map<std::string, std::pair<std::string, int>> &
    getSchemasForAttaching() const;
    const std::map<std::string, std::shared_ptr<MigrationInfo>> &
    getInfos() const;
    std::shared_ptr<MigrationInfo> getMigratingInfo();
    void markAsMigrationStarted();
    void markAsMigrationStarted(const std::string &table);
    void markAsMigrating(const std::string &table);
    void markAsMigrated(bool &schemasChanged);
    bool didMigratingStart() const;
    bool didMigrationDone() const;

    SharedLock &getSharedLock();

#ifdef DEBUG
    const int64_t hash;
    int64_t
    hashedInfos(const std::list<std::shared_ptr<MigrationInfo>> &infos) const;
#endif

protected:
    SharedLock m_lock;
    std::atomic<bool> m_migratingStarted;
    std::shared_ptr<MigrationInfo> m_migratingInfo;
    //schema -> {path, reference}
    std::map<std::string, std::pair<std::string, int>> m_schemas;
    //target table -> infos
    std::map<std::string, std::shared_ptr<MigrationInfo>> m_infos;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationInfos_hpp */
