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

#include "Compression.hpp"
#include "Configs.hpp"
#include "Factory.hpp"
#include "HandlePool.hpp"
#include "MergeFTSIndexLogic.hpp"
#include "Migration.hpp"
#include "Tag.hpp"
#include "ThreadLocal.hpp"
#include "TransactionGuard.hpp"
#include "WINQ.h"

namespace WCDB {

class BaseOperation;

// TODO: readonly manually - by removing basic config and adding query_only config?
// TODO: support authorize
class InnerDatabase final : private HandlePool,
                            public MigrationEvent,
                            public CompressionEvent,
                            public MergeFTSIndexHandleProvider,
                            public TransactionEvent {
    friend BaseOperation;
#pragma mark - Initializer
public:
    InnerDatabase(const UnsafeStringView &path);
    InnerDatabase() = delete;
    InnerDatabase(const InnerDatabase &) = delete;
    InnerDatabase &operator=(const InnerDatabase &) = delete;
    ~InnerDatabase() override;

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
    using HandlePool::numberOfAliveHandles;

protected:
    Tag m_tag;

    void didDrain() override final;
    bool checkShouldInterruptWhenClosing(const UnsafeStringView &sourceType);

#pragma mark - Handle
public:
    RecyclableHandle getHandle(bool writeHint = false);
    bool execute(const Statement &statement);
    bool execute(const UnsafeStringView &sql);
    Optional<bool> tableExists(const UnsafeStringView &table);
    StringView getRunningSQLInThread(uint64_t tid) const;

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
    void setFullSQLTraceEnable(bool enable);
    void setAutoCheckpointEnable(bool enable);

private:
    Configs m_configs;
    bool m_fullSQLTrace = false;
    bool m_autoCheckpoint;

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
    bool runPausableTransactionWithOneLoop(const TransactionCallbackForOneLoop &transaction);

#pragma mark - File
public:
    const StringView &getPath() const override;
    std::list<StringView> getPaths() const;
    static std::list<StringView> pathsOfDatabase(const UnsafeStringView &database);

    bool moveFiles(const UnsafeStringView &directory);
    bool removeFiles();
    Optional<size_t> getFilesSize();

#pragma mark - Repair
public:
    void markNeedLoadIncremetalMaterial();
    void tryLoadIncremetalMaterial();
    typedef Repair::Factory::Filter BackupFilter;
    void filterBackup(const BackupFilter &tableShouldBeBackedup);
    bool backup(bool interruptible);
    bool removeMaterials();

    bool deposit();
    bool removeDeposited();
    bool containsDeposited() const;

    typedef Repair::FactoryRetriever::ProgressUpdateCallback ProgressCallback;
    double retrieve(const ProgressCallback &onProgressUpdated);
    bool vacuum(const ProgressCallback &onProgressUpdated);

    void checkIntegrity(bool interruptible);

private:
    Repair::Factory m_factory;
    bool m_needLoadIncremetalMaterial;

#pragma mark - Migration
public:
    typedef Migration::TableFilter MigrationTableFilter;
    void addMigration(const UnsafeStringView &sourcePath,
                      const UnsafeData &sourceCipher,
                      const MigrationTableFilter &filter);

    typedef std::function<void(InnerDatabase *, const MigrationBaseInfo *)> MigratedCallback;
    void setNotificationWhenMigrated(const MigratedCallback &callback);

    Optional<bool> stepMigration(bool interruptible);

    bool isMigrated() const;

    StringViewSet getPathsOfSourceDatabases() const;

protected:
    void didMigrate(const MigrationBaseInfo *info) override final;
    Migration m_migration; // thread-safe
    MigratedCallback m_migratedCallback;

#pragma mark - Compression
public:
    typedef Compression::TableFilter CompressionTableFilter;
    void addCompression(const CompressionTableFilter &filter);

    void setCanCompressNewData(bool canCompress);

    typedef std::function<void(InnerDatabase *, const CompressionTableBaseInfo *)> CompressedCallback;
    void setNotificationWhenCompressed(const CompressedCallback &callback);

    Optional<bool> stepCompression(bool interruptible);

    bool isCompressed() const;

protected:
    void didCompress(const CompressionTableBaseInfo *info) override final;
    Compression m_compression; // thread-safe
    CompressedCallback m_compressedCallback;

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
    Optional<bool> mergeFTSIndex(TableArray newTables, TableArray modifiedTables);
    void proccessMerge();
    RecyclableHandle getMergeIndexHandle() override final;

private:
    MergeFTSIndexLogic m_mergeLogic;
};

} //namespace WCDB
