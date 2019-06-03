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

#include <WCDB/AutoMigrateConfig.hpp>
#include <WCDB/BusyRetryConfig.hpp>
#include <WCDB/Core.h>
#include <WCDB/FileManager.hpp>
#include <WCDB/Global.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Core
Core& Core::shared()
{
    static Core* s_shared = new Core;
    return *s_shared;
}

Core::Core()
// Database
: m_databasePool(this)
, m_modules(std::make_shared<TokenizerModules>())
, m_operationQueue(std::make_shared<OperationQueue>(OperationQueueName, this))
// Backup
, m_autoBackupConfig(std::make_shared<AutoBackupConfig>(m_operationQueue))
// Migration
, m_autoMigrateConfig(std::make_shared<AutoMigrateConfig>(m_operationQueue))
// Checkpoint
, m_autoCheckpointConfig(std::make_shared<AutoCheckpointConfig>(m_operationQueue))
// Trace
, m_globalSQLTraceConfig(std::make_shared<ShareableSQLTraceConfig>())
, m_globalPerformanceTraceConfig(std::make_shared<ShareablePerformanceTraceConfig>())
// Config
, m_configs({
  { GlobalSQLTraceConfigName, m_globalSQLTraceConfig, Configs::Priority::Highest },
  { GlobalPerformanceTraceConfigName, m_globalPerformanceTraceConfig, Configs::Priority::Highest },
  { BusyRetryConfigName, std::make_shared<BusyRetryConfig>(), Configs::Priority::Highest },
  { BasicConfigName, std::make_shared<BasicConfig>(), Configs::Priority::Higher },
  })
{
    Global::shared().setNotificationForLog(
    NotifierLoggerName,
    std::bind(&Core::globalLog, this, std::placeholders::_1, std::placeholders::_2));

    Notifier::shared().setNotificationForPreprocessing(
    NotifierPreprocessorName,
    std::bind(&Core::preprocessError, this, std::placeholders::_1));
}

Core::~Core()
{
    Global::shared().setNotificationForLog(NotifierLoggerName, nullptr);
    Notifier::shared().setNotificationForPreprocessing(NotifierPreprocessorName, nullptr);
}

#pragma mark - Database
RecyclableDatabase Core::getOrCreateDatabase(const String& path)
{
    return m_databasePool.getOrCreate(path);
}

RecyclableDatabase Core::getAlivingDatabase(const String& path)
{
    return m_databasePool.get(path);
}

void Core::purgeDatabasePool()
{
    m_databasePool.purge();
}

void Core::databaseDidCreate(Database* database)
{
    WCTInnerAssert(database != nullptr);

    database->setConfigs(m_configs);

    enableAutoCheckpoint(database, true);
}

void Core::preprocessError(Error& error)
{
    const auto& strings = error.infos.getStrings();

    auto iter = strings.find(ErrorStringKeyPath);
    if (iter != strings.end()) {
        auto database = m_databasePool.get(iter->second);
        if (database != nullptr) {
            auto tag = database->getTag();
            if (tag.isValid()) {
                error.infos.set(ErrorIntKeyTag, tag);
            }
        }
    }
}

#pragma mark - Tokenizer
void Core::registerTokenizer(const String& name, const TokenizerModule& module)
{
    m_modules->add(name, module);
}

bool Core::tokenizerExists(const String& name) const
{
    return m_modules->get(name) != nullptr;
}

std::shared_ptr<Config> Core::tokenizerConfig(const String& tokenizeName)
{
    return std::make_shared<TokenizerConfig>(tokenizeName, m_modules);
}

#pragma mark - Operation
std::pair<bool, bool> Core::migrationShouldBeOperated(const String& path)
{
    RecyclableDatabase database = m_databasePool.get(path);
    bool succeed = true; // mark as no error if database is not referenced.
    bool done = false;
    if (database != nullptr) {
        std::tie(succeed, done) = database->stepMigrationIfAlreadyInitialized();
    }
    return { succeed, done };
}

bool Core::backupShouldBeOperated(const String& path)
{
    RecyclableDatabase database = m_databasePool.get(path);
    bool succeed = true; // mark as no error if database is not referenced.
    if (database != nullptr) {
        succeed = database->backupIfAlreadyInitialized();
    }
    return succeed;
}

bool Core::checkpointShouldBeOperated(const String& path, bool critical)
{
    RecyclableDatabase database = m_databasePool.get(path);
    bool succeed = true; // mark as no error if database is not referenced.
    if (database != nullptr) {
        succeed = database->checkpointIfAlreadyInitialized(
        critical ? Database::CheckpointMode::Truncate : Database::CheckpointMode::Passive);
    }
    return succeed;
}

void Core::integrityShouldBeChecked(const String& path)
{
    RecyclableDatabase database = m_databasePool.get(path);
    if (database != nullptr) {
        database->checkIntegrityIfAlreadyInitialized();

        std::set<String> sourcePaths = database->getPathsOfSourceDatabases();
        for (const String& sourcePath : sourcePaths) {
            RecyclableDatabase sourceDatabase = m_databasePool.get(sourcePath);
            if (sourceDatabase != nullptr) {
                sourceDatabase->checkIntegrityIfAlreadyInitialized();
            }
        }
    }
}

void Core::purgeShouldBeOperated()
{
    purgeDatabasePool();
}

bool Core::isFileObservedCorrupted(const String& path)
{
    return m_operationQueue->isFileObservedCorrupted(path);
}

void Core::setNotificationWhenDatabaseCorrupted(const String& path,
                                                const CorruptedNotification& notification)
{
    OperationQueue::CorruptionNotification underlyingNotification = nullptr;
    if (notification != nullptr) {
        underlyingNotification
        = [this, notification](const String& path, uint32_t corruptedIdentifier) {
              RecyclableDatabase database = m_databasePool.get(path);
              if (database == nullptr) {
                  return;
              }
              database->blockade();
              do {
                  bool succeed;
                  bool exists;
                  std::tie(succeed, exists) = FileManager::fileExists(path);
                  if (!succeed) {
                      // I/O error
                      break;
                  }
                  if (!exists) {
                      // it's already not existing
                      break;
                  }
                  uint32_t identifier;
                  std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
                  if (!succeed) {
                      // I/O error
                      break;
                  }
                  if (identifier != corruptedIdentifier) {
                      // file is changed.
                      break;
                  }
                  notification(database.get());
              } while (false);
              database->unblockade();
          };
    }
    m_operationQueue->setNotificationWhenCorrupted(path, underlyingNotification);
}

#pragma mark - Checkpoint
void Core::enableAutoCheckpoint(Database* database, bool enable)
{
    if (enable) {
        database->setConfig(
        AutoCheckpointConfigName, m_autoCheckpointConfig, Configs::Priority::Highest);
        m_operationQueue->registerAsRequiredCheckpoint(database->getPath());
    } else {
        database->removeConfig(AutoCheckpointConfigName);
        m_operationQueue->registerAsNoCheckpointRequired(database->getPath());
    }
}

#pragma mark - Backup
void Core::enableAutoBackup(Database* database, bool enable)
{
    WCTInnerAssert(database != nullptr);
    if (enable) {
        database->setConfig(
        AutoBackupConfigName, m_autoBackupConfig, WCDB::Configs::Priority::Highest);
        m_operationQueue->registerAsRequiredBackup(database->getPath());
    } else {
        database->removeConfig(AutoBackupConfigName);
        m_operationQueue->registerAsNoBackupRequired(database->getPath());
    }
}

#pragma mark - Migration
void Core::enableAutoMigration(Database* database, bool enable)
{
    WCTInnerAssert(database != nullptr);
    if (enable) {
        database->setConfig(
        AutoMigrateConfigName, m_autoMigrateConfig, WCDB::Configs::Priority::Highest);
        m_operationQueue->registerAsRequiredMigration(database->getPath());
        m_operationQueue->asyncMigrate(database->getPath());
    } else {
        database->removeConfig(AutoMigrateConfigName);
        m_operationQueue->registerAsNoMigrationRequired(database->getPath());
    }
}

#pragma mark - Trace
void Core::globalLog(int rc, const char* message)
{
    Error error;
    error.setSQLiteCode(rc, message);
    switch (error.code()) {
    case Error::Code::Warning:
        error.level = Error::Level::Warning;
        break;
    case Error::Code::Notice:
        error.level = Error::Level::Ignore;
        break;
    default:
        error.level = Error::Level::Debug;
        break;
    }
    Notifier::shared().notify(error);
}

void Core::setNotificationForSQLGLobalTraced(const ShareableSQLTraceConfig::Notification& notification)
{
    static_cast<ShareableSQLTraceConfig*>(m_globalSQLTraceConfig.get())->setNotification(notification);
}

void Core::setNotificationWhenPerformanceGlobalTraced(const ShareablePerformanceTraceConfig::Notification& notification)
{
    static_cast<ShareablePerformanceTraceConfig*>(m_globalPerformanceTraceConfig.get())
    ->setNotification(notification);
}

} // namespace WCDB
