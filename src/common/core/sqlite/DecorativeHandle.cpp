//
// Created by qiuwenchen on 2023/11/15.
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

#include "DecorativeHandle.hpp"

namespace WCDB {

#pragma mark - HandleDecorator
DecorativeHandle *HandleDecorator::getHandle() const
{
    return static_cast<DecorativeHandle *>(m_decoratedObj);
}

Optional<StringViewSet>
HandleDecorator::getColumns(const Schema &schema, const UnsafeStringView &table)
{
    return WCDBCallDecoratedFunction(InnerHandle, getColumns, schema, table);
}

bool HandleDecorator::addColumn(const Schema &schema,
                                const UnsafeStringView &table,
                                const ColumnDef &column)
{
    return WCDBCallDecoratedFunction(InnerHandle, addColumn, schema, table, column);
}

bool HandleDecorator::commitTransaction()
{
    return WCDBCallDecoratedFunction(InnerHandle, commitTransaction);
}

void HandleDecorator::rollbackTransaction()
{
    WCDBCallDecoratedFunction(InnerHandle, rollbackTransaction);
}

DecorativeHandleStatement *HandleDecorator::getStatement(const UnsafeStringView &skipDecorator)
{
    return WCDBCallDecoratedFunction(InnerHandle, getStatement, skipDecorator);
}

void HandleDecorator::finalizeStatements()
{
    WCDBCallDecoratedFunction(InnerHandle, finalizeStatements);
}

#pragma mark - DecorativeHandle

void DecorativeHandle::onDecorationChange()
{
    m_mainStatement->finalize();
    returnStatement(m_mainStatement);
    m_mainStatement = getStatement();
    for (auto iter : m_operators) {
        iter.second->onDecorationChange();
    }
}

void DecorativeHandle::initDecorativeFunctions()
{
    WCDBRegisterDecorativeFunction(InnerHandle, getColumns);
    WCDBRegisterDecorativeFunction(InnerHandle, addColumn);
    WCDBRegisterDecorativeFunction(InnerHandle, commitTransaction);
    WCDBRegisterDecorativeFunction(InnerHandle, rollbackTransaction);
    WCDBRegisterDecorativeFunction(InnerHandle, getStatement);
    WCDBRegisterDecorativeFunction(InnerHandle, finalizeStatements);
}

Optional<StringViewSet>
DecorativeHandle::getColumns(const Schema &schema, const UnsafeStringView &table)
{
    return WCDBCallDecorativeFunction(InnerHandle, getColumns, schema, table);
}

bool DecorativeHandle::addColumn(const Schema &schema,
                                 const UnsafeStringView &table,
                                 const ColumnDef &column)
{
    return WCDBCallDecorativeFunction(InnerHandle, addColumn, schema, table, column);
}

bool DecorativeHandle::commitTransaction()
{
    return WCDBCallDecorativeFunction(InnerHandle, commitTransaction);
}

void DecorativeHandle::rollbackTransaction()
{
    WCDBCallDecorativeFunction(InnerHandle, rollbackTransaction);
}

DecorativeHandleStatement *DecorativeHandle::getStatement(const UnsafeStringView &skipDecorator)
{
    return WCDBCallDecorativeFunction(InnerHandle, getStatement, skipDecorator);
}

void DecorativeHandle::finalizeStatements()
{
    WCDBCallDecorativeFunction(InnerHandle, finalizeStatements);
}

} //namespace WCDB
