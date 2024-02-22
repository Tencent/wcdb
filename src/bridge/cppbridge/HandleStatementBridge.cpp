//
// Created by qiuwenchen on 2022/4/3.
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

#include "HandleStatementBridge.h"
#include "AbstractHandle.hpp"
#include "HandleStatement.hpp"
#include "ObjectBridge.hpp"
#include "UnsafeData.hpp"

CPPError WCDBHandleStatementGetError(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, CPPError());
    WCDB::AbstractHandle* cppHandle = cppHandleStatement->getHandle();
    if (!cppHandle) {
        return CPPError();
    }
    const WCDB::Error& error = cppHandle->getError();
    return WCDBCreateUnmanagedCPPObject(CPPError, &error);
}

bool WCDBHandleStatementPrepare(CPPHandleStatement handleStatement, CPPObject* statement)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, false);
    WCDB::Statement* cppStatement
    = (WCDB::Statement*) WCDB::ObjectBridge::extractOriginalCPPObject(statement);
    return cppHandleStatement->prepare(*cppStatement);
}

bool WCDBHandleStatementPrepareSQL(CPPHandleStatement handleStatement, const char* _Nonnull sql)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, false);
    return cppHandleStatement->prepareSQL(WCDB::UnsafeStringView(sql));
}

bool WCDBHandleStatementCheckPrepared(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, false);
    return cppHandleStatement->isPrepared();
}

bool WCDBHandleStatementStep(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, false);
    return cppHandleStatement->step();
}

void WCDBHandleStatementReset(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->reset();
}

void WCDBHandleStatementClearBindings(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->clearBindings();
}

void WCDBHandleStatementFinalize(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->finalize();
}

bool WCDBHandleStatementIsDone(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, true);
    return cppHandleStatement->done();
}

void WCDBHandleStatementBindInteger(CPPHandleStatement handleStatement,
                                    int index,
                                    signed long long intValue)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->bindInteger(intValue, index);
}

void WCDBHandleStatementBindDouble(CPPHandleStatement handleStatement, int index, double doubleValue)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->bindDouble(doubleValue, index);
}

void WCDBHandleStatementBindText(CPPHandleStatement handleStatement, int index, const char* _Nonnull text)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->bindText(WCDB::UnsafeStringView(text), index);
}

void WCDBHandleStatementBindText16(CPPHandleStatement handleStatement,
                                   int index,
                                   const short* _Nullable text,
                                   int textLength)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->bindText16((const char16_t*) text, textLength, index);
}

void WCDBHandleStatementBindBlob(CPPHandleStatement handleStatement,
                                 int index,
                                 const unsigned char* _Nonnull buffer,
                                 unsigned long long buffLength)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->bindBLOB(
    WCDB::UnsafeData::immutable(buffer, (size_t) buffLength), index);
}

void WCDBHandleStatementBindNull(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturn(handleStatement, WCDB::HandleStatement, cppHandleStatement);
    cppHandleStatement->bindNull(index);
}

int WCDBHandleStatementBindParameterIndex(CPPHandleStatement handleStatement,
                                          const char* parameterName)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, 0);
    return cppHandleStatement->bindParameterIndex(parameterName);
}

WCDBColumnValueType
WCDBHandleStatementGetColumnType(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, WCDBColumnValueTypeNull);
    switch (cppHandleStatement->getType(index)) {
    case WCDB::Syntax::ColumnType::Integer:
        return WCDBColumnValueTypeInterger;
    case WCDB::Syntax::ColumnType::Float:
        return WCDBColumnValueTypeFloat;
    case WCDB::Syntax::ColumnType::BLOB:
        return WCDBColumnValueTypeBLOB;
    case WCDB::Syntax::ColumnType::Text:
        return WCDBColumnValueTypeString;
    case WCDB::Syntax::ColumnType::Null:
        return WCDBColumnValueTypeNull;
    }
}

signed long long WCDBHandleStatementGetInteger(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, 0);
    return cppHandleStatement->getInteger(index);
}

double WCDBHandleStatementGetDouble(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, 0);
    return cppHandleStatement->getDouble(index);
}

const char* _Nullable WCDBHandleStatementGetText(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, nullptr);
    return cppHandleStatement->getText(index).data();
}

const short* _Nullable WCDBHandleStatementGetText16(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, nullptr);
    return (const short*) cppHandleStatement->getText16(index);
}

int WCDBHandleStatementGetText16Length(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, 0);
    return (int) cppHandleStatement->getText16Length(index);
}

const unsigned char*
WCDBHandleStatementGetBlob(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, nullptr);
    return cppHandleStatement->getBLOB(index).buffer();
}

signed long long
WCDBHandleStatementGetColumnSize(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, 0);
    return cppHandleStatement->getColumnSize(index);
}

int WCDBHandleStatementGetColumnCount(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, 0);
    return cppHandleStatement->getNumberOfColumns();
}

const char* _Nullable WCDBHandleStatementGetColumnName(CPPHandleStatement handleStatement, int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, nullptr);
    return cppHandleStatement->getColumnName(index).data();
}

const char* _Nullable WCDBHandleStatementGetOriginalColumnName(CPPHandleStatement handleStatement,
                                                               int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, nullptr);
    return cppHandleStatement->getOriginColumnName(index).data();
}

const char* _Nullable WCDBHandleStatementGetColumnTableName(CPPHandleStatement handleStatement,
                                                            int index)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, nullptr);
    return cppHandleStatement->getColumnTableName(index).data();
}

bool WCDBHandleStatementIsReadOnly(CPPHandleStatement handleStatement)
{
    WCDBGetObjectOrReturnValue(
    handleStatement, WCDB::HandleStatement, cppHandleStatement, false);
    return cppHandleStatement->isReadOnly();
}
