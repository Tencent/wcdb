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

#ifndef core_base_hpp
#define core_base_hpp

#include <WCDB/config.hpp>
#include <WCDB/handle_pool.hpp>
#include <WCDB/statement_recyclable.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

class Database;
class Transaction;

typedef int WCTTag;

enum class CoreType : int {
    None,
    Transaction,
    Database,
};

class CoreBase {
public:
    const std::string &getPath() const;

    CoreType getType() const;

    Tag getTag() const;

    //Handle Protocol
    virtual RecyclableStatement prepare(const Statement &statement,
                                        Error &error) = 0;
    virtual bool exec(const Statement &statement, Error &error) = 0;
    virtual bool isTableExists(const std::string &tableName, Error &error) = 0;

    //Transaction Protocol
    typedef std::function<bool(Error &)> TransactionBlock;
    enum class TransactionEventType {
        BeginFailed = 0,
        CommitFailed = 1,
        Rollback = 2,
        RollbackFailed = 3,
    };
    typedef std::function<void(TransactionEventType)> TransactionEvent;
    bool runTransaction(TransactionBlock transaction,
                        TransactionEvent event,
                        Error &error);

    virtual bool begin(StatementTransaction::Mode mode, Error &error) = 0;
    virtual bool commit(Error &error) = 0;
    virtual bool rollback(Error &error) = 0;
    virtual bool runEmbeddedTransaction(TransactionBlock transaction,
                                        Error &error) = 0;

protected:
    RecyclableStatement
    prepare(RecyclableHandle &handle, const Statement &statement, Error &error);
    bool
    exec(RecyclableHandle &handle, const Statement &statement, Error &error);
    bool isTableExists(RecyclableHandle &handle,
                       const std::string &tableName,
                       Error &error);

    CoreBase(const RecyclableHandlePool &pool, CoreType type);

    const CoreType m_type;

    RecyclableHandlePool m_pool;
};

} //namespace WCDB

#endif /* core_base_hpp */
