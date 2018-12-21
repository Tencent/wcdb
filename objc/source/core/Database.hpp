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

#ifndef _WCDB_DATABASE_HPP
#define _WCDB_DATABASE_HPP

#include <WCDB/Configs.hpp>
#include <WCDB/Factory.hpp>
#include <WCDB/HandlePool.hpp>
#include <WCDB/Migration.hpp>
#include <WCDB/Tag.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class Database final : private HandlePool {
#pragma mark - Initializer
public:
    Database(const String &path);
    Database() = delete;
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

protected:
    typedef SharedLockGuard InitializedGuard;
    InitializedGuard initialize();
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

#pragma mark - Handle
public:
    RecyclableHandle getHandle();
    bool execute(const Statement &statement);
    std::pair<bool, bool> tableExists(const String &table);

protected:
    RecyclableHandle getSlotHandle(Slot slot);

protected:
    static const Slot ConfiguredHandleSlot = 0;
    static const Slot MigrationHandleSlot = 1;
    static constexpr const Slot BackupReadSlot = 2;
    static constexpr const Slot BackupWriteSlot = 3;
    static constexpr const Slot AssemblerSlot = 4;
    static constexpr const Slot MigrationStepperSlot = 5;
    std::shared_ptr<Handle> generateHandle(Slot slot) override final;
    bool willConfigureHandle(Slot slot, Handle *handle) override final;

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
    using ThreadedHandles = std::map<const Database *, RecyclableHandle>;
    static ThreadLocal<ThreadedHandles> &threadedHandles();
    static void
    markHandleAsThreaded(const Database *database, const RecyclableHandle &handle);
    static void markHandleAsUnthreaded(const Database *database);

    class ThreadedGuard final {
    public:
        ThreadedGuard(const Database *database, const RecyclableHandle &handle);
        ~ThreadedGuard();

    private:
        bool m_isInTransactionBefore;
        const Database *m_database;
        RecyclableHandle m_handle;
    };

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
    bool removeMaterials();

    bool deposit();
    bool removeDeposited();

    typedef Repair::FactoryRetriever::ProgressUpdateCallback RetrieveProgressCallback;
    double retrieve(const RetrieveProgressCallback &onProgressUpdate);
    bool containsDeposited() const;

private:
    bool retrieveRenewed();
    Repair::Factory m_factory;

#pragma mark - Recovery
public:
    typedef std::function<bool(Database *)> RecoverNotification;
    void setNotificationWhenCorrupted(const RecoverNotification &notification);

    bool recover(uint32_t corruptedIdentifier);

private:
    RecoverNotification m_recoverNotification;

#pragma mark - Migration
public:
    typedef Migration::Filter MigrationFilter;
    void filterMigration(const MigrationFilter &filter);

    typedef Migration::MigratedCallback MigratedCallback;
    void setNotificationWhenMigrated(const MigratedCallback &callback);

    std::pair<bool, bool> stepMigration(bool interruptible);

    void interruptMigration();

protected:
    Migration m_migration;

#pragma mark - Memory
public:
    using HandlePool::purge;

#pragma mark - Error
public:
    using HandlePool::getThreadedError;
};

} //namespace WCDB

#endif /* database_hpp */
