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

#include <WCDB/InnerHandleStatement.hpp>
#include <WCDB/StatementOperation.hpp>

namespace WCDB {

StatementOperation::~StatementOperation() = default;

bool StatementOperation::execute(const Statement &statement)
{
    bool succeed = false;
    InnerHandleStatement *handleStatement = getInnerHandleStatement();
    if (handleStatement->prepare(statement)) {
        succeed = handleStatement->step();
        handleStatement->finalize();
    }
    return succeed;
}

bool StatementOperation::prepare(const Statement &statement)
{
    return getInnerHandleStatement()->prepare(statement);
}

bool StatementOperation::isPrepared()
{
    return getInnerHandleStatement()->isPrepared();
}

void StatementOperation::finalize()
{
    getInnerHandleStatement()->finalize();
}

bool StatementOperation::step()
{
    return getInnerHandleStatement()->step();
}

bool StatementOperation::done()
{
    return getInnerHandleStatement()->done();
}

void StatementOperation::reset()
{
    getInnerHandleStatement()->reset();
}

bool StatementOperation::isReadOnly()
{
    return getInnerHandleStatement()->isReadOnly();
}

void StatementOperation::bindInteger(const Integer &value, int index)
{
    getInnerHandleStatement()->bindInteger(value, index);
}

void StatementOperation::bindDouble(const Float &value, int index)
{
    getInnerHandleStatement()->bindDouble(value, index);
}

void StatementOperation::bindText(const Text &value, int index)
{
    getInnerHandleStatement()->bindText(value, index);
}

void StatementOperation::bindBLOB(const BLOB &value, int index)
{
    getInnerHandleStatement()->bindBLOB(value, index);
}

void StatementOperation::bindNull(int index)
{
    getInnerHandleStatement()->bindNull(index);
}

void StatementOperation::bindPointer(void *ptr,
                                     int index,
                                     const Text &type,
                                     void (*destructor)(void *))
{
    getInnerHandleStatement()->bindPointer(ptr, index, type, destructor);
}

void StatementOperation::bindValue(const Value &value, int index)
{
    getInnerHandleStatement()->bindValue(value, index);
}

void StatementOperation::bindRow(const OneRowValue &row)
{
    getInnerHandleStatement()->bindRow(row);
}

int StatementOperation::bindParameterIndex(const Text &parameterName)
{
    return getInnerHandleStatement()->bindParameterIndex(parameterName);
}

signed long long StatementOperation::getColumnSize(int index)
{
    return getInnerHandleStatement()->getColumnSize(index);
}

ColumnType StatementOperation::getType(int index)
{
    return getInnerHandleStatement()->getType(index);
}

int StatementOperation::getNumberOfColumns()
{
    return getInnerHandleStatement()->getNumberOfColumns();
}

StatementOperation::Integer StatementOperation::getInteger(int index)
{
    return getInnerHandleStatement()->getInteger(index);
}

StatementOperation::Float StatementOperation::getDouble(int index)
{
    return getInnerHandleStatement()->getDouble(index);
}

StatementOperation::Text StatementOperation::getText(int index)
{
    return getInnerHandleStatement()->getText(index);
}

const StatementOperation::BLOB StatementOperation::getBLOB(int index)
{
    return getInnerHandleStatement()->getBLOB(index);
}

Value StatementOperation::getValue(int index)
{
    return getInnerHandleStatement()->getValue(index);
}

OptionalOneColumn StatementOperation::getOneColumn(int index)
{
    return getInnerHandleStatement()->getOneColumn(index);
}

OneRowValue StatementOperation::getOneRow()
{
    return getInnerHandleStatement()->getOneRow();
}

OptionalMultiRows StatementOperation::getAllRows()
{
    return getInnerHandleStatement()->getAllRows();
}

const UnsafeStringView StatementOperation::getOriginColumnName(int index)
{
    return getInnerHandleStatement()->getOriginColumnName(index);
}

const UnsafeStringView StatementOperation::getColumnName(int index)
{
    return getInnerHandleStatement()->getColumnName(index);
}

const UnsafeStringView StatementOperation::getColumnTableName(int index)
{
    return getInnerHandleStatement()->getColumnTableName(index);
}

} //namespace WCDB
