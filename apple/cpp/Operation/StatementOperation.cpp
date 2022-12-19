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

#include <WCDB/HandleStatement.hpp>
#include <WCDB/MultiObject.hpp>
#include <WCDB/StatementOperation.hpp>

#define GetHandleStatementOrReturn                                             \
    auto handleStatement = getInnerHandleStatement();                          \
    if (handleStatement == nullptr) {                                          \
        return;                                                                \
    }

#define GetHandleStatementOrReturnValue(value)                                 \
    auto handleStatement = getInnerHandleStatement();                          \
    if (handleStatement == nullptr) {                                          \
        return value;                                                          \
    }

namespace WCDB {

StatementOperation::~StatementOperation() = default;

bool StatementOperation::step()
{
    GetHandleStatementOrReturnValue(false);
    return handleStatement->step();
}

bool StatementOperation::done()
{
    GetHandleStatementOrReturnValue(true);
    return handleStatement->done();
}

void StatementOperation::reset()
{
    GetHandleStatementOrReturn;
    handleStatement->reset();
}

bool StatementOperation::isReadOnly()
{
    GetHandleStatementOrReturnValue(false);
    return handleStatement->isReadOnly();
}

void StatementOperation::bindInteger(const Integer &value, int index)
{
    GetHandleStatementOrReturn;
    handleStatement->bindInteger(value, index);
}

void StatementOperation::bindDouble(const Float &value, int index)
{
    GetHandleStatementOrReturn;
    handleStatement->bindDouble(value, index);
}

void StatementOperation::bindText(const Text &value, int index)
{
    GetHandleStatementOrReturn;
    handleStatement->bindText(value, index);
}

void StatementOperation::bindBLOB(const BLOB &value, int index)
{
    GetHandleStatementOrReturn;
    handleStatement->bindBLOB(value, index);
}

void StatementOperation::bindNull(int index)
{
    GetHandleStatementOrReturn;
    handleStatement->bindNull(index);
}

void StatementOperation::bindPointer(void *ptr,
                                     int index,
                                     const Text &type,
                                     void (*destructor)(void *))
{
    GetHandleStatementOrReturn;
    handleStatement->bindPointer(ptr, index, type, destructor);
}

void StatementOperation::bindValue(const Value &value, int index)
{
    GetHandleStatementOrReturn;
    handleStatement->bindValue(value, index);
}

void StatementOperation::bindRow(const OneRowValue &row)
{
    GetHandleStatementOrReturn;
    handleStatement->bindRow(row);
}

int StatementOperation::bindParameterIndex(const Text &parameterName)
{
    GetHandleStatementOrReturnValue(0);
    return handleStatement->bindParameterIndex(parameterName);
}

signed long long StatementOperation::getColumnSize(int index)
{
    GetHandleStatementOrReturnValue(0);
    return handleStatement->getColumnSize(index);
}

ColumnType StatementOperation::getType(int index)
{
    GetHandleStatementOrReturnValue(ColumnType::Null);
    return handleStatement->getType(index);
}

int StatementOperation::getNumberOfColumns()
{
    GetHandleStatementOrReturnValue(0);
    return handleStatement->getNumberOfColumns();
}

StatementOperation::Integer StatementOperation::getInteger(int index)
{
    GetHandleStatementOrReturnValue(0);
    return handleStatement->getInteger(index);
}

StatementOperation::Float StatementOperation::getDouble(int index)
{
    GetHandleStatementOrReturnValue(0);
    return handleStatement->getDouble(index);
}

StatementOperation::Text StatementOperation::getText(int index)
{
    GetHandleStatementOrReturnValue(StatementOperation::Text());
    return handleStatement->getText(index);
}

const StatementOperation::BLOB StatementOperation::getBLOB(int index)
{
    GetHandleStatementOrReturnValue(StatementOperation::BLOB());
    return handleStatement->getBLOB(index);
}

Value StatementOperation::getValue(int index)
{
    GetHandleStatementOrReturnValue(0);
    return handleStatement->getValue(index);
}

OptionalOneColumn StatementOperation::getOneColumn(int index)
{
    GetHandleStatementOrReturnValue(OptionalOneColumn());
    return handleStatement->getOneColumn(index);
}

OneRowValue StatementOperation::getOneRow()
{
    GetHandleStatementOrReturnValue(OneRowValue());
    return handleStatement->getOneRow();
}

OptionalMultiRows StatementOperation::getAllRows()
{
    GetHandleStatementOrReturnValue(OptionalMultiRows());
    return handleStatement->getAllRows();
}

MultiObject StatementOperation::extractOneMultiObject(const ResultFields &resultFields)
{
    MultiObject result;
    GetHandleStatementOrReturnValue(result);
    int index = 0;
    for (const ResultField &field : resultFields) {
        const UnsafeStringView table = getColumnTableName(index);
        result.addField(table, field, getValue(index));
        ++index;
    }
    return result;
}

std::optional<ValueArray<MultiObject>>
StatementOperation::extractAllMultiObjects(const ResultFields &resultFields)
{
    ValueArray<MultiObject> result;
    bool succeed = false;
    while ((succeed = step()) && !done()) {
        result.push_back(extractOneMultiObject(resultFields));
    }
    return succeed ? result : std::optional<std::vector<MultiObject>>();
}

const UnsafeStringView StatementOperation::getOriginColumnName(int index)
{
    GetHandleStatementOrReturnValue(UnsafeStringView());
    return handleStatement->getOriginColumnName(index);
}

const UnsafeStringView StatementOperation::getColumnName(int index)
{
    GetHandleStatementOrReturnValue(UnsafeStringView());
    return handleStatement->getColumnName(index);
}

const UnsafeStringView StatementOperation::getColumnTableName(int index)
{
    GetHandleStatementOrReturnValue(UnsafeStringView());
    return handleStatement->getColumnTableName(index);
}

} //namespace WCDB
