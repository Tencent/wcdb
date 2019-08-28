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

#include <WCDB/Assertion.hpp>
#include <WCDB/Database.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Notifier.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/RepairKit.h>
#include <WCDB/StringView.hpp>

#include <WCDB/AssembleHandle.hpp>
#include <WCDB/MigrateHandle.hpp>
#include <WCDB/MigratingHandle.hpp>
#include <WCDB/OperationHandle.hpp>

namespace WCDB {

#pragma mark - Initializer
Database::Database(const UnsafeStringView &path)
: HandlePool(path)
, m_factory(path)
, m_tag(Tag::invalid())
, m_initialized(false)
, m_migratedCallback(nullptr)
, m_migration(this)
, m_closing(0)
{
}

Database::~Database() = default;

#pragma mark - Basic
void Database::setTag(const Tag &tag)
{
    LockGuard memoryGuard(m_memory);
    m_tag = tag;
}

Tag Database::getTag() const
{
    SharedLockGuard memoryGuard(m_memory);
    return m_tag;
}

bool Database::canOpen()
{
    return getHandle() != nullptr;
}

void Database::didDrain()
{
    WCTAssert(m_concurrency.writeSafety());
    WCTAssert(m_memory.writeSafety());
    WCTAssert(!isOpened());
    m_initialized = false;
}

void Database::close(const ClosedCallback &onClosed)
{
    ++m_closing;
    {
        SharedLockGuard concurrencyGuard(m_concurrency);
        SharedLockGuard memoryGuard(m_memory);
        // suspend auto checkpoint/backup/integrity check/migrate
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

bool Database::isOpened() const
{
    return isAliving();
}

Database::InitializedGuard Database::initialize()
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
        if (!FileManager::createDirectoryWithIntermediateDirectories(
            Path::getDirectoryName(path))) {
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
        if (!exists.has_value()) {
            assignWithSharedThreadedError();
            break;
        }
        // create file to make file identifier stable
        if (!exists.value() && !FileManager::createFile(path)) {
            assignWithSharedThreadedError();
            break;
        }
        m_initialized = true;
    } while (true);
    return nullptr;
}

Database::InitializedGuard Database::isInitialized() const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    if (m_initialized) {
        return concurrencyGuard;
    }
    return nullptr;
}

#pragma mark - Config
void Database::setConfigs(const Configs &configs)
{
    LockGuard memoryGuard(m_memory);
    m_configs = configs;
}

void Database::setConfig(const UnsafeStringView &name,
                         const std::shared_ptr<Config> &config,
                         int priority)
{
    LockGuard memoryGuard(m_memory);
    m_configs.insert(StringView(name), config, priority);
}

void Database::removeConfig(const UnsafeStringView &name)
{
    LockGuard memoryGuard(m_memory);
    m_configs.erase(StringView(name));
}

#pragma mark - Handle
RecyclableHandle Database::getHandle()
{
    // Additional shared lock is not needed because the threadedHandles is always empty when it's blocked. So threaded handles is thread safe.
    auto handle = m_transactionedHandles.getOrCreate();
    if (handle.get() != nullptr) {
        WCTAssert(m_concurrency.readSafety());
        return handle;
    }
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return nullptr;
    }
    return flowOut(m_migration.shouldMigrate() ? HandleType::Migrating : HandleType::Normal);
}

bool Database::execute(const Statement &statement)
{
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        TransactionGuard transactionedGuard(this, handle);
        if (handle->execute(statement)) {
            return true;
        }
        setThreadedError(handle->getError());
    }
    return false;
}

std::optional<bool> Database::tableExists(const UnsafeStringView &table)
{
    std::optional<bool> exists;
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        TransactionGuard transactionedGuard(this, handle);
        exists = handle->tableExists(table);
        if (!exists.has_value()) {
            setThreadedError(handle->getError());
        }
    }
    return exists;
}

std::shared_ptr<Handle> Database::generateSlotedHandle(HandleType type)
{
    WCTAssert(m_concurrency.readSafety());
    HandleSlot slot = slotOfHandleType(type);
    std::shared_ptr<Handle> handle;
    switch (slot) {
    case HandleSlotMigrating:
        handle = std::make_shared<MigratingHandle>(m_migration);
        break;
    case HandleSlotMigrate:
        handle = std::make_shared<MigrateHandle>();
        break;
    case HandleSlotAssemble:
        handle = std::make_shared<AssembleHandle>();
        break;
    case HandleSlotOperation:
        handle = std::make_shared<OperationHandle>();
        break;
    default:
        WCTAssert(slot == HandleSlotNormal);
        handle = std::make_shared<ConfiguredHandle>();
        break;
    }
    if (handle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return nullptr;
    }

    if (!setupHandle(type, handle.get())) {
        return nullptr;
    }

    return handle;
}

bool Database::willReuseSlotedHandle(HandleType type, Handle *handle)
{
    return setupHandle(type, handle);
}

bool Database::setupHandle(HandleType type, Handle *handle)
{
    WCTAssert(handle != nullptr);

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

    if (slot != HandleSlotAssemble) {
        handle->setPath(path);
        if (!handle->open()) {
            setThreadedError(handle->getError());
            return false;
        }
    } else {
        handle->clearPath();
    }

    return true;
}

#pragma mark - Threaded
void Database::markHandleAsTransactioned(const RecyclableHandle &handle)
{
    WCTAssert(m_transactionedHandles.getOrCreate().get() == nullptr);
    m_transactionedHandles.getOrCreate() = handle;
    WCTAssert(m_transactionedHandles.getOrCreate().get() != nullptr);
}

void Database::markHandleAsUntransactioned()
{
    WCTAssert(m_transactionedHandles.getOrCreate().get() != nullptr);
    m_transactionedHandles.getOrCreate() = nullptr;
    WCTAssert(m_transactionedHandles.getOrCreate().get() == nullptr);
}

Database::TransactionGuard::TransactionGuard(Database *database, const RecyclableHandle &handle)
: m_database(database), m_handle(handle), m_isInTransactionBefore(handle->isInTransaction())
{
    WCTAssert(m_database != nullptr);
    WCTAssert(m_handle != nullptr);
}

Database::TransactionGuard::~TransactionGuard()
{
    bool isInTransactionAfter = m_handle->isInTransaction();
    if (!m_isInTransactionBefore && isInTransactionAfter) {
        m_database->markHandleAsTransactioned(m_handle);
    } else if (m_isInTransactionBefore && !isInTransactionAfter) {
        m_database->markHandleAsUntransactioned();
    }
}

#pragma mark - Transaction
bool Database::isInTransaction()
{
    WCTAssert(m_transactionedHandles.getOrCreate().get() == nullptr
              || m_transactionedHandles.getOrCreate().get()->isInTransaction());
    return m_transactionedHandles.getOrCreate().get() != nullptr;
}

bool Database::beginTransaction()
{
    RecyclableHandle handle = getHandle();
    if (handle == nullptr) {
        return false;
    }
    TransactionGuard transactionedGuard(this, handle);
    if (handle->beginTransaction()) {
        return true;
    }
    setThreadedError(handle->getError());
    return false;
}

bool Database::commitOrRollbackTransaction()
{
    RecyclableHandle handle = getHandle();
    WCTRemedialAssert(handle != nullptr,
                      "Commit or rollback transaction should not be called without begin.",
                      return false;);
    TransactionGuard transactionedGuard(this, handle);
    if (handle->commitOrRollbackTransaction()) {
        return true;
    }
    setThreadedError(handle->getError());
    return false;
}

void Database::rollbackTransaction()
{
    RecyclableHandle handle = getHandle();
    WCTRemedialAssert(handle != nullptr,
                      "Rollback transaction should not be called without begin.",
                      return;);
    TransactionGuard transactionedGuard(this, handle);
    handle->rollbackTransaction();
}

bool Database::runTransaction(const TransactionCallback &transaction)
{
    if (!beginTransaction()) {
        return false;
    }
    // get threaded handle
    RecyclableHandle handle = getHandle();
    WCTAssert(handle != nullptr);
    if (transaction(handle.get())) {
        return commitOrRollbackTransaction();
    }
    rollbackTransaction();
    return false;
}

bool Database::beginNestedTransaction()
{
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        TransactionGuard transactionedGuard(this, handle);
        if (handle->beginNestedTransaction()) {
            return true;
        }
        setThreadedError(handle->getError());
    }
    return false;
}

bool Database::commitOrRollbackNestedTransaction()
{
    RecyclableHandle handle = getHandle();
    WCTRemedialAssert(handle != nullptr,
                      "Commit or rollback nested transaction should not be called without begin.",
                      return false;);
    TransactionGuard transactionedGuard(this, handle);
    if (handle->commitOrRollbackNestedTransaction()) {
        return true;
    }
    setThreadedError(handle->getError());
    return false;
}

void Database::rollbackNestedTransaction()
{
    RecyclableHandle handle = getHandle();
    WCTRemedialAssert(handle != nullptr,
                      "Rollback nested transaction should not be called without begin.",
                      return;);
    TransactionGuard transactionedGuard(this, handle);
    handle->rollbackNestedTransaction();
}

bool Database::runNestedTransaction(const TransactionCallback &transaction)
{
    if (!beginNestedTransaction()) {
        return false;
    }
    // get threaded handle
    RecyclableHandle handle = getHandle();
    WCTAssert(handle != nullptr);
    if (transaction(handle.get())) {
        return commitOrRollbackNestedTransaction();
    }
    rollbackNestedTransaction();
    return false;
}

#pragma mark - File
bool Database::removeFiles()
{
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

std::optional<size_t> Database::getFilesSize()
{
    auto size = FileManager::getItemsSize(getPaths());
    if (!size.has_value()) {
        assignWithSharedThreadedError();
    }
    return size;
}

bool Database::moveFiles(const UnsafeStringView &directory)
{
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

const StringView &Database::getPath() const
{
    return path;
}

std::list<StringView> Database::getPaths() const
{
    return pathsOfDatabase(path);
}

std::list<StringView> Database::pathsOfDatabase(const UnsafeStringView &database)
{
    return {
        StringView(database),
        Handle::walPathOfDatabase(database),
        Repair::Factory::firstMaterialPathForDatabase(database),
        Repair::Factory::lastMaterialPathForDatabase(database),
        Repair::Factory::factoryPathForDatabase(database),
        Handle::journalPathOfDatabase(database),
        Handle::shmPathOfDatabase(database),
    };
}

#pragma mark - Repair
void Database::filterBackup(const BackupFilter &tableShouldBeBackedup)
{
    LockGuard memoryGuard(m_memory);
    m_factory.filter(tableShouldBeBackedup);
}

bool Database::backup(bool autoInitialize)
{
    InitializedGuard initializedGuard = autoInitialize ? initialize() : isInitialized();
    if (!initializedGuard.valid()) {
        return autoInitialize ? false : true; // mark as succeed if it's not an auto initialize action.
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

    WCTAssert(backupReadHandle.get() != backupWriteHandle.get());

    OperationHandle *operationBackupReadHandle
    = static_cast<OperationHandle *>(backupReadHandle.get());
    OperationHandle *operationBackupWriteHandle
    = static_cast<OperationHandle *>(backupWriteHandle.get());
    if (!autoInitialize) {
        if (m_closing != 0) {
            Error error;
            error.level = Error::Level::Ignore;
            error.setSystemCode(errno, Error::Code::Interrupt);
            Notifier::shared().notify(error);
            setThreadedError(std::move(error));
            return false;
        }
        operationBackupReadHandle->markErrorAsIgnorable(Error::Code::Interrupt);
        operationBackupWriteHandle->markErrorAsIgnorable(Error::Code::Interrupt);
        operationBackupReadHandle->markAsCanBeSuspended(true);
        operationBackupWriteHandle->markAsCanBeSuspended(true);
    }
    Repair::FactoryBackup backup = m_factory.backup();
    backup.setBackupSharedDelegate(
    static_cast<OperationHandle *>(backupReadHandle.get()));
    backup.setBackupExclusiveDelegate(
    static_cast<OperationHandle *>(backupWriteHandle.get()));
    bool succeed = backup.work(getPath());
    if (!succeed) {
        if (backup.getError().level == Error::Level::Ignore) {
            succeed = true;
        } else {
            setThreadedError(backup.getError());
        }
    }
    if (!autoInitialize) {
        operationBackupWriteHandle->markAsCanBeSuspended(false);
        operationBackupReadHandle->markAsCanBeSuspended(false);
        operationBackupWriteHandle->markErrorAsUnignorable();
        operationBackupReadHandle->markErrorAsUnignorable();
    }
    return succeed;
}

bool Database::deposit()
{
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
        WCTAssert(backupReadHandle.get() != backupWriteHandle.get());
        WCTAssert(backupReadHandle.get() != assemblerHandle.get());
        WCTAssert(backupWriteHandle.get() != assemblerHandle.get());

        WCTAssert(!backupReadHandle->isOpened());
        WCTAssert(!backupWriteHandle->isOpened());
        WCTAssert(!assemblerHandle->isOpened());

        Repair::FactoryRenewer renewer = m_factory.renewer();
        renewer.setBackupSharedDelegate(
        static_cast<AssembleHandle *>(backupReadHandle.get()));
        renewer.setBackupExclusiveDelegate(
        static_cast<AssembleHandle *>(backupWriteHandle.get()));
        renewer.setAssembleDelegate(
        static_cast<AssembleHandle *>(assemblerHandle.get()));
        // Prepare a new database from material at renew directory and wait for moving
        if (!renewer.prepare()) {
            setThreadedError(renewer.getError());
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
            return;
        }
        result = true;
    });
    return result;
}

double Database::retrieve(const RetrieveProgressCallback &onProgressUpdated)
{
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
        WCTAssert(backupReadHandle.get() != backupWriteHandle.get());
        WCTAssert(backupReadHandle.get() != assemblerHandle.get());
        WCTAssert(backupWriteHandle.get() != assemblerHandle.get());

        WCTAssert(!backupReadHandle->isOpened());
        WCTAssert(!backupWriteHandle->isOpened());
        WCTAssert(!assemblerHandle->isOpened());

        Repair::FactoryRetriever retriever = m_factory.retriever();
        retriever.setBackupSharedDelegate(
        static_cast<AssembleHandle *>(backupReadHandle.get()));
        retriever.setBackupExclusiveDelegate(
        static_cast<AssembleHandle *>(backupWriteHandle.get()));
        retriever.setAssembleDelegate(
        static_cast<AssembleHandle *>(assemblerHandle.get()));
        retriever.setProgressCallback(onProgressUpdated);
        if (retriever.work()) {
            result = retriever.getScore().value();
        }
        setThreadedError(retriever.getError()); // retriever may have non-critical error even if it succeeds.
    });
    return result;
}

bool Database::containsDeposited() const
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    return m_factory.containsDeposited();
}

bool Database::removeDeposited()
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

bool Database::removeMaterials()
{
    bool result = false;
    close([&result, this]() {
        result = FileManager::removeItems(
        { Repair::Factory::firstMaterialPathForDatabase(path),
          Repair::Factory::lastMaterialPathForDatabase(path) });
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

void Database::checkIntegrity(bool autoInitialize)
{
    InitializedGuard initializedGuard = autoInitialize ? initialize() : isInitialized();
    if (!initializedGuard.valid()) {
        return; // mark as succeed if it's not an auto initialize action.
    }
    RecyclableHandle handle = flowOut(HandleType::Integrity);
    if (handle != nullptr) {
        WCTAssert(dynamic_cast<OperationHandle *>(handle.get()) != nullptr);
        OperationHandle *operationHandle = static_cast<OperationHandle *>(handle.get());
        if (!autoInitialize) {
            if (m_closing != 0) {
                Error error;
                error.level = Error::Level::Ignore;
                error.setSystemCode(errno, Error::Code::Interrupt);
                Notifier::shared().notify(error);
                setThreadedError(std::move(error));
                return;
            }

            operationHandle->markErrorAsIgnorable(Error::Code::Interrupt);
            operationHandle->markAsCanBeSuspended(true);
        }
        operationHandle->checkIntegrity();
        if (!autoInitialize) {
            operationHandle->markAsCanBeSuspended(false);
            operationHandle->markErrorAsUnignorable();
        }
    }
}

#pragma mark - Migration
std::optional<bool> Database::stepMigration(bool autoInitialize)
{
    InitializedGuard initializedGuard = autoInitialize ? initialize() : isInitialized();
    if (!initializedGuard.valid()) {
        return autoInitialize ? std::optional<bool>(nullptr) :
                                std::optional<bool>(false); // mark as succeed if it's not an auto initialize action.
    }
    WCTRemedialAssert(
    !isInTransaction(), "Migrating can't be run in transaction.", return std::nullopt;);
    WCTRemedialAssert(m_migration.shouldMigrate(),
                      "It's not configured for migration.",
                      return std::nullopt;);
    std::optional<bool> done;
    RecyclableHandle handle = flowOut(HandleType::Migrate);
    if (handle != nullptr) {
        WCTAssert(dynamic_cast<MigrateHandle *>(handle.get()) != nullptr);
        MigrateHandle *migrateHandle = static_cast<MigrateHandle *>(handle.get());

        if (!autoInitialize) {
            if (m_closing != 0) {
                Error error;
                error.level = Error::Level::Ignore;
                error.setSystemCode(errno, Error::Code::Interrupt);
                Notifier::shared().notify(error);
                setThreadedError(std::move(error));
                return nullptr;
            }
            migrateHandle->markErrorAsIgnorable(Error::Code::Interrupt);
            migrateHandle->markAsCanBeSuspended(true);
        }
        migrateHandle->markErrorAsIgnorable(Error::Code::Busy);
        done = m_migration.step(*migrateHandle);
        if (!done.has_value() && handle->getError().isIgnorable()) {
            done = false;
        }
        migrateHandle->markErrorAsUnignorable();
        if (!autoInitialize) {
            migrateHandle->markAsCanBeSuspended(false);
            migrateHandle->markErrorAsUnignorable();
        }
    }
    return done;
}

void Database::didMigrate(const MigrationBaseInfo *info)
{
    SharedLockGuard lockGuard(m_memory);
    if (m_migratedCallback != nullptr) {
        m_migratedCallback(this, info);
    }
}

void Database::setNotificationWhenMigrated(const MigratedCallback &callback)
{
    LockGuard lockGuard(m_memory);
    m_migratedCallback = callback;
}

void Database::filterMigration(const MigrationFilter &filter)
{
    close(std::bind(&Migration::filterTable, &m_migration, filter));
}

bool Database::isMigrated() const
{
    return m_migration.isMigrated();
}

std::set<StringView> Database::getPathsOfSourceDatabases() const
{
    return m_migration.getPathsOfSourceDatabases();
}

#pragma mark - Checkpoint
bool Database::checkpoint(bool autoInitialize)
{
    InitializedGuard initializedGuard = autoInitialize ? initialize() : isInitialized();
    if (!initializedGuard.valid()) {
        return autoInitialize ? false : true; // mark as succeed if it's not an auto initialize action.
    }
    bool succeed = false;
    RecyclableHandle handle = flowOut(HandleType::Checkpoint);
    if (handle != nullptr) {
        if (m_closing != 0) {
            Error error;
            error.level = Error::Level::Ignore;
            error.setSystemCode(errno, Error::Code::Interrupt);
            Notifier::shared().notify(error);
            setThreadedError(std::move(error));
            return false;
        }

        WCTAssert(dynamic_cast<OperationHandle *>(handle.get()) != nullptr);
        OperationHandle *operationHandle = static_cast<OperationHandle *>(handle.get());

        if (!autoInitialize) {
            operationHandle->markErrorAsIgnorable(Error::Code::Interrupt);
            operationHandle->markAsCanBeSuspended(true);
        }
        operationHandle->markErrorAsIgnorable(Error::Code::Busy);
        succeed = operationHandle->checkpoint();
        if (!succeed && operationHandle->getError().isIgnorable()) {
            succeed = true;
        }
        operationHandle->markErrorAsUnignorable();
        if (!autoInitialize) {
            operationHandle->markAsCanBeSuspended(false);
            operationHandle->markErrorAsUnignorable();
        }
    }
    return succeed;
}

} //namespace WCDB
