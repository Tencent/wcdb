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

#ifndef __WCDB_CORE_H
#define __WCDB_CORE_H

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/BackupQueue.hpp>
#include <WCDB/CheckpointQueue.hpp>
#include <WCDB/CorruptionQueue.hpp>
#include <WCDB/MigrationQueue.hpp>

#include <WCDB/BackupConfig.hpp>
#include <WCDB/BasicConfig.hpp>
#include <WCDB/CheckpointConfig.hpp>
#include <WCDB/CipherConfig.hpp>
#include <WCDB/Config.hpp>
#include <WCDB/Configs.hpp>
#include <WCDB/CustomConfig.hpp>
#include <WCDB/PerformanceTraceConfig.hpp>
#include <WCDB/SQLTraceConfig.hpp>
#include <WCDB/TokenizerConfig.hpp>

#include <WCDB/Database.hpp>
#include <WCDB/DatabasePool.hpp>
#include <WCDB/RecyclableHandle.hpp>

#include <WCDB/OneOrBinaryTokenizer.hpp>
#include <WCDB/TokenizerModule.hpp>
#include <WCDB/TokenizerModules.hpp>

#include <WCDB/MigrationInfo.hpp>

#include <WCDB/CoreConst.h>

namespace WCDB {

// The order of member variables here is important.
class Core final : public DatabasePoolEvent,
                   public CheckpointEvent,
                   public BackupEvent,
                   public MigrationEvent {
#pragma mark - Core
public:
    static Core* shared();
    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;
    ~Core();

protected:
    Core();
    static int vfsOpen(const char* path, int flags, int mode);
    static void globalLog(void* core, int code, const char* message);

#pragma mark - Database
public:
    RecyclableDatabase getOrCreateDatabase(const String& path);
    RecyclableDatabase getAlivingDatabase(const String& path);

    void purgeDatabasePool();

protected:
    void preprocessError(Error& error);
    void onDatabaseCreated(Database* database) override final;
    DatabasePool m_databasePool;

#pragma mark - Tokenizer
public:
    void addTokenizer(const String& name, const TokenizerModule& module);
    std::shared_ptr<Config> tokenizerConfig(const std::list<String>& tokenizeNames);

protected:
    TokenizerModules m_modules;

#pragma mark - Corruption
public:
    typedef std::function<bool(Database*)> CorruptedNotification;
    bool isFileCorrupted(const String& path);
    void setNotificationWhenDatabaseCorrupted(const String& path,
                                              const CorruptedNotification& notification);

protected:
    std::shared_ptr<CorruptionQueue> m_corruptionQueue;

#pragma mark - Checkpoint
public:
    bool databaseShouldCheckpoint(const String& path, int frames) override final;

protected:
    std::shared_ptr<CheckpointQueue> m_checkpointQueue;

#pragma mark - Backup
public:
    const std::shared_ptr<Config>& backupConfig();
    bool databaseShouldBackup(const String& path) override final;

protected:
    std::shared_ptr<BackupQueue> m_backupQueue;
    std::shared_ptr<Config> m_backupConfig;

#pragma mark - Migration
public:
    void setAutoMigration(const String& path, bool flag);
    std::pair<bool, bool> databaseShouldMigrate(const String& path) override final;

protected:
    std::shared_ptr<MigrationQueue> m_migrationQueue;

#pragma mark - Trace
public:
    void setNotificationForSQLGLobalTraced(const ShareableSQLTraceConfig::Notification& notification);
    void setNotificationWhenPerformanceGlobalTraced(
    const ShareablePerformanceTraceConfig::Notification& notification);
    std::shared_ptr<Config> sqlTraceConfig(const SQLTraceConfig::Notification& notification);
    std::shared_ptr<Config>
    performanceTraceConfig(const PerformanceTraceConfig::Notification& notification);

protected:
    std::shared_ptr<Config> m_globalSQLTraceConfig;
    std::shared_ptr<Config> m_globalPerformanceTraceConfig;

#pragma mark - Cipher
public:
    std::shared_ptr<Config> cipherConfig(const UnsafeData& cipher, int pageSize = 4096);

#pragma mark - Config
public:
    const std::shared_ptr<Configs>& configs();

    std::shared_ptr<Config>
    customConfig(const CustomConfig::Invocation& invocation,
                 const CustomConfig::Invocation& uninvocation = nullptr);

protected:
    std::shared_ptr<Configs> m_configs;
};

} // namespace WCDB

#endif /* __WCDB_CORE_H */
