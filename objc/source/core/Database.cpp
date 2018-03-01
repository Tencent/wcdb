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
#include <WCDB/Utility.hpp>
#include <sqliterk/SQLiteRepairKit.h>

namespace WCDB {

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

Database::Database(const std::string &thePath, bool existingOnly)
    : m_pool(!existingOnly ? HandlePool::GetPool(thePath)
                           : HandlePool::GetExistingPool(thePath))
{
}

Database::Database(const Tag &tag) : m_pool(HandlePool::GetExistingPool(tag))
{
}

const std::string &Database::getPath() const
{
    return m_pool->path;
}

bool Database::canOpen()
{
    Error innerError;
    return !m_pool->isDrained() || m_pool->fillOne(innerError);
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

void Database::purge()
{
    m_pool->purgeFreeHandles();
}

void Database::PurgeInAllDatabases()
{
    HandlePool::PurgeFreeHandlesInAllPool();
}

void Database::setTag(const Tag &tag)
{
    m_pool->tag = tag;
}

int Database::getTag() const
{
    return m_pool->tag.load();
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

void Database::setCipher(const void *key,
                         const int &keySize,
                         const int &pageSize)
{
    m_pool->setConfig(BuiltinConfig::cipherWithKey(key, keySize, pageSize));
}

void Database::setTokenizes(const std::list<std::string> &tokenizeNames)
{
    m_pool->setConfig(BuiltinConfig::tokenizeWithNames(tokenizeNames));
}

#pragma mark - File
bool Database::removeFiles(Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning(
            "Removing files on an opened database may cause unknown results");
    }
    return File::removeFiles(getPaths(), error);
}

size_t Database::getFilesSize(Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Getting files size on an opened database may get "
                             "incorrect results");
    }
    return File::getFilesSize(getPaths(), error);
}

bool Database::moveFiles(const std::string &directory, Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Moving files on an opened database may cause a "
                             "corrupted database");
    }
    return File::moveFiles(getPaths(), directory, error);
}

bool Database::moveFilesToDirectoryWithExtraFiles(
    const std::string &directory,
    const std::list<std::string> &extraFiles,
    Error &error)
{
    if (!isBlockaded() || isOpened()) {
        WCDB::Error::Warning("Moving files on an opened database may cause a "
                             "corrupted database");
    }
    std::list<std::string> paths = getPaths();
    paths.insert(paths.end(), extraFiles.begin(), extraFiles.end());
    return File::moveFiles(paths, directory, error);
}

const std::array<std::string, 5> &Database::subfixs()
{
    static const std::array<std::string, 5> s_subfixs = {
        "", //db file
        "-wal", "-journal", "-shm", Handle::backupSuffix,
    };
    return s_subfixs;
}

const std::list<std::string> Database::getPaths() const
{
    std::list<std::string> paths;
    for (const auto &subfix : Database::subfixs()) {
        paths.push_back(Path::addExtention(getPath(), subfix));
    }
    return paths;
}

#pragma mark - Repair Kit
bool Database::backup(const void *key, const unsigned int &length, Error &error)
{
    RecyclableHandle handle = flowOut(error);
    if (!handle) {
        return false;
    }
    bool result = handle->backup(key, length);
    error = handle->getError();
    return result;
}

bool Database::recoverFromPath(const std::string &corruptedDBPath,
                               const int pageSize,
                               const void *backupKey,
                               const unsigned int &backupKeyLength,
                               const void *databaseKey,
                               const unsigned int &databaseKeyLength,
                               Error &error)
{
    RecyclableHandle handle = flowOut(error);
    if (!handle) {
        return false;
    }
    bool result = handle->recoverFromPath(corruptedDBPath, pageSize, backupKey,
                                          backupKeyLength, databaseKey,
                                          databaseKeyLength);
    error = handle->getError();
    return result;
}

#pragma mark - SQL
RecyclableStatement Database::prepare(const Statement &statement, Error &error)
{
    switch (statement.getType()) {
        case Statement::Type::Begin:
        case Statement::Type::Commit:
        case Statement::Type::Rollback:
            Error::ReportCore(
                getTag(), getPath(), Error::CoreOperation::Prepare,
                Error::CoreCode::Misuse,
                "Using [getTransaction] method to do a transaction", &error);
            return RecyclableStatement(RecyclableHandle(nullptr, nullptr),
                                       nullptr);
        default: {
            RecyclableHandle handle = flowOut(error);
            std::shared_ptr<HandleStatement> handleStatement = nullptr;
            if (handle) {
                handleStatement = handle->prepare(statement);
                error = handle->getError();
            }
            return RecyclableStatement(handle, handleStatement);
        }
    }
}

bool Database::exec(const Statement &statement, Error &error)
{
    switch (statement.getType()) {
        case Statement::Type::Begin:
        case Statement::Type::Commit:
        case Statement::Type::Rollback:
            Error::ReportCore(
                getTag(), getPath(), Error::CoreOperation::Exec,
                Error::CoreCode::Misuse,
                "Using [getTransaction] method to do a transaction", &error);
            return false;
        default: {
            RecyclableHandle handle = flowOut(error);
            bool result = false;
            if (handle) {
                result = handle->exec(statement);
                error = handle->getError();
            }
            return result;
        }
    }
}

bool Database::isTableExists(const std::string &tableName, Error &error)
{
    RecyclableHandle handle = flowOut(error);
    bool result = false;
    if (handle) {
        Error::setThreadedSlient(true);
        StatementSelect select =
            StatementSelect().select(1).from(tableName).limit(0);
        std::shared_ptr<HandleStatement> handleStatement =
            handle->prepare(select);
        Error::setThreadedSlient(false);
        if (handleStatement) {
            handleStatement->step();
            result = handleStatement->isOK();
            if (!result) {
                error = handleStatement->getError();
            }
        } else {
            error = handle->getError();
        }
    }
    return result;
}

#pragma mark - Transaction
ThreadLocal<std::unordered_map<std::string, RecyclableHandle>>
    Database::s_threadedHandle;

RecyclableHandle Database::flowOut(Error &error)
{
    std::unordered_map<std::string, RecyclableHandle> *threadedHandle =
        s_threadedHandle.get();
    const auto &iter = threadedHandle->find(getPath());
    if (iter == threadedHandle->end()) {
        return m_pool->flowOut(error);
    }
    return iter->second;
}

bool Database::begin(const StatementBegin::Transaction &transaction,
                     Error &error)
{
    RecyclableHandle handle = flowOut(error);
    if (handle != nullptr && exec(StatementBegin().begin(transaction), error)) {
        std::unordered_map<std::string, RecyclableHandle> *threadedHandle =
            s_threadedHandle.get();
        threadedHandle->insert({getPath(), handle});
        return true;
    }
    return false;
}

bool Database::commit(Error &error)
{
    RecyclableHandle handle = flowOut(error);
    if (handle != nullptr && exec(StatementCommit().commit(), error)) {
        std::unordered_map<std::string, RecyclableHandle> *threadedHandle =
            s_threadedHandle.get();
        threadedHandle->erase(getPath());
        return true;
    }
    return false;
}

bool Database::rollback(Error &error)
{
    RecyclableHandle handle = flowOut(error);
    bool result = false;
    if (handle != nullptr) {
        result = exec(StatementRollback().rollback(), error);
        std::unordered_map<std::string, RecyclableHandle> *threadedHandle =
            s_threadedHandle.get();
        threadedHandle->erase(getPath());
    }
    return result;
}

bool Database::runEmbeddedTransaction(const TransactionBlock &transaction,
                                      Error &error)
{
    std::unordered_map<std::string, RecyclableHandle> *threadedHandle =
        s_threadedHandle.get();
    if (threadedHandle->find(getPath()) != threadedHandle->end()) {
        transaction(error);
    } else {
        runTransaction(transaction, error);
    }
    return error.isOK();
}

bool Database::runControllableTransaction(
    const ControllableTransactionBlock &transaction, Error &error)
{
    if (!begin(StatementBegin::Transaction::Immediate, error)) {
        return false;
    }
    if (transaction(error)) {
        error.reset(); //User discards error
        if (commit(error)) {
            return true;
        }
    }
    //Rollback errors do not need to be passed to the outside
    Error rollBackError;
    rollback(rollBackError);
    return false;
}

bool Database::runTransaction(const TransactionBlock &transaction, Error &error)
{
    if (!begin(StatementBegin::Transaction::Immediate, error)) {
        return false;
    }
    transaction(error);
    if (error.isOK()) {
        if (commit(error)) {
            return true;
        }
    }
    //Rollback errors do not need to be passed to the outside
    Error rollBackError;
    rollback(rollBackError);
    return false;
}

bool Database::isValid() const
{
    return m_pool != nullptr;
}

} //namespace WCDB
