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
#include <WCDB/Core.h>
#include <WCDB/Error.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/RepairKit.h>
#include <WCDB/String.hpp>

namespace WCDB {

#pragma mark - Initializer
std::shared_ptr<Database> Database::databaseWithExistingTag(const Tag &tag)
{
    std::shared_ptr<Database> database(
    new Database(Core::handlePools()->getExistingPool(tag)));
    if (database && database->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<Database> Database::databaseWithExistingPath(const std::string &path)
{
    std::shared_ptr<Database> database(
    new Database(Core::handlePools()->getExistingPool(path)));
    if (database && database->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<HandlePool> Database::generateHandlePool(const std::string &path)
{
    std::shared_ptr<HandlePool> handlePool(new HandlePool(path, Core::configs()));
    if (handlePool) {
        handlePool->setInitializeNotification(Database::initializeHandlePool);
    }
    return handlePool;
}

bool Database::initializeHandlePool(const HandlePool &handlePool)
{
    std::shared_ptr<Database> database
    = Database::databaseWithExistingPath(handlePool.path);
    WCTInnerAssert(database != nullptr);
    return database->retrieveRenewed();
}

std::shared_ptr<Database> Database::databaseWithPath(const std::string &path)
{
    std::shared_ptr<Database> database(new Database(
    Core::handlePools()->getOrGeneratePool(path, Database::generateHandlePool)));
    if (database && database->isValid()) {
        return database;
    }
    return nullptr;
}

Database::Database(const RecyclableHandlePool &pool) : HandlePoolHolder(pool)
{
}

bool Database::isValid() const
{
    return m_pool != nullptr;
}

#pragma mark - Basic

void Database::setTag(const Tag &tag)
{
    m_pool->setTag(tag);
}

Tag Database::getTag() const
{
    return m_pool->getTag();
}

bool Database::canOpen()
{
    return m_pool->canFlowOut();
}

void Database::close(const ClosedCallback &onClosed)
{
    m_pool->drain(onClosed);
}

void Database::blockade()
{
    m_pool->blockade();
}

bool Database::blockadeUntilDone(const BlockadeCallback &onBlockaded)
{
    return m_pool->blockadeUntilDone(onBlockaded);
}

bool Database::isBlockaded()
{
    return m_pool->isBlockaded();
}

void Database::unblockade()
{
    m_pool->unblockade();
}

bool Database::isOpened() const
{
    return !m_pool->isDrained();
}

#pragma mark - Memory
void Database::purge()
{
    m_pool->purgeFreeHandles();
}

void Database::purgeAllDatabases()
{
    Core::handlePools()->purge();
}

#pragma mark - Config
void Database::setConfig(const std::string &name, const std::shared_ptr<Config> &config, int priority)
{
    m_pool->setConfig(name, config, priority);
}

void Database::removeConfig(const std::string &name)
{
    m_pool->removeConfig(name);
}

#pragma mark - File
bool Database::removeFiles()
{
    bool result = false;
    close([&result, this]() {
        m_pool->attachment.corruption.markAsHandling();
        std::list<std::string> paths = getPaths();
        paths.reverse();
        result = FileManager::removeItems(paths);
        if (!result) {
            assignWithSharedThreadedError();
        }
        m_pool->attachment.corruption.markAsHandled(result);
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

bool Database::moveFiles(const std::string &directory)
{
    bool result = false;
    close([&result, &directory, this]() {
        m_pool->attachment.corruption.markAsHandling();
        std::list<std::string> paths = getPaths();
        paths.reverse();
        result = FileManager::moveItems(paths, directory);
        if (!result) {
            assignWithSharedThreadedError();
        }
        m_pool->attachment.corruption.markAsHandled(result);
    });
    return result;
}

bool Database::moveFilesToDirectoryWithExtraFiles(const std::string &directory,
                                                  const std::list<std::string> &extraFiles)
{
    bool result = false;
    close([&result, &directory, &extraFiles, this]() {
        m_pool->attachment.corruption.markAsHandling();
        std::list<std::string> paths = extraFiles;
        std::list<std::string> dbPaths = getPaths();
        dbPaths.reverse();
        paths.insert(paths.end(), dbPaths.begin(), dbPaths.end());
        result = FileManager::moveItems(paths, directory);
        if (!result) {
            assignWithSharedThreadedError();
        }
        m_pool->attachment.corruption.markAsHandled(result);
    });
    return result;
}

const std::string &Database::getPath() const
{
    return m_pool->path;
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
void Database::setReactionWhenCorrupted(CorruptionReaction reaction)
{
    m_pool->attachment.corruption.setReaction(reaction);
}

Database::CorruptionReaction Database::getReactionWhenCorrupted() const
{
    return m_pool->attachment.corruption.getReaction();
}

void Database::setExtraReactionWhenCorrupted(const CorruptionExtraReaction &extraReaction)
{
    m_pool->attachment.corruption.setExtraReaction(extraReaction);
}

bool Database::isCorrupted() const
{
    return m_pool->attachment.corruption.isCorrupted();
}

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
    return m_pool->attachment.factory.directory;
}

void Database::autoBackup(bool flag)
{
    if (flag) {
        setConfig(Core::BackupConfigName, Core::backupConfig(), Configs::Priority::Low);
    } else {
        removeConfig(Core::BackupConfigName);
    }
}

void Database::filterBackup(const BackupFilter &tableShouldBeBackedup)
{
    m_pool->attachment.factory.filter(tableShouldBeBackedup);
}

bool Database::backup(int maxWalFrame)
{
    Repair::FactoryBackup backup = m_pool->attachment.factory.backup();
    std::shared_ptr<Repair::Locker> locker(new Repair::SQLiteLocker);
    backup.setLocker(locker);
    if (backup.work(getPath(), maxWalFrame)) {
        return true;
    }
    setThreadedError(backup.getError());
    return false;
}

bool Database::deposit()
{
    bool result = false;
    close([&result, this]() {
        m_pool->attachment.corruption.markAsHandling();
        Repair::FactoryRenewer renewer = m_pool->attachment.factory.renewer();
        std::shared_ptr<Repair::Assembler> assembler(new Repair::SQLiteAssembler);
        renewer.setAssembler(assembler);
        std::shared_ptr<Repair::Locker> locker(new Repair::SQLiteLocker);
        renewer.setLocker(locker);
        do {
            if (!renewer.prepare()) {
                setThreadedError(renewer.getError());
                break;
            }
            Repair::FactoryDepositor depositor = m_pool->attachment.factory.depositor();
            if (!depositor.work()) {
                setThreadedError(depositor.getError());
                break;
            }
            if (!renewer.work()) {
                setThreadedError(renewer.getError());
                break;
            }
            result = true;
        } while (false);
        m_pool->attachment.corruption.markAsHandled(result);
    });
    return result;
}

double Database::retrieve(const RetrieveProgressCallback &onProgressUpdate)
{
    double score = 0;
    close([&score, &onProgressUpdate, this]() {
        m_pool->attachment.corruption.markAsHandling();
        Repair::FactoryRetriever retriever = m_pool->attachment.factory.retriever();
        std::shared_ptr<Repair::Assembler> assembler(new Repair::SQLiteAssembler);
        retriever.setAssembler(assembler);
        std::shared_ptr<Repair::Locker> locker(new Repair::SQLiteLocker);
        retriever.setLocker(locker);
        retriever.setProgressCallback(onProgressUpdate);
        bool result = retriever.work();
        setThreadedError(retriever.getError());
        score = result ? retriever.getScore().value() : -1;
        m_pool->attachment.corruption.markAsHandled(result);
    });
    return score;
}

bool Database::canRetrieve() const
{
    return m_pool->attachment.factory.canRetrieve();
}

bool Database::removeDeposit()
{
    if (m_pool->attachment.factory.removeDeposite()) {
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
    WCTInnerAssert(!isOpened());
    Repair::FactoryRenewer renewer = m_pool->attachment.factory.renewer();
    if (renewer.work()) {
        return true;
    }
    setThreadedError(renewer.getError());
    return false;
}

#pragma mark - Handle
ThreadLocal<Database::ThreadedHandles> &Database::threadedHandles()
{
    static ThreadLocal<ThreadedHandles> *s_threadedHandles
    = new ThreadLocal<ThreadedHandles>;
    return *s_threadedHandles;
}

RecyclableHandle Database::getHandle()
{
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    if (recyclableHandle == nullptr) {
        recyclableHandle = m_pool->flowOut();
    }
    return recyclableHandle;
}

RecyclableHandle Database::flowOutThreadedHandle()
{
    ThreadedHandles *threadedHandle = Database::threadedHandles().getOrCreate();
    const auto iter = threadedHandle->find(m_pool);
    if (iter != threadedHandle->end()) {
        return iter->second.first;
    }
    return nullptr;
}

bool Database::execute(const Statement &statement)
{
    RecyclableHandle handle = getHandle();
    if (handle != nullptr) {
        if (statement.getStatementType() == Statement::Type::Rollback) {
            releaseThreadedHandle();
        }
        if (handle->execute(statement)) {
            switch (statement.getStatementType()) {
            case Statement::Type::Begin:
            case Statement::Type::Savepoint:
                retainThreadedHandle(handle);
                break;
            case Statement::Type::Commit:
                releaseThreadedHandle();
                break;
            default:
                break;
            }
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
        auto pair = handle->tableExists(table);
        if (!pair.first) {
            setThreadedError(handle->getError());
        }
        return pair;
    }
    return { false, false };
}

void Database::retainThreadedHandle(const RecyclableHandle &recyclableHandle) const
{
    std::map<const HandlePool *, std::pair<RecyclableHandle, int>> *threadHandles
    = Database::threadedHandles().getOrCreate();
    auto iter = threadHandles->find(m_pool);
    WCTInnerAssert(iter == threadHandles->end()
                   || recyclableHandle.getHandle() == iter->second.first.getHandle());
    if (iter == threadHandles->end()) {
        threadHandles->insert({ m_pool, { recyclableHandle, 1 } });
    } else {
        ++iter->second.second;
    }
}

void Database::releaseThreadedHandle() const
{
    std::map<const HandlePool *, std::pair<RecyclableHandle, int>> *threadHandles
    = Database::threadedHandles().getOrCreate();
    auto iter = threadHandles->find(m_pool);
    WCTInnerAssert(iter != threadHandles->end());
    if (--iter->second.second == 0) {
        threadHandles->erase(iter);
    }
}

#pragma mark - Transaction
bool Database::beginTransaction()
{
    RecyclableHandle recyclableHandle = getHandle();
    if (recyclableHandle == nullptr) {
        return false;
    }

    if (recyclableHandle->beginTransaction()) {
        retainThreadedHandle(recyclableHandle);
        return true;
    }
    return false;
}

bool Database::commitOrRollbackTransaction()
{
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    WCTInnerAssert(recyclableHandle != nullptr);
    bool result = recyclableHandle->commitOrRollbackTransaction();
    releaseThreadedHandle();
    return result;
}

void Database::rollbackTransaction()
{
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    WCTInnerAssert(recyclableHandle != nullptr);
    recyclableHandle->rollbackTransaction();
    releaseThreadedHandle();
}

bool Database::runTransaction(const TransactionCallback &transaction)
{
    if (!beginTransaction()) {
        return false;
    }
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    WCTInnerAssert(recyclableHandle != nullptr);
    if (transaction(recyclableHandle.getHandle())) {
        return commitOrRollbackTransaction();
    }
    rollbackTransaction();
    return false;
}

bool Database::beginNestedTransaction()
{
    if (!isInThreadedTransaction()) {
        return beginTransaction();
    }

    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    WCTInnerAssert(recyclableHandle != nullptr);

    if (recyclableHandle->beginNestedTransaction()) {
        retainThreadedHandle(recyclableHandle);
        return true;
    }
    return false;
}

bool Database::commitOrRollbackNestedTransaction()
{
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    WCTInnerAssert(recyclableHandle != nullptr);

    bool result = recyclableHandle->commitOrRollbackNestedTransaction();
    releaseThreadedHandle();
    return result;
}

void Database::rollbackNestedTransaction()
{
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    WCTInnerAssert(recyclableHandle != nullptr);
    recyclableHandle->rollbackNestedTransaction();
    releaseThreadedHandle();
}

bool Database::runNestedTransaction(const TransactionCallback &transaction)
{
    if (!beginNestedTransaction()) {
        return false;
    }
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    WCTInnerAssert(recyclableHandle != nullptr);
    if (transaction(recyclableHandle.getHandle())) {
        return commitOrRollbackNestedTransaction();
    }
    rollbackNestedTransaction();
    return false;
}

bool Database::isInThreadedTransaction() const
{
    ThreadedHandles *threadedHandle = Database::threadedHandles().getOrCreate();
    return threadedHandle->find(m_pool) != threadedHandle->end();
}

} //namespace WCDB
