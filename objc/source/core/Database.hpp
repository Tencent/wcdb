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

#include <WCDB/CoreBase.hpp>
#include <WCDB/HandlePool.hpp>
#include <WCDB/RecyclableStatement.hpp>
#include <WCDB/abstract.h>
#include <WCDB/thread_local.hpp>
#include <array>

namespace WCDB {

class Transaction;

class Database : public CoreBase {
public:
    Database() = delete;

    Database(const std::string &path, bool existingOnly = false);

    Database(Tag tag);

    //basic
    void setTag(Tag tag);
    bool canOpen();
    bool isOpened() const;
    void blockade();
    void close(const std::function<void(void)> &onClosed);
    void unblockade();
    bool isBlockaded();

    void purge();
    static void PurgeInAllDatabases();

    void setConfig(const Config &config);
    void setConfig(const std::string &name, const Config::Callback &callback);
    void setCipher(const void *key, int keySize, int pageSize = 4096);
    void setTokenizes(const std::list<std::string> &tokenizeNames);

    //file
    bool moveFiles(const std::string &directory, Error &error);
    bool
    moveFilesToDirectoryWithExtraFiles(const std::string &directory,
                                       const std::list<std::string> &extraFiles,
                                       Error &error);
    bool removeFiles(Error &error);
    size_t getFilesSize(Error &error);
    const std::list<std::string> getPaths() const;

    //sql
    RecyclableStatement prepare(const Statement &statement,
                                Error &error) override;
    bool exec(const Statement &statement, Error &error) override;
    bool isTableExists(const std::string &tableName, Error &error) override;

    //transaction
    std::shared_ptr<Transaction> getTransaction(Error &error);

    bool begin(const StatementBegin::Transaction &transaction,
               Error &error) override;
    bool commit(Error &error) override;
    bool rollback(Error &error) override;
    bool runEmbeddedTransaction(const TransactionBlock &transaction,
                                Error &error) override;

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
    static const std::array<std::string, 5> &subfixs();

    RecyclableHandle flowOut(Error &error);
    static ThreadLocal<std::unordered_map<std::string, RecyclableHandle>>
        s_threadedHandle;

    static std::shared_ptr<PerformanceTrace> s_globalPerformanceTrace;
    static std::shared_ptr<SQLTrace> s_globalSQLTrace;
};

} //namespace WCDB

#endif /* database_hpp */
