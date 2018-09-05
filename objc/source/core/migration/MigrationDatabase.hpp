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

#ifndef MigrationDatabase_hpp
#define MigrationDatabase_hpp

#include <WCDB/Database.hpp>
#include <WCDB/MigrationHandlePool.hpp>
#include <WCDB/MigrationSetting.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class MigrationDatabase : public Database {
public:
#pragma mark - Initializer
    MigrationDatabase() = delete;
    MigrationDatabase(const MigrationDatabase &) = delete;
    MigrationDatabase &operator=(const MigrationDatabase &) = delete;

    static std::shared_ptr<Database> databaseWithPath(const std::string &path);
    static std::shared_ptr<Database> databaseWithExistingPath(const std::string &path);
    static std::shared_ptr<Database> databaseWithExistingTag(const Tag &tag);

protected:
    MigrationDatabase(const RecyclableHandlePool &pool);

    static std::shared_ptr<HandlePool> generateHandlePool(const std::string &path);

#pragma mark - Migration
public:
    void setMigrationInfos(const std::list<std::shared_ptr<MigrationInfo>> &infos);

    bool stepMigration(bool &done);

    enum State {
        Migrating,
        Done,
    };
    // Migrating -> stepped?, Done -> migrated?
    typedef std::function<bool(State, bool)> SteppedCallback;
    void asyncMigration(double interval = 3.0, int retryTimes = 10);
    void asyncMigration(const SteppedCallback &callback);

    MigrationSetting *getMigrationSetting();

protected:
    MigrationHandlePool *m_migrationPool;

    bool interruptIfDeposited();
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* MigrationDatabase_hpp */