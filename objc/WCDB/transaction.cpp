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

#include <WCDB/handle.hpp>
#include <WCDB/transaction.hpp>
#include <WCDB/utility.hpp>

namespace WCDB {

Transaction::Transaction(const RecyclableHandlePool &pool,
                         const RecyclableHandle &handle)
    : CoreBase(pool, CoreType::Transaction)
    , m_handle(handle)
    , m_mutex(new std::mutex)
    , m_isInTransaction(false)
{
}

RecyclableStatement Transaction::prepare(const Statement &statement,
                                         Error &error)
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    if (statement.getStatementType() == Statement::Type::Transaction) {
        Error::ReportCore(
            getTag(), getPath(), Error::CoreOperation::Prepare,
            Error::CoreCode::Misuse,
            "Using [begin], [commit], [rollback] method to do a transaction",
            &error);
        return RecyclableStatement(m_handle, nullptr);
    }
    return CoreBase::prepare(m_handle, statement, error);
}

bool Transaction::exec(const Statement &statement, Error &error)
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    if (statement.getStatementType() == Statement::Type::Transaction) {
        Error::ReportCore(
            getTag(), getPath(), Error::CoreOperation::Exec,
            Error::CoreCode::Misuse,
            "Using [begin], [commit], [rollback] method to do a transaction",
            &error);
        return false;
    }
    return CoreBase::exec(m_handle, statement, error);
}

bool Transaction::isTableExists(const std::string &tableName, Error &error)
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    return CoreBase::isTableExists(m_handle, tableName, error);
}

bool Transaction::begin(StatementTransaction::Mode mode, Error &error)
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    if (CoreBase::exec(m_handle, StatementTransaction().begin(mode), error)) {
        m_isInTransaction = true;
        return true;
    }
    return false;
}

bool Transaction::commit(Error &error)
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    bool result =
        CoreBase::exec(m_handle, StatementTransaction().commit(), error);
    if (result) {
        m_isInTransaction = false;
    }
    return result;
}

bool Transaction::rollback(Error &error)
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    bool result =
        CoreBase::exec(m_handle, StatementTransaction().rollback(), error);
    m_isInTransaction = false;
    return result;
}

bool Transaction::runEmbeddedTransaction(TransactionBlock transaction,
                                         WCDB::Error &error)
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    if (m_isInTransaction) {
        return transaction(error);
    }
    return runTransaction(transaction, nullptr, error);
}

int Transaction::getChanges()
{
    std::lock_guard<std::mutex> lockGuard(*m_mutex.get());
    return m_handle->getChanges();
}

Transaction::~Transaction()
{
    if (m_isInTransaction) {
        Error innerError;
        rollback(innerError);
    }
}

} //namespace WCDB
