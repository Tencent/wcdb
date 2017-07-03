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

#include <WCDB/database.hpp>
#include <WCDB/error.hpp>
#include <WCDB/transaction.hpp>

namespace WCDB {

ThreadLocal<std::unordered_map<std::string, RecyclableHandle>>
    Database::s_threadedHandle;

std::shared_ptr<Transaction> Database::getTransaction(Error &error)
{
    RecyclableHandle handle =
        m_pool->flowOut(error); //get a new handle for transaction
    if (handle != nullptr) {
        return std::shared_ptr<Transaction>(new Transaction(m_pool, handle));
    }
    return nullptr;
}

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

bool Database::begin(StatementTransaction::Mode mode, Error &error)
{
    RecyclableHandle handle = flowOut(error);
    if (handle != nullptr &&
        CoreBase::exec(handle, StatementTransaction().begin(mode), error)) {
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
    if (handle != nullptr &&
        CoreBase::exec(handle, StatementTransaction().commit(), error)) {
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
        result =
            CoreBase::exec(handle, StatementTransaction().rollback(), error);
        std::unordered_map<std::string, RecyclableHandle> *threadedHandle =
            s_threadedHandle.get();
        threadedHandle->erase(getPath());
    }
    return result;
}

bool Database::runEmbeddedTransaction(TransactionBlock transaction,
                                      Error &error)
{
    std::unordered_map<std::string, RecyclableHandle> *threadedHandle =
        s_threadedHandle.get();
    if (threadedHandle->find(getPath()) != threadedHandle->end()) {
        return transaction(error);
    }
    return runTransaction(transaction, nullptr, error);
}

} //namespace WCDB
