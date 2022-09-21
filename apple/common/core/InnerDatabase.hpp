//
// Created by sanhuazhang on 2019/05/02
//

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
#include <WCDB/MergeFTSIndexLogic.hpp>
#include <WCDB/Migration.hpp>
#include <WCDB/Tag.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <WCDB/TransactionGuard.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class BaseOperation;

// TODO: readonly manually - by removing basic config and adding query_only config?
// TODO: support authorize
class InnerDatabase final : private HandlePool,
                            public MigrationEvent,
                            public MergeFTSIndexHandleProvider,
                            public TransactionEvent {
    friend BaseOperation;
#pragma mark - Initializer
    friend BaseOperation;

public:
    InnerDatabase(const UnsafeStringView &path);
    InnerDatabase() = delete;
    InnerDatabase(const InnerDatabase &) = delete;
    InnerDatabase &operator=(const InnerDatabase &) = delete;
    ~InnerDatabase() override final;

protected:
    // All public interfaces that are related with concurrency should make sure the initialization.
    typedef SharedLockGuard InitializedGuard;
    InitializedGuard initialize();
    bool m_initialized;
    std::atomic<int> m_closing;

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
    RecyclableHandle getHandle() override;
    bool execute(const Statement &statement);
    bool execute(const UnsafeStringView &sql);
    std::optional<bool> tableExists(const UnsafeStringView &table);

protected:
    std::shared_ptr<InnerHandle> generateSlotedHandle(HandleType type) override final;
    bool willReuseSlotedHandle(HandleType type, InnerHandle *handle) override final;

private:
    bool setupHandle(HandleType type, InnerHandle *handle);

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
    void markHandleAsTransactioned(InnerHandle *handle) override final;
    void markHandleAsUntransactioned() override final;

    ThreadLocal<RecyclableHandle> m_transactionedHandles;

#pragma mark - Transaction
public:
    using TransactionCallback = InnerHandle::TransactionCallback;
    using TransactionCallbackForOneLoop = InnerHandle::TransactionCallbackForOneLoop;

    bool isInTransaction();

    bool beginTransaction();
    bool commitOrRollbackTransaction();
    void rollbackTransaction();
    bool runTransaction(const TransactionCallback &transaction);
    bool runPauseableTransactionWithOneLoop(const TransactionCallbackForOneLoop &transaction);

    bool beginNestedTransaction();
    bool commitOrRollbackNestedTransaction();
    void rollbackNestedTransaction();
    bool runNestedTransaction(const TransactionCallback &transaction);

#pragma mark - File
public:
    const StringView &getPath() const override;
    std::list<StringView> getPaths() const;
    static std::list<StringView> pathsOfDatabase(const UnsafeStringView &database);

    bool moveFiles(const UnsafeStringView &directory);
    bool removeFiles();
    std::optional<size_t> getFilesSize();

#pragma mark - Repair
public:
    typedef Repair::Factory::Filter BackupFilter;
    void filterBackup(const BackupFilter &tableShouldBeBackedup);
    bool backup(bool interruptible);
    bool removeMaterials();

    bool deposit();
    bool removeDeposited();

    typedef Repair::FactoryRetriever::ProgressUpdateCallback RetrieveProgressCallback;
    double retrieve(const RetrieveProgressCallback &onProgressUpdated);
    bool containsDeposited() const;

    void checkIntegrity(bool interruptible);

private:
    Repair::Factory m_factory;

#pragma mark - Migration
public:
    typedef Migration::Filter MigrationFilter;
    void filterMigration(const MigrationFilter &filter);

    typedef std::function<void(InnerDatabase *, const MigrationBaseInfo *)> MigratedCallback;
    void setNotificationWhenMigrated(const MigratedCallback &callback);

    std::optional<bool> stepMigration(bool interruptible);

    bool isMigrated() const;

    std::set<StringView> getPathsOfSourceDatabases() const;

protected:
    void didMigrate(const MigrationBaseInfo *info) override final;
    Migration m_migration; // thread-safe
    MigratedCallback m_migratedCallback;

#pragma mark - Checkpoint
public:
    using CheckPointMode = AbstractHandle::CheckpointMode;
    bool checkpoint(bool interruptible = true, CheckPointMode mode = CheckPointMode::Passive);

#pragma mark - Memory
public:
    using HandlePool::purge;
    void setInMemory();

private:
    bool m_isInMemory;
    std::shared_ptr<InnerHandle> m_sharedInMemoryHandle;

#pragma mark - Error
public:
    using HandlePool::getThreadedError;

#pragma mark - Merge FTS Index
public:
    using TableArray = std::shared_ptr<std::vector<StringView>>;
    std::optional<bool> mergeFTSIndex(TableArray newTables, TableArray modifiedTables);

private:
    MergeFTSIndexLogic m_mergeLogic;
};

} //namespace WCDB
