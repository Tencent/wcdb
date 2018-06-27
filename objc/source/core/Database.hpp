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
#include <WCDB/HandlePoolHolder.hpp>
#include <WCDB/ThreadLocal.hpp>

#pragma GCC visibility push(hidden)

namespace WCDB {

class Database : public HandlePoolHolder {
#pragma mark - Initializer
public:
    using Tag = HandlePool::Tag;

    Database() = delete;
    Database(const Database &) = delete;
    Database &operator=(const Database &) = delete;

    static std::shared_ptr<Database> databaseWithPath(const std::string &path);
    static std::shared_ptr<Database>
    databaseWithExistingPath(const std::string &path);
    static std::shared_ptr<Database> databaseWithExistingTag(const Tag &tag);

protected:
    Database(const RecyclableHandlePool &recyclableHandlePool);

    bool isValid() const;

#pragma mark - Identifier
public:
    uint32_t getIdentifier();

#pragma mark - Basic
public:
    void setTag(const Tag &tag);
    Tag getTag() const;

    bool canOpen();
    bool isOpened() const;
    void blockade();
    typedef HandlePool::BlockadeCallback BlockadeCallback;
    bool blockadeUntilDone(const BlockadeCallback &onBlockaded);
    typedef HandlePool::DrainedCallback ClosedCallback;
    void close(const ClosedCallback &onClosed);
    void unblockade();
    bool isBlockaded();

#pragma mark - Memory
public:
    void purge();
    static void purgeAllDatabases();

#pragma mark - Config
public:
    void setConfig(const std::shared_ptr<Config> &config,
                   int priority = Configs::Priority::Default);
    void removeConfig(const std::string &name);
    void setCipher(const Data &cipher, int pageSize = 4096);
    void setTokenizes(const std::list<std::string> &tokenizeNames);

    typedef Handle::SQLNotification SQLNotification;
    void setSQLTrace(const SQLNotification &onSQLTraced);

    typedef Handle::PerformanceNotification PerformanceNotification;
    void
    setPerformanceTrace(const PerformanceNotification &onPerformanceTraced);

#pragma mark - File
public:
    const std::string &getPath() const;
    std::string getSHMPath() const;
    std::string getWALPath() const;
    std::string getJournalPath() const;
    std::list<std::string> getPaths() const;

    bool moveFiles(const std::string &directory);
    bool moveFilesToDirectoryWithExtraFiles(
        const std::string &directory, const std::list<std::string> &extraFiles);
    bool removeFiles();
    std::pair<bool, size_t> getFilesSize();

#pragma mark - Repair
public:
    typedef Corruption::ExtraReaction CorruptionExtraReaction;
    typedef Corruption::Reaction CorruptionReaction;
    void setReactionWhenCorrupted(CorruptionReaction reaction);
    CorruptionReaction getReactionWhenCorrupted() const;
    void
    setExtraReactionWhenCorrupted(const CorruptionExtraReaction &notification);
    bool isCorrupted() const;

    std::string getFirstMaterialPath() const;
    std::string getLastMaterialPath() const;
    const std::string &getFactoryDirectory() const;

    typedef Repair::Factory::Filter BackupFilter;
    void autoBackup(bool flag = true);
    void filterBackup(const BackupFilter &tableShouldBeBackedup);
    bool backup(int maxWalFrame = std::numeric_limits<int>::max());
    bool removeMaterials();

    bool deposit();
    bool removeDeposit();

    typedef Repair::FactoryRetriever::ProgressUpdateCallback
        RetrieveProgressCallback;
    double retrieve(const RetrieveProgressCallback &onProgressUpdate);
    bool canRetrieve() const;

protected:
    bool retrieveRenewed();

#pragma mark - Handle
public:
    RecyclableHandle getHandle();
    bool execute(const Statement &statement);
    std::pair<bool, bool> tableExists(const TableOrSubquery &table);

protected:
    using ThreadedHandles =
        std::map<const HandlePool *, std::pair<RecyclableHandle, int>>;
    static ThreadLocal<ThreadedHandles> &threadedHandles();

    RecyclableHandle flowOutThreadedHandle();
    void retainThreadedHandle(const RecyclableHandle &recyclableHandle) const;
    void releaseThreadedHandle() const;

    static std::shared_ptr<HandlePool>
    generateHandlePool(const std::string &path);
    static bool initializeHandlePool(const HandlePool &handlePool);

#pragma mark - Transaction
public:
    using TransactionCallback = Handle::TransactionCallback;

    bool beginTransaction();
    bool commitOrRollbackTransaction();
    void rollbackTransaction();
    bool runTransaction(const TransactionCallback &transaction);

    bool beginNestedTransaction();
    bool commitOrRollbackNestedTransaction();
    void rollbackNestedTransaction();
    bool runNestedTransaction(const TransactionCallback &transaction);

    bool isInThreadedTransaction() const;
};

} //namespace WCDB

#pragma GCC visibility pop

#endif /* database_hpp */
