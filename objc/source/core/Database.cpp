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
#include <WCDB/MigrationHandle.hpp>
#include <WCDB/MigrationStepperHandle.hpp>

namespace WCDB {

#pragma mark - Initializer
Database::Database(const String &path)
: HandlePool(path)
, m_factory(path)
, m_tag(Tag::invalid())
, m_recoverNotification(nullptr)
, m_initialized(false)
, m_configs(nullptr)
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

void Database::close(const ClosedCallback &onClosed)
{
    drain([&onClosed, this]() {
        if (onClosed) {
            onClosed();
        }
        m_initialized = false;
    });
}

bool Database::isOpened() const
{
    return aliveHandleCount() > 0;
}

Database::InitializedGuard Database::initialize()
{
    do {
        {
            SharedLockGuard concurrencyGuard(m_concurrency);
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
    // additional shared lock is not needed since when it's blocked the threadedHandles is always empty and threaded handles is thread safe.
    ThreadedHandles *threadedHandle = Database::threadedHandles().getOrCreate();
    const auto iter = threadedHandle->find(this);
    if (iter != threadedHandle->end()) {
        WCTInnerAssert(m_concurrency.readSafety());
        return iter->second;
    }
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return nullptr;
    }
    // interrupt migration to do the real operation
    interruptMigration();
    return flowOut(m_migration.shouldMigrate() ? MigrationHandleSlot : ConfiguredHandleSlot);
}

RecyclableHandle Database::getSlotHandle(Slot slot)
{
    WCTInnerAssert(slot != MigrationHandleSlot && slot != ConfiguredHandleSlot);
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
        ThreadedGuard threadedGuard(this, handle);
        if (handle->execute(statement)) {
            return true;
        }
        setThreadedError(handle->getError());
    }
    return false;
}

std::pair<bool, bool> Database::tableExists(const TableOrSubquery &table)
{
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        ThreadedGuard threadedGuard(this, handle);
        auto pair = handle->tableExists(table);
        if (!pair.first) {
            setThreadedError(handle->getError());
        }
        return pair;
    }
    return { false, false };
}

std::shared_ptr<Handle> Database::generateHandle(Slot slot)
{
    std::shared_ptr<Handle> handle;
    bool open = false;
    switch (slot) {
    case MigrationHandleSlot:
        // It's safe since m_migration never change.
        handle.reset(new MigrationHandle(m_migration));
        open = true;
        break;
    case MigrationStepperSlot:
        handle.reset(new MigrationStepperHandle);
        open = true;
        break;
    case BackupReadSlot:
        handle.reset(new BackupReadHandle);
        break;
    case BackupWriteSlot:
        handle.reset(new BackupWriteHandle);
        break;
    case AssemblerSlot:
        handle.reset(new AssemblerHandle);
        break;
    default:
        WCTInnerAssert(slot == ConfiguredHandleSlot);
        handle.reset(new ConfiguredHandle);
        open = true;
        break;
    }
    if (handle == nullptr) {
        setThreadedError(Error(Error::Code::NoMemory));
        return nullptr;
    }
    if (open) {
        handle->setPath(path);
        if (!handle->open()) {
            setThreadedError(handle->getError());
            return nullptr;
        }
    }
    return handle;
}

bool Database::willConfigureHandle(Slot slot, Handle *handle)
{
    bool succeed = true;
    switch (slot) {
    case MigrationHandleSlot:
    case ConfiguredHandleSlot:
    case MigrationStepperSlot: {
        std::shared_ptr<Configs> configs;
        {
            SharedLockGuard memoryGuard(m_memory);
            configs = m_configs;
        }
        WCTInnerAssert(dynamic_cast<ConfiguredHandle *>(handle) != nullptr);
        if (!static_cast<ConfiguredHandle *>(handle)->reconfigure(configs)) {
            succeed = false;
            setThreadedError(handle->getError());
            break;
        }
    } break;
    default:
        break;
    }
    return succeed;
}

#pragma mark - Threaded
ThreadLocal<Database::ThreadedHandles> &Database::threadedHandles()
{
    static ThreadLocal<ThreadedHandles> *s_threadedHandles
    = new ThreadLocal<ThreadedHandles>;
    return *s_threadedHandles;
}

void Database::markHandleAsThreaded(const Database *database, const RecyclableHandle &handle)
{
    ThreadedHandles *threadedHandles = Database::threadedHandles().getOrCreate();
    WCTInnerAssert(threadedHandles->find(database) == threadedHandles->end());
    threadedHandles->emplace(database, handle);
}

void Database::markHandleAsUnthreaded(const Database *database)
{
    ThreadedHandles *threadedHandles = Database::threadedHandles().getOrCreate();
    WCTInnerAssert(threadedHandles->find(database) != threadedHandles->end());
    threadedHandles->erase(database);
}

Database::ThreadedGuard::ThreadedGuard(const Database *database, const RecyclableHandle &handle)
: m_database(database), m_handle(handle), m_isInTransactionBefore(handle->isInTransaction())
{
    WCTInnerAssert(m_database != nullptr);
    WCTInnerAssert(m_handle != nullptr);
}

Database::ThreadedGuard::~ThreadedGuard()
{
    bool isInTransactionAfter = m_handle->isInTransaction();
    if (!m_isInTransactionBefore && isInTransactionAfter) {
        markHandleAsThreaded(m_database, m_handle);
    } else if (m_isInTransactionBefore && !isInTransactionAfter) {
        markHandleAsUnthreaded(m_database);
    }
}

#pragma mark - Transaction
bool Database::isInTransaction()
{
    ThreadedHandles *threadedHandles = Database::threadedHandles().getOrCreate();
    return threadedHandles->find(this) != threadedHandles->end();
}

bool Database::beginTransaction()
{
    RecyclableHandle handle = getHandle();
    if (handle == nullptr) {
        return false;
    }
    ThreadedGuard threadedGuard(this, handle);
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
    ThreadedGuard threadedGuard(this, handle);
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
    ThreadedGuard threadedGuard(this, handle);
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
        ThreadedGuard threadedGuard(this, handle);
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
    ThreadedGuard threadedGuard(this, handle);
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
    ThreadedGuard threadedGuard(this, handle);
    handle->rollbackNestedTransaction();
}

bool Database::runNestedTransaction(const TransactionCallback &transaction)
{
    if (!beginNestedTransaction()) {
        return false;
    }
    // get threaded handle
    RecyclableHandle handle = getHandle();
    WCTInnerAssert(handle != nullptr) if (transaction(handle.get()))
    {
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
    return Path::addExtention(getPath(), Handle::getSHMSubfix());
}

String Database::getWALPath() const
{
    return Path::addExtention(getPath(), Handle::getWALSubfix());
}

String Database::getJournalPath() const
{
    return Path::addExtention(getPath(), Handle::getJournalSubfix());
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
    SharedLockGuard memoryGuard(m_memory);
    return m_factory.directory;
}

void Database::filterBackup(const BackupFilter &tableShouldBeBackedup)
{
    LockGuard memoryGuard(m_memory);
    m_factory.filter(tableShouldBeBackedup);
}

bool Database::backup()
{
    RecyclableHandle backupReadHandle = getSlotHandle(BackupReadSlot);
    if (backupReadHandle == nullptr) {
        return false;
    }

    RecyclableHandle backupWriteHandle = getSlotHandle(BackupWriteSlot);
    if (backupWriteHandle == nullptr) {
        return false;
    }

    SharedLockGuard memoryGuard(m_memory);
    // TODO: get backed up frame, update backup queue
    Repair::FactoryBackup backup = m_factory.backup();
    backup.setReadLocker(static_cast<BackupReadHandle *>(backupReadHandle.get()));
    backup.setWriteLocker(static_cast<BackupWriteHandle *>(backupWriteHandle.get()));
    if (backup.work(getPath())) {
        return true;
    }
    setThreadedError(backup.getError());
    return false;
}

bool Database::deposit()
{
    {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return false;
        }
    }
    bool result = false;
    close([&result, this]() {
        RecyclableHandle backupReadHandle = getSlotHandle(BackupReadSlot);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = getSlotHandle(BackupWriteSlot);
        if (backupWriteHandle == nullptr) {
            return;
        }
        RecyclableHandle assemblerHandle = getSlotHandle(AssemblerSlot);
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
    {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return 0;
        }
    }
    double result = -1;
    close([&result, &onProgressUpdate, this]() {
        RecyclableHandle backupReadHandle = getSlotHandle(BackupReadSlot);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = getSlotHandle(BackupWriteSlot);
        if (backupWriteHandle == nullptr) {
            return;
        }
        RecyclableHandle assemblerHandle = getSlotHandle(AssemblerSlot);
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
    SharedLockGuard memoryGuard(m_memory);
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

    SharedLockGuard memoryGuard(m_memory);
    Repair::FactoryRenewer renewer = m_factory.renewer();
    if (renewer.work()) {
        return true;
    }
    setThreadedError(renewer.getError());
    return false;
}

#pragma mark - Recovery
void Database::setNotificationWhenCorrupted(const RecoverNotification &notification)
{
    LockGuard memoryGuard(m_memory);
    m_recoverNotification = notification;
}

bool Database::recover(uint32_t corruptedIdentifier)
{
    {
        InitializedGuard initializedGuard = initialize();
        if (!initializedGuard.valid()) {
            return nullptr;
        }
        if (m_recoverNotification == nullptr) {
            return true;
        }
    }
    blockade();
    LockGuard memoryGuard(m_memory);
    bool succeed = true;
    if (m_recoverNotification != nullptr) {
        uint32_t identifier;
        std::tie(succeed, identifier) = FileManager::getFileIdentifier(path);
        if (!succeed) {
            return false;
        }
        if (identifier != corruptedIdentifier) {
            return true;
        }
        succeed = m_recoverNotification(this);
    }
    unblockade();
    return succeed;
}

#pragma mark - Migration
std::pair<bool, bool> Database::stepMigration(bool force)
{
    InitializedGuard initializedGuard = initialize();
    if (!initializedGuard.valid()) {
        return { false, false };
    }
    if (!force
        && (activeHandleCount(ConfiguredHandleSlot) > 0
            || activeHandleCount(MigrationHandleSlot) > 0)) {
        return { true, false };
    }
    RecyclableHandle handle = getSlotHandle(MigrationStepperSlot);
    WCTInnerAssert(dynamic_cast<MigrationStepperHandle *>(handle.get()));
    static_cast<MigrationStepperHandle *>(handle.get())->setInterruptible(!force);
    bool succeed, done;
    std::tie(succeed, done)
    = m_migration.step(*(static_cast<MigrationStepperHandle *>(handle.get())));
    if (!succeed && handle->getResultCode() == (int) Error::Code::Interrupt) {
        succeed = true;
    }
    return { succeed, done };
}

void Database::setNotificationWhenMigrated(const MigratedCallback &callback)
{
    LockGuard lockGuard(m_memory);
    m_migration.setNotificationWhenMigrated(callback);
}

void Database::filterMigration(const MigrationFilter &filter)
{
    LockGuard lockGuard(m_memory);
    WCTRemedialAssert(!m_initialized,
                      "Migration user info must be set before the very first operation.",
                      return;);
    m_migration.filterTable(filter);
}

void Database::interruptMigration()
{
    SharedLockGuard concurrencyGuard(m_concurrency);
    SharedLockGuard memoryGuard(m_memory);
    if (!m_initialized) {
        return;
    }
    for (const auto &handle : getAllHandles(MigrationStepperSlot)) {
        WCTInnerAssert(dynamic_cast<MigrationStepperHandle *>(handle.get()) != nullptr);
        static_cast<MigrationStepperHandle *>(handle.get())->interrupt();
    }
}

} //namespace WCDB
