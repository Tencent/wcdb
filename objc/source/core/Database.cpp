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

#include <WCDB/BuiltinConfig.hpp>
#include <WCDB/Database.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/File.hpp>
#include <WCDB/Path.hpp>
#include <WCDB/String.hpp>
#include <sqliterk/SQLiteRepairKit.h>

namespace WCDB {

#pragma mark - Initializer
std::shared_ptr<Database> Database::databaseWithExistingTag(const Tag &tag)
{
    std::shared_ptr<Database> database(new Database(tag));
    if (database->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<Database>
Database::databaseWithExistingPath(const std::string &path)
{
    std::shared_ptr<Database> database(new Database(path, true));
    if (database->isValid()) {
        return database;
    }
    return nullptr;
}

std::shared_ptr<Database> Database::databaseWithPath(const std::string &path)
{
    std::shared_ptr<Database> database(new Database(path, false));
    if (database->isValid()) {
        return database;
    }
    return nullptr;
}

#pragma mark - Basic

void Database::setTag(const Tag &tag)
{
    m_pool->tag = tag;
}

Tag Database::getTag() const
{
    return m_pool->tag.load();
}

bool Database::canOpen()
{
    return !m_pool->isDrained() || m_pool->fillOne();
}

void Database::close(const std::function<void(void)> &onClosed)
{
    m_pool->drain(onClosed);
}

void Database::blockade()
{
    m_pool->blockade();
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

const Error &Database::getError() const
{
    return m_pool->getThreadedError();
}

#pragma mark - Memory
void Database::purge()
{
    m_pool->purgeFreeHandles();
}

void Database::PurgeInAllDatabases()
{
    HandlePool::PurgeFreeHandlesInAllPool();
}

#pragma mark - Config
void Database::setConfig(const Config &config)
{
    m_pool->setConfig(config);
}

void Database::setConfig(const std::string &name,
                         const Config::Callback &callback)
{
    m_pool->setConfig(name, callback);
}

void Database::setCipher(const void *key, int keySize, int pageSize)
{
    m_pool->setConfig(BuiltinConfig::cipherWithKey(key, keySize, pageSize));
}

void Database::setTokenizes(const std::list<std::string> &tokenizeNames)
{
    m_pool->setConfig(BuiltinConfig::tokenizeWithNames(tokenizeNames));
}

#pragma mark - File
bool Database::removeFiles()
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning(
            "Removing files on an opened database may cause undefined results");
    }
    if (File::removeFiles(getPaths())) {
        return true;
    }
    m_pool->setAndReportThreadedError(File::getError());
    return false;
}

std::pair<bool, size_t> Database::getFilesSize()
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Getting files size on an opened database may get "
                             "incorrect results");
    }
    auto ret = File::getFilesSize(getPaths());
    if (ret.first) {
        m_pool->resetThreadedError();
    } else {
        m_pool->setAndReportThreadedError(File::getError());
    }
    return ret;
}

bool Database::moveFiles(const std::string &directory)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Moving files on an opened database may cause a "
                             "corrupted database");
    }
    if (File::moveFiles(getPaths(), directory)) {
        return true;
    }
    m_pool->setAndReportThreadedError(File::getError());
    return false;
}

bool Database::moveFilesToDirectoryWithExtraFiles(
    const std::string &directory, const std::list<std::string> &extraFiles)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Moving files on an opened database may cause a "
                             "corrupted database");
    }
    std::list<std::string> paths = getPaths();
    paths.insert(paths.end(), extraFiles.begin(), extraFiles.end());
    if (File::moveFiles(paths, directory)) {
        return true;
    }
    m_pool->setAndReportThreadedError(File::getError());
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

std::string Database::getBackupPath() const
{
    return Path::addExtention(getPath(), Handle::getBackupSubfix());
}

std::list<std::string> Database::getPaths() const
{
    return {getPath(), getWALPath(), getSHMPath(), getJournalPath(),
            getBackupPath()};
}

#pragma mark - Repair Kit
bool Database::backup(const void *key, unsigned int length)
{
    RecyclableHandle handle = flowOut();
    if (handle == nullptr) {
        return false;
    }
    if (!handle->backup(key, length)) {
        m_pool->setAndReportThreadedError(handle->getError());
        return false;
    }
    return true;
}

bool Database::recoverFromPath(const std::string &corruptedDBPath,
                               int pageSize,
                               const void *backupKey,
                               unsigned int backupKeyLength,
                               const void *databaseKey,
                               unsigned int databaseKeyLength)
{
    RecyclableHandle handle = flowOut();
    if (handle == nullptr) {
        return false;
    }
    return handle->recoverFromPath(corruptedDBPath, pageSize, backupKey,
                                   backupKeyLength, databaseKey,
                                   databaseKeyLength);
}

#pragma mark - Handle
RecyclableHandle Database::prepare(const Statement &statement)
{
    RecyclableHandle handle = flowOut();
    if (handle != nullptr) {
        if (handle->prepare(statement)) {
            return handle;
        }
        m_pool->setAndReportThreadedError(handle->getError());
    }
    return nullptr;
}

bool Database::execute(const Statement &statement)
{
    RecyclableHandle handle = flowOut();
    if (handle != nullptr) {
        if (statement.getType() == Statement::Type::Rollback) {
            s_threadedHandle.get()->erase(m_pool);
        }
        if (handle->execute(statement)) {
            switch (statement.getType()) {
                case Statement::Type::Begin:
                    s_threadedHandle.get()->insert({m_pool, handle});
                    break;
                case Statement::Type::Commit:
                    s_threadedHandle.get()->erase(m_pool);
                    break;
                default:
                    break;
            }
            return true;
        }
        m_pool->setAndReportThreadedError(handle->getError());
    }
    return false;
}

std::pair<bool, bool> Database::isTableExists(const std::string &tableName)
{
    RecyclableHandle handle = flowOut();
    if (handle != nullptr) {
        Error::setThreadedSlient(true);
        static const StatementSelect s_statementSelect =
            StatementSelect().select(1).limit(0);
        StatementSelect statementSelect = s_statementSelect;
        statementSelect.from(tableName);
        if (handle->prepare(statementSelect) && handle->step()) {
            m_pool->resetThreadedError();
            return {true, true};
        }
        Error::setThreadedSlient(false);
        const Error &error = handle->getError();
        if (error.getCode() == SQLITE_ERROR) {
            m_pool->resetThreadedError();
            return {true, false};
        }
        m_pool->setAndReportThreadedError(handle->getError());
    }
    return {false, false};
}

#pragma mark - Transaction

bool Database::runNestedTransaction(const TransactionBlock &transaction)
{
    std::unordered_map<const HandlePool *, RecyclableHandle> *threadedHandle =
        s_threadedHandle.get();

    auto iter = threadedHandle->find(m_pool);
    if (iter != threadedHandle->end()) {
        Handle *handle = iter->second.getHandle();
        handle->resetError();
        transaction(handle);
        if (!handle->getError().isOK()) {
            m_pool->setAndReportThreadedError(handle->getError());
            return false;
        }
        return true;
    } else {
        return runTransaction(transaction);
    }
}

bool Database::runControllableTransaction(
    const ControllableTransactionBlock &transaction)
{
    RecyclableHandle recyclableHandle = flowOut();
    if (recyclableHandle == nullptr) {
        return false;
    }

    bool result = false;

    std::unordered_map<const HandlePool *, RecyclableHandle> *threadedHandle =
        s_threadedHandle.get();

    auto iter = threadedHandle->insert({m_pool, recyclableHandle}).first;

    do {
        Handle *handle = recyclableHandle.getHandle();

        if (!handle->execute(StatementBegin::immediate)) {
            m_pool->setAndReportThreadedError(handle->getError());
            break;
        }

        bool commit = transaction(handle);
        handle->resetError();
        if (commit) {
            if (handle->execute(StatementCommit::default_)) {
                result = true;
                break;
            }
            m_pool->setAndReportThreadedError(handle->getError());
        }

        handle->skipError(true);
        handle->execute(StatementRollback::default_);
        handle->skipError(false);
    } while (false);

    threadedHandle->erase(iter);

    return result;
}

bool Database::runTransaction(const TransactionBlock &transaction)
{
    return runControllableTransaction(
        [&transaction, this](Handle *handle) -> bool {
            handle->resetError();
            transaction(handle);
            if (!handle->getError().isOK()) {
                m_pool->setAndReportThreadedError(handle->getError());
                return false;
            }
            return true;
        });
}

#pragma mark - Protected
Database::Database(const std::string &thePath, bool existingOnly)
    : m_recyclablePool(!existingOnly ? HandlePool::GetPool(thePath)
                                     : HandlePool::GetExistingPool(thePath))
{
    m_pool =
        m_recyclablePool != nullptr ? m_recyclablePool.get().get() : nullptr;
}

Database::Database(const Tag &tag)
    : m_recyclablePool(HandlePool::GetExistingPool(tag))
{
    m_pool =
        m_recyclablePool != nullptr ? m_recyclablePool.get().get() : nullptr;
}

bool Database::isValid() const
{
    return m_pool != nullptr;
}

ThreadLocal<std::unordered_map<const HandlePool *, RecyclableHandle>>
    Database::s_threadedHandle;

RecyclableHandle Database::flowOut()
{
    std::unordered_map<const HandlePool *, RecyclableHandle> *threadedHandle =
        s_threadedHandle.get();
    const auto &iter = threadedHandle->find(m_pool);
    if (iter == threadedHandle->end()) {
        return m_pool->flowOut();
    }
    return iter->second;
}

} //namespace WCDB
