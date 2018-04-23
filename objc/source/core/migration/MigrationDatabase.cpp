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
    const std::shared_ptr<MigrationInfos> &migrationInfos)
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
        configs.setConfig(MigrationBuiltinConfig::autoAttachAndDetachWithInfos(
            migrationInfos.get()));
        return std::shared_ptr<HandlePool>(
            new MigrationHandlePool(path, configs, migrationInfos));
    };

    std::shared_ptr<Database> database(new MigrationDatabase(
        HandlePools::defaultPools()->getPool(path, s_generator)));
    if (database &&
        static_cast<MigrationDatabase *>(database.get())->isValid()) {
#ifdef DEBUG
        WCTAssert(static_cast<MigrationDatabase *>(database.get())
                          ->m_migrationPool->getMigrationInfos()
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
bool MigrationDatabase::startMigration(bool &done)
{
    done = false;
    MigrationInfos *infos = m_migrationPool->getMigrationInfos();
    WCTInnerAssert(!infos->getInfos().empty());
    RecyclableHandle handle = getHandle();
    if (handle == nullptr) {
        return false;
    }
    //continue last unfinished migration
    MigrationHandle *migrationHandle =
        static_cast<MigrationHandle *>(handle.getHandle());
    if (!migrationHandle->lazySetupVeryFirstMigratingInfo()) {
        return false;
    }
    LockGuard lockGuard(infos->getSharedLock());
    std::shared_ptr<MigrationInfo> info = infos->getMigratingInfo();
    if (info != nullptr) {
        return true;
    }
    if (infos->getInfos().empty()) {
        done = true;
        return true;
    }
    //pick one info to migrate
    info = infos->getInfos().begin()->second;
    if (info->isSameDatabaseMigration()) {
        //cross database migration first
        for (const auto &infoToBePicked : infos->getInfos()) {
            if (!infoToBePicked.second->isSameDatabaseMigration()) {
                info = infoToBePicked.second;
                break;
            }
        }
    }
    bool commited = runTransaction([info](Handle *handle) -> bool {
        //save migrating value
        KeyValueTable keyValueTable(handle);
        if (!keyValueTable.createTable() ||
            !keyValueTable.setMigratingValue(info->targetTable)) {
            return false;
        }
        //sync sequence
        MigrationHandle *migrationHandle =
            static_cast<MigrationHandle *>(handle);
        auto targetSequenceExists =
            migrationHandle->isTableExists("sqlite_sequence");
        if (!targetSequenceExists.second) {
            //failed or sqlite_sequence not exists
            return targetSequenceExists.first;
        }
        //sync sequence if sqlite_sequence exists in target database
        auto sourceSequenceExists =
            migrationHandle->isTableExists(info->getSourceTable());
        if (!sourceSequenceExists.first) {
            return false;
        }
        //Get expected sequence
        StatementSelect unionStatement = info->getStatementForGettingMaxRowID();
        unionStatement.union_(info->getSelectionForGettingTargetSequence());
        if (sourceSequenceExists.second) {
            unionStatement.union_(info->getSelectionForGettingSourceSequence());
        }
        StatementSelect statementForMergedSequence =
            info->getStatementForMergedSequence(unionStatement);
        if (!migrationHandle->prepareWithoutTampering(
                statementForMergedSequence)) {
            return false;
        }
        bool done;
        if (!migrationHandle->step(done) || done) {
            //failed or empty
            migrationHandle->finalize();
            return done;
        }
        int sequence = migrationHandle->getInteger32(0);
        migrationHandle->finalize();

        //Update sequence
        if (!migrationHandle->executeWithoutTampering(
                info->getStatementForUpdatingSequence(sequence))) {
            return false;
        }

        //Or insert sequence
        if (migrationHandle->getChanges() == 0 &&
            !migrationHandle->executeWithoutTampering(
                info->getStatementForInsertingSequence(sequence))) {
            return false;
        }
        return true;
    });
    if (!commited) {
        return false;
    }
    infos->markAsMigrationStarted(info->targetTable);
    return true;
}

bool MigrationDatabase::stepMigration(
    bool &done, const TableMigratedCallback &onMigratingCompleted)
{
#ifdef DEBUG
    WCTAssert(m_migrationPool->debug_checkMigratingThread(),
              "Migration stepping is not thread-safe.");
#endif
    //TODO debug check migrating thread
    MigrationInfos *infos = m_migrationPool->getMigrationInfos();
    if (infos->didMigrationDone()) {
        done = true;
        return true;
    }
    std::shared_ptr<MigrationInfo> migratingInfo = infos->getMigratingInfo();
    if (migratingInfo == nullptr) {
        return startMigration(done);
    }
    //Migration infos will be changed only in migrating thread expect the lazy init migrating info. So no lock is needed.
    bool dropped = false;
    bool empty = false;
    bool committed = runTransaction(
        [this, migratingInfo, &dropped, &empty](Handle *handle) -> bool {
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            std::pair<bool, bool> sourceTableExists =
                handle->isTableExists(migratingInfo->getSourceTable());
            if (!sourceTableExists.first) {
                return false;
            }
            if (!sourceTableExists.second) {
                dropped = true;
                return false;
            }
            //migration
            if (!migrationHandle->executeWithoutTampering(
                    migratingInfo->getStatementForMigration())) {
                m_migrationPool->setThreadedError(handle->getError());
                return false;
            }
            if (migrationHandle->getChanges() == 0) {
                //nothing to migrate
                empty = true;
                return false;
            }
            return migrationHandle->executeWithoutTampering(
                migratingInfo->getStatementForDeleteingMigratedRow());
        });
    if (!dropped && !empty) {
        return committed;
    }
    {
        LockGuard lockGuard(infos->getSharedLock());
        if (empty &&
            !execute(migratingInfo->getStatementForDroppingOldTable())) {
            return false;
        }
        bool schemaChanged = false;
        infos->markAsMigrated(schemaChanged);
        if (schemaChanged) {
            setConfig(
                MigrationBuiltinConfig::autoAttachAndDetachWithInfos(infos));
        }
    }
    if (onMigratingCompleted) {
        onMigratingCompleted(migratingInfo);
    }
    return true;
}

void MigrationDatabase::asyncMigration(
    const SteppedCallback &onStepped,
    const TableMigratedCallback &onTableMigrated,
    const MigratedCallback &onMigrated)
{
    std::string name("com.Tencent.WCDB.Migration.");
    name.append(std::to_string(getTag()));
    std::string path = getPath();
    Dispatch::async(name, [path, onStepped, onTableMigrated,
                           onMigrated](const std::atomic<bool> &stop) {
        bool done = false;
        while (!done && !stop.load()) {
            std::shared_ptr<Database> database =
                MigrationDatabase::databaseWithExistingPath(path);
            if (!database) {
                break;
            }
            MigrationDatabase *migrationDatabase =
                static_cast<MigrationDatabase *>(database.get());
            bool result =
                migrationDatabase->stepMigration(done, onTableMigrated);
            if (onStepped) {
                result = onStepped(migrationDatabase->m_migrationPool
                                       ->getMigrationInfos()
                                       ->getMigratingInfo(),
                                   result) ||
                         result;
            }
            if (!result) {
                break;
            }
        }
        if (onMigrated) {
            onMigrated(done);
        }
    });
}

} //namespace WCDB
