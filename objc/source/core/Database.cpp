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
Database::Database(const std::string &path)
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
    m_tag.store(tag);
}

Tag Database::getTag() const
{
    return m_tag.load();
}

bool Database::canOpen()
{
    return canFlowOut();
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
RecyclableHandle Database::getHandle()
{
    do {
        {
            SharedLockGuard lockGuard(m_lock);
            if (m_aliveHandleCount != 0) {
                break;
            }
        }
        LockGuard lockConcurrencyGuard(m_concurrency);
        LockGuard lockGuard(m_lock);
        if (m_aliveHandleCount != 0) {
            break;
        }
        // Blocked initialization
        if (!FileManager::createDirectoryWithIntermediateDirectories(
            Path::getDirectoryName(path))) {
            assignWithSharedThreadedError();
            return nullptr;
        }
        if (!retrieveRenewed()) {
            return nullptr;
        }
        if (m_migration.shouldMigrate()) {
            // This temporary handle will be dropped since it's dirty.
            MigrationInitializerHandle handle;
            handle.setPath(path);
            if (!m_migration.initialize(handle)) {
                return nullptr;
            }
        }
    } while (false);
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    ThreadedHandles *threadedHandle = Database::threadedHandles().getOrCreate();
    const auto iter = threadedHandle->find(this);
    if (iter != threadedHandle->end()) {
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
    SharedLockGuard lockGuard(m_lock);
    std::shared_ptr<Handle> handle;
    if (m_migration.shouldMigrate()) {
        handle.reset(new MigrationHandle(&m_migration));
    } else {
        handle.reset(new Handle);
    }
    return handle;
}

void Database::handleWillFlowBack(Handle *handle)
{
#warning TODO a threaded transaction will never flowback
    WCTRemedialAssert(
    !handle->isInTransaction(), "Unpaired transaction.", handle->rollbackTransaction(););
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
    RecyclableHandle handle = getHandle();
    if (handle == nullptr) {
        return false;
    }
    ThreadedGuard threadedGuard(this, handle);
    if (handle->runTransaction(transaction)) {
        return true;
    }
    setThreadedError(handle->getError());
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
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        ThreadedGuard threadedGuard(this, handle);
        if (handle->runNestedTransaction(transaction)) {
            return true;
        }
        setThreadedError(handle->getError());
    }
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
    LockGuard lockGuard(m_concurrency);
    close(nullptr);
    std::list<std::string> paths = getPaths();
    paths.reverse();
    result = FileManager::removeItems(paths);
    if (!result) {
        assignWithSharedThreadedError();
    }
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

bool Database::moveFiles(const std::string &directory)
{
    LockGuard lockGuard(m_concurrency);
    close(nullptr);
    std::list<std::string> paths = getPaths();
    paths.reverse();
    if (FileManager::moveItems(paths, directory)) {
        return true;
    }
    assignWithSharedThreadedError();
    return false;
}

bool Database::moveFilesToDirectoryWithExtraFiles(const std::string &directory,
                                                  const std::list<std::string> &extraFiles)
{
    LockGuard lockGuard(m_concurrency);
    close(nullptr);
    std::list<std::string> paths = extraFiles;
    std::list<std::string> dbPaths = getPaths();
    dbPaths.reverse();
    paths.insert(paths.end(), dbPaths.begin(), dbPaths.end());
    if (FileManager::moveItems(paths, directory)) {
        return true;
    }
    assignWithSharedThreadedError();
    return false;
}

const std::string &Database::getPath() const
{
    return path;
}

std::string Database::getSHMPath() const
{
    return Path::addExtention(getPath(), Handle::getSHMSubfix());
}

std::string Database::getWALPath() const
{
    return Path::addExtention(getPath(), Handle::getWALSubfix());
}

std::string Database::getJournalPath() const
{
    return Path::addExtention(getPath(), Handle::getJournalSubfix());
}

std::list<std::string> Database::getPaths() const
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
std::string Database::getFirstMaterialPath() const
{
    return Repair::Factory::firstMaterialPathForDatabase(getPath());
}

std::string Database::getLastMaterialPath() const
{
    return Repair::Factory::lastMaterialPathForDatabase(getPath());
}

const std::string &Database::getFactoryDirectory() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_factory.directory;
}

void Database::filterBackup(const BackupFilter &tableShouldBeBackedup)
{
    LockGuard lockGuard(m_lock);
    m_factory.filter(tableShouldBeBackedup);
}

bool Database::backup()
{
    SharedLockGuard lockConcurrencyGuard(m_concurrency);
    SharedLockGuard lockGuard(m_lock);
    Repair::FactoryBackup backup = m_factory.backup();
    if (backup.work(getPath())) {
        return true;
    }
    setThreadedError(backup.getError());
    return false;
}

bool Database::deposit()
{
    LockGuard lockConcurrencyGuard(m_concurrency);
    SharedLockGuard lockGuard(m_lock);
    close(nullptr);
    Repair::FactoryRenewer renewer = m_factory.renewer();
    // Prepare a new database from material at renew directory and wait for moving
    if (!renewer.prepare()) {
        setThreadedError(renewer.getError());
        return false;
    }
    Repair::FactoryDepositor depositor = m_factory.depositor();
    if (!depositor.work()) {
        setThreadedError(depositor.getError());
        return false;
    }
    // If app stop here, it results that the old database is moved to deposited directory and the renewed one is not moved to the origin directory.
    // At next time this database launchs, the retriveRenewed method will do the remaining work. So data will not lost.
    if (!renewer.work()) {
        setThreadedError(renewer.getError());
        return false;
    }
    return true;
}

double Database::retrieve(const RetrieveProgressCallback &onProgressUpdate)
{
    LockGuard lockConcurrencyGuard(m_concurrency);
    SharedLockGuard lockGuard(m_lock);
    close(nullptr);
    Repair::FactoryRetriever retriever = m_factory.retriever();
    retriever.setProgressCallback(onProgressUpdate);
    bool result = retriever.work();
    setThreadedError(retriever.getError()); // retriever may have non-critical error even if it succeeds.
    return result ? retriever.getScore().value() : -1;
}

bool Database::canRetrieve() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_factory.canRetrieve();
}

bool Database::removeDeposit()
{
    SharedLockGuard lockGuard(m_lock);
    if (m_factory.removeDeposite()) {
        return true;
    }
    assignWithSharedThreadedError();
    return false;
}

bool Database::removeMaterials()
{
    if (FileManager::removeItems({ getFirstMaterialPath(), getLastMaterialPath() })) {
        return true;
    }
    assignWithSharedThreadedError();
    return false;
}

bool Database::retrieveRenewed()
{
    WCTInnerAssert(isBlockaded());
    SharedLockGuard lockGuard(m_lock);
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
    LockGuard lockGuard(m_lock);
    m_recoveryMode = mode;
}

Database::RecoveryMode Database::getRecoverMode() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_recoveryMode;
}

void Database::setNotificationWhenRecovering(const RecoverNotification &notification)
{
    LockGuard lockGuard(m_lock);
    m_recoverNotification = notification;
}

bool Database::containsRecoverScheme() const
{
    SharedLockGuard lockGuard(m_lock);
    return m_recoveryMode != RecoveryMode::Custom || m_recoverNotification != nullptr;
}

bool Database::recover()
{
    LockGuard lockGuard(m_concurrency);
    if (!containsRecoverScheme()) {
        return true;
    }
    close(nullptr);
    bool succeed = true;
    switch (m_recoveryMode) {
    case RecoveryMode::Remove:
        succeed = removeFiles();
        break;
    case RecoveryMode::Deposit:
        succeed = deposit();
        break;
    default:
        break;
    }
    if (succeed && m_recoverNotification != nullptr) {
        succeed = m_recoverNotification(this);
    }
    return succeed;
}

bool Database::isCorrupted() const
{
#warning TODO
    return false;
}

#pragma mark - Migration
void Database::addMigrationInfo(const MigrationUserInfo &userInfo)
{
    LockGuard lockGuard(m_lock);
    WCTRemedialAssert(m_aliveHandleCount == 0,
                      "Migration method must be set before the very first operation.",
                      return;);
    m_migration.addUserInfo(userInfo);
}

void Database::filterMigration(const MigrationTableFilter &filter)
{
    LockGuard lockGuard(m_lock);
    WCTRemedialAssert(m_aliveHandleCount == 0,
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
