//
// Created by sanhuazhang on 2019/05/19
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

#include "ColumnType.hpp"
#include "HandleRelated.hpp"
#include "SQLiteDeclaration.h"
#include "Statement.hpp"
#include "Value.hpp"

namespace WCDB {

class HandleStatementDecorator;

class HandleStatement : public HandleRelated {
    friend class AbstractHandle;
    friend class HandleStatementDecorator;

public:
    HandleStatement() = delete;
    HandleStatement(const HandleStatement &) = delete;
    HandleStatement &operator=(const HandleStatement &) = delete;

    HandleStatement(HandleStatement &&other);
    HandleStatement(AbstractHandle *handle);

    void enableTableMonitor();

    virtual ~HandleStatement() override;

    virtual bool prepare(const Statement &statement);
    bool prepareSQL(const UnsafeStringView &sql);
    bool isPrepared();
    virtual void finalize();

    virtual bool step();
    bool done();
    virtual void reset();
    virtual void clearBindings();

    using Integer = ColumnTypeInfo<ColumnType::Integer>::UnderlyingType;
    using Text = ColumnTypeInfo<ColumnType::Text>::UnderlyingType;
    using Float = ColumnTypeInfo<ColumnType::Float>::UnderlyingType;
    using BLOB = ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType;

    virtual void bindInteger(const Integer &value, int index = 1);
    virtual void bindDouble(const Float &value, int index = 1);
    virtual void bindText(const Text &value, int index = 1);
    virtual void bindText16(const char16_t *value, size_t valueLength, int index = 1);
    virtual void bindBLOB(const BLOB &value, int index = 1);
    virtual void bindNull(int index = 1);
    virtual void
    bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *));
    int bindParameterIndex(const Text &parameterName);

    void bindValue(const Value &value, int index = 1);
    void bindRow(const OneRowValue &row);

    ColumnType getType(int index = 0);
    signed long long getColumnSize(int index = 0);
    int getNumberOfColumns();

    Integer getInteger(int index = 0);
    Float getDouble(int index = 0);
    Text getText(int index = 0);
    const char16_t *getText16(int index = 0);
    size_t getText16Length(int index = 0);
    const BLOB getBLOB(int index = 0);

    Value getValue(int index = 0);
    OptionalOneColumn getOneColumn(int index = 0);
    OneRowValue getOneRow();
    OptionalMultiRows getAllRows();

    const UnsafeStringView getOriginColumnName(int index);
    const UnsafeStringView getColumnName(int index);
    const UnsafeStringView getColumnTableName(int index);
    int getBindParameterCount();

    bool isReadOnly();
    void enableAutoAddColumn();

protected:
    bool isBusy();

private:
    void analysisStatement(const Statement &statement);
    bool tryExtractColumnInfo(const Statement &statement,
                              const StringView &msg,
                              StringView &columnName,
                              StringView &tableName,
                              StringView &schemaName,
                              const BaseBinding **binding);
    sqlite3_stmt *m_stmt;
    bool m_done;
    StringView m_newTable;
    StringView m_modifiedTable;
    bool m_needAutoAddColumn;
    StringView m_sql;

#pragma mark - Full trace sql
private:
    void tryReportSQL();
    void clearReport();

    bool m_fullTrace;
    bool m_needReport;
    int m_stepCount;
    std::ostringstream m_stream;
};

} //namespace WCDB
