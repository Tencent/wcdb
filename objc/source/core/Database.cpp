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
        new Database(HandlePools::defaultPools()->getExistingPool(tag)));
    if (database && database->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<Database>
Database::databaseWithExistingPath(const std::string &path)
{
    std::shared_ptr<Database> database(
        new Database(HandlePools::defaultPools()->getExistingPool(path)));
    if (database && database->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<Database> Database::databaseWithPath(const std::string &path)
{
    static const HandlePools::Generator *s_generator =
        new HandlePools::Generator(
            [](const std::string &path) -> std::shared_ptr<HandlePool> {
                return std::shared_ptr<HandlePool>(
                    new HandlePool(path, Configs::default_()));
            });
    std::shared_ptr<Database> database(
        new Database(HandlePools::defaultPools()->getPool(path, *s_generator)));
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
    return getHandlePool() != nullptr;
}

#pragma mark - Basic

void Database::setTag(const Tag &tag)
{
    getHandlePool()->setTag(tag);
}

Database::Tag Database::getTag() const
{
    return getHandlePool()->getTag();
}

bool Database::canOpen()
{
    return getHandlePool()->canFlowOut();
}

void Database::close(const ClosedCallback &onClosed)
{
    getHandlePool()->drain(onClosed);
}

void Database::blockade()
{
    getHandlePool()->blockade();
}

bool Database::blockadeUntilDone(const BlockadeCallback &onBlockaded)
{
    return getHandlePool()->blockadeUntilDone(onBlockaded);
}

bool Database::isBlockaded()
{
    return getHandlePool()->isBlockaded();
}

void Database::unblockade()
{
    getHandlePool()->unblockade();
}

bool Database::isOpened() const
{
    return !getHandlePool()->isDrained();
}

void Database::closeAllDatabases()
{
    HandlePools::defaultPools()->drain();
}

#pragma mark - Memory
void Database::purge()
{
    getHandlePool()->purgeFreeHandles();
}

void Database::purgeAllDatabases()
{
    HandlePools::defaultPools()->purge();
}

#pragma mark - Config
void Database::setConfig(const std::shared_ptr<Config> &config)
{
    getHandlePool()->setConfig(config);
}

void Database::removeConfig(const std::string &name)
{
    getHandlePool()->removeConfig(name);
}

void Database::setCipher(const Data &cipher, int pageSize)
{
    getHandlePool()->setConfig(CipherConfig::configWithKey(cipher, pageSize));
}

void Database::setTokenizes(const std::list<std::string> &tokenizeNames)
{
    getHandlePool()->setConfig(TokenizeConfig::configWithName(tokenizeNames));
}

#pragma mark - File
bool Database::removeFiles()
{
    bool result = false;
    close([&result, this]() {
        std::list<std::string> paths = getPaths();
        paths.reverse();
        result = FileManager::shared()->removeItems(paths);
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

std::pair<bool, size_t> Database::getFilesSize()
{
    if (!isBlockaded() || isOpened()) {
        WCTWarning("Getting files size on an opened database may get "
                   "incorrect results.");
    }
    auto pair = FileManager::shared()->getItemsSize(getPaths());
    if (!pair.first) {
        assignWithSharedThreadedError();
    }
    return pair;
}

bool Database::moveFiles(const std::string &directory)
{
    bool result = false;
    close([&result, &directory, this]() {
        std::list<std::string> paths = getPaths();
        paths.reverse();
        result = FileManager::shared()->moveItems(paths, directory);
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

bool Database::moveFilesToDirectoryWithExtraFiles(
    const std::string &directory, const std::list<std::string> &extraFiles)
{
    bool result = false;
    close([&result, &directory, &extraFiles, this]() {
        std::list<std::string> paths = extraFiles;
        std::list<std::string> dbPaths = getPaths();
        dbPaths.reverse();
        paths.insert(paths.end(), dbPaths.begin(), dbPaths.end());
        result = FileManager::shared()->moveItems(paths, directory);
        if (!result) {
            assignWithSharedThreadedError();
        }
    });
    return result;
}

const std::string &Database::getPath() const
{
    return getHandlePool()->path;
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

#pragma mark - Repair Kit
void Database::setCorruptionReaction(CorruptionReaction reaction)
{
    getHandlePool()->attachment.corruption.setReaction(reaction);
}

void Database::setExtraReactionWhenCorrupted(
    const CorruptionExtraReaction &extraReaction)
{
    getHandlePool()->attachment.corruption.setExtraReaction(extraReaction);
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
    return getHandlePool()->attachment.factory.directory;
}

void Database::filterBackup(const BackupFilter &tableShouldBeBackedup)
{
    getHandlePool()->attachment.factory.filter(tableShouldBeBackedup);
}

bool Database::backup(int maxWalFrame)
{
    Repair::FactoryBackup factoryBackup =
        getHandlePool()->attachment.factory.backup();
    if (factoryBackup.work(maxWalFrame)) {
        return true;
    }
    setThreadedError(factoryBackup.getError());
    return false;
}

bool Database::deposit()
{
    bool result = false;
    close([&result, this]() {
        Repair::FactoryDepositor factoryDepositor =
            getHandlePool()->attachment.factory.depositor();
        result = factoryDepositor.work();
        if (!result) {
            setThreadedError(factoryDepositor.getError());
        }
    });
    return result;
}

double Database::retrieve(const RetrieveProgressCallback &onProgressUpdate)
{
    double score = 0;
    close([&score, &onProgressUpdate, this]() {
        Repair::FactoryRetriever factoryRetriever =
            getHandlePool()->attachment.factory.retriever();
        std::shared_ptr<Repair::Assembler> assembler(
            new Repair::SQLiteAssembler);
        factoryRetriever.setAssembler(assembler);
        factoryRetriever.setProgressCallback(onProgressUpdate);
        score = factoryRetriever.work();
        setThreadedError(factoryRetriever.getCriticalError());
    });
    return score;
}

bool Database::canRetrieve() const
{
    return getHandlePool()->attachment.factory.canRetrieve();
}

bool Database::removeFactory()
{
    if (FileManager::shared()->removeDirectory(getFactoryDirectory())) {
        return true;
    }
    assignWithSharedThreadedError();
    return false;
}

#pragma mark - Handle
ThreadLocal<Database::ThreadedHandles> &Database::threadedHandles()
{
    static ThreadLocal<ThreadedHandles> *s_threadedHandles =
        new ThreadLocal<ThreadedHandles>;
    return *s_threadedHandles;
}

RecyclableHandle Database::getHandle()
{
    RecyclableHandle recyclableHandle = flowOutThreadedHandle();
    if (recyclableHandle == nullptr) {
        recyclableHandle = getHandlePool()->flowOut();
    }
    return recyclableHandle;
}

RecyclableHandle Database::flowOutThreadedHandle()
{
    ThreadedHandles *threadedHandle = Database::threadedHandles().get();
    const auto iter = threadedHandle->find(getHandlePool());
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
    return {false, false};
}

void Database::retainThreadedHandle(
    const RecyclableHandle &recyclableHandle) const
{
    std::map<const HandlePool *, std::pair<RecyclableHandle, int>>
        *threadHandles = Database::threadedHandles().get();
    auto iter = threadHandles->find(getHandlePool());
    WCTInnerAssert(iter == threadHandles->end() ||
                   recyclableHandle.getHandle() ==
                       iter->second.first.getHandle());
    if (iter == threadHandles->end()) {
        threadHandles->insert({getHandlePool(), {recyclableHandle, 1}});
    } else {
        ++iter->second.second;
    }
}

void Database::releaseThreadedHandle() const
{
    std::map<const HandlePool *, std::pair<RecyclableHandle, int>>
        *threadHandles = Database::threadedHandles().get();
    auto iter = threadHandles->find(getHandlePool());
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
    ThreadedHandles *threadedHandle = Database::threadedHandles().get();
    return threadedHandle->find(getHandlePool()) != threadedHandle->end();
}

} //namespace WCDB
