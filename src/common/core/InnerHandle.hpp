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

#pragma once

#include "AbstractHandle.hpp"
#include "Configs.hpp"
#include "CoreConst.h"
#include "TransactionGuard.hpp"

namespace WCDB {

class Handle;
class HandleDecorator;

HandleStatement *GetMainHandleStatement(InnerHandle *handle);

class InnerHandle : public AbstractHandle {
    friend Handle;
    friend HandleStatement *GetMainHandleStatement(InnerHandle *handle);
    friend HandleDecorator;
#pragma mark - Initialize
public:
    InnerHandle();
    virtual ~InnerHandle() override = 0;

    void setType(HandleType type);
    HandleType getType() const;
    bool getWriteHint();
    void setWriteHint(bool hint);

private:
    HandleType m_type;
    bool m_writeHint;

#pragma mark - Config
public:
    bool open() override final;
    void close() override final;
    bool reconfigure(const Configs &newConfigs);

protected:
    bool configure();

private:
    Configs m_invokeds;
    Configs m_pendings;

#pragma mark - Statement
public:
    virtual bool execute(const Statement &statement);
    bool execute(const UnsafeStringView &sql);

    bool prepare(const Statement &statement);
    bool prepare(const UnsafeStringView &sql);
    bool isPrepared();
    virtual void finalize();

    bool step();
    bool done();
    void reset();
    void clearBindings();

    using Integer = HandleStatement::Integer;
    using Text = HandleStatement::Text;
    using Float = HandleStatement::Float;
    using BLOB = HandleStatement::BLOB;

    void bindInteger(const Integer &value, int index = 1);
    void bindDouble(const Float &value, int index = 1);
    void bindText(const Text &value, int index = 1);
    void bindBLOB(const BLOB &value, int index = 1);
    void bindNull(int index);
    void bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *));
    virtual int bindParameterIndex(const Text &parameterName);

    void bindValue(const Value &value, int index = 1);
    void bindRow(const OneRowValue &row);

    Integer getInteger(int index = 0);
    Float getDouble(int index = 0);
    Text getText(int index = 0);
    BLOB getBLOB(int index = 0);

    Value getValue(int index = 0);
    OptionalOneColumn getOneColumn(int index = 0);
    OneRowValue getOneRow();
    OptionalMultiRows getAllRows();

    const UnsafeStringView getOriginColumnName(int index);
    const UnsafeStringView getColumnName(int index);
    const UnsafeStringView getColumnTableName(int index);

    ColumnType getColumnType(int index);
    bool isStatementReadonly();
    int getNumberOfColumns();

protected:
    DecorativeHandleStatement *m_mainStatement;

#pragma mark - Transaction
public:
    typedef std::function<bool(InnerHandle *)> TransactionCallback;
    typedef std::function<bool(InnerHandle *, bool &, bool)> TransactionCallbackForOneLoop;
    bool beginTransaction() override final;
    void rollbackTransaction() override;

    bool checkMainThreadBusyRetry();
    bool checkHasBusyRetry();
    bool runTransaction(const TransactionCallback &transaction);
    bool runTransactionIfNotInTransaction(const TransactionCallback &transaction);
    bool runPausableTransactionWithOneLoop(const TransactionCallbackForOneLoop &transaction);

    void configTransactionEvent(TransactionEvent *event);
    bool commitTransaction() override;

private:
    TransactionEvent *m_transactionEvent;
};

class ConfiguredHandle final : public InnerHandle {
public:
    ~ConfiguredHandle() override;
};

} //namespace WCDB
