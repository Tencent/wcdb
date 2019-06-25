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

#include <WCDB/OperationQueue.hpp>

#include <WCDB/Config.hpp>
#include <WCDB/Configs.hpp>
#include <WCDB/PerformanceTraceConfig.hpp>
#include <WCDB/SQLTraceConfig.hpp>

#include <WCDB/Database.hpp>
#include <WCDB/DatabasePool.hpp>

#include <WCDB/TokenizerModules.hpp>

#include <WCDB/Notifier.hpp>

namespace WCDB {

// The order of member variables here is important.
class Core final : public DatabasePoolEvent, public OperationEvent {
#pragma mark - Core
public:
    static Core& shared();
    ~Core();

protected:
    Core();
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

#pragma mark - Database
public:
    RecyclableDatabase getOrCreateDatabase(const UnsafeStringView& path);
    RecyclableDatabase getAlivingDatabase(const UnsafeStringView& path);

    void purgeDatabasePool();

protected:
    void preprocessError(Error& error);
    void databaseDidCreate(Database* database) override final;
    DatabasePool m_databasePool;

#pragma mark - Tokenizer
public:
    void registerTokenizer(const UnsafeStringView& name, const TokenizerModule& module);
    std::shared_ptr<Config> tokenizerConfig(const UnsafeStringView& tokenizeName);
    bool tokenizerExists(const UnsafeStringView& name) const;

protected:
    std::shared_ptr<TokenizerModules> m_modules;

#pragma mark - Operation
public:
    typedef std::function<void(Database*)> CorruptedNotification;
    bool isFileObservedCorrupted(const UnsafeStringView& path);
    void setNotificationWhenDatabaseCorrupted(const UnsafeStringView& path,
                                              const CorruptedNotification& notification);

protected:
    std::optional<bool>
    migrationShouldBeOperated(const UnsafeStringView& path) override final;
    bool backupShouldBeOperated(const UnsafeStringView& path) override final;
    bool checkpointShouldBeOperated(const UnsafeStringView& path) override final;
    void integrityShouldBeChecked(const UnsafeStringView& path) override final;
    void purgeShouldBeOperated() override final;

    std::shared_ptr<OperationQueue> m_operationQueue;

#pragma mark - Checkpoint
public:
    void enableAutoCheckpoint(Database* database, bool enable);

private:
    std::shared_ptr<Config> m_autoCheckpointConfig;

#pragma mark - Backup
public:
    void enableAutoBackup(Database* database, bool enable);

protected:
    std::shared_ptr<Config> m_autoBackupConfig;

#pragma mark - Migration
public:
    void enableAutoMigration(Database* database, bool enable);

protected:
    std::shared_ptr<Config> m_autoMigrateConfig;

#pragma mark - Trace
public:
    void setNotificationForSQLGLobalTraced(const ShareableSQLTraceConfig::Notification& notification);
    void setNotificationWhenPerformanceGlobalTraced(
    const ShareablePerformanceTraceConfig::Notification& notification);
    void setNotificationWhenErrorTraced(const Notifier::Callback& notification);

protected:
    std::shared_ptr<Config> m_globalSQLTraceConfig;
    std::shared_ptr<Config> m_globalPerformanceTraceConfig;

    void globalLog(int rc, const char* message);

    static void breakpoint() WCDB_USED WCDB_NO_INLINE;
    static void onErrorTraced(const Error& error);
    static void print(const UnsafeStringView& message);

#pragma mark - Config
protected:
    Configs m_configs;
};

} // namespace WCDB
