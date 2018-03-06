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
#include <WCDB/ThreadLocal.hpp>
#include <array>

namespace WCDB {

class Database {
public:
#pragma mark - Initializer
    Database() = delete;

    static std::shared_ptr<Database> databaseWithPath(const std::string &path);
    static std::shared_ptr<Database>
    databaseWithExistingPath(const std::string &path);
    static std::shared_ptr<Database> databaseWithExistingTag(const Tag &tag);

#pragma mark - Basic
    void setTag(const Tag &tag);
    Tag getTag() const;

    bool canOpen();
    bool isOpened() const;
    void blockade();
    typedef HandlePool::DrainedCallback ClosedCallback;
    void close(const ClosedCallback &onClosed);
    void unblockade();
    bool isBlockaded();

    const Error &getError() const;

#pragma mark - Memory
    void purge();
    static void PurgeInAllDatabases();

#pragma mark - Config
    void setConfig(const Config &config);
    void setConfig(const std::string &name, const Config::Callback &callback);
    void setCipher(const void *key, int keySize, int pageSize = 4096);
    void setTokenizes(const std::list<std::string> &tokenizeNames);

#pragma mark - File
    const std::string &getPath() const;
    std::string getSHMPath() const;
    std::string getWALPath() const;
    std::string getJournalPath() const;
    std::string getBackupPath() const;
    std::list<std::string> getPaths() const;

    bool moveFiles(const std::string &directory);
    bool moveFilesToDirectoryWithExtraFiles(
        const std::string &directory, const std::list<std::string> &extraFiles);
    bool removeFiles();
    std::pair<bool, size_t> getFilesSize();

#pragma mark - Repair Kit
    bool backup(const void *key, unsigned int length);
    bool recoverFromPath(const std::string &corruptedDBPath,
                         int pageSize,
                         const void *backupKey,
                         unsigned int backupKeyLength,
                         const void *databaseKey,
                         unsigned int databaseKeyLength);

#pragma mark - Handle
    RecyclableHandle flowOut();
    RecyclableHandle prepare(const Statement &statement);
    bool execute(const Statement &statement);
    std::pair<bool, bool> isTableExists(const std::string &tableName);

#pragma mark - Transaction
    typedef std::function<bool(Handle *)> ControllableTransactionCallback;
    typedef std::function<void(Handle *)> TransactionCallback;

    bool runNestedTransaction(const TransactionCallback &transaction);
    bool runControllableTransaction(
        const ControllableTransactionCallback &transaction);
    bool runTransaction(const TransactionCallback &transaction);

#pragma mark - Protected
protected:
    Database(const std::string &path, bool existingOnly);
    Database(const Tag &tag);

    bool isValid() const;
    HandlePool *m_pool;
    RecyclableHandlePool m_recyclablePool;

    static ThreadLocal<std::unordered_map<const HandlePool *, RecyclableHandle>>
        s_threadedHandle;
};

} //namespace WCDB

#endif /* database_hpp */
