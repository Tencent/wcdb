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

#include <WCDB/Core.h>
#include <WCDB/FileManager.hpp>
#include <fcntl.h>

namespace WCDB {

Core* Core::shared()
{
    static Core* s_shared = new Core;
    return s_shared;
}

Core::Core()
: m_corruptionQueue(CorruptionQueueName)
, m_checkpointQueue(CheckpointQueueName)
, m_backupQueue(BackupQueueName)
// Configs
, m_basicConfig(new BasicConfig(BasicConfigName))
, m_backupConfig(new BackupConfig(BackupConfigName, &m_backupQueue))
, m_checkpointConfig(new CheckpointConfig(CheckpointConfigName, &m_checkpointQueue))
, m_globalSQLTraceConfig(new ShareableSQLTraceConfig(GlobalSQLTraceConfigName))
, m_globalPerformanceTraceConfig(new ShareablePerformanceTraceConfig(GlobalPerformanceTraceConfigName))
, m_configs(new Configs(OrderedUniqueList<std::string, std::shared_ptr<Config>>({
  { Configs::Priority::Highest, m_globalSQLTraceConfig->name, m_globalSQLTraceConfig },
  { Configs::Priority::Highest, m_globalPerformanceTraceConfig->name, m_globalPerformanceTraceConfig },
  { Configs::Priority::Higher, m_basicConfig->name, m_basicConfig },
  { Configs::Priority::Low, m_checkpointConfig->name, m_checkpointConfig },
  })))
{
    GlobalConfig::enableMultithread();
    GlobalConfig::enableMemoryStatus(false);
    //        GlobalConfig::setMemoryMapSize(0x7fff0000, 0x7fff0000);
    GlobalConfig::setNotificationForLog(CoreNotifier::globalLogger);
    GlobalConfig::hookVFSOpen(Core::vfsOpen);

    m_corruptionQueue.run();
    m_checkpointQueue.run();
    m_backupQueue.run();

    m_notifier.setCorruptionQueue(&m_corruptionQueue);
}

int Core::vfsOpen(const char* path, int flags, int mode)
{
    int fd = open(path, flags, mode);
    if (fd != -1 && (flags & O_CREAT)) {
        FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
    }
    return fd;
}

HandlePools* Core::handlePools()
{
    return &shared()->m_handlePools;
}

const std::shared_ptr<Configs>& Core::configs()
{
    return shared()->m_configs;
}

FTS::Modules* Core::modules()
{
    return &shared()->m_modules;
}

CoreNotifier* Core::notifier()
{
    return &shared()->m_notifier;
}

CorruptionQueue* Core::corruptionQueue()
{
    return &shared()->m_corruptionQueue;
}

CheckpointQueue* Core::checkpointQueue()
{
    return &shared()->m_checkpointQueue;
}

BackupQueue* Core::backupQueue()
{
    return &shared()->m_backupQueue;
}

const std::shared_ptr<Config>& Core::backupConfig()
{
    return shared()->m_backupConfig;
}

ShareableSQLTraceConfig* Core::globalSQLTraceConfig()
{
    return static_cast<ShareableSQLTraceConfig*>(
    shared()->m_globalSQLTraceConfig.get());
}

ShareablePerformanceTraceConfig* Core::globalPerformanceTraceConfig()
{
    return static_cast<ShareablePerformanceTraceConfig*>(
    shared()->m_globalPerformanceTraceConfig.get());
}

std::shared_ptr<Config> Core::tokenizeConfig(const std::list<std::string>& tokenizeNames)
{
    return std::shared_ptr<Config>(
    new TokenizeConfig(TokenizeConfigName, tokenizeNames, shared()->modules()));
}

std::shared_ptr<Config> Core::cipherConfig(const UnsafeData& cipher, int pageSize)
{
    return std::shared_ptr<Config>(new CipherConfig(CipherConfigName, cipher, pageSize));
}

std::shared_ptr<Config> Core::sqlTraceConfig(const SQLTraceConfig::Notification& notification)
{
    return std::shared_ptr<Config>(new SQLTraceConfig(SQLTraceConfigName, notification));
}

std::shared_ptr<Config>
Core::performanceTraceConfig(const PerformanceTraceConfig::Notification& notification)
{
    return std::shared_ptr<Config>(
    new PerformanceTraceConfig(PerformanceTraceConfigName, notification));
}

} // namespace WCDB
