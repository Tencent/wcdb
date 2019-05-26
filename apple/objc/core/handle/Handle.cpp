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

#include <WCDB/Assertion.hpp>
#include <WCDB/Handle.hpp>

namespace WCDB {

Handle::Handle() : m_mainStatement(getStatement())
{
}

Handle::~Handle()
{
    returnStatement(m_mainStatement);
}

#pragma mark - Config
bool Handle::open()
{
    if (!AbstractHandle::open()) {
        return false;
    }
    if (m_configs != nullptr && !m_configs->invoke(this)) {
        close();
        return false;
    }
    return true;
}

void Handle::close()
{
    if (isOpened()) {
        unconfigure(false);
    }
    AbstractHandle::close();
}

bool Handle::isConfigured() const
{
    return m_configs != nullptr;
}

bool Handle::reconfigure(const std::shared_ptr<Configs> &newConfigs)
{
    if (m_configs == newConfigs) {
        return true;
    }
    if (isOpened()) {
        if (!unconfigure(true)) {
            return false;
        }
        WCTInnerAssert(m_configs == nullptr);
        if (newConfigs != nullptr) {
            if (!newConfigs->invoke(this)) {
                return false;
            }
        }
    }
    m_configs = newConfigs;
    return true;
}

bool Handle::unconfigure(bool stopIfFailed)
{
    WCTInnerAssert(isOpened());
    if (m_configs != nullptr) {
        if (!m_configs->uninvoke(this, stopIfFailed)) {
            return false;
        }
        m_configs = nullptr;
    }
    return true;
}

#pragma mark - Statement
bool Handle::execute(const Statement &statement)
{
    bool succeed = false;
    if (prepare(statement)) {
        succeed = step();
        finalize();
    }
    return succeed;
}

bool Handle::prepare(const Statement &statement)
{
    return m_mainStatement->prepare(statement);
}

bool Handle::isPrepared()
{
    return m_mainStatement->isPrepared();
}

void Handle::reset()
{
    m_mainStatement->reset();
}

bool Handle::done()
{
    return m_mainStatement->done();
}

bool Handle::step()
{
    return m_mainStatement->step();
}

int Handle::getNumberOfColumns()
{
    return m_mainStatement->getNumberOfColumns();
}

const UnsafeString Handle::getOriginColumnName(int index)
{
    return m_mainStatement->getOriginColumnName(index);
}

const UnsafeString Handle::getColumnName(int index)
{
    return m_mainStatement->getColumnName(index);
}

const UnsafeString Handle::getColumnTableName(int index)
{
    return m_mainStatement->getColumnTableName(index);
}

ColumnType Handle::getType(int index)
{
    return m_mainStatement->getType(index);
}

void Handle::bindInteger32(const Integer32 &value, int index)
{
    m_mainStatement->bindInteger32(value, index);
}

void Handle::bindInteger64(const Integer64 &value, int index)
{
    m_mainStatement->bindInteger64(value, index);
}

void Handle::bindDouble(const Float &value, int index)
{
    m_mainStatement->bindDouble(value, index);
}

void Handle::bindText(const Text &value, int index)
{
    m_mainStatement->bindText(value, index);
}

void Handle::bindBLOB(const BLOB &value, int index)
{
    m_mainStatement->bindBLOB(value, index);
}

void Handle::bindNull(int index)
{
    m_mainStatement->bindNull(index);
}

Handle::Integer32 Handle::getInteger32(int index)
{
    return m_mainStatement->getInteger32(index);
}

Handle::Integer64 Handle::getInteger64(int index)
{
    return m_mainStatement->getInteger64(index);
}

Handle::Float Handle::getDouble(int index)
{
    return m_mainStatement->getDouble(index);
}

Handle::Text Handle::getText(int index)
{
    return m_mainStatement->getText(index);
}

Handle::BLOB Handle::getBLOB(int index)
{
    return m_mainStatement->getBLOB(index);
}

void Handle::finalize()
{
    m_mainStatement->finalize();
}

bool Handle::isStatementReadonly()
{
    return m_mainStatement->isReadonly();
}

#pragma mark - Transaction
bool Handle::runNestedTransaction(const TransactionCallback &transaction)
{
    if (beginNestedTransaction()) {
        if (transaction(this)) {
            return commitOrRollbackNestedTransaction();
        } else {
            rollbackNestedTransaction();
        }
    }
    return false;
}

bool Handle::runTransaction(const TransactionCallback &transaction)
{
    if (beginTransaction()) {
        if (transaction(this)) {
            return commitOrRollbackTransaction();
        } else {
            rollbackTransaction();
        }
    }
    return false;
}

} //namespace WCDB
