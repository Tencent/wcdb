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

#include <WCDB/AssemblerHandle.hpp>
#include <WCDB/Assertion.hpp>
#include <WCDB/BackupHandle.hpp>
#include <WCDB/Database.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/MigrationHandle.hpp>
#include <WCDB/MigrationInitializerHandle.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/RepairKit.h>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initializer
Database::Database(const String &path)
: HandlePool(path)
, m_factory(path)
, m_tag(Tag::invalid())
, m_recoveryMode(RecoveryMode::Custom)
, m_recoverNotification(nullptr)
, m_initialized(false)
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
        // When migration associated is not set, the initialize state is default to true
        if (!m_migration.isInitialized()) {
            WCTInnerAssert(m_concurrency.readSafety());
            {
                // This temporary handle will be dropped since it's dirty.
                RecyclableHandle handle = flowOut((Slot) MigrationInitializerSlot);
                WCTInnerAssert(
                dynamic_cast<MigrationInitializerHandle *>(handle.get()) != nullptr);
                MigrationInitializerHandle *initializer
                = static_cast<MigrationInitializerHandle *>(handle.get());
                succeed = m_migration.initialize(*initializer);
            }
            purge();
            WCTInnerAssert(aliveHandleCount() == 0); // check it's already purged.
            if (!succeed) {
                break;
            }
        }
        m_initialized = true;
    } while (true);
    return nullptr;
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
    return flowOut(m_migration.shouldMigrate() ? (Slot) MigrationHandleSlot : (Slot) HandleSlot);
}

RecyclableHandle Database::getSlotHandle(const Slot &slot)
{
    WCTInnerAssert(slot != MigrationHandleSlot && slot != HandleSlot);
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

std::shared_ptr<Handle> Database::generateHandle(const Slot &slot)
{
    std::shared_ptr<Handle> handle;
    bool open = false;
    switch (slot) {
    case MigrationHandleSlot:
        WCTInnerAssert(m_migration.isInitialized());
        handle.reset(new MigrationHandle(m_migration));
        open = true;
        break;
    case MigrationInitializerSlot:
        WCTInnerAssert(!m_migration.isInitialized());
        handle.reset(new MigrationInitializerHandle);
        open = true;
        break;
    case MigrationStepperSlot:
#warning TODO
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
        WCTInnerAssert(slot == HandleSlot);
        handle.reset(new Handle);
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

bool Database::willConfigureHandle(const Slot &slot, ConfiguredHandle *handle)
{
    bool succeed = true;
    switch (slot) {
    case HandleSlot:
        succeed = HandlePool::willConfigureHandle(slot, handle);
        break;
    case MigrationHandleSlot:
        WCTInnerAssert(dynamic_cast<MigrationHandle *>(handle->get()) != nullptr);
        if (m_migration.shouldMigrate()) {
            if (!static_cast<MigrationHandle *>(handle->get())->rebindMigration()) {
                succeed = false;
                break;
            }
        }
        succeed = HandlePool::willConfigureHandle(slot, handle);
        break;
    default:
        break;
    }
    return true;
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
std::pair<bool, uint32_t> Database::getIdentifier()
{
    auto result = FileManager::getFileIdentifier(path);
    if (!result.first) {
        assignWithSharedThreadedError();
    }
    return result;
}

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
    RecyclableHandle backupReadHandle = getSlotHandle((Slot) BackupReadSlot);
    if (backupReadHandle == nullptr) {
        return false;
    }

    RecyclableHandle backupWriteHandle = getSlotHandle((Slot) BackupWriteSlot);
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
    bool result = false;
    close([&result, this]() {
        RecyclableHandle backupReadHandle = getSlotHandle((Slot) BackupReadSlot);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = getSlotHandle((Slot) BackupWriteSlot);
        if (backupWriteHandle == nullptr) {
            return;
        }
        RecyclableHandle assemblerHandle = getSlotHandle((Slot) AssemblerSlot);
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
        RecyclableHandle backupReadHandle = getSlotHandle((Slot) BackupReadSlot);
        if (backupReadHandle == nullptr) {
            return;
        }
        RecyclableHandle backupWriteHandle = getSlotHandle((Slot) BackupWriteSlot);
        if (backupWriteHandle == nullptr) {
            return;
        }
        RecyclableHandle assemblerHandle = getSlotHandle((Slot) AssemblerSlot);
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
    SharedLockGuard memoryGuard(m_memory);
    WCTInnerAssert(!isOpened());

    RecyclableHandle backupReadHandle = getSlotHandle((Slot) BackupReadSlot);
    if (backupReadHandle == nullptr) {
        return false;
    }
    RecyclableHandle backupWriteHandle = getSlotHandle((Slot) BackupWriteSlot);
    if (backupWriteHandle == nullptr) {
        return false;
    }
    RecyclableHandle assemblerHandle = getSlotHandle((Slot) AssemblerSlot);
    if (assemblerHandle == nullptr) {
        return false;
    }

    Repair::FactoryRenewer renewer = m_factory.renewer();
    renewer.setReadLocker(static_cast<BackupReadHandle *>(backupReadHandle.get()));
    renewer.setWriteLocker(static_cast<BackupWriteHandle *>(backupWriteHandle.get()));
    renewer.setAssembler(static_cast<AssemblerHandle *>(assemblerHandle.get()));
    if (renewer.work()) {
        return true;
    }
    setThreadedError(renewer.getError());
    return false;
}

#pragma mark - Recovery
void Database::setRecoveryMode(RecoveryMode mode)
{
    LockGuard memoryGuard(m_memory);
    m_recoveryMode = mode;
}

Database::RecoveryMode Database::getRecoverMode() const
{
    SharedLockGuard memoryGuard(m_memory);
    return m_recoveryMode;
}

void Database::setNotificationWhenRecovering(const RecoverNotification &notification)
{
    LockGuard memoryGuard(m_memory);
    m_recoverNotification = notification;
}

bool Database::containsRecoverScheme() const
{
    SharedLockGuard memoryGuard(m_memory);
    return m_recoveryMode != RecoveryMode::Custom || m_recoverNotification != nullptr;
}

bool Database::recover()
{
    bool result = true;
    close([&result, this]() {
        if (!containsRecoverScheme()) {
            return;
        }
        switch (m_recoveryMode) {
        case RecoveryMode::Remove:
            result = removeFiles();
            break;
        case RecoveryMode::Deposit:
            result = deposit();
            break;
        default:
            break;
        }
        if (result && m_recoverNotification != nullptr) {
            result = m_recoverNotification(this);
        }
    });
    return result;
}

#pragma mark - Migration
void Database::setNotificationWhenMigrated(const MigratedCallback &callback)
{
    m_migration.setNotificationWhenMigrated(callback);
}

void Database::filterMigration(const MigrationTableFilter &filter)
{
    WCTRemedialAssert(
    !isOpened(), "Migration user info must be set before the very first operation.", return;);
    m_migration.filterTable(filter);
}

} //namespace WCDB
