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
    drain(onClosed);
}

bool Database::isOpened() const
{
    return !isDrained();
}

#pragma mark - Handle
RecyclableHandle Database::flowOut()
{
    RecyclableHandle handle = nullptr;
    do {
        {
            SharedLockGuard concurrencyGuard(m_concurrency);
            if (aliveHandleCount() > 0) {
                handle = HandlePool::flowOut();
                break;
            }
        }

        // Blocked initialization
        LockGuard concurrencyGuard(m_concurrency);
        LockGuard memoryGuard(m_memory);
        if (aliveHandleCount() > 0) {
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
            // This temporary handle will be dropped since it's dirty.
            MigrationInitializerHandle handle(path);
            if (!m_migration.initialize(handle)) {
                break;
            }
        }
        WCTInnerAssert(aliveHandleCount() == 0);
        // acquire one handle to make pool not empty.
        if (HandlePool::flowOut() == nullptr) {
            break;
        }
    } while (true);
    return handle;
}

RecyclableHandle Database::getHandle()
{
    // additional shared lock is not needed since when it's blocked the threadedHandles is always empty and threaded handles is thread safe.
    ThreadedHandles *threadedHandle = Database::threadedHandles().getOrCreate();
    const auto iter = threadedHandle->find(this);
    if (iter != threadedHandle->end()) {
        WCTInnerAssert(m_concurrency.readSafety());
        return iter->second;
    }
    return flowOut();
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

std::shared_ptr<Handle> Database::generateHandle()
{
    SharedLockGuard memoryGuard(m_memory);
    std::shared_ptr<Handle> handle;
    WCTInnerAssert(m_migration.isInitialized());
    if (m_migration.shouldMigrate()) {
        handle.reset(new MigrationHandle(path));
    } else {
        handle.reset(new Handle(path));
    }
    return handle;
}

bool Database::willConfigureHandle(Handle *handle)
{
    SharedLockGuard memoryGuard(m_memory);
    if (m_migration.shouldMigrate()) {
        rebindMigration(handle);
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
    auto iter = threadedHandles->find(database);
    WCTInnerAssert(iter == threadedHandles->end());
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
    SharedLockGuard concurrencyGuard(m_concurrency); // lock concurrency shared since backup is kind of handle operation
    SharedLockGuard memoryGuard(m_memory);
    // TODO: get backed up frame and update backup queue
    Repair::FactoryBackup backup = m_factory.backup();
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
        Repair::FactoryRenewer renewer = m_factory.renewer();
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
        Repair::FactoryRetriever retriever = m_factory.retriever();
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

bool Database::removeDeposit()
{
    bool result = false;
    close([&result, this]() {
        result = m_factory.removeDeposite();
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
    Repair::FactoryRenewer renewer = m_factory.renewer();
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

bool Database::isCorrupted() const
{
#warning TODO
    return false;
}

#pragma mark - Migration
bool Database::rebindMigration(Handle *handle)
{
    WCTInnerAssert(handle != nullptr);
    WCTInnerAssert(m_migration.shouldMigrate());
    WCTInnerAssert(dynamic_cast<MigrationHandle *>(handle) != nullptr);
    std::set<const MigrationInfo *> migratingInfos;
    {
        SharedLockGuard memoryGuard(m_memory);
        migratingInfos = m_migration.getMigratingInfos();
    }
    return static_cast<MigrationHandle *>(handle)->rebindMigration(migratingInfos);
}

void Database::addMigrationInfo(const MigrationUserInfo &userInfo)
{
    LockGuard memoryGuard(m_memory);
    WCTRemedialAssert(aliveHandleCount() == 0,
                      "Migration method must be set before the very first operation.",
                      return;);
    m_migration.addUserInfo(userInfo);
}

void Database::filterMigration(const MigrationTableFilter &filter)
{
    LockGuard memoryGuard(m_memory);
    WCTRemedialAssert(aliveHandleCount() == 0,
                      "Migration user info must be set before the very first operation.",
                      return;);
    m_migration.filterTable(filter);
}

void Database::asyncMigration()
{
#warning TODO
}

void Database::stepMigration()
{
#warning TODO
}

} //namespace WCDB
