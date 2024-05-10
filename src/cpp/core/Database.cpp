//
// Created by qiuwenchen on 2022/8/3.
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

#include "Database.hpp"
#include "CipherConfig.hpp"
#include "CompressionCenter.hpp"
#include "CompressionConst.hpp"
#include "Core.hpp"
#include "CoreConst.h"
#include "CustomConfig.hpp"
#include "DBOperationNotifier.hpp"
#include "FileManager.hpp"
#include "InnerDatabase.hpp"
#include "WCDBVersion.h"
#include <errno.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <conio.h>
#include <direct.h>
#endif

namespace WCDB {

#pragma mark - Basic

Database::Database(const UnsafeStringView& path)
{
    if (path.compare(":memory:") == 0) {
        m_databaseHolder = Core::shared().getOrCreateDatabase(path);
        m_innerDatabase = m_databaseHolder.get();
        return;
    }
#ifndef _WIN32
    const char* resolvePath = realpath(path.data(), nullptr);
    if (resolvePath == nullptr && errno == ENOENT) {
        Core::shared().setThreadedErrorPath(path);
        if (FileManager::createDirectoryWithIntermediateDirectories(Path::getDirectory(path))
            && FileManager::createFile(path)) {
            resolvePath = realpath(path.data(), nullptr);
            if (resolvePath == NULL) {
                Error error;
                error.level = Error::Level::Warning;
                error.setSystemCode(errno, Error::Code::IOError);
                error.infos.insert_or_assign(ErrorStringKeyPath, path);
                Notifier::shared().notify(error);
            }
            FileManager::removeItem(path);
        }
        Core::shared().setThreadedErrorPath(nullptr);
    }
    if (resolvePath != nullptr) {
        UnsafeStringView newPath = UnsafeStringView(resolvePath);
#ifdef __APPLE__
        /*
         /var is the symlink to /private/var.
         In most cases, realpath will return the path with the /var prefix,
         while in a small number of cases it will return the path with the /private/var prefix.
         In order to avoid the inconsistency of the path of the same file, remove the /private prefix of path here
         */
        if (newPath.hasPrefix("/private")) {
            newPath = UnsafeStringView(resolvePath + 8);
        }
#endif
        m_databaseHolder = Core::shared().getOrCreateDatabase(newPath);
        free((void*) resolvePath);
    }
#else
    wchar_t resolvePath[_MAX_PATH];
    if (::_wfullpath(resolvePath, GetPathString(path), _MAX_PATH) != NULL) {
        StringView newPath = StringView::createFromWString(resolvePath);
        m_databaseHolder = Core::shared().getOrCreateDatabase(newPath);
    }
#endif
    else {
        m_databaseHolder = Core::shared().getOrCreateDatabase(path);
    }
    m_innerDatabase = m_databaseHolder.get();
}

Database::Database(const Database&) = default;

Database& Database::operator=(const Database&) = default;

Database::~Database() = default;

Database::Database(Recyclable<InnerDatabase*> database)
: m_databaseHolder(database), m_innerDatabase(database.get())
{
}

Database::Database(InnerDatabase* database) : m_innerDatabase(database)
{
    m_databaseHolder = RecyclableDatabase(m_innerDatabase, nullptr);
}

RecyclableHandle Database::getHandleHolder(bool writeHint)
{
    return m_databaseHolder->getHandle(writeHint);
}

Recyclable<InnerDatabase*> Database::getDatabaseHolder()
{
    return m_databaseHolder;
}

void Database::setTag(const long& tag)
{
    m_innerDatabase->setTag(tag);
}

long Database::getTag() const
{
    return m_innerDatabase->getTag();
}

const StringView& Database::getPath() const
{
    return m_innerDatabase->getPath();
}

const Error& Database::getError() const
{
    return m_innerDatabase->getThreadedError();
}

Handle Database::getHandle()
{
    return Handle(m_databaseHolder);
}

bool Database::canOpen() const
{
    return m_innerDatabase->canOpen();
}

bool Database::isOpened() const
{
    return m_innerDatabase->isOpened();
}

void Database::close(const Database::ClosedCallback& onClosed)
{
    m_innerDatabase->close(onClosed);
}

void Database::blockade()
{
    m_innerDatabase->blockade();
}

bool Database::isBlockaded() const
{
    return m_innerDatabase->isBlockaded();
}

void Database::unblockade()
{
    m_innerDatabase->unblockade();
}

bool Database::truncateCheckpoint()
{
    return m_innerDatabase->checkpoint(false, InnerDatabase::CheckPointMode::Truncate);
}

bool Database::passiveCheckpoint()
{
    return m_innerDatabase->checkpoint(false, InnerDatabase::CheckPointMode::Passive);
}

bool Database::vacuum(ProgressUpdateCallback onProgressUpdated)
{
    return m_innerDatabase->vacuum(onProgressUpdated);
}

#if defined(_WIN32)
void Database::setUIThreadId(std::thread::id uiThreadId)
{
    Thread::setUIThreadId(uiThreadId);
}
#endif

#pragma mark - Monitor

void Database::globalTraceError(Database::ErrorNotification trace)
{
    Core::shared().setNotificationWhenErrorTraced(trace);
}

void Database::traceError(ErrorNotification trace)
{
    Core::shared().setNotificationWhenErrorTraced(getPath(), trace);
}

static_assert(sizeof(Database::PerformanceInfo) == sizeof(InnerHandle::PerformanceInfo), "");
static_assert(offsetof(Database::PerformanceInfo, tablePageReadCount)
              == offsetof(InnerHandle::PerformanceInfo, tablePageReadCount),
              "");
static_assert(offsetof(Database::PerformanceInfo, tablePageWriteCount)
              == offsetof(InnerHandle::PerformanceInfo, tablePageWriteCount),
              "");
static_assert(offsetof(Database::PerformanceInfo, indexPageReadCount)
              == offsetof(InnerHandle::PerformanceInfo, indexPageReadCount),
              "");
static_assert(offsetof(Database::PerformanceInfo, indexPageWriteCount)
              == offsetof(InnerHandle::PerformanceInfo, indexPageWriteCount),
              "");
static_assert(offsetof(Database::PerformanceInfo, overflowPageReadCount)
              == offsetof(InnerHandle::PerformanceInfo, overflowPageReadCount),
              "");
static_assert(offsetof(Database::PerformanceInfo, overflowPageWriteCount)
              == offsetof(InnerHandle::PerformanceInfo, overflowPageWriteCount),
              "");
static_assert(offsetof(Database::PerformanceInfo, costInNanoseconds)
              == offsetof(InnerHandle::PerformanceInfo, costInNanoseconds),
              "");

void Database::globalTracePerformance(Database::PerformanceNotification trace)
{
    if (trace != nullptr) {
        Core::shared().setNotificationWhenPerformanceGlobalTraced(
        [trace](long tag,
                const UnsafeStringView& path,
                const void* handleIdentifier,
                const UnsafeStringView& sql,
                const InnerHandle::PerformanceInfo& info) {
            PerformanceInfo newInfo;
            memcpy(&newInfo, &info, sizeof(info));
            trace(tag, path, (uint64_t) handleIdentifier, sql, newInfo);
        });
    } else {
        Core::shared().setNotificationWhenPerformanceGlobalTraced(nullptr);
    }
}

void Database::tracePerformance(Database::PerformanceNotification trace)
{
    if (trace != nullptr) {
        m_innerDatabase->setConfig(
        PerformanceTraceConfigName,
        std::static_pointer_cast<Config>(std::make_shared<PerformanceTraceConfig>(
        [trace](long tag,
                const UnsafeStringView& path,
                const void* handleIdentifier,
                const UnsafeStringView& sql,
                const InnerHandle::PerformanceInfo& info) {
            PerformanceInfo newInfo;
            memcpy(&newInfo, &info, sizeof(info));
            trace(tag, path, (uint64_t) handleIdentifier, sql, newInfo);
        })),
        Configs::Priority::Highest);
    } else {
        m_innerDatabase->removeConfig(PerformanceTraceConfigName);
    }
}

void Database::globalTraceSQL(Database::SQLNotification trace)
{
    Core::shared().setNotificationForSQLGLobalTraced(trace);
}

void Database::traceSQL(Database::SQLNotification trace)
{
    if (trace != nullptr) {
        m_innerDatabase->setConfig(
        SQLTraceConfigName,
        std::static_pointer_cast<Config>(std::make_shared<SQLTraceConfig>(trace)),
        Configs::Priority::Highest);
    } else {
        m_innerDatabase->removeConfig(SQLTraceConfigName);
    }
}

void Database::setFullSQLTraceEnable(bool enable)
{
    m_innerDatabase->setFullSQLTraceEnable(enable);
}

const StringView& Database::MonitorInfoKeyHandleCount = WCDB::MonitorInfoKeyHandleCount;
const StringView& Database::MonitorInfoKeySchemaUsage = WCDB::MonitorInfoKeySchemaUsage;
const StringView& Database::MonitorInfoKeyHandleOpenTime = WCDB::MonitorInfoKeyHandleOpenTime;
const StringView& Database::MonitorInfoKeyHandleOpenCPUTime
= WCDB::MonitorInfoKeyHandleOpenCPUTime;
const StringView& Database::MonitorInfoKeyTableCount = WCDB::MonitorInfoKeyTableCount;
const StringView& Database::MonitorInfoKeyIndexCount = WCDB::MonitorInfoKeyIndexCount;
const StringView& Database::MonitorInfoKeyTriggerCount = WCDB::MonitorInfoKeyTriggerCount;

void Database::globalTraceDatabaseOperation(DBOperationTrace trace)
{
    if (trace != nullptr) {
        DBOperationNotifier::shared().setNotification([=](InnerDatabase* innerDatabase,
                                                          DBOperationNotifier::Operation operation,
                                                          StringViewMap<Value>& info) {
            Database database = Database(innerDatabase);
            trace(database, (Operation) operation, info);
        });
    } else {
        DBOperationNotifier::shared().setNotification(nullptr);
    }
}

void Database::globalTraceBusy(BusyTrace trace, double timeOut)
{
    Core::shared().setBusyMonitor(trace, timeOut);
}

#pragma mark - File

bool Database::removeFiles()
{
    return m_innerDatabase->removeFiles();
}

bool Database::moveFiles(const UnsafeStringView& directory)
{
    return m_innerDatabase->moveFiles(directory);
}

std::list<StringView> Database::getPaths() const
{
    return m_innerDatabase->getPaths();
}

Optional<size_t> Database::getFilesSize() const
{
    return m_innerDatabase->getFilesSize();
}

#pragma mark - FTS

void Database::enableAutoMergeFTS5Index(bool flag)
{
    Core::shared().enableAutoMergeFTSIndex(m_innerDatabase, flag);
}

void Database::addTokenizer(const UnsafeStringView& tokenize)
{
    StringView configName
    = StringView::formatted("%s%s", TokenizeConfigPrefix.data(), tokenize.data());
    m_innerDatabase->setConfig(
    configName, Core::shared().tokenizerConfig(tokenize), Configs::Priority::Higher);
}

void Database::registerTokenizer(const UnsafeStringView& name, const TokenizerModule& module)
{
    Core::shared().registerTokenizer(name, module);
}

void Database::addAuxiliaryFunction(const UnsafeStringView& functionName)
{
    StringView configName = StringView::formatted(
    "%s%s", AuxiliaryFunctionConfigPrefix.data(), functionName.data());
    m_innerDatabase->setConfig(configName,
                               Core::shared().auxiliaryFunctionConfig(functionName),
                               Configs::Priority::Higher);
}

void Database::registerAuxiliaryFunction(const UnsafeStringView& name,
                                         const FTS5AuxiliaryFunctionModule& module)
{
    Core::shared().registerAuxiliaryFunction(name, module);
}

void Database::configSymbolDetector(SymbolDetector detector)
{
    FTSTokenizerUtil::configSymbolDetector(detector);
}

void Database::configUnicodeNormalizer(UnicodeNormalizer normalizer)
{
    FTSTokenizerUtil::configUnicodeNormalizer(normalizer);
}

void Database::configPinyinConverter(PinYinConverter converter)
{
    FTSTokenizerUtil::configPinyinConverter(converter);
}

void Database::configTraditionalChineseConverter(TraditionalChineseConverter converter)
{
    FTSTokenizerUtil::configTraditionalChineseConverter(converter);
}

#pragma mark - Memory

void Database::purge()
{
    m_innerDatabase->purge();
}

void Database::purgeAll()
{
    Core::shared().purgeDatabasePool();
}

#pragma mark - Repair

void Database::setNotificationWhenCorrupted(Database::CorruptionNotification onCorrupted)
{
    Core::shared().setNotificationWhenDatabaseCorrupted(
    getPath(), [onCorrupted](InnerDatabase* innerDatabase) {
        Database database = Database(innerDatabase);
        onCorrupted(database);
    });
}

bool Database::checkIfCorrupted()
{
    m_innerDatabase->checkIntegrity(false);
    return isAlreadyCorrupted();
}

bool Database::isAlreadyCorrupted()
{
    return Core::shared().isFileObservedCorrupted(getPath());
}

void Database::enableAutoBackup(bool flag)
{
    Core::shared().enableAutoBackup(m_innerDatabase, flag);
}

bool Database::backup()
{
    return m_innerDatabase->backup(false);
}

void Database::filterBackup(Database::BackupFilter tableShouldBeBackedUp)
{
    m_innerDatabase->filterBackup(tableShouldBeBackedUp);
}

bool Database::deposit()
{
    return m_innerDatabase->deposit();
}

bool Database::removeDeposited()
{
    return m_innerDatabase->removeDeposited();
}

bool Database::containsDeposited() const
{
    return m_innerDatabase->containsDeposited();
}

double Database::retrieve(Database::ProgressUpdateCallback onProgressUpdated)
{
    return m_innerDatabase->retrieve(onProgressUpdated);
}

#pragma mark - Config

void Database::setCipherKey(const UnsafeData& cipherKey, int cipherPageSize, CipherVersion cipherVersion)
{
    if (cipherKey.size() > 0) {
        m_innerDatabase->setConfig(
        CipherConfigName,
        std::static_pointer_cast<Config>(std::make_shared<CipherConfig>(
        cipherKey, cipherPageSize, cipherVersion)),
        Configs::Priority::Highest);
    } else {
        m_innerDatabase->removeConfig(CipherConfigName);
    }
}

void Database::setDefaultCipherConfiguration(CipherVersion version)
{
    Core::shared().setDefaultCipherConfiguration(version);
}

void Database::setConfig(const UnsafeStringView& name,
                         Invocation invocation,
                         Invocation unInvocation,
                         Priority priority)
{
    m_innerDatabase->purge();
    InnerDatabase* database = m_innerDatabase;
    CustomConfig::Invocation configInvocation
    = [invocation, database](InnerHandle* innerHandle) -> bool {
        Handle handle = Handle(RecyclableDatabase(database, nullptr), innerHandle);
        return invocation(handle);
    };
    CustomConfig::Invocation configUninvocation = nullptr;
    if (unInvocation != nullptr) {
        configUninvocation = [unInvocation, database](InnerHandle* innerHandle) -> bool {
            Handle handle = Handle(RecyclableDatabase(database, nullptr), innerHandle);
            return unInvocation(handle);
        };
    }
    m_innerDatabase->setConfig(name,
                               std::static_pointer_cast<Config>(std::make_shared<CustomConfig>(
                               configInvocation, configUninvocation)),
                               priority);
}

void Database::removeConfig(const UnsafeStringView& name)
{
    m_innerDatabase->removeConfig(name);
}

bool Database::setDefaultTemporaryDirectory(const UnsafeStringView& directory)
{
    return Core::shared().setDefaultTemporaryDirectory(directory);
}

void Database::registerScalarFunction(const ScalarFunctionModule& module,
                                      const UnsafeStringView& name)
{
    Core::shared().registerScalarFunction(name, module);
}

void Database::addScalarFunction(const UnsafeStringView& name)
{
    WCDB::StringView configName = WCDB::StringView::formatted(
    "%s%s", WCDB::ScalarFunctionConfigPrefix.data(), name.data());
    m_innerDatabase->setConfig(configName,
                               WCDB::Core::shared().scalarFunctionConfig(name),
                               WCDB::Configs::Priority::Higher);
}

#pragma mark - Migration

void Database::addMigration(const UnsafeStringView& sourcePath,
                            const UnsafeData& sourceCipher,
                            const TableFilter& filter)
{
    InnerDatabase::MigrationTableFilter callback = nullptr;
    if (filter != nullptr) {
        callback = [filter](MigrationUserInfo& userInfo) {
            MigrationInfo info;
            info.table = userInfo.getTable();
            info.sourceTable = userInfo.getSourceTable();
            filter(info);
            if (info.sourceTable.length() > 0) {
                userInfo.setSource(info.sourceTable);
                userInfo.setFilter(info.filterCondition);
            }
        };
    }
    m_innerDatabase->addMigration(sourcePath, sourceCipher, callback);
}

bool Database::stepMigration()
{
    auto done = m_innerDatabase->stepMigration(false);
    return done.succeed();
}

void Database::enableAutoMigration(bool flag)
{
    Core::shared().enableAutoMigrate(m_innerDatabase, flag);
}

void Database::setNotificationWhenMigrated(Database::MigratedCallback onMigrated)
{
    InnerDatabase::MigratedCallback callback = nullptr;
    if (onMigrated != nullptr) {
        callback = [onMigrated](InnerDatabase* innerDatabase,
                                const MigrationBaseInfo* baseInfo) {
            Optional<MigrationInfo> info;
            if (baseInfo != nullptr) {
                info = MigrationInfo();
                info->table = baseInfo->getTable();
                info->sourceTable = baseInfo->getSourceTable();
            }
            Database database = Database(innerDatabase);
            onMigrated(database, info);
        };
    }
    m_innerDatabase->setNotificationWhenMigrated(callback);
}

bool Database::isMigrated() const
{
    return m_innerDatabase->isMigrated();
}

#pragma mark - Compression

Database::CompressionInfo::CompressionInfo(void* innerInfo)
: m_innerInfo(innerInfo)
{
}

const StringView& Database::CompressionInfo::getTableName() const
{
    return ((CompressionTableInfo*) m_innerInfo)->getTable();
}

void Database::CompressionInfo::addZSTDNormalCompressField(const Field& field)
{
    CompressionColumnInfo columnInfo(field, CompressionType::Normal);
    ((CompressionTableUserInfo*) m_innerInfo)->addCompressingColumn(columnInfo);
}

void Database::CompressionInfo::addZSTDDictCompressField(const Field& field, DictId dictId)
{
    CompressionColumnInfo columnInfo(field, CompressionType::Dict);
    columnInfo.setCommonDict(dictId);
    ((CompressionTableUserInfo*) m_innerInfo)->addCompressingColumn(columnInfo);
}

void Database::CompressionInfo::addZSTDDictCompressField(const Field& field,
                                                         const Field& matchField,
                                                         const std::map<int64_t, DictId>& dictIds)
{
    if (dictIds.empty()) {
        return;
    }
    CompressionColumnInfo columnInfo(field, matchField);
    for (const auto& iter : dictIds) {
        if (iter.first == DictDefaultMatchValue) {
            columnInfo.setCommonDict(iter.second);
        } else {
            columnInfo.addMatchDict(iter.first, iter.second);
        }
    }
    ((CompressionTableUserInfo*) m_innerInfo)->addCompressingColumn(columnInfo);
}

Optional<Data> Database::trainDict(const std::vector<std::string>& strings, DictId dictId)
{
    int index = 0;
    return CompressionCenter::shared().trainDict(dictId, [&]() -> Optional<UnsafeData> {
        if (index < strings.size()) {
            const std::string& string = strings[index];
            index++;
            return UnsafeData((unsigned char*) string.data(), string.size());
        } else {
            return NullOpt;
        }
    });
}

Optional<Data> Database::trainDict(const std::vector<Data>& datas, DictId dictId)
{
    int index = 0;
    return CompressionCenter::shared().trainDict(dictId, [&]() -> Optional<UnsafeData> {
        if (index < datas.size()) {
            const Data& data = datas[index];
            index++;
            return data;
        } else {
            return NullOpt;
        }
    });
}

bool Database::registerZSTDDict(const UnsafeData& dict, DictId dictId)
{
    return CompressionCenter::shared().registerDict(dictId, dict);
}

void Database::setCompression(const CompressionFilter& filter)
{
    InnerDatabase::CompressionTableFilter callback = nullptr;
    if (filter != nullptr) {
        callback = [filter](CompressionTableUserInfo& userInfo) {
            CompressionInfo info(&userInfo);
            filter(info);
        };
        StringView configName = StringView::formatted(
        "%s%s", ScalarFunctionConfigPrefix.data(), DecompressFunctionName.data());
        m_innerDatabase->setConfig(configName,
                                   Core::shared().scalarFunctionConfig(DecompressFunctionName),
                                   Configs::Priority::Higher);
    }
    m_innerDatabase->addCompression(callback);
}

void Database::disableCompresssNewData(bool disable)
{
    m_innerDatabase->setCanCompressNewData(!disable);
}

bool Database::stepCompression()
{
    return m_innerDatabase->stepCompression(false).succeed();
}

void Database::enableAutoCompression(bool flag)
{
    Core::shared().enableAutoCompress(m_innerDatabase, flag);
}

void Database::setNotificationWhenCompressed(const CompressedCallback& onCompressd)
{
    InnerDatabase::CompressedCallback callback = nullptr;
    if (onCompressd != nullptr) {
        callback = [onCompressd](InnerDatabase* innerDatabase,
                                 const CompressionTableBaseInfo* info) {
            Database database = Database(innerDatabase);
            if (info != nullptr) {
                onCompressd(database, info->getTable());
            } else {
                onCompressd(database, NullOpt);
            }
        };
    }
    m_innerDatabase->setNotificationWhenCompressed(callback);
}

bool Database::isCompressed() const
{
    return m_innerDatabase->isCompressed();
}

bool Database::rollbackCompression(ProgressUpdateCallback onProgressUpdated)
{
    return m_innerDatabase->rollbackCompression(onProgressUpdated);
}

#pragma mark - Version

const StringView Database::getVersion()
{
    return StringView(WCDB_VERSION_STRING);
}

const StringView Database::getSourceId()
{
    return StringView(WCDB_SOURCE_ID);
}

} //namespace WCDB
