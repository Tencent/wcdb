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

#ifndef Transaction_hpp
#define Transaction_hpp

#include <WCDB/Abstract.h>
#include <WCDB/CoreBase.hpp>
#include <WCDB/Error.hpp>
#include <WCDB/Recyclable.hpp>
#include <WCDB/RecyclableStatement.hpp>

namespace WCDB {

class HandleStatement;
class Statement;

class Transaction : public CoreBase {
public:
    ~Transaction();

    RecyclableStatement prepare(const Statement &statement,
                                Error &error) override;
    bool exec(const Statement &statement, Error &error) override;
    bool isTableExists(const std::string &tableName, Error &error) override;

    bool begin(const StatementBegin::Transaction &transaction,
               Error &error) override;
    bool commit(Error &error) override;
    bool rollback(Error &error) override;
    bool runEmbeddedTransaction(const TransactionBlock &transaction,
                                WCDB::Error &error) override;

    int getChanges();

protected:
    Transaction() = delete;
    Transaction(const Transaction &) = delete;
    Transaction &operator=(const Transaction &) = delete;

    Transaction(const RecyclableHandlePool &pool,
                const RecyclableHandle &handle);

    RecyclableHandle m_handle;
    bool m_isInTransaction;
    std::shared_ptr<std::recursive_mutex> m_mutex;

    friend class Database;
};

} //namespace WCDB

#endif /* Transaction_hpp */
