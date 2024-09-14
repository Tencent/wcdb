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

#include "InnerDatabase.hpp"
#include "Assertion.hpp"
#include "FileManager.hpp"
#include "Notifier.hpp"
#include "Path.hpp"
#include "RepairKit.h"
#include "StringView.hpp"
#include "WCDBError.hpp"

#include "AssembleHandleOperator.hpp"
#include "BackupHandleOperator.hpp"
#include "CompressHandleOperator.hpp"
#include "IntegerityHandleOperator.hpp"
#include "MigrateHandleOperator.hpp"
#include "VacuumHandleOperator.hpp"

#include "CompressingHandleDecorator.hpp"
#include "MigratingHandleDecorator.hpp"

#include "AutoVacuumConfig.hpp"
#include "BusyRetryConfig.hpp"
#include "CipherHandle.hpp"
#include "CommonCore.hpp"
#include "DBOperationNotifier.hpp"
#include "DecorativeHandle.hpp"
#include "SQLite.h"

#include <ctime>

namespace WCDB {

#pragma mark - Initializer
InnerDatabase::InnerDatabase(const UnsafeStringView &path)
: HandlePool(path)
, m_initialized(false)
, m_closing(0)
, m_tag(Tag::invalid())
, m_fullSQLTrace(false)
, m_autoCheckpoint(true)
, m_factory(path)
, m_needLoadIncremetalMaterial(false)
, m_migration(this)
, m_migratedCallback(nullptr)
, m_compression(this)
, m_compressedCallback(nullptr)
, m_isInMemory(false)
, m_sharedInMemoryHandle(nullptr)
, m_mergeLogic(this)
{
    StringViewMap<Value> info;
    DBOperationNotifier::shared().notifyOperation(
    this, DBOperationNotifier::Operation::Create, info);
}

InnerDatabase::~InnerDatabase() = default;

#pragma mark - Basic
void InnerDatabase::setTag(const Tag &tag)
{
    LockGuard memoryGuard(m_memory);
    m_tag = tag;
    StringViewMap<Value> info;
    DBOperationNotifier::shared().notifyOperation(
    this, DBOperationNotifier::Operation::SetTag, info);
}

Tag InnerDatabase::getTag() const
{
    SharedLockGuard memoryGuard(m_memory);
    return m_tag;
}

bool InnerDatabase::canOpen()
{
    CommonCore::shared().skipIntegrityCheck(getPath());
    auto handle = getHandle();
    CommonCore::shared().skipIntegrityCheck(nullptr);
    return handle != nullptr;
}

void InnerDatabase::didDrain()
{
    WCTAssert(m_concurrency.writeSafety());
    WCTAssert(m_memory.writeSafety());
    WCTAssert(!isOpened());
    m_initialized = false;
}

bool InnerDatabase::checkShouldInterruptWhenClosing(const UnsafeStringView &sourceType)
{
    if (m_closing != 0) {
        Error error(Error::Code::Interrupt, Error::Level::Ignore, "Interrupt due to it's closing.");
        error.infos.insert_or_assign(ErrorStringKeyPath, path);
        error.infos.insert_or_assign(ErrorStringKeyType, sourceType);
        Notifier::shared().notify(error);
        setThreadedError(std::move(error));
        return true;
    }
    return false;
}

void InnerDatabase::close(const ClosedCallback &onClosed)
{
    if (m_isInMemory) {
        if (m_sharedInMemoryHandle != nullptr) {
            m_sharedInMemoryHandle->close();
        }
        m_sharedInMemoryHandle = nullptr;
        if (onClosed != nullptr) {
            onClosed();
            didDrain();
        }
        return;
    }
    ++m_closing;
    {
        SharedLockGuard concurrencyGuard(m_concurrency);
        SharedLockGuard memoryGuard(m_memory);
        // suspend auto checkpoint/backup/integrity check/migrate/compress/merge fts5 index
        for (auto &handle : getHandlesOfSlot(HandleSlot::HandleSlotAutoTask)) {
            handle->suspend(true);
        }
    }
    CommonCore::shared().stopAllDatabaseEvent(getPath());
    drain(onClosed);
    --m_closing;
}

bool InnerDatabase::isOpened() const
{
    return isAliving();
}

InnerDatabase::InitializedGuard InnerDatabase::initialize()
{
    do {
        {
            SharedLockGuard concurrencyGuard(m_concurrency);
            SharedLockGuard memoryGuard(m_memory);
            if (m_initialized) {
                return concurrencyGuard;
            }
        }

        // Blocked initialization
        LockGuard concurrencyGuard(m_concurrency);
        LockGuard memoryGuard(m_memory);
        if (m_initialized) {
            // retry
            continue;
        }
        if (m_isInMemory) {
            m_initialized = true;
            continue;
        }
        CommonCore::shared().setThreadedErrorPath(path);
        if (!FileManager::createDirectoryWithIntermediateDirectories(Path::getDirectory(path))) {
            assignWithSharedThreadedError();
            break;
        }
        // vacuum
        {
            Repair::FactoryVacuum vacuumer = m_factory.vacuumer();
            if (!vacuumer.work()) {
                setThreadedError(vacuumer.getError());
                break;
            }
        }
        // retrieveRenewed
        {
            Repair::FactoryRenewer renewer = m_factory.renewer();
            if (!renewer.work()) {
                setThreadedError(renewer.getError());
                break;
            }
        }
        auto exists = FileManager::fileExists(path);
        if (!exists.succeed()) {
            assignWithSharedThreadedError();
            break;
        }
        // create file to make file identifier stable
        if (!exists.value() && !FileManager::createFile(path)) {
            assignWithSharedThreadedError();
            break;
        }
        CommonCore::shared().setThreadedErrorPath(nullptr);
        m_initialized = true;
    } while (true);
    return nullptr;
}

#pragma mark - Config
void InnerDatabase::setConfigs(const Configs &configs)
{
    LockGuard memoryGuard(m_memory);
    m_configs = configs;
}

void InnerDatabase::setConfig(const UnsafeStringView &name,
                              const std::shared_ptr<Config> &config,
                              int priority)
{
    LockGuard memoryGuard(m_memory);
    m_configs.insert(StringView(name), config, priority);
}

void InnerDatabase::removeConfig(const UnsafeStringView &name)
{
    LockGuard memoryGuard(m_memory);
    m_configs.erase(StringView(name));
}

void InnerDatabase::setFullSQLTraceEnable(bool enable)
{
    m_fullSQLTrace = enable;
}

void InnerDatabase::setAutoCheckpointEnable(bool enable)
{
    m_autoCheckpoint = enable;
}

#pragma mark - Handle
RecyclableHandle InnerDatabase::getHandle(bool writeHint)
{
    HandleType type = HandleType::Normal;
    if (m_isInMemory) {
        InitializedGuard initializedGuard = initialize();
        if (m_sharedInMemoryHandle == nullptr) {
            m_sharedInMemoryHandle = generateSlotedHandle(type);
        }
        return RecyclableHandle(m_sharedInMemoryHandle, nullptr);
    }
    // Additional shared lock is not needed because the threadedHandles is always empty when it's blocked. So threaded handles is thread safe.
    auto handle = m_transactionedHandles.getOrCreate();
    if (handle.get() != nullptr) {
        handle->configTransactionEvent(this);
        WCTAssert(m_concurrency.readSafety());
        return handle;
    }
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return nullptr;
    }
    handle = flowOut(type, writeHint);
    if (handle != nullptr) {
        handle->configTransactionEvent(this);
    }
    return handle;
}

bool InnerDatabase::execute(const Statement &statement)
{
    RecyclableHandle handle = getHandle(statement.isWriteStatement());
    if (handle != nullptr) {
        if (handle->execute(statement)) {
            return true;
        }
        setThreadedError(handle->getError());
    }
    return false;
}

bool InnerDatabase::execute(const UnsafeStringView &sql)
{
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        if (handle->execute(sql)) {
            return true;
        }
        setThreadedError(handle->getError());
    }
    return false;
}

Optional<bool> InnerDatabase::tableExists(const UnsafeStringView &table)
{
    Optional<bool> exists;
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        exists = handle->tableExists(table);
        if (!exists.succeed()) {
            setThreadedError(handle->getError());
        }
    }
    return exists;
}

StringView InnerDatabase::getRunningSQLInThread(uint64_t tid) const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    for (const auto &handles : m_handles) {
        for (const auto &handle : handles) {
            if (handle->isUsingInThread(tid)) {
                StringView sql = handle->getCurrentSQL();
                if (!sql.empty()) {
                    return sql;
                }
            }
        }
    }
    return StringView();
}

std::shared_ptr<InnerHandle> InnerDatabase::generateSlotedHandle(HandleType type)
{
    WCTAssert(m_concurrency.readSafety());
    HandleSlot slot = slotOfHandleType(type);
    std::shared_ptr<InnerHandle> handle;
    switch (slot) {
    case HandleSlotNormal:
    case HandleSlotAutoTask:
        handle = std::make_shared<DecorativeHandle>();
        break;
    case HandleSlotCipher:
        handle = std::make_shared<CipherHandle>();
        break;
    default:
        WCTAssert(slot == HandleSlotAssemble || slot == HandleSlotVacuum);
        handle = std::make_shared<ConfiguredHandle>();
        break;
    }

    if (handle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return nullptr;
    }

    handle->setThreadedErrorProne(this);

    if (!setupHandle(type, handle.get())) {
        return nullptr;
    }
    return handle;
}

bool InnerDatabase::willReuseSlotedHandle(HandleType type, InnerHandle *handle)
{
    return setupHandle(type, handle);
}

bool InnerDatabase::setupHandle(HandleType type, InnerHandle *handle)
{
    WCTAssert(handle != nullptr);

    handle->setTag(getTag());
    handle->setType(type);
    handle->setFullSQLTraceEnable(m_fullSQLTrace);
    handle->setBusyTraceEnable(CommonCore::shared().isBusyTraceEnable());
    HandleSlot slot = slotOfHandleType(type);
    handle->enableWriteMainDB(slot == HandleSlotAutoTask || slot == HandleSlotAssemble
                              || slot == HandleSlotVacuum);
    handle->markAsCanBeSuspended(false);
    handle->markErrorAsUnignorable(99); //Clear all ignorable code

    // Decoration
    if (slot == HandleSlotNormal || slot == HandleSlotAutoTask) {
        bool hasDecorator = false;
        WCTAssert(dynamic_cast<DecorativeHandle *>(handle) != nullptr);
        DecorativeHandle *decorativeHandle = static_cast<DecorativeHandle *>(handle);
        // CompressingHandleDecorator must be added before MigratingHandleDecorator.
        if (m_compression.shouldCompress()) {
            hasDecorator = true;
            decorativeHandle->tryAddDecorator<CompressingHandleDecorator>(
            DecoratorCompressingHandle, m_compression);
        }
        if (type == HandleType::Normal && m_migration.shouldMigrate()) {
            hasDecorator = true;
            decorativeHandle->tryAddDecorator<MigratingHandleDecorator>(
            DecoratorMigratingHandle, m_migration);
        }
        if (!hasDecorator) {
            decorativeHandle->clearDecorators();
        }
    }

    Configs configs;
    {
        SharedLockGuard memoryGuard(m_memory);
        configs = m_configs;
    }
    bool succeed = handle->reconfigure(configs);
    if (!succeed) {
        setThreadedError(handle->getError());
        return false;
    }

    if (slot != HandleSlotAssemble && slot != HandleSlotVacuum && slot != HandleSlotCipher) {
        handle->setPath(path);
        bool hasOpened = handle->isOpened();
        Time start = Time::now();
        uint64_t cpuStart = Time::currentThreadCPUTimeInMicroseconds();
        if (!handle->open()) {
            setThreadedError(handle->getError());
            return false;
        }
        if (!hasOpened && slot == HandleSlotNormal) {
            std::time_t openTime
            = (Time::now().nanoseconds() - start.nanoseconds()) / 1000;
            uint64_t openCPUTime = Time::currentThreadCPUTimeInMicroseconds() - cpuStart;
            int memoryUsed, tableCount, indexCount, triggerCount;
            if (handle->getSchemaInfo(memoryUsed, tableCount, indexCount, triggerCount)) {
                StringViewMap<Value> info;
                info.insert_or_assign(MonitorInfoKeyHandleOpenTime, openTime);
                info.insert_or_assign(MonitorInfoKeyHandleOpenCPUTime, openCPUTime);
                info.insert_or_assign(MonitorInfoKeySchemaUsage, memoryUsed);
                info.insert_or_assign(MonitorInfoKeyTableCount, tableCount);
                info.insert_or_assign(MonitorInfoKeyIndexCount, indexCount);
                info.insert_or_assign(MonitorInfoKeyTriggerCount, triggerCount);
                info.insert_or_assign(MonitorInfoKeyHandleCount,
                                      numberOfAliveHandlesInSlot(slot) + 1);
                DBOperationNotifier::shared().notifyOperation(
                this, DBOperationNotifier::Operation::OpenHandle, info);
            }
        }
    } else if (slot == HandleSlotCipher) {
        WCTAssert(dynamic_cast<CipherHandle *>(handle) != nullptr);
        CipherHandle *cipherHandle = static_cast<CipherHandle *>(handle);
        if (!cipherHandle->openCipherInMemory()) {
            setThreadedError(cipherHandle->getCipherError());
            return false;
        }
        auto salt = cipherHandle->tryGetSaltFromDatabase(getPath());
        if (salt.failed()) {
            assignWithSharedThreadedError();
            return false;
        } else if (salt.value().length() > 0) {
            cipherHandle->setCipherSalt(salt.value());
        }
    }

    return true;
}

#pragma mark - Threaded
void InnerDatabase::markHandleAsTransactioned(InnerHandle *handle)
{
    WCTAssert(m_transactionedHandles.getOrCreate().get() == nullptr);
    RecyclableHandle currentHandle = getHandle();
    WCTAssert(currentHandle.get() == handle);
    m_transactionedHandles.getOrCreate() = currentHandle;
    WCTAssert(m_transactionedHandles.getOrCreate().get() != nullptr);
}

void InnerDatabase::markHandleAsUntransactioned()
{
    WCTAssert(m_transactionedHandles.getOrCreate().get() != nullptr);
    m_transactionedHandles.getOrCreate() = nullptr;
    WCTAssert(m_transactionedHandles.getOrCreate().get() == nullptr);
}

#pragma mark - Transaction
bool InnerDatabase::isInTransaction()
{
    WCTAssert(m_transactionedHandles.getOrCreate().get() == nullptr
              || m_transactionedHandles.getOrCreate().get()->isInTransaction());
    return m_transactionedHandles.getOrCreate().get() != nullptr;
}

bool InnerDatabase::beginTransaction()
{
    RecyclableHandle handle = getHandle(true);
    if (handle == nullptr) {
        return false;
    }
    if (handle->beginTransaction()) {
        return true;
    }
    setThreadedError(handle->getError());
    return false;
}

bool InnerDatabase::commitOrRollbackTransaction()
{
    RecyclableHandle handle = getHandle();
    WCTRemedialAssert(handle != nullptr,
                      "Commit or rollback transaction should not be called without begin.",
                      return false;);
    if (handle->commitOrRollbackTransaction()) {
        return true;
    }
    setThreadedError(handle->getError());
    return false;
}

void InnerDatabase::rollbackTransaction()
{
    RecyclableHandle handle = getHandle();
    WCTRemedialAssert(handle != nullptr,
                      "Rollback transaction should not be called without begin.",
                      return;);
    handle->rollbackTransaction();
}

bool InnerDatabase::runTransaction(const TransactionCallback &transaction)
{
    RecyclableHandle handle = getHandle(true);
    if (handle == nullptr) return false;
    if (!handle->runTransaction(transaction)) {
        setThreadedError(handle->getError());
        return false;
    }
    return true;
}

bool InnerDatabase::runPausableTransactionWithOneLoop(const TransactionCallbackForOneLoop &transaction)
{
    // get threaded handle
    RecyclableHandle handle = getHandle(true);
    if (handle == nullptr) return false;
    if (!handle->runPausableTransactionWithOneLoop(transaction)) {
        setThreadedError(handle->getError());
        return false;
    }
    return true;
}

#pragma mark - File
bool InnerDatabase::removeFiles()
{
    if (m_isInMemory) {
        return false;
    }
    bool result = false;
    close([&result, this]() {
        std::list<StringView> paths = getPaths();
        paths.reverse(); // reverse to remove the non-critical paths first avoiding app stopped between the removing
        result = FileManager::removeItems(paths);
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

Optional<size_t> InnerDatabase::getFilesSize()
{
    if (m_isInMemory) {
        return 0;
    }
    auto size = FileManager::getItemsSize(getPaths());
    if (!size.succeed()) {
        assignWithSharedThreadedError();
    }
    return size;
}

bool InnerDatabase::moveFiles(const UnsafeStringView &directory)
{
    if (m_isInMemory) {
        return false;
    }
    bool result = false;
    close([&result, &directory, this]() {
        std::list<StringView> paths = getPaths();
        paths.reverse();
        result = FileManager::moveItems(paths, directory);
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

const StringView &InnerDatabase::getPath() const
{
    return path;
}

std::list<StringView> InnerDatabase::getPaths() const
{
    if (m_isInMemory) {
        return { path };
    }
    return pathsOfDatabase(path);
}

std::list<StringView> InnerDatabase::pathsOfDatabase(const UnsafeStringView &database)
{
    return {
        StringView(database),
        InnerHandle::walPathOfDatabase(database),
        Repair::Factory::incrementalMaterialPathForDatabase(database),
        Repair::Factory::firstMaterialPathForDatabase(database),
        Repair::Factory::lastMaterialPathForDatabase(database),
        Repair::Factory::factoryPathForDatabase(database),
        InnerHandle::journalPathOfDatabase(database),
        InnerHandle::shmPathOfDatabase(database),
    };
}

void InnerDatabase::setInMemory()
{
    WCTAssert(!m_initialized);
    m_isInMemory = true;
}

#pragma mark - Repair

void InnerDatabase::markNeedLoadIncremetalMaterial()
{
    m_needLoadIncremetalMaterial = true;
}

void InnerDatabase::tryLoadIncremetalMaterial()
{
    if (!m_needLoadIncremetalMaterial) {
        return;
    }
    auto config = CommonCore::shared().getABTestConfig("clicfg_wcdb_incremental_backup");
    if (config.failed() || config.value().length() == 0
        || atoi(config.value().data()) != 1) {
        m_needLoadIncremetalMaterial = false;
        return;
    }

    const StringView &databasePath = getPath();
    StringView materialPath
    = Repair::Factory::incrementalMaterialPathForDatabase(databasePath);
    auto exist = FileManager::fileExists(materialPath);
    if (!exist.hasValue()) {
        return;
    }
    if (!exist.value()) {
        m_needLoadIncremetalMaterial = false;
        return;
    }
    SharedIncrementalMaterial material = std::make_shared<Repair::IncrementalMaterial>();
    RecyclableHandle handle = flowOut(HandleType::BackupCipher);
    if (handle == nullptr) {
        return;
    }
    WCTAssert(dynamic_cast<CipherHandle *>(handle.get()) != nullptr);
    CipherHandle *cipherHandle = static_cast<CipherHandle *>(handle.get());
    bool useMaterial = false;
    if (cipherHandle->isCipherDB()) {
        material->setCipherDelegate(cipherHandle);
        useMaterial = material->decryptedDeserialize(materialPath, false);
        material->setCipherDelegate(nullptr);
    } else {
        useMaterial = material->deserialize(materialPath);
    }
    if (useMaterial) {
        if (material->pages.size() < BackupMaxAllowIncrementalPageCount) {
            CommonCore::shared().tryRegisterIncrementalMaterial(getPath(), material);
        } else {
            FileManager::removeItem(materialPath);
            Error error(Error::Code::Error, Error::Level::Warning, "Remove large incremental material");
            error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
            error.infos.insert_or_assign(ErrorStringKeyPath, databasePath);
            error.infos.insert_or_assign("PageCount", material->pages.size());
            Notifier::shared().notify(error);
        }
    } else {
        FileManager::removeItem(materialPath);
        Error error(Error::Code::Error, Error::Level::Warning, "Remove unresolved incremental material");
        error.infos.insert_or_assign(ErrorStringKeySource, ErrorSourceRepair);
        error.infos.insert_or_assign(ErrorStringKeyPath, databasePath);
        Notifier::shared().notify(error);
    }
    m_needLoadIncremetalMaterial = false;
}

void InnerDatabase::filterBackup(const BackupFilter &tableShouldBeBackedup)
{
    LockGuard memoryGuard(m_memory);
    m_factory.filter(tableShouldBeBackedup);
}

bool InnerDatabase::backup(bool interruptible)
{
    if (m_isInMemory) {
        return false;
    }
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return false; // mark as succeed if it's not an auto initialize action.
    }

    WCTRemedialAssert(
    !isInTransaction(), "Backup can't be run in transaction.", return false;);

    RecyclableHandle backupReadHandle = flowOut(HandleType::BackupRead);
    if (backupReadHandle == nullptr) {
        return false;
    }
    RecyclableHandle backupWriteHandle = flowOut(HandleType::BackupWrite);
    if (backupWriteHandle == nullptr) {
        return false;
    }

    RecyclableHandle backupCipherHandle = flowOut(HandleType::BackupCipher);
    if (backupCipherHandle == nullptr) {
        return false;
    }
    WCTAssert(backupReadHandle.get() != backupCipherHandle.get());
    WCTAssert(backupReadHandle.get() != backupWriteHandle.get());
    WCTAssert(backupWriteHandle.get() != backupCipherHandle.get());

    if (interruptible) {
        backupReadHandle->markAsCanBeSuspended(true);
        backupWriteHandle->markAsCanBeSuspended(true);
        if (checkShouldInterruptWhenClosing(ErrorTypeBackup)) {
            return false;
        }
    }

    CommonCore::shared().setThreadedErrorPath(path);

    Repair::FactoryBackup backup = m_factory.backup();
    Repair::BackupHandleOperator &backupReadOperator
    = backupReadHandle.getDecorative()->getOrCreateOperator<Repair::BackupHandleOperator>(
    OperatorBackup);
    backup.setBackupSharedDelegate(&backupReadOperator);

    Repair::BackupHandleOperator &backupWriteOperator
    = backupWriteHandle.getDecorative()->getOrCreateOperator<Repair::BackupHandleOperator>(
    OperatorBackup);
    backup.setBackupExclusiveDelegate(&backupWriteOperator);
    WCTAssert(dynamic_cast<CipherHandle *>(backupCipherHandle.get()) != nullptr);
    backup.setCipherDelegate(static_cast<CipherHandle *>(backupCipherHandle.get()));

    bool succeed = backup.work(getPath(), interruptible);
    if (!succeed) {
        if (backup.getError().level == Error::Level::Ignore) {
            succeed = true;
        } else {
            setThreadedError(backup.getError());
        }
    }
    CommonCore::shared().setThreadedErrorPath("");
    return succeed;
}

bool InnerDatabase::deposit()
{
    if (m_isInMemory) {
        return false;
    }
    bool result = false;
    close([&result, this]() {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return;
        }

        RecyclableHandle backupReadHandle = flowOut(HandleType::AssembleBackupRead);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = flowOut(HandleType::AssembleBackupWrite);
        if (backupWriteHandle == nullptr) {
            return;
        }
        RecyclableHandle assemblerHandle = flowOut(HandleType::Assemble);
        if (assemblerHandle == nullptr) {
            return;
        }
        RecyclableHandle cipherHandle = flowOut(HandleType::AssembleCipher);
        if (cipherHandle == nullptr) {
            return;
        }
        WCTAssert(backupReadHandle.get() != backupWriteHandle.get());
        WCTAssert(backupReadHandle.get() != assemblerHandle.get());
        WCTAssert(backupWriteHandle.get() != assemblerHandle.get());
        WCTAssert(backupReadHandle.get() != cipherHandle.get());
        WCTAssert(backupWriteHandle.get() != cipherHandle.get());
        WCTAssert(assemblerHandle.get() != cipherHandle.get());

        WCTAssert(!backupReadHandle->isOpened());
        WCTAssert(!backupWriteHandle->isOpened());
        WCTAssert(!assemblerHandle->isOpened());

        CommonCore::shared().setThreadedErrorPath(path);

        Repair::FactoryRenewer renewer = m_factory.renewer();
        Repair::BackupHandleOperator backupReadOperator(backupReadHandle.get());
        renewer.setBackupSharedDelegate(&backupReadOperator);
        Repair::BackupHandleOperator backupWriteOperator(backupWriteHandle.get());
        renewer.setBackupExclusiveDelegate(&backupWriteOperator);
        AssembleHandleOperator assembleOperator(assemblerHandle.get());
        renewer.setAssembleDelegate(&assembleOperator);
        WCTAssert(dynamic_cast<CipherHandle *>(cipherHandle.get()) != nullptr);
        renewer.setCipherDelegate(static_cast<CipherHandle *>(cipherHandle.get()));
        // Prepare a new database from material at renew directory and wait for moving
        if (!renewer.prepare()) {
            setThreadedError(renewer.getError());
            CommonCore::shared().setThreadedErrorPath("");
            return;
        }
        Repair::FactoryDepositor depositor = m_factory.depositor();
        if (!depositor.work()) {
            setThreadedError(depositor.getError());
            return;
        }
        // If app stop here, it results that the old database is moved to deposited directory and the renewed one is not moved to the origin directory.
        // At next time this database launchs, the retrieveRenewed method will do the remaining work. So data will never lost.
        if (!renewer.work()) {
            setThreadedError(renewer.getError());
            CommonCore::shared().setThreadedErrorPath("");
        } else {
            result = true;
        }
        cipherHandle->close();
    });
    CommonCore::shared().setThreadedErrorPath("");
    return result;
}

bool InnerDatabase::containsDeposited() const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    return m_factory.containsDeposited();
}

bool InnerDatabase::removeDeposited()
{
    bool result = false;
    close([&result, this]() {
        result = m_factory.removeDeposited();
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

double InnerDatabase::retrieve(const ProgressCallback &onProgressUpdated)
{
    if (m_isInMemory) {
        return 0;
    }
    double result = -1;
    close([&result, &onProgressUpdated, this]() {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return;
        }

        RecyclableHandle backupReadHandle = flowOut(HandleType::AssembleBackupRead);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = flowOut(HandleType::AssembleBackupWrite);
        if (backupWriteHandle == nullptr) {
            return;
        }
        RecyclableHandle assemblerHandle = flowOut(HandleType::Assemble);
        if (assemblerHandle == nullptr) {
            return;
        }
        assemblerHandle->markAsCanBeSuspended(true);
        RecyclableHandle cipherHandle = flowOut(HandleType::AssembleCipher);
        if (cipherHandle == nullptr) {
            return;
        }
        WCTAssert(backupReadHandle.get() != backupWriteHandle.get());
        WCTAssert(backupReadHandle.get() != assemblerHandle.get());
        WCTAssert(backupWriteHandle.get() != assemblerHandle.get());
        WCTAssert(backupReadHandle.get() != cipherHandle.get());
        WCTAssert(backupWriteHandle.get() != cipherHandle.get());
        WCTAssert(assemblerHandle.get() != cipherHandle.get());

        WCTAssert(!backupReadHandle->isOpened());
        WCTAssert(!backupWriteHandle->isOpened());
        WCTAssert(!assemblerHandle->isOpened());

        CommonCore::shared().setThreadedErrorPath(path);

        Repair::FactoryRetriever retriever = m_factory.retriever();
        Repair::BackupHandleOperator backupReadOperator(backupReadHandle.get());
        retriever.setBackupSharedDelegate(&backupReadOperator);
        Repair::BackupHandleOperator backupWriteOperator(backupWriteHandle.get());
        retriever.setBackupExclusiveDelegate(&backupWriteOperator);
        AssembleHandleOperator assembleOperator(assemblerHandle.get());
        retriever.setAssembleDelegate(&assembleOperator);
        WCTAssert(dynamic_cast<CipherHandle *>(cipherHandle.get()) != nullptr);
        retriever.setCipherDelegate(static_cast<CipherHandle *>(cipherHandle.get()));
        retriever.setProgressCallback(onProgressUpdated);
        if (retriever.work()) {
            result = retriever.getScore().value();
        }
        setThreadedError(retriever.getError()); // retriever may have non-critical error even if it succeeds.
        CommonCore::shared().setThreadedErrorPath("");
        cipherHandle->close();
    });
    return result;
}

bool InnerDatabase::removeMaterials()
{
    bool result = false;
    close([&result, this]() {
        result = FileManager::removeItems(
        { Repair::Factory::incrementalMaterialPathForDatabase(path),
          Repair::Factory::firstMaterialPathForDatabase(path),
          Repair::Factory::lastMaterialPathForDatabase(path) });
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

void InnerDatabase::checkIntegrity(bool interruptible)
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return; // mark as succeed if it's not an auto initialize action.
    }
    RecyclableHandle handle = flowOut(HandleType::IntegrityCheck);
    if (handle != nullptr) {
        IntegerityHandleOperator &integerityOperator
        = handle.getDecorative()->getOrCreateOperator<IntegerityHandleOperator>(OperatorCheckIntegrity);
        if (interruptible) {
            if (checkShouldInterruptWhenClosing(ErrorTypeIntegrity)) {
                return;
            }
            handle->markAsCanBeSuspended(true);
        }
        integerityOperator.checkIntegrity();
    }
}

#pragma mark - Vacuum

bool InnerDatabase::vacuum(const ProgressCallback &onProgressUpdated)
{
    if (m_isInMemory) {
        return true;
    }
    bool result = false;
    close([&result, &onProgressUpdated, this]() {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return;
        }

        RecyclableHandle vacuumHandle = flowOut(HandleType::Vacuum);
        if (vacuumHandle == nullptr) {
            return;
        }

        CommonCore::shared().setThreadedErrorPath(path);

        Repair::FactoryVacuum vacuummer = m_factory.vacuumer();
        VacuumHandleOperator vacuumOperator(vacuumHandle.get());
        vacuummer.setVacuumDelegate(&vacuumOperator);
        vacuummer.setProgressCallback(onProgressUpdated);

        if (!vacuummer.prepare()) {
            setThreadedError(vacuummer.getError());
            CommonCore::shared().setThreadedErrorPath("");
            return;
        }

        if (!vacuummer.work()) {
            setThreadedError(vacuummer.getError());
            CommonCore::shared().setThreadedErrorPath("");
            return;
        }
        CommonCore::shared().setThreadedErrorPath("");
        result = true;
    });
    return result;
}

void InnerDatabase::enableAutoVacuum(bool incremental)
{
    setConfig(AutoVacuumConfigName,
              std::static_pointer_cast<WCDB::Config>(
              std::make_shared<WCDB::AutoVacuumConfig>(incremental)),
              Configs::Priority::Highest);
}

bool InnerDatabase::incrementalVacuum(int pages)
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return false; // mark as succeed if it's not an auto initialize action.
    }
    RecyclableHandle handle = flowOut(HandleType::AutoVacuum);
    if (!handle->prepare(StatementPragma().pragma(Pragma::incrementalVacuum()).with(pages))) {
        return false;
    }
    bool succeed = false;
    do {
        succeed = handle->step();
    } while (succeed && !handle->done());
    handle->finalize();
    return succeed;
}

#pragma mark - Migration
Optional<bool> InnerDatabase::stepMigration(bool interruptible)
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return NullOpt;
    }
    WCTRemedialAssert(
    !isInTransaction(), "Migrating can't be run in transaction.", return NullOpt;);
    WCTRemedialAssert(
    m_migration.shouldMigrate(), "It's not configured for migration.", return NullOpt;);
    Optional<bool> done;
    RecyclableHandle handle = flowOut(HandleType::Migrate);
    if (handle != nullptr) {
        MigrateHandleOperator &migrateOperator
        = handle.getDecorative()->getOrCreateOperator<MigrateHandleOperator>(OperatorMigrate);
        if (interruptible) {
            if (checkShouldInterruptWhenClosing(ErrorTypeMigrate)) {
                return false;
            }
            handle->markAsCanBeSuspended(true);
        }
        handle->markErrorAsIgnorable(Error::Code::Busy);

        done = m_migration.step(migrateOperator);
        if (!done.succeed() && handle->getError().isIgnorable()) {
            done = false;
        }
    }
    return done;
}

void InnerDatabase::didMigrate(const MigrationBaseInfo *info)
{
    MigratedCallback callback = nullptr;
    {
        SharedLockGuard lockGuard(m_memory);
        callback = m_migratedCallback;
    }
    if (callback != nullptr) {
        callback(this, info);
    }
}

void InnerDatabase::setNotificationWhenMigrated(const MigratedCallback &callback)
{
    LockGuard lockGuard(m_memory);
    m_migratedCallback = callback;
}

void InnerDatabase::addMigration(const UnsafeStringView &sourcePath,
                                 const UnsafeData &sourceCipher,
                                 const MigrationTableFilter &filter)
{
    StringView sourceDatabase = Path::normalize(sourcePath);
    if (sourceDatabase.compare(getPath()) != 0) {
        close([=]() {
            m_migration.addMigration(sourceDatabase, sourceCipher, filter);
        });
    } else {
        close([=]() {
            m_migration.addMigration(UnsafeStringView(), sourceCipher, filter);
        });
    }
}

bool InnerDatabase::isMigrated() const
{
    return m_migration.isMigrated();
}

StringViewSet InnerDatabase::getPathsOfSourceDatabases() const
{
    return m_migration.getPathsOfSourceDatabases();
}

#pragma mark - Compression
Optional<bool> InnerDatabase::stepCompression(bool interruptible)
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return NullOpt;
    }
    WCTRemedialAssert(
    !isInTransaction(), "Compressing can't be run in transaction.", return NullOpt;);
    WCTRemedialAssert(m_compression.shouldCompress(),
                      "It's not configured for compression.",
                      return NullOpt;);
    Optional<bool> done;
    RecyclableHandle handle = flowOut(HandleType::Compress);
    if (handle != nullptr) {
        CompressHandleOperator &compressOperator
        = handle.getDecorative()->getOrCreateOperator<CompressHandleOperator>(OperatorCompress);
        if (interruptible) {
            if (checkShouldInterruptWhenClosing(ErrorTypeCompress)) {
                return false;
            }
            handle->markAsCanBeSuspended(true);
        }
        handle->markErrorAsIgnorable(Error::Code::Busy);

        done = m_compression.step(compressOperator);
        if (!done.succeed() && handle->getError().isIgnorable()) {
            done = false;
        }
    }
    return done;
}

void InnerDatabase::didCompress(const CompressionTableBaseInfo *info)
{
    CompressedCallback callback = nullptr;
    {
        SharedLockGuard lockGuard(m_memory);
        callback = m_compressedCallback;
    }
    if (callback != nullptr) {
        callback(this, info);
    }
}

void InnerDatabase::setNotificationWhenCompressed(const CompressedCallback &callback)
{
    LockGuard lockGuard(m_memory);
    m_compressedCallback = callback;
}

void InnerDatabase::addCompression(const CompressionTableFilter &filter)
{
    close([=]() { m_compression.setTableFilter(filter); });
}

void InnerDatabase::setCanCompressNewData(bool canCompress)
{
    m_compression.setCanCompressNewData(canCompress);
}

bool InnerDatabase::isCompressed() const
{
    return m_compression.isCompressed();
}

bool InnerDatabase::rollbackCompression(const ProgressCallback &callback)
{
    WCTRemedialAssert(
    !isInTransaction(), "Can't revert compression in transaction.", return false;);

    bool ret = false;
    close([&]() {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return; // mark as succeed if it's not an auto initialize action.
        }

        m_compression.setProgressCallback(callback);

        RecyclableHandle handle = flowOut(HandleType::Compress);
        if (handle != nullptr) {
            CompressHandleOperator &compressOperator
            = handle.getDecorative()->getOrCreateOperator<CompressHandleOperator>(OperatorCompress);

            ret = m_compression.rollbackCompression(compressOperator);
            if (ret) {
                CommonCore::shared().enableAutoCompress(this, false);
            }
        }
    });
    return ret;
}

#pragma mark - Checkpoint
bool InnerDatabase::checkpoint(bool interruptible, CheckPointMode mode)
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return false; // mark as succeed if it's not an auto initialize action.
    }
    bool succeed = false;
    RecyclableHandle handle = flowOut(HandleType::Checkpoint);
    if (handle != nullptr) {
        if (interruptible) {
            if (checkShouldInterruptWhenClosing(ErrorTypeCheckpoint)) {
                return false;
            }
            handle->markAsCanBeSuspended(true);
        }
        tryLoadIncremetalMaterial();
        handle->markErrorAsIgnorable(Error::Code::Busy);
        succeed = handle->checkpoint(mode);
        if (!succeed && handle->getError().isIgnorable()) {
            succeed = true;
        }
    }
    return succeed;
}

#pragma mark - AutoMergeFTSIndex

Optional<bool> InnerDatabase::mergeFTSIndex(TableArray newTables, TableArray modifiedTables)
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return false; // mark as succeed if it's not an auto initialize action.
    }
    if (checkShouldInterruptWhenClosing(ErrorTypeMergeIndex)) {
        return false;
    }
    return m_mergeLogic.triggerMerge(newTables, modifiedTables);
}

void InnerDatabase::proccessMerge()
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return; // mark as succeed if it's not an auto initialize action.
    }
    if (checkShouldInterruptWhenClosing(ErrorTypeMergeIndex)) {
        return;
    }
    return m_mergeLogic.proccessMerge();
}

RecyclableHandle InnerDatabase::getMergeIndexHandle()
{
    return flowOut(HandleType::MergeIndex);
}

} //namespace WCDB
