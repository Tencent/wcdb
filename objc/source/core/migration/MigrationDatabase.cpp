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
    const std::shared_ptr<MigrationInfo> &migrationInfo)
{
    const HandlePools::Generator s_generator =
        [migrationInfo](
            const std::string &path) -> std::shared_ptr<HandlePool> {
        return std::shared_ptr<HandlePool>(new MigrationHandlePool(
            path,
            MigrationBuiltinConfig::defaultConfigsWithMigrationInfo(
                migrationInfo),
            migrationInfo));
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
{
    assert(pool == nullptr || m_migrationPool != nullptr);
}

#pragma mark - Migration
bool MigrationDatabase::stepMigration(bool &done)
{
    assert(m_migrationPool->getMigrationInfo() != nullptr);
    if (m_migrationPool->getMigrationInfo() == nullptr) {
        done = true;
        return true;
    }
    //It should not be run within a transaction
    assert(!isInThreadedTransaction());
    if (isInThreadedTransaction()) {
        return false;
    }
    MigrationInfo *info = m_migrationPool->getMigrationInfo();
    if (!info->isMigrating()) {
        done = false;
        return startMigration();
    }
    bool committed =
        runTransaction([&done, info, this](Handle *handle) -> bool {
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            std::pair<bool, bool> pair =
                handle->isTableExists(info->getSourceTable());
            if (!pair.first) {
                return false;
            }
            if (!pair.second) {
                //return if table is not exists
                done = true;
                return true;
            }
            if (!migrationHandle->executeWithoutTampering(
                    info->getStatementForMigration())) {
                m_migrationPool->setThreadedError(handle->getError());
                return false;
            }
            if (!migrationHandle->executeWithoutTampering(
                    info->getStatementForDeleteingMigratedRow())) {
                return false;
            }
            if (migrationHandle->getChanges() == 0) {
                //nothing to migrate
                if (!migrationHandle->executeWithoutTampering(
                        info->getStatementForDroppingOldTable())) {
                    return false;
                }
                done = true;
            }
            return true;
        });
    done = committed && done;
    if (done) {
        clearMigrationInfo();
    }
    return committed;
}

bool MigrationDatabase::startMigration()
{
    MigrationInfo *info = m_migrationPool->getMigrationInfo();
    assert(!info->isMigrating());
    bool result = false;
    m_pool->blockadeUntilDone([&result, info, this](Handle *handle) {
        //run transaction to avoid attached database operating.
        result = handle->runTransaction([&result, info,
                                         this](Handle *handle) -> bool {
            // Sync sequence before migration
            // It should blockade all other operations to avoid insertion between sequence synchronized and migrating is set to true.
            MigrationHandle *migrationHandle =
                static_cast<MigrationHandle *>(handle);
            auto targetSequenceExists =
                migrationHandle->isTableExists("sqlite_sequence");
            if (!targetSequenceExists.first) {
                return false;
            }
            while (targetSequenceExists.second) {
                //sync sequence if sqlite_sequence exists in target database

                auto sourceSequenceExists =
                    migrationHandle->isTableExists(info->getSourceTable());
                if (!sourceSequenceExists.first) {
                    return false;
                }
                //Get expected sequence
                StatementSelect unionStatement =
                    info->getStatementForGettingMaxRowID();
                unionStatement.union_(
                    info->getSelectionForGettingTargetSequence());
                if (sourceSequenceExists.second) {
                    unionStatement.union_(
                        info->getSelectionForGettingSourceSequence());
                }
                StatementSelect statementForMergedSequence =
                    info->getStatementForMergedSequence(unionStatement);
                if (!migrationHandle->prepareWithoutTampering(
                        statementForMergedSequence) ||
                    !migrationHandle->step()) {
                    return false;
                }
                if (migrationHandle->getType(0) == ColumnType::Null) {
                    migrationHandle->finalize();
                    break;
                }

                int sequence = migrationHandle->getInteger32(0);
                migrationHandle->finalize();

                //Update sequence
                if (!migrationHandle->executeWithoutTampering(
                        info->getStatementForUpdatingSequence(sequence))) {
                    return false;
                }

                //Or insert sequence
                if (migrationHandle->getChanges() != 0) {
                    //succeeds
                    break;
                }
                if (!migrationHandle->executeWithoutTampering(
                        info->getStatementForInsertingSequence(sequence))) {
                    return false;
                }
                break;
            }
            info->setMigrating(true);
            return true;
        });
    });
    return result;
}

//TODO async migration
void MigrationDatabase::clearMigrationInfo()
{
    m_migrationPool->clearMigrationInfo();
    setConfig(MigrationBuiltinConfig::autoDetach());
}

const MigrationInfo *MigrationDatabase::getMigrationInfo() const
{
    return m_migrationPool->getMigrationInfo();
}

} //namespace WCDB
