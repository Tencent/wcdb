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

#include "AssembleHandle.hpp"
#include "BusyRetryConfig.hpp"
#include "CipherHandle.hpp"
#include "Core.hpp"
#include "DBOperationNotifier.hpp"
#include "MigrateHandle.hpp"
#include "MigratingHandle.hpp"
#include "OperationHandle.hpp"
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
, m_factory(path)
, m_needLoadIncremetalMaterial(false)
, m_migration(this)
, m_migratedCallback(nullptr)
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
    Core::shared().skipIntegrityCheck(getPath());
    auto handle = getHandle();
    Core::shared().skipIntegrityCheck(nullptr);
    return handle != nullptr;
}

void InnerDatabase::didDrain()
{
    WCTAssert(m_concurrency.writeSafety());
    WCTAssert(m_memory.writeSafety());
    WCTAssert(!isOpened());
    m_initialized = false;
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
        // suspend auto checkpoint/backup/integrity check/migrate/merge fts5 index
        for (auto &handle : getHandlesOfSlot(HandleSlot::HandleSlotOperation)) {
            handle->suspend(true);
        }
        for (auto &handle : getHandlesOfSlot(HandleSlot::HandleSlotMigrate)) {
            handle->suspend(true);
        }
    }
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
        Core::shared().setThreadedErrorPath(path);
        if (!FileManager::createDirectoryWithIntermediateDirectories(Path::getDirectory(path))) {
            assignWithSharedThreadedError();
            break;
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
        Core::shared().setThreadedErrorPath(nullptr);
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

#pragma mark - Handle
RecyclableHandle InnerDatabase::getHandle(bool writeHint)
{
    HandleType type
    = m_migration.shouldMigrate() ? HandleType::Migrating : HandleType::Normal;
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

std::shared_ptr<InnerHandle> InnerDatabase::generateSlotedHandle(HandleType type)
{
    WCTAssert(m_concurrency.readSafety());
    HandleSlot slot = slotOfHandleType(type);
    std::shared_ptr<InnerHandle> handle;
    switch (slot) {
    case HandleSlotMigrating:
        handle = std::make_shared<MigratingHandle>(m_migration);
        break;
    case HandleSlotMigrate:
        handle = std::make_shared<MigrateHandle>();
        break;
    case HandleSlotCipher:
        handle = std::make_shared<CipherHandle>();
        break;
    case HandleSlotAssemble:
        handle = std::make_shared<AssembleHandle>();
        break;
    case HandleSlotOperation:
        handle = std::make_shared<OperationHandle>();
        break;
    case HandleSlotCheckPoint: {
        handle = std::make_shared<OperationHandle>();
        handle->enableWriteMainDB(true);
    } break;
    default:
        WCTAssert(slot == HandleSlotNormal);
        handle = std::make_shared<ConfiguredHandle>();
        break;
    }

    if (handle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return nullptr;
    }

    handle->setTag(getTag());

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

    handle->setType(type);
    handle->setFullSQLTraceEnable(m_fullSQLTrace);
    HandleSlot slot = slotOfHandleType(type);

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

    if (slot != HandleSlotAssemble && slot != HandleSlotCipher) {
        handle->setPath(path);
        bool hasOpened = handle->isOpened();
        Time start = Time::now();
        uint64_t cpuStart = Time::currentThreadCPUTimeInMicroseconds();
        if (!handle->open()) {
            setThreadedError(handle->getError());
            return false;
        }
        if (!hasOpened && (slot == HandleSlotNormal || slot == HandleSlotMigrating)) {
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
    auto config = Core::shared().getABTestConfig("clicfg_wcdb_incremental_backup");
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
            Core::shared().tryRegisterIncrementalMaterial(getPath(), material);
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

    RecyclableHandle backupReadHandle = flowOut(HandleType::BackupRead);
    if (backupReadHandle == nullptr) {
        return false;
    }
    OperationHandle *operationBackupReadHandle
    = static_cast<OperationHandle *>(backupReadHandle.get());

    WCTRemedialAssert(
    !isInTransaction(), "Backup can't be run in transaction.", return false;);

    RecyclableHandle backupWriteHandle = flowOut(HandleType::BackupWrite);
    if (backupWriteHandle == nullptr) {
        return false;
    }

    RecyclableHandle backupCipherHandle = flowOut(HandleType::BackupCipher);
    if (backupCipherHandle == nullptr) {
        return false;
    }
    CipherHandle *operationBackupCipherHandle
    = static_cast<CipherHandle *>(backupCipherHandle.get());
    WCTAssert(backupReadHandle.get() != backupCipherHandle.get());

    WCTAssert(backupReadHandle.get() != backupWriteHandle.get());
    WCTAssert(backupWriteHandle.get() != backupCipherHandle.get());

    OperationHandle *operationBackupWriteHandle
    = static_cast<OperationHandle *>(backupWriteHandle.get());
    if (interruptible) {
        operationBackupReadHandle->markErrorAsIgnorable(Error::Code::Interrupt);
        operationBackupWriteHandle->markErrorAsIgnorable(Error::Code::Interrupt);
        operationBackupReadHandle->markAsCanBeSuspended(true);
        operationBackupWriteHandle->markAsCanBeSuspended(true);
        if (m_closing != 0) {
            Error error(Error::Code::Interrupt, Error::Level::Ignore, "Interrupt due to it's closing.");
            error.infos.insert_or_assign(ErrorStringKeyPath, path);
            error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeBackup);
            Notifier::shared().notify(error);
            setThreadedError(std::move(error));
            return false;
        }
    }

    Core::shared().setThreadedErrorPath(path);

    Repair::FactoryBackup backup = m_factory.backup();
    backup.setBackupSharedDelegate(operationBackupReadHandle);
    backup.setBackupExclusiveDelegate(
    static_cast<OperationHandle *>(backupWriteHandle.get()));
    backup.setCipherDelegate(operationBackupCipherHandle);

    bool succeed = backup.work(getPath(), interruptible);
    if (!succeed) {
        if (backup.getError().level == Error::Level::Ignore) {
            succeed = true;
        } else {
            setThreadedError(backup.getError());
        }
    }
    if (interruptible) {
        operationBackupWriteHandle->markAsCanBeSuspended(false);
        operationBackupReadHandle->markAsCanBeSuspended(false);
        operationBackupWriteHandle->markErrorAsUnignorable();
        operationBackupReadHandle->markErrorAsUnignorable();
    }
    Core::shared().setThreadedErrorPath("");
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

        Core::shared().setThreadedErrorPath(path);

        Repair::FactoryRenewer renewer = m_factory.renewer();
        renewer.setBackupSharedDelegate(
        static_cast<AssembleHandle *>(backupReadHandle.get()));
        renewer.setBackupExclusiveDelegate(
        static_cast<AssembleHandle *>(backupWriteHandle.get()));
        renewer.setAssembleDelegate(
        static_cast<AssembleHandle *>(assemblerHandle.get()));
        renewer.setCipherDelegate(static_cast<CipherHandle *>(cipherHandle.get()));
        // Prepare a new database from material at renew directory and wait for moving
        if (!renewer.prepare()) {
            setThreadedError(renewer.getError());
            Core::shared().setThreadedErrorPath("");
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
            Core::shared().setThreadedErrorPath("");
        } else {
            result = true;
        }
        cipherHandle->close();
    });
    Core::shared().setThreadedErrorPath("");
    return result;
}

double InnerDatabase::retrieve(const RetrieveProgressCallback &onProgressUpdated)
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

        Core::shared().setThreadedErrorPath(path);

        Repair::FactoryRetriever retriever = m_factory.retriever();
        retriever.setBackupSharedDelegate(
        static_cast<AssembleHandle *>(backupReadHandle.get()));
        retriever.setBackupExclusiveDelegate(
        static_cast<AssembleHandle *>(backupWriteHandle.get()));
        retriever.setAssembleDelegate(
        static_cast<AssembleHandle *>(assemblerHandle.get()));
        retriever.setCipherDelegate(static_cast<CipherHandle *>(cipherHandle.get()));
        retriever.setProgressCallback(onProgressUpdated);
        if (retriever.work()) {
            result = retriever.getScore().value();
        }
        setThreadedError(retriever.getError()); // retriever may have non-critical error even if it succeeds.
        Core::shared().setThreadedErrorPath("");
        cipherHandle->close();
    });
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
    RecyclableHandle handle = flowOut(HandleType::Integrity);
    if (handle != nullptr) {
        WCTAssert(dynamic_cast<OperationHandle *>(handle.get()) != nullptr);
        OperationHandle *operationHandle = static_cast<OperationHandle *>(handle.get());
        if (interruptible) {
            if (m_closing != 0) {
                Error error(Error::Code::Interrupt, Error::Level::Ignore, "Interrupt due to it's closing");
                error.infos.insert_or_assign(ErrorStringKeyPath, path);
                error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeIntegrity);
                Notifier::shared().notify(error);
                setThreadedError(std::move(error));
                return;
            }

            operationHandle->markErrorAsIgnorable(Error::Code::Interrupt);
            operationHandle->markAsCanBeSuspended(true);
        }
        operationHandle->checkIntegrity();
        if (interruptible) {
            operationHandle->markAsCanBeSuspended(false);
            operationHandle->markErrorAsUnignorable();
        }
    }
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
        WCTAssert(dynamic_cast<MigrateHandle *>(handle.get()) != nullptr);
        MigrateHandle *migrateHandle = static_cast<MigrateHandle *>(handle.get());

        if (interruptible) {
            if (m_closing != 0) {
                Error error(Error::Code::Interrupt, Error::Level::Ignore, "Interrupt due to it's closing");
                error.infos.insert_or_assign(ErrorStringKeyPath, path);
                error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeMigrate);
                Notifier::shared().notify(error);
                setThreadedError(std::move(error));
                return false;
            }
            migrateHandle->markErrorAsIgnorable(Error::Code::Interrupt);
            migrateHandle->markAsCanBeSuspended(true);
        }
        migrateHandle->markErrorAsIgnorable(Error::Code::Busy);
        done = m_migration.step(*migrateHandle);
        if (!done.succeed() && handle->getError().isIgnorable()) {
            done = false;
        }
        migrateHandle->markErrorAsUnignorable();
        if (interruptible) {
            migrateHandle->markAsCanBeSuspended(false);
            migrateHandle->markErrorAsUnignorable();
        }
    }
    return done;
}

void InnerDatabase::didMigrate(const MigrationBaseInfo *info)
{
    SharedLockGuard lockGuard(m_memory);
    if (m_migratedCallback != nullptr) {
        m_migratedCallback(this, info);
    }
}

void InnerDatabase::setNotificationWhenMigrated(const MigratedCallback &callback)
{
    LockGuard lockGuard(m_memory);
    m_migratedCallback = callback;
}

void InnerDatabase::addMigration(const UnsafeStringView &sourcePath,
                                 const UnsafeData &sourceCipher,
                                 const TableFilter &filter)
{
    StringView sourceDatabase;
    if (sourcePath.compare(getPath()) != 0) {
        sourceDatabase = sourcePath;
    }
    close(
    [=]() { m_migration.addMigration(sourceDatabase, sourceCipher, filter); });
}

bool InnerDatabase::isMigrated() const
{
    return m_migration.isMigrated();
}

std::set<StringView> InnerDatabase::getPathsOfSourceDatabases() const
{
    return m_migration.getPathsOfSourceDatabases();
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
        if (m_closing != 0 && interruptible) {
            Error error(Error::Code::Interrupt, Error::Level::Ignore, "Interrupt due to it's closing");
            error.infos.insert_or_assign(ErrorStringKeyPath, path);
            error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeCheckpoint);
            Notifier::shared().notify(error);
            setThreadedError(std::move(error));
            return false;
        }

        tryLoadIncremetalMaterial();

        WCTAssert(dynamic_cast<OperationHandle *>(handle.get()) != nullptr);
        OperationHandle *operationHandle = static_cast<OperationHandle *>(handle.get());

        if (interruptible) {
            operationHandle->markErrorAsIgnorable(Error::Code::Interrupt);
            operationHandle->markAsCanBeSuspended(true);
        }
        operationHandle->markErrorAsIgnorable(Error::Code::Busy);
        succeed = operationHandle->checkpoint(mode);
        if (!succeed && operationHandle->getError().isIgnorable()) {
            succeed = true;
        }
        operationHandle->markErrorAsUnignorable();
        if (interruptible) {
            operationHandle->markAsCanBeSuspended(false);
            operationHandle->markErrorAsUnignorable();
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
    if (m_closing != 0) {
        Error error(Error::Code::Interrupt,
                    Error::Level::Ignore,
                    "Interrupt merge fts index due to it's closing.");
        error.infos.insert_or_assign(ErrorStringKeyPath, path);
        error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeBackup);
        Notifier::shared().notify(error);
        setThreadedError(std::move(error));
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
    if (m_closing != 0) {
        Error error(Error::Code::Interrupt,
                    Error::Level::Ignore,
                    "Interrupt merge fts index due to it's closing.");
        error.infos.insert_or_assign(ErrorStringKeyPath, path);
        error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeBackup);
        Notifier::shared().notify(error);
        setThreadedError(std::move(error));
        return;
    }
    return m_mergeLogic.proccessMerge();
}

RecyclableHandle InnerDatabase::getMergeIndexHandle()
{
    return flowOut(HandleType::MergeIndex);
}

} //namespace WCDB
