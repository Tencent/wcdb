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

#include <WCDB/HandleStatement.hpp>
#include <WCDB/core_base.hpp>

namespace WCDB {

CoreBase::CoreBase(const RecyclableHandlePool &pool, CoreType type)
    : m_pool(pool), m_type(pool != nullptr ? type : CoreType::None)
{
}

const std::string &CoreBase::getPath() const
{
    return m_pool->path;
}

CoreType CoreBase::getType() const
{
    return m_type;
}

RecyclableStatement CoreBase::prepare(RecyclableHandle &handle,
                                      const Statement &statement,
                                      Error &error)
{
    std::shared_ptr<HandleStatement> handleStatement = nullptr;
    if (handle) {
        handleStatement = handle->prepare(statement);
        error = handle->getError();
    }
    return RecyclableStatement(handle, handleStatement);
}

bool CoreBase::exec(RecyclableHandle &handle,
                    const Statement &statement,
                    Error &error)
{
    bool result = false;
    if (handle) {
        result = handle->exec(statement);
        error = handle->getError();
    }
    return result;
}

bool CoreBase::isTableExists(RecyclableHandle &handle,
                             const std::string &tableName,
                             Error &error)
{
    bool result = false;
    if (handle) {
        Error::setThreadedSlient(true);
        StatementSelect select =
            StatementSelect().select(1).from(tableName).limit(0);
        std::shared_ptr<HandleStatement> handleStatement =
            handle->prepare(select);
        Error::setThreadedSlient(false);
        if (handleStatement) {
            handleStatement->step();
            result = handleStatement->isOK();
            if (!result) {
                error = handleStatement->getError();
            }
        } else {
            error = handle->getError();
        }
    }
    return result;
}

bool CoreBase::runControllableTransaction(
    ControllableTransactionBlock transaction, Error &error)
{
    if (!begin(StatementBegin::Transaction::Immediate, error)) {
        return false;
    }
    if (transaction(error)) {
        error.reset(); //User discards error
        if (commit(error)) {
            return true;
        }
    }
    //Rollback errors do not need to be passed to the outside
    Error rollBackError;
    rollback(rollBackError);
    return false;
}

bool CoreBase::runTransaction(TransactionBlock transaction, Error &error)
{
    if (!begin(StatementBegin::Transaction::Immediate, error)) {
        return false;
    }
    transaction(error);
    if (error.isOK()) {
        if (commit(error)) {
            return true;
        }
    }
    //Rollback errors do not need to be passed to the outside
    Error rollBackError;
    rollback(rollBackError);
    return false;
}

int CoreBase::getTag() const
{
    return m_pool->tag.load();
}

} //namespace WCDB
