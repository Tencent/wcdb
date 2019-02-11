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
#include <WCDB/TokenizeConfig.hpp>

#include <WCDB/Database.hpp>
#include <WCDB/DatabasePool.hpp>
#include <WCDB/RecyclableHandle.hpp>

#include <WCDB/Modules.hpp>
#include <WCDB/Tokenizer.hpp>

#include <WCDB/MigrationInfo.hpp>

#include <WCDB/CoreConst.h>

namespace WCDB {

class Core final : public DatabasePoolEvent,
                   public CheckpointEvent,
                   public BackupEvent,
                   public MigrationEvent {
public:
    static Core* shared();
    ~Core();

    RecyclableDatabase getOrCreateDatabase(const String& path);
    RecyclableDatabase getExistingDatabase(const String& path);

    typedef std::function<bool(Database*)> CorruptedNotification;
    bool isFileCorrupted(const String& path);
    void setNotificationWhenDatabaseCorrupted(const String& path,
                                              const CorruptedNotification& notification);

    void setAutoMigration(const String& path, bool flag);

    void purgeDatabasePool();

    const std::shared_ptr<Configs>& configs();

    void addTokenizer(const String& name, unsigned char* address);
    void setNotificationForSQLGLobalTraced(const ShareableSQLTraceConfig::Notification& notification);
    void setNotificationWhenPerformanceGlobalTraced(
    const ShareablePerformanceTraceConfig::Notification& notification);

    const std::shared_ptr<Config>& backupConfig();

    std::shared_ptr<Config> tokenizeConfig(const std::list<String>& tokenizeNames);

    std::shared_ptr<Config> cipherConfig(const UnsafeData& cipher, int pageSize = 4096);

    std::shared_ptr<Config> sqlTraceConfig(const SQLTraceConfig::Notification& notification);

    std::shared_ptr<Config>
    performanceTraceConfig(const PerformanceTraceConfig::Notification& notification);

    std::shared_ptr<Config>
    customConfig(const CustomConfig::Invocation& invocation,
                 const CustomConfig::Invocation& uninvocation = nullptr);

    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

protected:
    Core();

    static int vfsOpen(const char* path, int flags, int mode);
    static void globalLog(void* core, int code, const char* message);
    void preprocessError(const Error& error, Error::Infos& infos);
    void onDatabaseCreated(Database* database) override final;
    bool databaseShouldCheckpoint(const String& path, int frames) override final;
    bool databaseShouldBackup(const String& path) override final;
    std::pair<bool, bool> databaseShouldMigrate(const String& path) override final;

    // The order of member variables here is important.
    DatabasePool m_databasePool;
    std::shared_ptr<FTS::Modules> m_modules;

    std::shared_ptr<CorruptionQueue> m_corruptionQueue;
    std::shared_ptr<CheckpointQueue> m_checkpointQueue;
    std::shared_ptr<BackupQueue> m_backupQueue;
    std::shared_ptr<MigrationQueue> m_migrationQueue;

    std::shared_ptr<Config> m_backupConfig;
    std::shared_ptr<Config> m_checkpointConfig;
    std::shared_ptr<Config> m_globalSQLTraceConfig;
    std::shared_ptr<Config> m_globalPerformanceTraceConfig;
    std::shared_ptr<Configs> m_configs;
};

} // namespace WCDB

#endif /* __WCDB_CORE_H */
