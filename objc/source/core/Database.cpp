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

#include <WCDB/Core.h>
#include <WCDB/Error.hpp>
#include <WCDB/FileManager.hpp>
#include <WCDB/Path.hpp>
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

void Database::setCipher(const NoCopyData &cipher, int pageSize)
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
        result = FileManager::shared()->removeFiles(paths);
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
    FileManager *fileManager = FileManager::shared();
    auto pair = fileManager->getFilesSize(getPaths());
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
        result = FileManager::shared()->moveFiles(paths, directory);
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
        result = FileManager::shared()->moveFiles(paths, directory);
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

std::string Database::getBackupPath() const
{
    return Path::addExtention(getPath(), Handle::getBackupSubfix());
}

std::list<std::string> Database::getPaths() const
{
    return {getPath(), getWALPath(), getBackupPath(), getJournalPath(),
            getSHMPath()};
}

#pragma mark - Repair Kit
bool Database::backup(const NoCopyData &data)
{
    RecyclableHandle handle = getHandle();
    if (handle == nullptr) {
        return false;
    }
    if (handle->backup(data)) {
        return true;
    }
    setThreadedError(handle->getError());
    return false;
}

bool Database::recoverFromPath(const std::string &corruptedDBPath,
                               int pageSize,
                               const NoCopyData &backupCipher,
                               const NoCopyData &databaseCipher)
{
    RecyclableHandle handle = getHandle();
    if (handle == nullptr) {
        return false;
    }
    if (handle->recoverFromPath(corruptedDBPath, pageSize, backupCipher,
                                databaseCipher)) {
        return true;
    }
    setThreadedError(handle->getError());
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
    std::unordered_map<const HandlePool *, std::pair<RecyclableHandle, int>>
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
    std::unordered_map<const HandlePool *, std::pair<RecyclableHandle, int>>
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
