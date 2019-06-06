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
#include <WCDB/Path.hpp>
#include <WCDB/RepairKit.h>
#include <WCDB/String.hpp>

#include <WCDB/AssemblerHandle.hpp>
#include <WCDB/BackupHandle.hpp>
#include <WCDB/ConfiguredHandle.hpp>
#include <WCDB/IntegrityHandle.hpp>
#include <WCDB/MigrateHandle.hpp>
#include <WCDB/MigratingHandle.hpp>

namespace WCDB {

#pragma mark - Initializer
Database::Database(const String &path)
: HandlePool(path)
, m_factory(path)
, m_tag(Tag::invalid())
, m_initialized(false)
, m_migratedCallback(nullptr)
, m_migration(this)
{
}

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
    WCTInnerAssert(m_memory.writeSafety());
    WCTInnerAssert(!isOpened());
    m_initialized = false;
}

void Database::close(const ClosedCallback &onClosed)
{
    drain(onClosed);
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
        if (!retrieveRenewed()) {
            break;
        }
        bool succeed, exists;
        std::tie(succeed, exists) = FileManager::fileExists(path);
        if (!succeed) {
            assignWithSharedThreadedError();
            break;
        }
        if (!exists && !FileManager::createFile(path)) {
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

void Database::setConfig(const String &name, const std::shared_ptr<Config> &config, int priority)
{
    LockGuard memoryGuard(m_memory);
    m_configs.insert(name, config, priority);
}

void Database::removeConfig(const String &name)
{
    LockGuard memoryGuard(m_memory);
    m_configs.erase(name);
}

#pragma mark - Handle
RecyclableHandle Database::getHandle()
{
    // Additional shared lock is not needed because the threadedHandles is always empty when it's blocked. So threaded handles is thread safe.
    auto handle = m_transactionedHandles.getOrCreate();
    if (handle->get() != nullptr) {
        WCTInnerAssert(m_concurrency.readSafety());
        return *handle;
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

std::pair<bool, bool> Database::tableExists(const String &table)
{
    bool succeed = false;
    bool exists = false;
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        TransactionGuard transactionedGuard(this, handle);
        std::tie(succeed, exists) = handle->tableExists(table);
        if (!succeed) {
            setThreadedError(handle->getError());
        }
    }
    return { succeed, exists };
}

std::shared_ptr<Handle> Database::generateSlotedHandle(Slot slot)
{
    WCTInnerAssert(slot < HandlePoolNumberOfSlots);
    WCTInnerAssert(m_concurrency.readSafety());
    HandleType type = (HandleType) slot;
    std::shared_ptr<Handle> handle;
    switch (type) {
    case HandleType::Checkpoint:
        handle = std::make_shared<CheckpointHandle>();
        break;
    case HandleType::Migrating:
        handle = std::make_shared<MigratingHandle>(m_migration);
        break;
    case HandleType::Migrate:
        handle = std::make_shared<MigrateHandle>();
        break;
    case HandleType::Backup:
        handle = std::make_shared<BackupHandle>();
        break;
    case HandleType::Assemble:
        handle = std::make_shared<AssemblerHandle>();
        break;
    case HandleType::Integrity:
        handle = std::make_shared<IntegrityHandle>();
        break;
    default:
        WCTInnerAssert(type == HandleType::Normal);
        handle = std::make_shared<ConfiguredHandle>();
        break;
    }
    if (handle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return nullptr;
    }

    if (!setupHandle(slot, handle.get())) {
        return nullptr;
    }

    return handle;
}

bool Database::willReuseSlotedHandle(Slot slot, Handle *handle)
{
    return setupHandle(slot, handle);
}

bool Database::setupHandle(Slot slot, Handle *handle)
{
    WCTInnerAssert(slot < HandlePoolNumberOfSlots);
    WCTInnerAssert(handle != nullptr);

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

    HandleType type = (HandleType) slot;
    if (type == HandleType::Normal || type == HandleType::Migrating || type == HandleType::Migrate
        || type == HandleType::Checkpoint || type == HandleType::Integrity) {
        handle->setPath(path);
        if (!handle->open()) {
            setThreadedError(handle->getError());
            return false;
        }
    } else {
        WCTInnerAssert(!handle->isOpened());
    }

    return true;
}

#pragma mark - Threaded
void Database::markHandleAsTransactioned(const RecyclableHandle &handle)
{
    WCTInnerAssert(m_transactionedHandles.getOrCreate()->get() == nullptr);
    *m_transactionedHandles.getOrCreate() = handle;
    WCTInnerAssert(m_transactionedHandles.getOrCreate()->get() != nullptr);
}

void Database::markHandleAsUntransactioned()
{
    WCTInnerAssert(m_transactionedHandles.getOrCreate()->get() != nullptr);
    *m_transactionedHandles.getOrCreate() = nullptr;
    WCTInnerAssert(m_transactionedHandles.getOrCreate()->get() == nullptr);
}

Database::TransactionGuard::TransactionGuard(Database *database, const RecyclableHandle &handle)
: m_database(database), m_handle(handle), m_isInTransactionBefore(handle->isInTransaction())
{
    WCTInnerAssert(m_database != nullptr);
    WCTInnerAssert(m_handle != nullptr);
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
    WCTInnerAssert(m_transactionedHandles.getOrCreate()->get() == nullptr
                   || m_transactionedHandles.getOrCreate()->get()->isInTransaction());
    return m_transactionedHandles.getOrCreate()->get() != nullptr;
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
    WCTInnerAssert(handle != nullptr);
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
    WCTInnerAssert(handle != nullptr);
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
        std::list<String> paths = getPaths();
        paths.reverse(); // reverse to remove the non-critical paths first avoiding app stopped between the removing
        result = FileManager::removeItems(paths);
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

std::pair<bool, size_t> Database::getFilesSize()
{
    auto pair = FileManager::getItemsSize(getPaths());
    if (!pair.first) {
        assignWithSharedThreadedError();
    }
    return pair;
}

bool Database::moveFiles(const String &directory)
{
    bool result = false;
    close([&result, &directory, this]() {
        std::list<String> paths = getPaths();
        paths.reverse();
        result = FileManager::moveItems(paths, directory);
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

const String &Database::getPath() const
{
    return path;
}

String Database::getSHMPath() const
{
    return Path::addExtention(getPath(), Handle::getSHMSuffix());
}

String Database::getWALPath() const
{
    return Path::addExtention(getPath(), Handle::getWALSuffix());
}

String Database::getJournalPath() const
{
    return Path::addExtention(getPath(), Handle::getJournalSuffix());
}

std::list<String> Database::getPaths() const
{
    return {
        getPath(),
        getWALPath(),
        getFirstMaterialPath(),
        getLastMaterialPath(),
        getFactoryDirectory(),
        getJournalPath(),
        getSHMPath(),
    };
}

#pragma mark - Repair
String Database::getFirstMaterialPath() const
{
    return Repair::Factory::firstMaterialPathForDatabase(getPath());
}

String Database::getLastMaterialPath() const
{
    return Repair::Factory::lastMaterialPathForDatabase(getPath());
}

const String &Database::getFactoryDirectory() const
{
    return m_factory.directory;
}

void Database::filterBackup(const BackupFilter &tableShouldBeBackedup)
{
    LockGuard memoryGuard(m_memory);
    m_factory.filter(tableShouldBeBackedup);
}

bool Database::backup()
{
    InitializedGuard initializedGuard = initialize();
    bool result = false;
    if (initializedGuard.valid()) {
        result = doBackup();
    }
    return result;
}

bool Database::backupIfAlreadyInitialized()
{
    InitializedGuard initializedGuard = isInitialized();
    bool succeed = true;
    if (initializedGuard.valid()) {
        succeed = doBackup();
    }
    return succeed;
}

bool Database::doBackup()
{
    WCTRemedialAssert(
    !isInTransaction(), "Backup can't be run in transaction.", return false;);
    WCTInnerAssert(m_concurrency.readSafety());
    WCTInnerAssert(m_initialized);
    RecyclableHandle backupReadHandle = flowOut(HandleType::Backup);
    if (backupReadHandle == nullptr) {
        return false;
    }

    RecyclableHandle backupWriteHandle = flowOut(HandleType::Backup);
    if (backupWriteHandle == nullptr) {
        return false;
    }

    WCTInnerAssert(backupReadHandle.get() != backupWriteHandle.get());

    // TODO: get backed up frame, update backup queue
    Repair::FactoryBackup backup = m_factory.backup();
    backup.setReadLocker(static_cast<BackupHandle *>(backupReadHandle.get()));
    backup.setWriteLocker(static_cast<BackupHandle *>(backupWriteHandle.get()));
    if (!backup.work(getPath())) {
        setThreadedError(backup.getError());
        return false;
    }
    return true;
}

bool Database::deposit()
{
    bool result = false;
    close([&result, this]() {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return;
        }

        RecyclableHandle backupReadHandle = flowOut(HandleType::Backup);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = flowOut(HandleType::Backup);
        if (backupWriteHandle == nullptr) {
            return;
        }
        WCTInnerAssert(backupReadHandle.get() != backupWriteHandle.get());
        RecyclableHandle assemblerHandle = flowOut(HandleType::Assemble);
        if (assemblerHandle == nullptr) {
            return;
        }

        Repair::FactoryRenewer renewer = m_factory.renewer();
        renewer.setReadLocker(static_cast<BackupHandle *>(backupReadHandle.get()));
        renewer.setWriteLocker(static_cast<BackupHandle *>(backupWriteHandle.get()));
        renewer.setAssembler(static_cast<AssemblerHandle *>(assemblerHandle.get()));
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
        // At next time this database launchs, the retrieveRenewed method will do the remaining work. So data will not lost.
        if (!renewer.work()) {
            setThreadedError(renewer.getError());
            return;
        }
        result = true;
    });
    return result;
}

double Database::retrieve(const RetrieveProgressCallback &onProgressUpdate)
{
    double result = -1;
    close([&result, &onProgressUpdate, this]() {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return;
        }

        RecyclableHandle backupReadHandle = flowOut(HandleType::Backup);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = flowOut(HandleType::Backup);
        if (backupWriteHandle == nullptr) {
            return;
        }
        WCTInnerAssert(backupWriteHandle.get() != backupReadHandle.get());
        RecyclableHandle assemblerHandle = flowOut(HandleType::Assemble);
        if (assemblerHandle == nullptr) {
            return;
        }

        Repair::FactoryRetriever retriever = m_factory.retriever();
        retriever.setReadLocker(static_cast<BackupHandle *>(backupReadHandle.get()));
        retriever.setWriteLocker(static_cast<BackupHandle *>(backupWriteHandle.get()));
        retriever.setAssembler(static_cast<AssemblerHandle *>(assemblerHandle.get()));
        retriever.setProgressCallback(onProgressUpdate);
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
        result
        = FileManager::removeItems({ getFirstMaterialPath(), getLastMaterialPath() });
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

bool Database::retrieveRenewed()
{
    WCTInnerAssert(isBlockaded());
    WCTInnerAssert(!m_initialized);

    Repair::FactoryRenewer renewer = m_factory.renewer();
    if (renewer.work()) {
        return true;
    }
    setThreadedError(renewer.getError());
    return false;
}

void Database::checkIntegrity()
{
    InitializedGuard initializedGuard = initialize();
    if (initializedGuard.valid()) {
        RecyclableHandle handle = flowOut(HandleType::Integrity);
        if (handle != nullptr) {
            WCTInnerAssert(dynamic_cast<IntegrityHandle *>(handle.get()) != nullptr);
            static_cast<IntegrityHandle *>(handle.get())->check();
        }
    }
}

void Database::checkIntegrityIfAlreadyInitialized()
{
    InitializedGuard initializedGuard = isInitialized();
    if (initializedGuard.valid()) {
        RecyclableHandle handle = flowOut(HandleType::Integrity);
        if (handle != nullptr) {
            WCTInnerAssert(dynamic_cast<IntegrityHandle *>(handle.get()) != nullptr);
            static_cast<IntegrityHandle *>(handle.get())->check();
        }
    }
}

#pragma mark - Migration
std::pair<bool, bool> Database::stepMigration()
{
    InitializedGuard initializedGuard = initialize();
    std::pair<bool, bool> result = { false, false };
    if (initializedGuard.valid()) {
        result = doStepMigration();
    }
    return result;
}

std::pair<bool, bool> Database::stepMigrationIfAlreadyInitialized()
{
    InitializedGuard initializedGuard = isInitialized();
    bool succeed = true;
    bool done = false;
    if (initializedGuard.valid()) {
        std::tie(succeed, done) = doStepMigration();
    }
    return { succeed, done };
}

std::pair<bool, bool> Database::doStepMigration()
{
    WCTRemedialAssert(!isInTransaction(),
                      "Migrating can't be run in transaction.",
                      return std::make_pair(false, false););
    WCTInnerAssert(m_concurrency.readSafety());
    WCTRemedialAssert(m_migration.shouldMigrate(),
                      "It's not configured for migration.",
                      return std::make_pair(false, false););
    WCTInnerAssert(m_initialized);
    bool succeed = false;
    bool done = false;

    RecyclableHandle handle = flowOut(HandleType::Migrate);
    if (handle != nullptr) {
        WCTInnerAssert(dynamic_cast<MigrateHandle *>(handle.get()) != nullptr);
        std::tie(succeed, done)
        = m_migration.step(*(static_cast<MigrateHandle *>(handle.get())));
        if (!succeed && handle->isErrorIgnorable()) {
            succeed = true;
        }
    }
    return { succeed, done };
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
    {
        SharedLockGuard lockGuard(m_memory);
        WCTRemedialAssert(!m_initialized,
                          "Migration user info must be set before the very first operation.",
                          return;);
    }
    m_migration.filterTable(filter);
}

bool Database::isMigrated() const
{
    return m_migration.isMigrated();
}

std::set<String> Database::getPathsOfSourceDatabases() const
{
    return m_migration.getPathsOfSourceDatabases();
}

#pragma mark - Checkpoint
bool Database::checkpointIfAlreadyInitialized(CheckpointMode mode)
{
    InitializedGuard initializedGuard = isInitialized();
    bool succeed = false;
    if (initializedGuard.valid()) {
        RecyclableHandle handle = flowOut(HandleType::Checkpoint);
        if (handle != nullptr) {
            WCTInnerAssert(dynamic_cast<CheckpointHandle *>(handle.get()) != nullptr);
            succeed = static_cast<CheckpointHandle *>(handle.get())->checkpoint(mode);
        }
    }
    return succeed;
}

} //namespace WCDB
