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

#include <WCDB/AsyncLoop.hpp>
#include <WCDB/BuiltinConfig.hpp>
#include <WCDB/HandlePools.hpp>
#include <WCDB/MigrationBuiltinConfig.hpp>
#include <WCDB/MigrationDatabase.hpp>
#include <WCDB/MigrationHandle.hpp>

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
        assert(migrationInfos != nullptr);
        if (migrationInfos->isSameDatabaseMigration()) {
            return std::shared_ptr<HandlePool>(new MigrationHandlePool(
                path, BuiltinConfig::defaultConfigs(), migrationInfos));
        }
        Configs configs = BuiltinConfig::defaultConfigs();
        configs.setConfig(MigrationBuiltinConfig::autoAttachAndDetachWithInfos(
            migrationInfos.get()));
        return std::shared_ptr<HandlePool>(
            new MigrationHandlePool(path, configs, migrationInfos));
    };

    std::shared_ptr<Database> database(new MigrationDatabase(
        HandlePools::defaultPools()->getPool(path, s_generator)));
    if (database &&
        static_cast<MigrationDatabase *>(database.get())->isValid()) {
        return database;
    }
    return nullptr;
}

MigrationDatabase::MigrationDatabase(const RecyclableHandlePool &pool)
    : Database(pool)
    , m_migrationPool(pool != nullptr ? dynamic_cast<MigrationHandlePool *>(
                                            pool.getHandlePool())
                                      : nullptr)
    , m_migratingThread(nullptr)
{
    assert(pool == nullptr || m_migrationPool != nullptr);
}

#pragma mark - Migration
bool MigrationDatabase::startMigration(bool &done)
{
    done = false;
    MigrationInfos *infos = m_migrationPool->getMigrationInfos();
    assert(!infos->getInfos().empty());
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
    MigrationInfo *info = infos->getMigratingInfo();
    if (info != nullptr) {
        return true;
    }
    if (infos->getInfos().empty()) {
        done = true;
        return true;
    }
    //pick one info to migrate
    info = infos->getInfos().begin()->second.get();
    if (info->isSameDatabaseMigration()) {
        //cross database migration first
        for (const auto &infoToBePicked : infos->getInfos()) {
            if (!infoToBePicked.second->isSameDatabaseMigration()) {
                info = infoToBePicked.second.get();
                break;
            }
        }
    }
    info->prepareForMigrating();
    bool result = false;
    bool needCommit = false;
    bool commited = runTransaction([&result, &needCommit,
                                    info](Handle *handle) -> bool {
        MigrationHandle *migrationHandle =
            static_cast<MigrationHandle *>(handle);
        auto targetSequenceExists =
            migrationHandle->isTableExists("sqlite_sequence");
        if (!targetSequenceExists.second) {
            result = targetSequenceExists.first;
            return false;
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
            result = done;
            migrationHandle->finalize();
            return false;
        }
        int sequence = migrationHandle->getInteger32(0);
        migrationHandle->finalize();

        needCommit = true;

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
        result = true;
        return true;
    });
    if ((needCommit && !commited) || !result) {
        return false;
    }
    infos->markAsMigrationStarted(info->targetTable);
    return true;
}

bool MigrationDatabase::stepMigration(bool &done)
{
#ifdef DEBUG
    if (m_migratingThread == nullptr) {
        m_migratingThread = pthread_self();
    }
    assert(m_migratingThread == pthread_self());
#endif
    MigrationInfos *infos = m_migrationPool->getMigrationInfos();
    if (infos->didMigrationDone()) {
        done = true;
        return true;
    }
    MigrationInfo *migratingInfo = infos->getMigratingInfo();
    if (migratingInfo == nullptr) {
        return startMigration(done);
    }
    //Migration infos will be changed only in migrating thread expect the lazy init migrating info. So no lock is needed.
    bool migrated = false;
    bool needDropTable = false;
    bool needCommit = false;
    bool committed = runTransaction([this, migratingInfo, &needCommit,
                                     &needDropTable,
                                     &migrated](Handle *handle) -> bool {
        MigrationHandle *migrationHandle =
            static_cast<MigrationHandle *>(handle);
        std::pair<bool, bool> sourceTableExists =
            handle->isTableExists(migratingInfo->getSourceTable());
        if (!sourceTableExists.second) {
            migrated = true;
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
            needDropTable = true;
            return false;
        }
        needCommit = true;
        if (!migrationHandle->executeWithoutTampering(
                migratingInfo->getStatementForDeleteingMigratedRow())) {
            return false;
        }

        if (migrationHandle->prepareWithoutTampering(
                migratingInfo->getStatementForCheckingSourceTableEmpty())) {
            //`done == true` will be returned if there is no row to select, which indicates the source table is about to be dropped.
            //The result can be discarded since it will be re-run in next step.
            migrationHandle->step(needDropTable);
            migrationHandle->finalize();
        }
        return true;
    });
    if (!needDropTable && !migrated) {
        return !needCommit || committed;
    }
    //The result can be discarded since it will be re-run in next step.
    LockGuard lockGuard(infos->getSharedLock());
    if (needDropTable &&
        !execute(migratingInfo->getStatementForDroppingOldTable())) {
        return true;
    }
    bool schemaChanged = false;
    infos->markAsMigrated(schemaChanged);
    if (schemaChanged) {
        setConfig(MigrationBuiltinConfig::autoAttachAndDetachWithInfos(infos));
    }
    return true;
}

//TODO async migration

} //namespace WCDB
