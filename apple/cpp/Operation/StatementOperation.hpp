//
// Created by qiuwenchen on 2022/8/11.
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
#include <WCDB/Statement.hpp>
#include <WCDB/Value.hpp>

namespace WCDB {

class InnerHandleStatement;

class StatementOperation {
public:
#pragma mark - Basic
    bool execute(const Statement &statement);
    bool prepare(const Statement &statement);
    bool isPrepared();
    void finalize();

    bool step();
    bool done();
    void reset();
    bool isReadOnly();

#pragma mark - Bind Value
    using Integer = ColumnTypeInfo<ColumnType::Integer>::UnderlyingType;
    using Text = ColumnTypeInfo<ColumnType::Text>::UnderlyingType;
    using Float = ColumnTypeInfo<ColumnType::Float>::UnderlyingType;
    using BLOB = ColumnTypeInfo<ColumnType::BLOB>::UnderlyingType;

    void bindInteger(const Integer &value, int index = 1);
    void bindDouble(const Float &value, int index = 1);
    void bindText(const Text &value, int index = 1);
    void bindBLOB(const BLOB &value, int index = 1);
    void bindNull(int index = 1);
    void bindPointer(void *ptr, int index, const Text &type, void (*destructor)(void *));
    int bindParameterIndex(const Text &parameterName);

    void bindValue(const Value &value, int index = 1);
    void bindRow(const OneRowValue &row);

#pragma mark - Extract Result
    ColumnType getType(int index = 0);
    signed long long getColumnSize(int index = 0);
    int getNumberOfColumns();

    Integer getInteger(int index = 0);
    Float getDouble(int index = 0);
    Text getText(int index = 0);
    const BLOB getBLOB(int index = 0);

    Value getValue(int index = 0);
    OneColumnValue getOneColumn(int index = 0);
    OneRowValue getOneRow();
    MultiRowsValue getAllRows();

    const UnsafeStringView getOriginColumnName(int index);
    const UnsafeStringView getColumnName(int index);
    const UnsafeStringView getColumnTableName(int index);

protected:
    virtual ~StatementOperation();
    virtual InnerHandleStatement *getInnerHandleStatement() = 0;
};

} //namespace WCDB
