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

#include "Core.h"
#include "AutoMigrateConfig.hpp"
#include "BusyRetryConfig.hpp"
#include "CompressionConst.hpp"
#include "DecompressFunction.hpp"
#include "FTS5AuxiliaryFunctionTemplate.hpp"
#include "FTSConst.h"
#include "FileManager.hpp"
#include "Global.hpp"
#include "Notifier.hpp"
#include "OneOrBinaryTokenizer.hpp"
#include "PinyinTokenizer.hpp"
#include "SQLite.h"
#include "ScalarFunctionTemplate.hpp"
#include "StringView.hpp"
#include "SubstringMatchInfo.hpp"

namespace WCDB {

#pragma mark - Core
Core& Core::shared()
{
    static Core* s_core = new Core;
    return *s_core;
}

Core::Core()
// Database
: m_databasePool(this)
, m_scalarFunctionModules(std::make_shared<ScalarFunctionModules>())
, m_tokenizerModules(std::make_shared<TokenizerModules>())
, m_auxiliaryFunctionModules(std::make_shared<AuxiliaryFunctionModules>())
, m_operationQueue(std::make_shared<OperationQueue>(OperationQueueName, this))
// Checkpoint
, m_autoCheckpointConfig(std::make_shared<AutoCheckpointConfig>(m_operationQueue))
// Backup
, m_autoBackupConfig(std::make_shared<AutoBackupConfig>(m_operationQueue))
// Migration
, m_autoMigrateConfig(std::make_shared<AutoMigrateConfig>(m_operationQueue))
// Compression
, m_autoCompressConfig(std::make_shared<AutoCompressConfig>(m_operationQueue))
// Trace
, m_globalSQLTraceConfig(std::make_shared<ShareableSQLTraceConfig>())
, m_globalPerformanceTraceConfig(std::make_shared<ShareablePerformanceTraceConfig>())
// Busy Retry
, m_globalBusyRetryConfig(std::make_shared<BusyRetryConfig>())
, m_enableBusyTrace(false)
//Merge
, m_AutoMergeFTSConfig(std::make_shared<AutoMergeFTSIndexConfig>(m_operationQueue))
// Config
, m_configs({
  { StringView(GlobalSQLTraceConfigName), m_globalSQLTraceConfig, Configs::Priority::Highest },
  { StringView(GlobalPerformanceTraceConfigName), m_globalPerformanceTraceConfig, Configs::Priority::Highest },
  { StringView(BusyRetryConfigName), m_globalBusyRetryConfig, Configs::Priority::Highest },
  { StringView(BasicConfigName), std::make_shared<BasicConfig>(), Configs::Priority::Higher },
  })
{
    Global::initialize();

    Global::shared().setNotificationForLog(
    NotifierLoggerName,
    std::bind(&Core::globalLog, this, std::placeholders::_1, std::placeholders::_2));

    Notifier::shared().setNotificationForPreprocessing(
    NotifierPreprocessorName,
    std::bind(&Core::preprocessError, this, std::placeholders::_1));

    m_operationQueue->run();

    //config FTS
    registerTokenizer(BuiltinTokenizer::OneOrBinary,
                      FTS3TokenizerModuleTemplate<OneOrBinaryTokenizer>::specialize());
    registerTokenizer(BuiltinTokenizer::LegacyOneOrBinary,
                      FTS3TokenizerModuleTemplate<OneOrBinaryTokenizer>::specialize());
    registerTokenizer(
    BuiltinTokenizer::Verbatim,
    FTS5TokenizerModuleTemplate<OneOrBinaryTokenizer>::specializeWithContext(nullptr));
    registerTokenizer(
    BuiltinTokenizer::Pinyin,
    FTS5TokenizerModuleTemplate<PinyinTokenizer>::specializeWithContext(nullptr));

    registerAuxiliaryFunction(
    BuiltinAuxiliaryFunction::SubstringMatchInfo,
    FTS5AuxiliaryFunctionTemplate<SubstringMatchInfo>::specializeWithContext(nullptr));
    registerScalarFunction(DecompressFunctionName,
                           ScalarFunctionTemplate<DecompressFunction>::specialize(2));
}

Core::~Core()
{
    Global::shared().setNotificationForLog(NotifierLoggerName, nullptr);
    Notifier::shared().setNotificationForPreprocessing(NotifierPreprocessorName, nullptr);
}

#pragma mark - Database
RecyclableDatabase Core::getOrCreateDatabase(const UnsafeStringView& path)
{
    return m_databasePool.getOrCreate(path);
}

void Core::purgeDatabasePool()
{
    m_databasePool.purge();
}

void Core::releaseSQLiteMemory(int bytes)
{
    sqlite3_release_memory(bytes);
}

void Core::setSoftHeapLimit(int64_t limit)
{
    sqlite3_soft_heap_limit64(limit);
}

void Core::stopQueue()
{
    m_operationQueue->stop();
}

void Core::databaseDidCreate(InnerDatabase* database)
{
    WCTAssert(database != nullptr);

    database->setConfigs(m_configs);

    enableAutoCheckpoint(database, true);
}

#pragma mark - Error

void Core::setThreadedErrorPath(const UnsafeStringView& path)
{
    m_errorPath.getOrCreate() = path;
}

void Core::setThreadedErrorIgnorable(bool ignorable)
{
    m_errorIgnorable.getOrCreate() = ignorable;
}

void Core::preprocessError(Error& error)
{
    auto& infos = error.infos;

    const StringView& path = error.getPath();
    const StringView& associatePath = error.getAssociatePath();
    const StringView& threadedPath = m_errorPath.getOrCreate();

    if (path.empty()) {
        if (!threadedPath.empty()) {
            error.infos.insert_or_assign(ErrorStringKeyPath, threadedPath);
        } else if (!associatePath.empty()) {
            error.infos.insert_or_assign(ErrorStringKeyPath, associatePath);
            error.infos.erase(ErrorStringKeyAssociatePath);
        }
    }
    if (error.getPath().equal(associatePath)) {
        error.infos.erase(ErrorStringKeyAssociatePath);
    }

    if (error.level == Error::Level::Error && m_errorIgnorable.getOrCreate()) {
        error.level = Error::Level::Warning;
    }

    auto iter = infos.find(UnsafeStringView(ErrorStringKeyPath));
    if (iter != infos.end() && iter->second.getType() == Value::Type::Text) {
        auto tag = m_databasePool.getTag(iter->second.textValue());
        if (tag.isValid()) {
            error.infos.insert_or_assign(ErrorIntKeyTag, (long) tag);
        }
    }
}

#pragma mark - ScalarFunction
void Core::registerScalarFunction(const UnsafeStringView& name,
                                  const ScalarFunctionModule& module)
{
    m_scalarFunctionModules->add(name, module);
}

bool Core::scalarFunctionExists(const UnsafeStringView& name) const
{
    return m_scalarFunctionModules->get(name) != nullptr;
}

std::shared_ptr<Config> Core::scalarFunctionConfig(const UnsafeStringView& scalarFunctionName)
{
    return std::make_shared<ScalarFunctionConfig>(scalarFunctionName, m_scalarFunctionModules);
}

#pragma mark - Tokenizer
void Core::registerTokenizer(const UnsafeStringView& name, const TokenizerModule& module)
{
    m_tokenizerModules->add(name, module);
}

bool Core::tokenizerExists(const UnsafeStringView& name) const
{
    return m_tokenizerModules->get(name) != nullptr;
}

std::shared_ptr<Config> Core::tokenizerConfig(const UnsafeStringView& tokenizeName)
{
    return std::make_shared<TokenizerConfig>(tokenizeName, m_tokenizerModules);
}

#pragma mark - AuxiliaryFunction
void Core::registerAuxiliaryFunction(const UnsafeStringView& name,
                                     const FTS5AuxiliaryFunctionModule& module)
{
    m_auxiliaryFunctionModules->add(name, module);
}

bool Core::auxiliaryFunctionExists(const UnsafeStringView& name) const
{
    return m_auxiliaryFunctionModules->get(name) != nullptr;
}

std::shared_ptr<Config>
Core::auxiliaryFunctionConfig(const UnsafeStringView& auxiliaryFunctionName)
{
    return std::make_shared<AuxiliaryFunctionConfig>(
    auxiliaryFunctionName, m_auxiliaryFunctionModules);
}

#pragma mark - Operation
Optional<bool> Core::migrationShouldBeOperated(const UnsafeStringView& path)
{
    RecyclableDatabase database = m_databasePool.getOrCreate(path);
    Optional<bool> done = false; // mark as no error if database is not referenced.
    if (database != nullptr) {
        done = database->stepMigration(true);
    }
    return done;
}

Optional<bool> Core::compressionShouldBeOperated(const UnsafeStringView& path)
{
    RecyclableDatabase database = m_databasePool.getOrCreate(path);
    Optional<bool> done = false; // mark as no error if database is not referenced.
    if (database != nullptr) {
        done = database->stepCompression(true);
    }
    return done;
}

void Core::backupShouldBeOperated(const UnsafeStringView& path)
{
    RecyclableDatabase database = m_databasePool.getOrCreate(path);
    if (database != nullptr) {
        database->backup(true);
    }
}

void Core::checkpointShouldBeOperated(const UnsafeStringView& path)
{
    RecyclableDatabase database = m_databasePool.getOrCreate(path);
    if (database != nullptr) {
        database->checkpoint(true);
    }
}

void Core::integrityShouldBeChecked(const UnsafeStringView& path)
{
    RecyclableDatabase database = m_databasePool.getOrCreate(path);
    if (database != nullptr) {
        database->checkIntegrity(true);

        StringViewSet sourcePaths = database->getPathsOfSourceDatabases();
        for (const UnsafeStringView& sourcePath : sourcePaths) {
            RecyclableDatabase sourceDatabase = m_databasePool.getOrCreate(sourcePath);
            if (sourceDatabase != nullptr) {
                sourceDatabase->checkIntegrity(true);
            }
        }
    }
}

void Core::purgeShouldBeOperated()
{
    purgeDatabasePool();
}

void Core::stopAllDatabaseEvent(const UnsafeStringView& path)
{
    m_operationQueue->stopAllDatabaseEvent(path);
}

bool Core::isFileObservedCorrupted(const UnsafeStringView& path)
{
    return m_operationQueue->isFileObservedCorrupted(path);
}

void Core::setNotificationWhenDatabaseCorrupted(const UnsafeStringView& path,
                                                const CorruptedNotification& notification)
{
    OperationQueue::CorruptionNotification underlyingNotification = nullptr;
    if (notification != nullptr) {
        underlyingNotification
        = [this, notification](const UnsafeStringView& path, uint32_t corruptedIdentifier) {
              RecyclableDatabase database = m_databasePool.getOrCreate(path);
              if (database == nullptr) {
                  return;
              }
              database->blockade();
              do {
                  auto exists = FileManager::fileExists(path);
                  if (!exists.succeed()) {
                      // I/O error
                      break;
                  }
                  if (!exists.value()) {
                      // it's already not existing
                      break;
                  }
                  auto identifier = FileManager::getFileIdentifier(path);
                  if (!identifier.succeed()) {
                      // I/O error
                      break;
                  }
                  if (identifier.value() != corruptedIdentifier) {
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
void Core::enableAutoCheckpoint(InnerDatabase* database, bool enable)
{
    database->setAutoCheckpointEnable(enable);
    if (enable) {
        database->setConfig(
        AutoCheckpointConfigName, m_autoCheckpointConfig, Configs::Priority::Highest);
        m_operationQueue->registerAsRequiredCheckpoint(database->getPath());
    } else {
        database->removeConfig(AutoCheckpointConfigName);
        m_operationQueue->registerAsNoCheckpointRequired(database->getPath());
    }
}

void Core::setCheckPointMinFrames(int frames)
{
    AutoCheckpointConfig* checkpointConfig
    = dynamic_cast<AutoCheckpointConfig*>(m_autoCheckpointConfig.get());
    WCTAssert(checkpointConfig != nullptr);
    if (checkpointConfig != nullptr) {
        checkpointConfig->setMinFrames(frames);
    }
}

#pragma mark - Backup
void Core::enableAutoBackup(InnerDatabase* database, bool enable)
{
    WCTAssert(database != nullptr);
    if (enable) {
        database->setConfig(
        AutoBackupConfigName, m_autoBackupConfig, WCDB::Configs::Priority::Highest);
        m_operationQueue->registerAsRequiredBackup(database->getPath());
        database->markNeedLoadIncremetalMaterial();
    } else {
        database->removeConfig(AutoBackupConfigName);
        m_operationQueue->registerAsNoBackupRequired(database->getPath());
    }
}

void Core::tryRegisterIncrementalMaterial(const UnsafeStringView& path,
                                          SharedIncrementalMaterial material)
{
    if (!m_operationQueue->isAutoBackup(path)) {
        return;
    }
    WCTAssert(dynamic_cast<AutoBackupConfig*>(m_autoBackupConfig.get()) != nullptr);
    AutoBackupConfig* backupConfig
    = static_cast<AutoBackupConfig*>(m_autoBackupConfig.get());
    if (backupConfig != nullptr) {
        backupConfig->tryRegisterIncrementalMaterial(path, material);
    }
}

SharedIncrementalMaterial Core::tryGetIncrementalMaterial(const UnsafeStringView& path)
{
    WCTAssert(dynamic_cast<AutoBackupConfig*>(m_autoBackupConfig.get()) != nullptr);
    AutoBackupConfig* backupConfig
    = static_cast<AutoBackupConfig*>(m_autoBackupConfig.get());
    if (backupConfig != nullptr) {
        return backupConfig->tryGetIncrementalMaterial(path);
    }
    return nullptr;
}

#pragma mark - Migration
void Core::enableAutoMigrate(InnerDatabase* database, bool enable)
{
    WCTAssert(database != nullptr);
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

#pragma mark - Compression
void Core::enableAutoCompress(InnerDatabase* database, bool enable)
{
    WCTAssert(database != nullptr);
    if (enable) {
        database->setConfig(
        AutoCompressConfigName, m_autoCompressConfig, WCDB::Configs::Priority::Highest);
        m_operationQueue->registerAsRequiredCompression(database->getPath());
        m_operationQueue->asyncCompress(database->getPath());
    } else {
        database->removeConfig(AutoCompressConfigName);
        m_operationQueue->registerAsNoCompressionRequired(database->getPath());
    }
}

#pragma mark - Merge FTS Index
void Core::enableAutoMergeFTSIndex(InnerDatabase* database, bool enable)
{
    WCTAssert(database != nullptr);
    if (enable) {
        database->setConfig(AutoMergeFTSIndexConfigName,
                            m_AutoMergeFTSConfig,
                            WCDB::Configs::Priority::Highest);
        m_operationQueue->registerAsRequiredMergeFTSIndex(database->getPath());
        m_operationQueue->asyncMergeFTSIndex(database->getPath(), nullptr, nullptr);
    } else {
        database->removeConfig(AutoMergeFTSIndexConfigName);
        m_operationQueue->registerAsNoMergeFTSIndexRequired(database->getPath());
    }
}

Optional<bool> Core::mergeFTSIndexShouldBeOperated(const UnsafeStringView& path,
                                                   TableArray newTables,
                                                   TableArray modifiedTables)
{
    RecyclableDatabase database = m_databasePool.getOrCreate(path);
    Optional<bool> done = false; // mark as no error if database is not referenced.
    if (database != nullptr) {
        done = database->mergeFTSIndex(newTables, modifiedTables);
    }
    return done;
}

#pragma mark - Trace
void Core::globalLog(int rc, const char* message)
{
    Error::Code code = Error::rc2c(rc);
    if (code == Error::Code::Warning || code == Error::Code::Corrupt) {
        Error error;
        error.setSQLiteCode(rc, message);
        error.level = Error::Level::Warning;
        Notifier::shared().notify(error);
    }
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

void Core::setNotificationWhenErrorTraced(const Notifier::Callback& notification)
{
    if (notification != nullptr) {
        Notifier::shared().setNotification(
        std::numeric_limits<int>::min(), WCDB::NotifierLoggerName, notification);
    } else {
        Notifier::shared().unsetNotification(WCDB::NotifierLoggerName);
    }
}

void Core::setNotificationWhenErrorTraced(const UnsafeStringView& path,
                                          const Notifier::Callback& notification)
{
    StringView notifierKey
    = StringView::formatted("%s_%s", NotifierLoggerName.data(), path.data());
    if (notification != nullptr) {
        StringView catchedPath = StringView(path);
        Notifier::Callback realNotification = [=](const Error& error) {
            if (error.getPath().length() == 0 || error.getPath().compare(catchedPath) == 0) {
                notification(error);
            }
        };
        Notifier::shared().setNotification(
        std::numeric_limits<int>::min() + 1, notifierKey, realNotification);
    } else {
        Notifier::shared().unsetNotification(notifierKey);
    }
}

void Core::setBusyMonitor(BusyMonitor monitor, double timeOut)
{
    if (monitor != nullptr && timeOut > 0) {
        m_enableBusyTrace = true;
        static_cast<BusyRetryConfig*>(m_globalBusyRetryConfig.get())
        ->setBusyMonitor(
        [=](const UnsafeStringView& path, uint64_t tid) {
            if (m_errorIgnorable.getOrCreate()) {
                return;
            }
            auto database = getOrCreateDatabase(path);
            StringView sql = database->getRunningSQLInThread(tid);
            monitor(database->getTag(), path, tid, sql);
        },
        timeOut);
    } else {
        m_enableBusyTrace = false;
        static_cast<BusyRetryConfig*>(m_globalBusyRetryConfig.get())->setBusyMonitor(nullptr, 0);
    }
}

bool Core::isBusyTraceEnable() const
{
    return m_enableBusyTrace;
}

#pragma mark - Integrity

void Core::skipIntegrityCheck(const UnsafeStringView& path)
{
    m_operationQueue->skipIntegrityCheck(path);
}

#pragma mark - Config
void Core::setABTestConfig(const UnsafeStringView& configName, const UnsafeStringView& configValue)
{
    LockGuard memoryGuard(m_memory);
    m_abtestConfig[configName] = configValue;
}

void Core::removeABTestConfig(const UnsafeStringView& configName)
{
    LockGuard memoryGuard(m_memory);
    m_abtestConfig.erase(configName);
}

Optional<UnsafeStringView> Core::getABTestConfig(const UnsafeStringView& configName)
{
    SharedLockGuard memoryGuard(m_memory);
    if (m_abtestConfig.find(configName) != m_abtestConfig.end()) {
        return m_abtestConfig[configName];
    }
    return NullOpt;
}

void Core::setDefaultCipherConfiguration(int version)
{
    switch (version) {
    case 1:
        sqlcipher_set_default_hmac_algorithm(0);
        sqlcipher_set_default_kdf_algorithm(0);
        sqlcipher_set_default_kdf_iter(4000);
        sqlcipher_set_default_use_hmac(0);
        break;

    case 2:
        sqlcipher_set_default_hmac_algorithm(0);
        sqlcipher_set_default_kdf_algorithm(0);
        sqlcipher_set_default_kdf_iter(4000);
        sqlcipher_set_default_use_hmac(1);
        break;

    case 3:
        sqlcipher_set_default_hmac_algorithm(0);
        sqlcipher_set_default_kdf_algorithm(0);
        sqlcipher_set_default_kdf_iter(64000);
        sqlcipher_set_default_use_hmac(1);
        break;

    default:
        sqlcipher_set_default_hmac_algorithm(2);
        sqlcipher_set_default_kdf_algorithm(2);
        sqlcipher_set_default_kdf_iter(256000);
        sqlcipher_set_default_use_hmac(1);
        break;
    }
}

bool Core::setDefaultTemporaryDirectory(const UnsafeStringView& dir)
{
    if (dir.length() > 0) {
        if (!FileManager::createDirectoryWithIntermediateDirectories(dir)) {
            return false;
        }
    }
    sqlite3_temp_directory = (char*) StringView::createConstant(dir.data()).data();
    return true;
}

} // namespace WCDB
