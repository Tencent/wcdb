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
#include <WCDB/MigrationHandle.hpp>
#include <WCDB/MigrationStepperHandle.hpp>

namespace WCDB {

#pragma mark - Initializer
Database::Database(const String &path)
: HandlePool(path)
, m_factory(path)
, m_tag(Tag::invalid())
, m_initialized(false)
, m_configs(nullptr)
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
void Database::setConfigs(const std::shared_ptr<Configs> &configs)
{
    LockGuard memoryGuard(m_memory);
    m_configs = configs;
}

void Database::setConfig(const String &name, const std::shared_ptr<Config> &config, int priority)
{
    LockGuard memoryGuard(m_memory);
    std::shared_ptr<Configs> configs(new Configs(*m_configs.get()));
    configs->insert(name, config, priority);
    m_configs = configs;
}

void Database::removeConfig(const String &name)
{
    LockGuard memoryGuard(m_memory);
    std::shared_ptr<Configs> configs(new Configs(*m_configs.get()));
    configs->remove(name);
    m_configs = configs;
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
    return flowOut(m_migration.shouldMigrate() ? HandleType::Migrate : HandleType::Normal);
}

RecyclableHandle Database::getSlotHandle(Slot slot)
{
    WCTInnerAssert(slot != HandleType::Migrate);
    WCTInnerAssert(slot != HandleType::Normal);
    WCTInnerAssert(slot < HandlePoolNumberOfSlots);
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return nullptr;
    }
    return flowOut(slot);
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
    return generateHandle((HandleType) slot);
}

void Database::handleWillStep(HandleStatement *handleStatement)
{
    // Interrupt when a write operation will run
    // If already in transaction, it's no need to interrupt since it's already blocked.
    if (!handleStatement->isReadonly() && !handleStatement->getHandle()->isInTransaction()) {
        interruptMigration();
        interruptCheckpoint();
    }
}

bool Database::willReuseSlotedHandle(Slot slot, Handle *handle)
{
    WCTInnerAssert(handle->isOpened());
    WCTInnerAssert(slot < HandlePoolNumberOfSlots);
    WCDB_UNUSED(slot)
    std::shared_ptr<Configs> configs;
    {
        SharedLockGuard memoryGuard(m_memory);
        configs = m_configs;
    }
    bool succeed = handle->reconfigure(configs);
    if (!succeed) {
        setThreadedError(handle->getError());
    }
    return succeed;
}

std::shared_ptr<Handle> Database::generateHandle(HandleType type)
{
    WCTInnerAssert(m_concurrency.readSafety());
    std::shared_ptr<Handle> handle;
    switch (type) {
    case HandleType::InterruptibleCheckpoint:
        handle.reset(new InterruptibleCheckpointHandle);
        break;
    case HandleType::Migrate:
        handle.reset(new MigrationHandle(m_migration));
        break;
    case HandleType::MigrationStep:
        handle.reset(new MigrationStepperHandle);
        break;
    case HandleType::BackupRead:
        handle.reset(new BackupReadHandle);
        break;
    case HandleType::BackupWrite:
        handle.reset(new BackupWriteHandle);
        break;
    case HandleType::Assemble:
        handle.reset(new AssemblerHandle);
        break;
    case HandleType::Integrity:
        handle.reset(new IntegrityHandle);
        break;
    default:
        WCTInnerAssert(type == HandleType::Normal);
        handle.reset(new ConfiguredHandle);
        break;
    }
    if (handle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory, Error::Level::Error));
        return nullptr;
    }

    if (type != HandleType::Integrity) {
        std::shared_ptr<Configs> configs;
        {
            SharedLockGuard memoryGuard(m_memory);
            configs = m_configs;
        }

        // reconfigure
        if (!handle->reconfigure(configs)) {
            setThreadedError(handle->getError());
            return nullptr;
        }
    }

    // open
    if (type == HandleType::Normal || type == HandleType::Migrate
        || type == HandleType::MigrationStep || type == HandleType::InterruptibleCheckpoint
        || type == HandleType::Integrity) {
        handle->setPath(path);
        if (!handle->open()) {
            setThreadedError(handle->getError());
            return nullptr;
        }
    }

    // set stepped notification
    if (type == HandleType::Normal || type == HandleType::Migrate) {
        handle->setNotificationWhenStatementWillStep(
        String::formatted("Interrupt-%p", this),
        std::bind(&Database::handleWillStep, this, std::placeholders::_1));
    }

    return handle;
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
    bool succeed = false;
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
    RecyclableHandle backupReadHandle = flowOut(HandleType::BackupRead);
    if (backupReadHandle == nullptr) {
        return false;
    }

    RecyclableHandle backupWriteHandle = flowOut(HandleType::BackupWrite);
    if (backupWriteHandle == nullptr) {
        return false;
    }

    // TODO: get backed up frame, update backup queue
    Repair::FactoryBackup backup = m_factory.backup();
    backup.setReadLocker(static_cast<BackupReadHandle *>(backupReadHandle.get()));
    backup.setWriteLocker(static_cast<BackupWriteHandle *>(backupWriteHandle.get()));
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

        std::shared_ptr<Handle> backupReadHandle = generateHandle(HandleType::BackupRead);
        if (backupReadHandle == nullptr) {
            return;
        }
        std::shared_ptr<Handle> backupWriteHandle = generateHandle(HandleType::BackupWrite);
        if (backupWriteHandle == nullptr) {
            return;
        }
        std::shared_ptr<Handle> assemblerHandle = generateHandle(HandleType::Assemble);
        if (assemblerHandle == nullptr) {
            return;
        }

        Repair::FactoryRenewer renewer = m_factory.renewer();
        renewer.setReadLocker(static_cast<BackupReadHandle *>(backupReadHandle.get()));
        renewer.setWriteLocker(
        static_cast<BackupWriteHandle *>(backupWriteHandle.get()));
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

        std::shared_ptr<Handle> backupReadHandle = generateHandle(HandleType::BackupRead);
        if (backupReadHandle == nullptr) {
            return;
        }
        std::shared_ptr<Handle> backupWriteHandle = generateHandle(HandleType::BackupWrite);
        if (backupWriteHandle == nullptr) {
            return;
        }
        std::shared_ptr<Handle> assemblerHandle = generateHandle(HandleType::Assemble);
        if (assemblerHandle == nullptr) {
            return;
        }

        Repair::FactoryRetriever retriever = m_factory.retriever();
        retriever.setReadLocker(static_cast<BackupReadHandle *>(backupReadHandle.get()));
        retriever.setWriteLocker(
        static_cast<BackupWriteHandle *>(backupWriteHandle.get()));
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
        std::shared_ptr<Handle> handle = generateHandle(HandleType::Integrity);
        WCTInnerAssert(dynamic_cast<IntegrityHandle *>(handle.get()) != nullptr);
        static_cast<IntegrityHandle *>(handle.get())->check();
    }
}

void Database::checkIntegrityIfAlreadyInitialized()
{
    InitializedGuard initializedGuard = isInitialized();
    if (initializedGuard.valid()) {
        std::shared_ptr<Handle> handle = generateHandle(HandleType::Integrity);
        WCTInnerAssert(dynamic_cast<IntegrityHandle *>(handle.get()) != nullptr);
        static_cast<IntegrityHandle *>(handle.get())->check();
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
    bool succeed = false;
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
                      std::make_pair(false, false););
    WCTInnerAssert(m_concurrency.readSafety());
    WCTInnerAssert(m_initialized);
    bool succeed = false;
    bool done = false;
    RecyclableHandle handle = getSlotHandle(HandleType::MigrationStep);
    if (handle != nullptr) {
        WCTInnerAssert(dynamic_cast<MigrationStepperHandle *>(handle.get()) != nullptr);
        std::tie(succeed, done)
        = m_migration.step(*(static_cast<MigrationStepperHandle *>(handle.get())));
        if (!succeed) {
            if (handle->getResultCode() == Error::Code::Interrupt
                || handle->getResultCode() == Error::Code::Busy) {
                succeed = true;
            }
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

void Database::interruptMigration()
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    if (m_initialized) {
        for (const auto &handle : getHandles(HandleType::MigrationStep)) {
            WCTInnerAssert(dynamic_cast<MigrationStepperHandle *>(handle.get()) != nullptr);
            static_cast<MigrationStepperHandle *>(handle.get())->interrupt();
        }
    }
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
bool Database::interruptibleCheckpointIfAlreadyInitialized(CheckpointMode mode)
{
    InitializedGuard initializedGuard = isInitialized();
    bool succeed = false;
    if (initializedGuard.valid()) {
        RecyclableHandle handle = getSlotHandle(HandleType::InterruptibleCheckpoint);
        if (handle != nullptr) {
            WCTInnerAssert(dynamic_cast<InterruptibleCheckpointHandle *>(handle.get()) != nullptr);
            succeed
            = static_cast<InterruptibleCheckpointHandle *>(handle.get())->checkpoint(mode);
        }
    }
    return succeed;
}

void Database::interruptCheckpoint()
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    if (m_initialized) {
        for (const auto &handle : getHandles(HandleType::InterruptibleCheckpoint)) {
            WCTInnerAssert(dynamic_cast<InterruptibleCheckpointHandle *>(handle.get()) != nullptr);
            static_cast<InterruptibleCheckpointHandle *>(handle.get())->interrupt();
        }
    }
}

} //namespace WCDB
