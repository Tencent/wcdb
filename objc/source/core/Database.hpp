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

#ifndef __WCDB_DATABASE_HPP
#define __WCDB_DATABASE_HPP

#include <WCDB/Configs.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/HandlePool.hpp>
#include <WCDB/InterruptibleCheckpointHandle.hpp>
#include <WCDB/Migration.hpp>
#include <WCDB/Tag.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class DatabaseEvent {
public:
    virtual ~DatabaseEvent() = 0;

protected:
    friend class Database;
    virtual void databaseDidBackup(const String &path) = 0;
};

// TODO: readonly manually - by removing basic config and adding query_only config?
// TODO: support authorize
class Database final : private HandlePool, public MigrationEvent {
#pragma mark - Initializer
public:
    Database(const String &path);
    Database() = delete;
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

protected:
    // All public interfaces that are related with concurrency should make sure the initialization.
    typedef SharedLockGuard InitializedGuard;
    InitializedGuard initialize();
    InitializedGuard isInitialized();
    bool m_initialized;

#pragma mark - Event
public:
    void setEvent(DatabaseEvent *event);

protected:
    DatabaseEvent *m_event;

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

#pragma mark - Handle
public:
    RecyclableHandle getHandle();
    bool execute(const Statement &statement);
    std::pair<bool, bool> tableExists(const String &table);

protected:
    RecyclableHandle getSlotHandle(Slot slot);

protected:
    enum HandleType : unsigned int {
        Normal = 0,
        Migrate,
        MigrationStep,
        InterruptibleCheckpoint,

        // The handles below are not in HandlePool
        BackupRead = HandlePoolNumberOfSlots,
        BackupWrite,
        Assemble,
    };
    std::shared_ptr<Handle> generateSlotedHandle(Slot slot) override final;
    bool willReuseSlotedHandle(Slot slot, Handle *handle) override final;
    void handleWillStep(HandleStatement *handleStatement);

private:
    std::shared_ptr<Handle> generateHandle(HandleType type);

#pragma mark - Config
public:
    void setConfigs(const std::shared_ptr<Configs> &configs);
    void setConfig(const String &name,
                   const std::shared_ptr<Config> &config,
                   int priority = Configs::Priority::Default);
    void removeConfig(const String &name);

private:
    std::shared_ptr<Configs> m_configs;

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
    const String &getPath() const;
    String getSHMPath() const;
    String getWALPath() const;
    String getJournalPath() const;
    std::list<String> getPaths() const;

    bool moveFiles(const String &directory);
    bool removeFiles();
    std::pair<bool, size_t> getFilesSize();

#pragma mark - Repair
public:
    String getFirstMaterialPath() const;
    String getLastMaterialPath() const;
    const String &getFactoryDirectory() const;

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

private:
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

    void interruptMigration();

    bool isMigrated() const;

protected:
    std::pair<bool, bool> doStepMigration();
    void didMigrate(const MigrationBaseInfo *info) override final;
    MigratedCallback m_migratedCallback;
    Migration m_migration;

#pragma mark - Checkpoint
public:
    typedef InterruptibleCheckpointHandle::Type CheckpointType;
    bool interruptibleCheckpointIfAlreadyInitialized(CheckpointType type);

    void interruptCheckpoint();

#pragma mark - Memory
public:
    using HandlePool::purge;

#pragma mark - Error
public:
    using HandlePool::getThreadedError;
};

} //namespace WCDB

#endif /* database_hpp */
