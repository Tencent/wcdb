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

#ifndef _WCDB_CORE_H
#define _WCDB_CORE_H

#include <WCDB/Abstract.h>

#include <WCDB/AsyncQueue.hpp>
#include <WCDB/BackupQueue.hpp>
#include <WCDB/CheckpointQueue.hpp>
#include <WCDB/CorruptionQueue.hpp>

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

#include <WCDB/ConfiguredHandle.hpp>
#include <WCDB/Database.hpp>
#include <WCDB/DatabasePool.hpp>
#include <WCDB/HandlePool.hpp>
#include <WCDB/RecyclableDatabase.hpp>
#include <WCDB/RecyclableHandle.hpp>

#include <WCDB/Modules.hpp>
#include <WCDB/Tokenizer.hpp>

#include <WCDB/MigrationHandle.hpp>

namespace WCDB {

class Core final : public DatabasePoolEvent {
public:
    static Core* shared();
    ~Core();

    RecyclableDatabase getOrCreateDatabase(const String& path);
    RecyclableDatabase getExistingDatabase(const String& path);
    RecyclableDatabase getExistingDatabase(const Tag& tag);

    void purge();

    const std::shared_ptr<Configs>& configs();

    void addTokenizer(const String& name, unsigned char* address);
    void setNotificationForGlobalSQLTrace(const ShareableSQLTraceConfig::Notification& notification);
    void setNotificationForGlobalPerformanceTrace(
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

    static constexpr const char* notifierPreprocessorName
    = "com.Tencent.WCDB.Notifier.PreprocessTag";

    static constexpr const char* corruptionQueueName = "com.Tencent.WCDB.Queue.Corruption";
    static constexpr const char* checkpointQueueName = "com.Tencent.WCDB.Queue.Checkpoint";
    static constexpr const char* backupQueueName = "com.Tencent.WCDB.Queue.Backup";

    static constexpr const char* basicConfigName = "com.Tencent.WCDB.Config.Basic";
    static constexpr const char* backupConfigName = "com.Tencent.WCDB.Config.Backup";
    static constexpr const char* checkpointConfigName = "com.Tencent.WCDB.Config.Checkpoint";
    static constexpr const char* tokenizeConfigName = "com.Tencent.WCDB.Config.Tokenize";
    static constexpr const char* cipherConfigName = "com.Tencent.WCDB.Config.Cipher";
    static constexpr const char* sqlTraceConfigName = "com.Tencent.WCDB.Config.SQLTrace";
    static constexpr const char* performanceTraceConfigName
    = "com.Tencent.WCDB.Config.PerformanceTrace";
    static constexpr const char* globalSQLTraceConfigName
    = "com.Tencent.WCDB.Config.GlobalSQLTrace";
    static constexpr const char* globalPerformanceTraceConfigName
    = "com.Tencent.WCDB.Config.GlobalPerformanceTrace";

    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

protected:
    Core();

    static int vfsOpen(const char* path, int flags, int mode);
    static void handleLog(void* unused, int code, const char* message);
    void preprocessError(const Error& error, Error::Infos& infos);
    void onDatabaseCreated(Database* database) override final;

    // The order of member variables here is important.
    std::shared_ptr<DatabasePool> m_databasePool;
    std::shared_ptr<FTS::Modules> m_modules;

    std::shared_ptr<CorruptionQueue> m_corruptionQueue;
    std::shared_ptr<CheckpointQueue> m_checkpointQueue;
    std::shared_ptr<BackupQueue> m_backupQueue;

    std::shared_ptr<Config> m_backupConfig;
    std::shared_ptr<Config> m_checkpointConfig;
    std::shared_ptr<Config> m_globalSQLTraceConfig;
    std::shared_ptr<Config> m_globalPerformanceTraceConfig;
    std::shared_ptr<Configs> m_configs;
};

} // namespace WCDB

#endif /* _WCDB_CORE_H */
