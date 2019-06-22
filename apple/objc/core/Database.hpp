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

#pragma once

#include <WCDB/Configs.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/HandlePool.hpp>
#include <WCDB/Migration.hpp>
#include <WCDB/Tag.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

// TODO: readonly manually - by removing basic config and adding query_only config?
// TODO: support authorize
class Database final : private HandlePool, public MigrationEvent {
#pragma mark - Initializer
public:
    Database(const UnsafeStringView &path);
    Database() = delete;
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

protected:
    // All public interfaces that are related with concurrency should make sure the initialization.
    typedef SharedLockGuard InitializedGuard;
    InitializedGuard initialize();
    InitializedGuard isInitialized() const;
    bool m_initialized;

#pragma mark - Basic
public:
    void setTag(const Tag &tag);
    Tag getTag() const;

    bool canOpen();
    bool isOpened() const;
    using HandlePool::blockade;
    using ClosedCallback = HandlePool::DrainedCallback;
    void close(const ClosedCallback &onClosed);
    using HandlePool::unblockade;
    using HandlePool::isBlockaded;

protected:
    Tag m_tag;

    void didDrain() override final;

#pragma mark - Handle
public:
    RecyclableHandle getHandle();
    bool execute(const Statement &statement);
    std::optional<bool> tableExists(const UnsafeStringView &table);

protected:
    std::shared_ptr<Handle> generateSlotedHandle(HandleType type) override final;
    bool willReuseSlotedHandle(HandleType type, Handle *handle) override final;

private:
    bool setupHandle(HandleType type, Handle *handle);

#pragma mark - Config
public:
    void setConfigs(const Configs &configs);
    void setConfig(const UnsafeStringView &name,
                   const std::shared_ptr<Config> &config,
                   int priority = Configs::Priority::Default);
    void removeConfig(const UnsafeStringView &name);

private:
    Configs m_configs;

#pragma mark - Threaded
private:
    void markHandleAsTransactioned(const RecyclableHandle &handle);
    void markHandleAsUntransactioned();

    class TransactionGuard final {
    public:
        TransactionGuard(Database *database, const RecyclableHandle &handle);
        ~TransactionGuard();

    private:
        Database *m_database;
        bool m_isInTransactionBefore;
        RecyclableHandle m_handle;
    };

    ThreadLocal<RecyclableHandle> m_transactionedHandles;

#pragma mark - Transaction
public:
    using TransactionCallback = Handle::TransactionCallback;

    bool isInTransaction();

    bool beginTransaction();
    bool commitOrRollbackTransaction();
    void rollbackTransaction();
    bool runTransaction(const TransactionCallback &transaction);

    bool beginNestedTransaction();
    bool commitOrRollbackNestedTransaction();
    void rollbackNestedTransaction();
    bool runNestedTransaction(const TransactionCallback &transaction);

#pragma mark - File
public:
    const StringView &getPath() const;
    StringView getSHMPath() const;
    StringView getWALPath() const;
    StringView getJournalPath() const;
    std::list<StringView> getPaths() const;

    bool moveFiles(const UnsafeStringView &directory);
    bool removeFiles();
    std::optional<size_t> getFilesSize();

#pragma mark - Repair
public:
    StringView getFirstMaterialPath() const;
    StringView getLastMaterialPath() const;
    const StringView &getFactoryDirectory() const;

    typedef Repair::Factory::Filter BackupFilter;
    void filterBackup(const BackupFilter &tableShouldBeBackedup);
    bool backup();
    bool backupIfAlreadyInitialized();
    bool removeMaterials();

    bool deposit();
    bool removeDeposited();

    typedef Repair::FactoryRetriever::ProgressUpdateCallback RetrieveProgressCallback;
    double retrieve(const RetrieveProgressCallback &onProgressUpdate);
    bool containsDeposited() const;

    void checkIntegrity();
    void checkIntegrityIfAlreadyInitialized();

private:
    void doCheckIntegrity();
    bool doBackup();
    bool retrieveRenewed();
    Repair::Factory m_factory;

#pragma mark - Migration
public:
    typedef Migration::Filter MigrationFilter;
    void filterMigration(const MigrationFilter &filter);

    typedef std::function<void(Database *, const MigrationBaseInfo *)> MigratedCallback;
    void setNotificationWhenMigrated(const MigratedCallback &callback);

    std::pair<bool, bool> stepMigration();
    std::pair<bool, bool> stepMigrationIfAlreadyInitialized();

    bool isMigrated() const;

    std::set<StringView> getPathsOfSourceDatabases() const;

protected:
    std::pair<bool, bool> doStepMigration();
    void didMigrate(const MigrationBaseInfo *info) override final;
    MigratedCallback m_migratedCallback;
    Migration m_migration; // thread-safe

#pragma mark - Checkpoint
public:
    bool checkpointIfAlreadyInitialized();

#pragma mark - Memory
public:
    using HandlePool::purge;

#pragma mark - Error
public:
    using HandlePool::getThreadedError;
};

} //namespace WCDB
