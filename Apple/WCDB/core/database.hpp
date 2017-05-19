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

#ifndef database_hpp
#define database_hpp

#include <WCDB/abstract.h>
#include <WCDB/handle_pool.hpp>
#include <WCDB/statement_recyclable.hpp>
#include <WCDB/core_base.hpp>
#include <WCDB/thread_local.hpp>
#include <array>

namespace WCDB {

class Transaction;

class DataBase : public CoreBase
{    
public:
    DataBase() = delete;
    DataBase(const std::string& path);

    //basic
    void setTag(Tag tag);
    bool canOpen();
    bool isOpened() const;
    void blockade();
    void close(std::function<void(void)> onClosed);
    void unblockade();
    bool isBlockaded();
    
    void purgeFreeHandles();
    static void PurgeFreeHandlesInAllDataBases();

    //config
    static const std::string defaultConfigName;
    static const std::string defaultCipherConfigName;
    static const Configs defaultConfigs;
    void setConfig(const std::string& name, const Config& config, Configs::Order order);
    void setConfig(const std::string& name, const Config& config);
    void setCipherKey(const void* key, int size);

    //file
    bool moveFiles(const std::string& directory, Error& error);
    bool moveFilesToDirectoryWithExtraFiles(const std::string& directory, const std::list<std::string>& extraFiles, Error& error);
    bool removeFiles(Error& error);
    size_t getFilesSize(Error& error);
    const std::list<std::string> getPaths() const;

    //sql
    RecyclableStatement prepare(const Statement& statement, Error& error) override;
    bool exec(const Statement& statement, Error& error) override;
    bool isTableExists(const std::string& tableName, Error& error) override;

    //transaction
    std::shared_ptr<Transaction> getTransaction(Error& error);

    bool begin(StatementTransaction::Mode mode, Error& error) override;
    bool commit(Error& error) override;
    bool rollback(Error& error) override;
    bool runEmbeddedTransaction(TransactionBlock transaction, Error& error) override;

    //Repair Kit
//    bool backup(Error& error);
//    bool backup(const std::string& backupPath, Error& error);
//    bool removeBackup(Error& error);
//    bool removeBackup(const std::string& backupPath, Error& error);
//    bool repair(const std::string& newDBPath, const Assist& assist, Error& error);
//    bool repair(const std::string& newDBPath, const std::string& backupPath, const Assist& assist, Error& error);
//    std::string defaultBackupPath() const;
protected:
//    static const std::string s_defaultBackupSubfix;
    static const std::array<std::string, 4>& subfixs();

    RecyclableHandle flowOut(Error& error);
    static ThreadLocal<std::unordered_map<std::string, RecyclableHandle>> s_threadedHandle;
};

}//namespace WCDB

#endif /* database_hpp */
