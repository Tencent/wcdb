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

#include <WCDB/AbstractHandle.hpp>
#include <WCDB/Configs.hpp>

namespace WCDB {

class Handle : public AbstractHandle {
#pragma mark - Initialize
public:
    Handle();
    virtual ~Handle() = 0;

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
    bool execute(const Statement &statement);

    virtual bool prepare(const Statement &statement);
    virtual bool isPrepared();
    virtual void finalize();

    virtual bool step();
    virtual bool done();
    virtual void reset();

    using Integer32 = HandleStatement::Integer32;
    using Integer64 = HandleStatement::Integer64;
    using Text = HandleStatement::Text;
    using Float = HandleStatement::Float;
    using BLOB = HandleStatement::BLOB;

    virtual void bindInteger32(const Integer32 &value, int index);
    virtual void bindInteger64(const Integer64 &value, int index);
    virtual void bindDouble(const Float &value, int index);
    virtual void bindText(const Text &value, int index);
    virtual void bindBLOB(const BLOB &value, int index);
    virtual void bindNull(int index);

    virtual Integer32 getInteger32(int index);
    virtual Integer64 getInteger64(int index);
    virtual Float getDouble(int index);
    virtual Text getText(int index);
    virtual BLOB getBLOB(int index);

    virtual ColumnType getType(int index);
    virtual const UnsafeString getOriginColumnName(int index);
    virtual const UnsafeString getColumnName(int index);
    virtual const UnsafeString getColumnTableName(int index);

    virtual bool isStatementReadonly();
    virtual int getNumberOfColumns();

protected:
    HandleStatement *m_mainStatement;

#pragma mark - Transaction
public:
    typedef std::function<bool(Handle *)> TransactionCallback;
    bool runTransaction(const TransactionCallback &transaction);
    bool runNestedTransaction(const TransactionCallback &transaction);
};

} //namespace WCDB
