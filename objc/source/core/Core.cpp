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
#include <WCDB/Notifier.hpp>
#include <WCDB/String.hpp>
#include <fcntl.h>

namespace WCDB {

Core* Core::shared()
{
    static Core* s_shared = new Core;
    return s_shared;
}

Core::Core()
: m_modules(new FTS::Modules)
, m_corruptionQueue(new CorruptionQueue(corruptionQueueName))
, m_checkpointQueue(new CheckpointQueue(checkpointQueueName))
, m_backupQueue(new BackupQueue(backupQueueName))
// Configs
, m_backupConfig(new BackupConfig(m_backupQueue))
, m_globalSQLTraceConfig(new ShareableSQLTraceConfig)
, m_globalPerformanceTraceConfig(new ShareablePerformanceTraceConfig)
, m_configs(new Configs(OrderedUniqueList<String, std::shared_ptr<Config>>({
  { Configs::Priority::Highest, globalSQLTraceConfigName, m_globalSQLTraceConfig },
  { Configs::Priority::Highest, globalPerformanceTraceConfigName, m_globalPerformanceTraceConfig },
  { Configs::Priority::Higher, basicConfigName, std::shared_ptr<Config>(new BasicConfig) },
  { Configs::Priority::Low, checkpointConfigName, std::shared_ptr<Config>(new CheckpointConfig(m_checkpointQueue)) },
  })))
{
    m_databasePool.setEvent(this);
    m_corruptionQueue->setEvent(this);
    m_checkpointQueue->setEvent(this);
    m_backupQueue->setEvent(this);

    Handle::enableMultithread();
    Handle::enableMemoryStatus(false);
    //        Handle::setMemoryMapSize(0x7fff0000, 0x7fff0000);
    Handle::setNotificationForLog(Core::handleLog);
    Handle::setNotificationWhenVFSOpened(Core::vfsOpen);

    Notifier::shared()->setNotificationForPreprocessing(
    notifierPreprocessorName,
    std::bind(&Core::preprocessError, this, std::placeholders::_1, std::placeholders::_2));
}

Core::~Core()
{
    m_corruptionQueue->setEvent(nullptr);
    m_checkpointQueue->setEvent(nullptr);
    m_databasePool.setEvent(nullptr);
    m_backupQueue->setEvent(nullptr);
    Notifier::shared()->setNotificationForPreprocessing(notifierPreprocessorName, nullptr);
}

RecyclableDatabase Core::getOrCreateDatabase(const String& path)
{
    return m_databasePool.getOrCreate(path);
}

RecyclableDatabase Core::getExistingDatabase(const String& path)
{
    return m_databasePool.get(path);
}

RecyclableDatabase Core::getExistingDatabase(const Tag& tag)
{
    return m_databasePool.get(tag);
}

void Core::purge()
{
    m_databasePool.purge();
}

void Core::onDatabaseCreated(Database* database)
{
    WCTInnerAssert(database != nullptr);
    database->setConfigs(m_configs);
}

void Core::databaseDidBecomeCorrupted(const String& path, uint32_t corruptedIdentifier)
{
    RecyclableDatabase database = m_databasePool.get(path);
    if (database == nullptr || !database->containsRecoverScheme()) {
        return;
    }
    database->recover(corruptedIdentifier);
}

bool Core::databaseShouldCheckpoint(const String& path, const StatementPragma& checkpointStatement)
{
    RecyclableDatabase database = m_databasePool.get(path);
    if (database == nullptr) {
        return true;
    }
    return database->execute(checkpointStatement);
}

bool Core::databaseShouldBackup(const String& path)
{
    RecyclableDatabase database = m_databasePool.get(path);
    if (database == nullptr) {
        return true;
    }
    return database->backup();
}

const std::shared_ptr<Configs>& Core::configs()
{
    return m_configs;
}

void Core::addTokenizer(const String& name, unsigned char* address)
{
    m_modules->addAddress(name, address);
}

void Core::setNotificationForGlobalSQLTrace(const ShareableSQLTraceConfig::Notification& notification)
{
    static_cast<ShareableSQLTraceConfig*>(m_globalSQLTraceConfig.get())->setNotification(notification);
}

void Core::setNotificationForGlobalPerformanceTrace(const ShareablePerformanceTraceConfig::Notification& notification)
{
    static_cast<ShareablePerformanceTraceConfig*>(m_globalPerformanceTraceConfig.get())
    ->setNotification(notification);
}

const std::shared_ptr<Config>& Core::backupConfig()
{
    return m_backupConfig;
}

std::shared_ptr<Config> Core::tokenizeConfig(const std::list<String>& tokenizeNames)
{
    return std::shared_ptr<Config>(new TokenizeConfig(tokenizeNames, m_modules));
}

std::shared_ptr<Config> Core::cipherConfig(const UnsafeData& cipher, int pageSize)
{
    return std::shared_ptr<Config>(new CipherConfig(cipher, pageSize));
}

std::shared_ptr<Config> Core::sqlTraceConfig(const SQLTraceConfig::Notification& notification)
{
    return std::shared_ptr<Config>(new SQLTraceConfig(notification));
}

std::shared_ptr<Config>
Core::performanceTraceConfig(const PerformanceTraceConfig::Notification& notification)
{
    return std::shared_ptr<Config>(new PerformanceTraceConfig(notification));
}

std::shared_ptr<Config> Core::customConfig(const CustomConfig::Invocation& invocation,
                                           const CustomConfig::Invocation& uninvocation)
{
    return std::shared_ptr<Config>(new CustomConfig(invocation, uninvocation));
}

int Core::vfsOpen(const char* path, int flags, int mode)
{
    int fd = open(path, flags, mode);
    if (fd != -1 && (flags & O_CREAT)) {
        FileManager::setFileProtectionCompleteUntilFirstUserAuthenticationIfNeeded(path);
    }
    return fd;
}

void Core::handleLog(void* unused, int code, const char* message)
{
    Error error;
    switch (code) {
    case SQLITE_WARNING:
        error.level = Error::Level::Warning;
        break;
    case SQLITE_NOTICE:
        error.level = Error::Level::Ignore;
        break;
    default:
        error.level = Error::Level::Debug;
        break;
    }
    error.setSQLiteCode(code);
    error.message = message ? message : String::null();
    Notifier::shared()->notify(error);
}

void Core::preprocessError(const Error& error, Error::Infos& infos)
{
    const auto& strings = error.infos.getStrings();
    auto iter = strings.find("Path");
    if (iter == strings.end()) {
        return;
    }
    auto database = m_databasePool.get(iter->second);
    if (database == nullptr) {
        return;
    }
    auto tag = database->getTag();
    if (tag != Tag::invalid()) {
        infos.set("Tag", tag);
    }
}

} // namespace WCDB
