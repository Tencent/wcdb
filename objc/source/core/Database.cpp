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

Database::Database(const RecyclableHandlePool &pool) : m_pool(pool)
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

Database::Tag Database::getTag() const
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

const Error &Database::getError()
{
    return getThreadedError();
}

void Database::closeAllDatabases()
{
    HandlePools::defaultPools()->drain();
}

#pragma mark - Memory
void Database::purge()
{
    m_pool->purgeFreeHandles();
}

void Database::purgeAllDatabases()
{
    HandlePools::defaultPools()->purge();
}

#pragma mark - Config
void Database::setConfig(const std::shared_ptr<Config> &config)
{
    m_pool->setConfig(config);
}

void Database::removeConfig(const std::string &name)
{
    m_pool->removeConfig(name);
}

void Database::setCipher(const Data &cipher, int pageSize)
{
    m_pool->setConfig(CipherConfig::configWithKey(cipher, pageSize));
}

void Database::setTokenizes(const std::list<std::string> &tokenizeNames)
{
    m_pool->setConfig(TokenizeConfig::configWithName(tokenizeNames));
}

#pragma mark - File
bool Database::removeFiles()
{
    if (isBlockaded() && !isOpened()) {
        std::list<std::string> paths = getPaths();
        paths.reverse();
        if (FileManager::shared()->removeItems(paths)) {
            return true;
        }
        assignWithSharedThreadedError();
    } else {
        setThreadedError(Error(Error::Code::Misuse, "Removing files on an "
                                                    "opened database may cause "
                                                    "undefined results."));
    }
    return false;
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
    if (isBlockaded() && !isOpened()) {
        FileManager *fileManager = FileManager::shared();
        std::list<std::string> paths = getPaths();
        paths.reverse();
        if (fileManager->moveItems(paths, directory)) {
            return true;
        }
        assignWithSharedThreadedError();
    } else {
        setThreadedError(Error(Error::Code::Misuse,
                               "Moving files on an opened "
                               "database may cause undefined results."));
    }
    return false;
}

bool Database::moveFilesToDirectoryWithExtraFiles(
    const std::string &directory, const std::list<std::string> &extraFiles)
{
    if (isBlockaded() && !isOpened()) {
        std::list<std::string> paths = extraFiles;
        std::list<std::string> dbPaths = getPaths();
        dbPaths.reverse();
        paths.insert(paths.end(), dbPaths.begin(), dbPaths.end());
        FileManager *fileManager = FileManager::shared();
        if (fileManager->moveItems(paths, directory)) {
            return true;
        }
        assignWithSharedThreadedError();
    } else {
        setThreadedError(Error(Error::Code::Misuse,
                               "Moving files on an opened "
                               "database may may cause undefined results."));
    }
    return false;
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

std::string Database::getFirstBackupPath() const
{
    return Path::addExtention(getPath(), Database::getFirstBackupSubfix());
}

std::string Database::getLastBackupPath() const
{
    return Path::addExtention(getPath(), Database::getLastBackupSubfix());
}

std::list<std::string> Database::getPaths() const
{
    return {
        getPath(),
        getWALPath(),
        getFirstBackupPath(),
        getLastBackupPath(),
        getMaterialsDirectory(),
        getSHMPath(),
        getJournalPath(),
    };
}

#pragma mark - Repair Kit
const std::string &Database::getFirstBackupSubfix()
{
    static const std::string s_firstBackupSubfix("-first.material");
    return s_firstBackupSubfix;
}

const std::string &Database::getLastBackupSubfix()
{
    static const std::string s_lastBackupSubfix("-last.material");
    return s_lastBackupSubfix;
}

std::pair<bool, std::string> Database::pickUpOldBackup()
{
    return pickUpBackup(true);
}

std::pair<bool, std::string> Database::pickUpNewBackup()
{
    return pickUpBackup(false);
}

std::string Database::getArchiveSubfix(int i)
{
    return "." + std::to_string(i);
}

int Database::findNextAvailableArchiveID()
{
    int i = 1;
    std::string archivedSubfix;
    FileManager *fileManager = FileManager::shared();
    while (true) {
        archivedSubfix = getArchiveSubfix(i);
        auto exists = fileManager->fileExists(
            Path::addExtention(getPath(), archivedSubfix));
        if (!exists.first) {
            assignWithSharedThreadedError();
            return 0;
        }
        if (!exists.second) {
            return i;
        }
        ++i;
    }
}

bool Database::archiveAsMaterial()
{
    if (!isBlockaded() || isOpened()) {
        setThreadedError(Error(
            Error::Code::Misuse,
            "Archiving an opened  database may cause undefined results."));
        return false;
    }

    FileManager *fileManager = FileManager::shared();
    const std::string materialDirectory = getMaterialsDirectory();

    int nextAvailbleID = findNextAvailableArchiveID();
    if (nextAvailbleID == 0) {
        return false;
    }

    // create material directory
    if (!fileManager->createDirectoryWithIntermediateDirectories(
            materialDirectory)) {
        assignWithSharedThreadedError();
        return false;
    }

    //resolve archived paths
    std::string archivedSubfix = getArchiveSubfix(nextAvailbleID);
    std::list<std::pair<std::string, std::string>> pairedPaths;
    for (auto &path : getPaths()) {
        std::string fileName =
            Path::addExtention(Path::getFileName(path), archivedSubfix);
        std::string newPath =
            Path::addComponent(materialDirectory, std::move(fileName));
        pairedPaths.push_back({std::move(path), std::move(newPath)});
    }
    if (!fileManager->moveItems(pairedPaths)) {
        assignWithSharedThreadedError();
        return false;
    }
    return true;
}

std::string Database::getMaterialsDirectory() const
{
    return Path::addComponent(getPath(), ".materials");
}

std::pair<bool, std::string> Database::pickUpBackup(bool old)
{
    FileManager *fileManager = FileManager::shared();

    bool first = false;
    bool result = true;
    std::string firstBackupPath = getFirstBackupPath();
    std::string lastBackupPath = getLastBackupPath();
    do {
        std::pair<bool, bool> exists = fileManager->fileExists(firstBackupPath);
        if (!exists.first) {
            result = false;
            break;
        }
        if (!exists.second) {
            first = old;
            break;
        }

        exists = fileManager->fileExists(lastBackupPath);
        if (!exists.first) {
            result = false;
            break;
        }
        if (!exists.second) {
            first = !old;
            break;
        }

        //get the old one
        auto firstBackupTime =
            fileManager->getFileModifiedTime(firstBackupPath);
        auto lastBackupTime = fileManager->getFileModifiedTime(lastBackupPath);
        if (!firstBackupTime.first || !lastBackupTime.second) {
            result = false;
            break;
        }
        first = (firstBackupTime.second > lastBackupTime.second) ^ old;
    } while (false);

    if (!result) {
        assignWithSharedThreadedError();
        return {false, String::empty()};
    }
    return {true, first ? firstBackupPath : lastBackupPath};
}

bool Database::backup(const BackupFilter &shouldTableBeBackedup)
{
    auto backupPath = pickUpOldBackup();
    if (!backupPath.first) {
        return false;
    }

    Repair::Deconstructor deconstructor(getPath());
    if (shouldTableBeBackedup != nullptr) {
        deconstructor.filter(shouldTableBeBackedup);
    }
    if (!deconstructor.work()) {
        assignWithSharedThreadedError();
        return false;
    }
    Data material = deconstructor.getMaterial().serialize();
    if (material.empty()) {
        assignWithSharedThreadedError();
        return false;
    }

    //TODO cipher
    FileHandle fileHandle(backupPath.second);
    bool result = true;
    if (!fileHandle.open(FileHandle::Mode::ReadWrite) ||
        !fileHandle.write(material.buffer(), 0, material.size())) {
        assignWithSharedThreadedError();
        result = false;
    }
    fileHandle.close();
    return result;
}

bool Database::removeMaterials()
{
    if (FileManager::shared()->removeDirectory(getMaterialsDirectory())) {
        return true;
    }
    assignWithSharedThreadedError();
    return false;
}

bool Database::restore()
{
    if (!isBlockaded() || isOpened()) {
        setThreadedError(Error(
            Error::Code::Misuse,
            "Restoring an opened  database may cause undefined results."));
        return false;
    }
    return false;
}

std::string Database::getRestorePath() const
{
    return Path::addComponent(getMaterialsDirectory(),
                              Path::addExtention(getPath(), ".restore"));
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
        recyclableHandle = m_pool->flowOut();
    }
    return recyclableHandle;
}

RecyclableHandle Database::flowOutThreadedHandle()
{
    ThreadedHandles *threadedHandle = Database::threadedHandles().get();
    const auto iter = threadedHandle->find(m_pool.getHandlePool());
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
    auto iter = threadHandles->find(m_pool.getHandlePool());
    WCTInnerAssert(iter == threadHandles->end() ||
                   recyclableHandle.getHandle() ==
                       iter->second.first.getHandle());
    if (iter == threadHandles->end()) {
        threadHandles->insert({m_pool.getHandlePool(), {recyclableHandle, 1}});
    } else {
        ++iter->second.second;
    }
}

void Database::releaseThreadedHandle() const
{
    std::map<const HandlePool *, std::pair<RecyclableHandle, int>>
        *threadHandles = Database::threadedHandles().get();
    auto iter = threadHandles->find(m_pool.getHandlePool());
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
    return threadedHandle->find(m_pool.getHandlePool()) !=
           threadedHandle->end();
}

#pragma mark - HandlePoolThreadedErrorProne
HandlePool *Database::getHandlePool()
{
    return m_pool.getHandlePool();
}

void Database::assignWithSharedThreadedError()
{
    setThreadedError(std::move(ThreadedErrors::shared()->moveThreadedError()));
}

} //namespace WCDB
