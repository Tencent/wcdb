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
#include <WCDB/RecyclableHandle.hpp>
#include <WCDB/SyntaxForwardDeclaration.h>
#include <WCDB/Value.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

class RecyclableHandle;
class Handle;

class HandleOperation {
#pragma mark - insert
public:
    bool insertOneRow(const OneRowValue& row, const Columns& columns, const UnsafeStringView& table);
    bool insertMultiRows(const MultiRowsValue& rows,
                         const Columns& columns,
                         const UnsafeStringView& table);
    bool insertOrReplaceOneRow(const OneRowValue& row,
                               const Columns& columns,
                               const UnsafeStringView& table);
    bool insertOrReplaceMultiRows(const MultiRowsValue& rows,
                                  const Columns& columns,
                                  const UnsafeStringView& table);

#pragma mark - update
public:
    bool updateValues(const OneRowValue& values,
                      const Columns& columns,
                      const UnsafeStringView& table,
                      const Expression& where);

#pragma mark - delete
public:
    bool deleteValues(const UnsafeStringView& table, const Expression& where);

#pragma mark - select
public:
    Value selectValue(const ResultColumn& column, const UnsafeStringView& table);
    Value selectValue(const ResultColumn& column,
                      const UnsafeStringView& table,
                      const Expression& where);

    OneColumnValue
    selectOneColumn(const ResultColumn& column, const UnsafeStringView& table);
    OneColumnValue selectOneColumn(const ResultColumn& column,
                                   const UnsafeStringView& table,
                                   const Expression& where);
    OneColumnValue selectOneColumn(const ResultColumn& column,
                                   const UnsafeStringView& table,
                                   const Expression& where,
                                   const OrderingTerm& order);
    OneColumnValue selectOneColumn(const ResultColumn& column,
                                   const UnsafeStringView& table,
                                   const Expression& where,
                                   const OrderingTerm& order,
                                   const LiteralValue& limit);

    OneRowValue selectOneRow(const ResultColumns& columns, const UnsafeStringView& table);
    OneRowValue selectOneRow(const ResultColumns& columns,
                             const UnsafeStringView& table,
                             const Expression& where);

    MultiRowsValue
    selectAllRow(const ResultColumns& columns, const UnsafeStringView& table);
    MultiRowsValue selectAllRow(const ResultColumns& columns,
                                const UnsafeStringView& table,
                                const Expression& where);
    MultiRowsValue selectAllRow(const ResultColumns& columns,
                                const UnsafeStringView& table,
                                const Expression& where,
                                const OrderingTerm& order);
    MultiRowsValue selectAllRow(const ResultColumns& columns,
                                const UnsafeStringView& table,
                                const Expression& where,
                                const OrderingTerm& order,
                                const LiteralValue& limit);

#pragma mark - Statement
public:
    Value getValueFromStatement(const Statement& statement, int index = 0);
    OneColumnValue getOneColumnFromStatement(const Statement& statement, int index = 0);
    OneRowValue getOneRowFromStatement(const Statement& statement);
    MultiRowsValue getAllRowsFromStatement(const Statement& statement);

#pragma mark - Transaction
public:
    typedef std::function<bool(Handle&)> TransactionCallback;
    typedef std::function<bool(Handle&, bool&, bool)> TransactionCallbackForOneLoop;

    bool runTransaction(TransactionCallback inTransaction);
    bool runPauseableTransactionWithOneLoop(TransactionCallbackForOneLoop inTransaction);
    bool runNestedTransaction(TransactionCallback inTransaction);

protected:
    bool lazyRunTransaction(TransactionCallback inTransaction);

#pragma mark - Table
public:
    std::optional<bool> tableExists(const UnsafeStringView& tableName);
    bool dropTable(const UnsafeStringView& tableName);
    bool dropIndex(const UnsafeStringView& indexName);

protected:
    virtual ~HandleOperation();
    virtual RecyclableHandle getHandleHolder() = 0;
};

} //namespace WCDB
