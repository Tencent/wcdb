//
// Created by sanhuazhang on 2019/08/15
//

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

#include "InnerHandle.hpp"
#include "Assertion.hpp"
#include "BusyRetryConfig.hpp"
#include "CipherConfig.hpp"
#include "CoreConst.h"

namespace WCDB {

InnerHandle::InnerHandle()
: m_type(HandleType::Normal)
, m_writeHint(false)
, m_mainStatement(nullptr)
, m_transactionEvent(nullptr)
{
    m_mainStatement = getStatement();
}

InnerHandle::~InnerHandle()
{
    returnStatement(m_mainStatement);
}

void InnerHandle::setType(HandleType type)
{
    switch (type) {
    case HandleType::Migrate:
        m_error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeMigrate);
        break;
    case HandleType::Compress:
        m_error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeCompress);
        break;
    case HandleType::BackupRead:
    case HandleType::BackupWrite:
    case HandleType::BackupCipher:
        m_error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeBackup);
        break;
    case HandleType::Checkpoint:
        m_error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeCheckpoint);
        break;
    case HandleType::IntegrityCheck:
        m_error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeIntegrity);
        break;
    case HandleType::MergeIndex:
        m_error.infos.insert_or_assign(ErrorStringKeyType, ErrorTypeMergeIndex);
        break;
    default:
        m_error.infos.erase(ErrorStringKeyType);
        break;
    }
    m_type = type;
}

HandleType InnerHandle::getType() const
{
    return m_type;
}

bool InnerHandle::getWriteHint()
{
    return m_writeHint;
}

void InnerHandle::setWriteHint(bool hint)
{
    m_writeHint = hint;
}

#pragma mark - Config
bool InnerHandle::open()
{
    bool succeed = false;
    if (AbstractHandle::open()) {
        if (configure()) {
            succeed = true;
        } else {
            close();
        }
    }
    return succeed;
}

void InnerHandle::close()
{
    if (isOpened()) {
        while (!m_invokeds.empty()) {
            const auto last = m_invokeds.back();
            last.value()->uninvoke(this); // ignore errors
            m_invokeds.pop_back();
        }
    }
    AbstractHandle::close();
}

bool InnerHandle::reconfigure(const Configs &newConfigs)
{
    if (newConfigs != m_pendings) {
        m_pendings = newConfigs;
        if (isOpened()) {
            return configure();
        }
    }
    return true;
}

bool InnerHandle::configure()
{
    if (m_pendings != m_invokeds) {
        while (!m_invokeds.empty()) {
            auto last = m_invokeds.back();
            if (!last.value()->uninvoke(this)) {
                return false;
            }
            m_invokeds.pop_back();
        }
        WCTAssert(m_invokeds.empty());
        std::shared_ptr<Config> cipherConfig = nullptr;
        for (const auto &element : m_pendings) {
            if (!element.value()->invoke(this)) {
                if (element.key().caseInsensitiveEqual(BasicConfigName)
                    && !canWriteMainDB()) {
                    //Setting the WAL journal mode requires writing the main DB.
                    enableWriteMainDB(true);
                    close();
                    bool success = open();
                    enableWriteMainDB(false);
                    return success;
                }
                return false;
            }
            m_invokeds.insert(element.key(), element.value(), element.order());
            if (element.key().compare(CipherConfigName) == 0) {
                cipherConfig = element.value();
            }
        }
        m_pendings = m_invokeds;
        if (cipherConfig != nullptr) {
            WCTAssert(dynamic_cast<CipherConfig *>(cipherConfig.get()) != nullptr);
            CipherConfig *convertedConfig
            = static_cast<CipherConfig *>(cipherConfig.get());
            if (convertedConfig != nullptr) {
                convertedConfig->trySaveRawKey(this);
            }
        }
    }
    return true;
}

#pragma mark - Statement
bool InnerHandle::execute(const Statement &statement)
{
    TransactionGuard transactionedGuard(m_transactionEvent, this);
    bool succeed = false;
    if (prepare(statement)) {
        succeed = step();
        finalize();
    }
    return succeed;
}

bool InnerHandle::execute(const UnsafeStringView &sql)
{
    TransactionGuard transactionedGuard(m_transactionEvent, this);
    bool succeed = false;
    if (prepare(sql)) {
        succeed = step();
        finalize();
    }
    return succeed;
}

bool InnerHandle::prepare(const Statement &statement)
{
    return m_mainStatement->prepare(statement);
}

bool InnerHandle::prepare(const UnsafeStringView &sql)
{
    return m_mainStatement->prepareSQL(sql);
}

bool InnerHandle::isPrepared()
{
    return m_mainStatement->isPrepared();
}

void InnerHandle::reset()
{
    m_mainStatement->reset();
}

void InnerHandle::clearBindings()
{
    m_mainStatement->clearBindings();
}

bool InnerHandle::done()
{
    return m_mainStatement->done();
}

bool InnerHandle::step()
{
    return m_mainStatement->step();
}

int InnerHandle::getNumberOfColumns()
{
    return m_mainStatement->getNumberOfColumns();
}

const UnsafeStringView InnerHandle::getOriginColumnName(int index)
{
    return m_mainStatement->getOriginColumnName(index);
}

const UnsafeStringView InnerHandle::getColumnName(int index)
{
    return m_mainStatement->getColumnName(index);
}

const UnsafeStringView InnerHandle::getColumnTableName(int index)
{
    return m_mainStatement->getColumnTableName(index);
}

ColumnType InnerHandle::getColumnType(int index)
{
    return m_mainStatement->getType(index);
}

void InnerHandle::bindInteger(const Integer &value, int index)
{
    m_mainStatement->bindInteger(value, index);
}

void InnerHandle::bindDouble(const Float &value, int index)
{
    m_mainStatement->bindDouble(value, index);
}

void InnerHandle::bindText(const Text &value, int index)
{
    m_mainStatement->bindText(value, index);
}

void InnerHandle::bindBLOB(const BLOB &value, int index)
{
    m_mainStatement->bindBLOB(value, index);
}

void InnerHandle::bindNull(int index)
{
    m_mainStatement->bindNull(index);
}

void InnerHandle::bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *))
{
    m_mainStatement->bindPointer(ptr, index, type, destructor);
}

void InnerHandle::bindValue(const Value &value, int index)
{
    m_mainStatement->bindValue(value, index);
}

void InnerHandle::bindRow(const OneRowValue &row)
{
    m_mainStatement->bindRow(row);
}

int InnerHandle::bindParameterIndex(const Text &parameterName)
{
    return m_mainStatement->bindParameterIndex(parameterName);
}

InnerHandle::Integer InnerHandle::getInteger(int index)
{
    return m_mainStatement->getInteger(index);
}

InnerHandle::Float InnerHandle::getDouble(int index)
{
    return m_mainStatement->getDouble(index);
}

InnerHandle::Text InnerHandle::getText(int index)
{
    return m_mainStatement->getText(index);
}

InnerHandle::BLOB InnerHandle::getBLOB(int index)
{
    return m_mainStatement->getBLOB(index);
}

Value InnerHandle::getValue(int index)
{
    return m_mainStatement->getValue(index);
}

OptionalOneColumn InnerHandle::getOneColumn(int index)
{
    return m_mainStatement->getOneColumn(index);
}

OneRowValue InnerHandle::getOneRow()
{
    return m_mainStatement->getOneRow();
}

OptionalMultiRows InnerHandle::getAllRows()
{
    return m_mainStatement->getAllRows();
}

void InnerHandle::finalize()
{
    m_mainStatement->finalize();
}

bool InnerHandle::isStatementReadonly()
{
    return m_mainStatement->isReadOnly();
}

#pragma mark - Transaction

bool InnerHandle::beginTransaction()
{
    TransactionGuard transactionedGuard(m_transactionEvent, this);
    return AbstractHandle::beginTransaction();
}

bool InnerHandle::commitTransaction()
{
    TransactionGuard transactionedGuard(m_transactionEvent, this);
    return AbstractHandle::commitTransaction();
}

void InnerHandle::rollbackTransaction()
{
    TransactionGuard transactionedGuard(m_transactionEvent, this);
    return AbstractHandle::rollbackTransaction();
}

bool InnerHandle::checkMainThreadBusyRetry()
{
    const auto &element = m_pendings.find(StringView(BusyRetryConfigName));
    if (element == m_pendings.end()) {
        return false;
    }
    std::shared_ptr<BusyRetryConfig> config
    = std::dynamic_pointer_cast<BusyRetryConfig>(element->value());
    if (config == nullptr) {
        return false;
    }
    return config->checkMainThreadBusyRetry(getPath());
}

bool InnerHandle::checkHasBusyRetry()
{
    const auto &element = m_pendings.find(StringView(BusyRetryConfigName));
    if (element == m_pendings.end()) {
        return false;
    }
    std::shared_ptr<BusyRetryConfig> config
    = std::dynamic_pointer_cast<BusyRetryConfig>(element->value());
    if (config == nullptr) {
        return false;
    }
    return config->checkHasBusyRetry(getPath());
}

bool InnerHandle::runTransactionIfNotInTransaction(const TransactionCallback &transaction)
{
    if (isInTransaction()) {
        return transaction(this);
    } else {
        return runTransaction(transaction);
    }
}

bool InnerHandle::runTransaction(const TransactionCallback &transaction)
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

bool InnerHandle::runPausableTransactionWithOneLoop(const TransactionCallbackForOneLoop &transaction)
{
    bool stop = false;
    bool needBegin = true;
    bool isNewTransaction;
    do {
        isNewTransaction = needBegin;
        if (needBegin && !beginTransaction()) {
            return false;
        }
        needBegin = false;
        if (!transaction(this, stop, isNewTransaction)) {
            rollbackTransaction();
            return false;
        }
        if (checkMainThreadBusyRetry() || stop) {
            if (!commitOrRollbackTransaction()) {
                return false;
            }
            if (!stop) {
                needBegin = true;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        }
    } while (!stop);
    return true;
}

void InnerHandle::configTransactionEvent(TransactionEvent *event)
{
    m_transactionEvent = event;
}

ConfiguredHandle::~ConfiguredHandle() = default;

} //namespace WCDB
