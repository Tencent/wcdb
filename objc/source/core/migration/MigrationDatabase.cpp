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

#include <WCDB/Core.h>
#include <WCDB/Dispatch.hpp>
#include <future>

namespace WCDB {

#pragma mark - Initializer
std::shared_ptr<Database>
MigrationDatabase::databaseWithExistingTag(const Tag &tag)
{
    std::shared_ptr<Database> database(new MigrationDatabase(
        HandlePools::defaultPools()->getExistingPool(tag)));
    if (database &&
        static_cast<MigrationDatabase *>(database.get())->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<Database>
MigrationDatabase::databaseWithExistingPath(const std::string &path)
{
    std::shared_ptr<Database> database(new MigrationDatabase(
        HandlePools::defaultPools()->getExistingPool(path)));
    if (database &&
        static_cast<MigrationDatabase *>(database.get())->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<Database> MigrationDatabase::databaseWithPath(
    const std::string &path,
    const std::shared_ptr<MigrationSetting> &migrationInfos)
{
    const HandlePools::Generator s_generator =
        [&migrationInfos](
            const std::string &path) -> std::shared_ptr<HandlePool> {
        WCTAssert(migrationInfos, "Migration infos can't be null");
        BuiltinConfig *builtinConfig = BuiltinConfig::shared();
        if (migrationInfos->isSameDatabaseMigration()) {
            return std::shared_ptr<HandlePool>(new MigrationHandlePool(
                path, builtinConfig->defaultConfigs, migrationInfos));
        }
        Configs configs = builtinConfig->defaultConfigs;
        configs.setConfig(
            MigrationBuiltinConfig::migrationWithSetting(migrationInfos.get()));
        return std::shared_ptr<HandlePool>(
            new MigrationHandlePool(path, configs, migrationInfos));
    };

    std::shared_ptr<Database> database(new MigrationDatabase(
        HandlePools::defaultPools()->getPool(path, s_generator)));
    if (database &&
        static_cast<MigrationDatabase *>(database.get())->isValid()) {
#ifdef DEBUG
        WCTAssert(static_cast<MigrationDatabase *>(database.get())
                          ->m_migrationPool->getMigrationSetting()
                          ->hash == migrationInfos->hash,
                  "Migration info can't be changed after the very first "
                  "initialization.");
#endif
        return database;
    }
    return nullptr;
}

MigrationDatabase::MigrationDatabase(const RecyclableHandlePool &pool)
    : Database(pool)
    , m_migrationPool(pool != nullptr ? dynamic_cast<MigrationHandlePool *>(
                                            pool.getHandlePool())
                                      : nullptr)
{
    WCTInnerAssert(pool == nullptr || m_migrationPool != nullptr);
}

#pragma mark - Migration
bool MigrationDatabase::stepMigration(bool &done)
{
#ifdef DEBUG
    WCTAssert(m_migrationPool->debug_checkMigratingThread(),
              "Migration stepping is not thread-safe.");
#endif
    done = false;
    MigrationSetting *setting = m_migrationPool->getMigrationSetting();
    std::shared_ptr<MigrationInfo> info;
    bool migratedEve = false;
    {
        SharedLockGuard lockGuard(setting->getSharedLock());
        if (setting->isMigrated()) {
            done = true;
            return true;
        }
        info = setting->pickUpForMigration();
        bool isMigrated = false;
        bool committed = runTransaction([this, &migratedEve, &isMigrated, &info,
                                         setting](Handle *handle) -> bool {
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            {
                //check if source table exists
                std::pair<bool, bool> sourceTableExists =
                    migrationHandle->isTableExists(info->getSourceTable());
                if (!sourceTableExists.first || !sourceTableExists.second) {
                    isMigrated = sourceTableExists.first;
                    return false;
                }
            }
            int rowPerStep = setting->getMigrationRowPerStep();
            std::list<long long> rowids;
            {
                //pick up row ids
                if (!migrationHandle->prepare(
                        info->getStatementForPickingRowIDs())) {
                    return false;
                }
                migrationHandle->bindInteger64(rowPerStep, 1);
                bool done = false;
                while (migrationHandle->step(done) && !done) {
                    rowids.push_back(migrationHandle->getInteger64(0));
                }
                migrationHandle->finalize();
                if (!done) {
                    return false;
                }
            }
            //migration
            bool result = true;
            for (const auto &rowid : rowids) {
                if (migrationHandle->migrateWithRowID(
                        rowid, info, Lang::InsertSTMT::Type::Insert)) {
                    continue;
                }
                //handle failure
                if (migrationHandle->getResultCode() != SQLITE_CONSTRAINT) {
                    result = false;
                } else if (setting->invokeConflictCallback(info.get(), rowid)) {
                    //override
                    result = migrationHandle->migrateWithRowID(
                        rowid, info, Lang::InsertSTMT::Type::InsertOrReplace);
                } //ignore conflict
                if (!result) {
                    break;
                }
            }
            migrationHandle->finalize();
            if (result) {
                migratedEve = rowids.size() < rowPerStep;
            } else {
                m_migrationPool->setThreadedError(migrationHandle->getError());
            }
            return result;
        });
        migratedEve = migratedEve && committed;
        if (!isMigrated && !migratedEve) {
            return committed;
        }
    }
    {
        LockGuard lockGuard(setting->getSharedLock());
        if (migratedEve && !execute(info->getStatementForDroppingOldTable())) {
            return false;
        }
        if (setting->markAsMigrated(info->targetTable)) {
            //schema changed
            setConfig(MigrationBuiltinConfig::migrationWithSetting(setting));
        }
        done = setting->isMigrated();
    }
    return true;
}

void MigrationDatabase::asyncMigration()
{
    std::string name("com.Tencent.WCDB.Migration.");
    name.append(std::to_string(getTag()));
    std::string path = getPath();
    Dispatch::async(name, [path](const std::atomic<bool> &stop) {
        bool done = false;
        while (!done && !stop.load()) {
            std::shared_ptr<Database> database =
                MigrationDatabase::databaseWithExistingPath(path);
            if (!database) {
                break;
            }
            MigrationDatabase *migrationDatabase =
                static_cast<MigrationDatabase *>(database.get());
            bool result = migrationDatabase->stepMigration(done);
            //todo control stepping
            if (!result) {
                break;
            }
        }
    });
}

MigrationSetting *MigrationDatabase::getMigrationSetting()
{
    return m_migrationPool->getMigrationSetting();
}

} //namespace WCDB
