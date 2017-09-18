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

#ifndef transaction_hpp
#define transaction_hpp

#include <WCDB/abstract.h>
#include <WCDB/core_base.hpp>
#include <WCDB/error.hpp>
#include <WCDB/recyclable.hpp>
#include <WCDB/statement_recyclable.hpp>

namespace WCDB {

class StatementHandle;
class Statement;

class Transaction : public CoreBase {
public:
    ~Transaction();

    RecyclableStatement prepare(const Statement &statement,
                                Error &error) override;
    bool exec(const Statement &statement, Error &error) override;
    bool isTableExists(const std::string &tableName, Error &error) override;

    bool begin(StatementTransaction::Mode mode, Error &error) override;
    bool commit(Error &error) override;
    bool rollback(Error &error) override;
    bool runEmbeddedTransaction(TransactionBlock transaction,
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
    std::shared_ptr<std::mutex> m_mutex;

    friend class Database;
};

} //namespace WCDB

#endif /* transaction_hpp */
