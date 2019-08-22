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

#include <WCDB/core_base.hpp>
#include <WCDB/handle_statement.hpp>

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
    std::shared_ptr<StatementHandle> statementHandle = nullptr;
    if (handle) {
        statementHandle = handle->prepare(statement);
        error = handle->getError();
    }
    return RecyclableStatement(handle, statementHandle);
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
        static const ColumnResultList resultList = {ColumnResult(Expr(1))};
        StatementSelect select =
            StatementSelect().select(resultList).from(tableName).limit(0);
        std::shared_ptr<StatementHandle> statementHandle =
            handle->prepare(select);
        Error::setThreadedSlient(false);
        if (statementHandle) {
            statementHandle->step();
            result = statementHandle->isOK();
            if (!result) {
                error = statementHandle->getError();
            }
        } else {
            error = handle->getError();
        }
    }
    return result;
}

bool CoreBase::runTransaction(TransactionBlock transaction,
                              TransactionEvent event,
                              Error &error)
{
#define TRANSATION_EVENT(eventType)                                            \
    if (event) {                                                               \
        event(eventType);                                                      \
    }
    if (!begin(StatementTransaction::Mode::Immediate, error)) {
        TRANSATION_EVENT(TransactionEventType::BeginFailed);
        return false;
    }
    if (transaction(error)) {
        error.reset(); //User discards error
        if (commit(error)) {
            return true;
        }
        TRANSATION_EVENT(TransactionEventType::CommitFailed);
    }
    TRANSATION_EVENT(TransactionEventType::Rollback);
    Error
        rollBackError; //Rollback errors do not need to be passed to the outside
    if (!rollback(rollBackError)) {
        TRANSATION_EVENT(TransactionEventType::RollbackFailed);
    }
    return false;
}

int CoreBase::getTag() const
{
    return m_pool->tag.load();
}

} //namespace WCDB
