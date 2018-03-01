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

#ifndef Database_hpp
#define Database_hpp

#include <WCDB/Abstract.h>
#include <WCDB/HandlePool.hpp>
#include <WCDB/RecyclableStatement.hpp>
#include <WCDB/ThreadLocal.hpp>
#include <array>

namespace WCDB {

class Database {
public:
    Database() = delete;

    static std::shared_ptr<Database> databaseWithPath(const std::string &path);
    static std::shared_ptr<Database>
    databaseWithExistingPath(const std::string &path);
    static std::shared_ptr<Database> databaseWithExistingTag(const Tag &tag);

#pragma mark - Basic
    void setTag(const Tag &tag);
    int getTag() const;

    bool canOpen();
    bool isOpened() const;
    void blockade();
    void close(const std::function<void(void)> &onClosed);
    void unblockade();
    bool isBlockaded();

#pragma mark - Memory
    void purge();
    static void PurgeInAllDatabases();

#pragma mark - Config
    void setConfig(const Config &config);
    void setConfig(const std::string &name, const Config::Callback &callback);
    void
    setCipher(const void *key, const int &keySize, const int &pageSize = 4096);
    void setTokenizes(const std::list<std::string> &tokenizeNames);

#pragma mark - File
    bool moveFiles(const std::string &directory, Error &error);
    bool
    moveFilesToDirectoryWithExtraFiles(const std::string &directory,
                                       const std::list<std::string> &extraFiles,
                                       Error &error);
    bool removeFiles(Error &error);
    size_t getFilesSize(Error &error);
    const std::list<std::string> getPaths() const;
    const std::string &getPath() const;

    //sql
    RecyclableStatement prepare(const Statement &statement, Error &error);
    bool exec(const Statement &statement, Error &error);
    bool isTableExists(const std::string &tableName, Error &error);

    bool begin(const StatementBegin::Transaction &transaction, Error &error);
    bool commit(Error &error);
    bool rollback(Error &error);

    //Transaction Protocol
    typedef std::function<bool(Error &)> ControllableTransactionBlock;
    typedef std::function<void(Error &)> TransactionBlock;

    bool runEmbeddedTransaction(const TransactionBlock &transaction,
                                Error &error);
    bool
    runControllableTransaction(const ControllableTransactionBlock &transaction,
                               Error &error);
    bool runTransaction(const TransactionBlock &transaction, Error &error);

    //Repair Kit
    bool backup(const void *key, const unsigned int &length, Error &error);
    bool recoverFromPath(const std::string &corruptedDBPath,
                         const int pageSize,
                         const void *backupKey,
                         const unsigned int &backupKeyLength,
                         const void *databaseKey,
                         const unsigned int &databaseKeyLength,
                         Error &error);

protected:
    Database(const std::string &path, bool existingOnly);

    Database(const Tag &tag);

    bool isValid() const;

    static const std::array<std::string, 5> &subfixs();

    RecyclableHandle flowOut(Error &error);
    static ThreadLocal<std::unordered_map<std::string, RecyclableHandle>>
        s_threadedHandle;

    RecyclableHandlePool m_pool;
};

} //namespace WCDB

#endif /* database_hpp */
