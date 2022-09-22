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

#include <WCDB/Assertion.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/HandleOperation.hpp>
#include <WCDB/InnerHandle.hpp>
#include <WCDB/WINQ.h>

namespace WCDB {

HandleOperation::~HandleOperation() = default;

bool HandleOperation::insertOneRow(const OneRowValue& row,
                                   const Columns& columns,
                                   const UnsafeStringView& table)
{
    WCTRemedialAssert(row.size() == columns.size(),
                      "Number of values is not equal to number of columns",
                      return false;) return getHandleHolder()
    ->execute(WCDB::StatementInsert().insertIntoTable(table).columns(columns).values(row));
}

bool HandleOperation::insertMultiRows(const MultiRowsValue& rows,
                                      const Columns& columns,
                                      const UnsafeStringView& table)
{
    return lazyRunTransaction([&](Handle& handle) {
        StatementInsert insert
        = StatementInsert().insertIntoTable(table).columns(columns).values(
        BindParameter::bindParameters(columns.size()));
        if (!handle.prepare(insert)) {
            return false;
        }
        for (const OneRowValue& row : rows) {
            WCTRemedialAssert(columns.size() == row.size(),
                              "Number of values is not equal to number of columns",
                              handle.finalize();
                              return false;) handle.reset();
            handle.bindRow(row);
            if (!handle.step()) {
                handle.finalize();
                return false;
            }
        }
        handle.finalize();
        return true;
    });
}

bool HandleOperation::insertOrReplaceOneRow(const OneRowValue& row,
                                            const Columns& columns,
                                            const UnsafeStringView& table)
{
    WCTRemedialAssert(row.size() == columns.size(),
                      "Number of values is not equal to number of columns",
                      return false;) return getHandleHolder()
    ->execute(
    WCDB::StatementInsert().insertIntoTable(table).orReplace().columns(columns).values(row));
}

bool HandleOperation::insertOrReplaceMultiRows(const MultiRowsValue& rows,
                                               const Columns& columns,
                                               const UnsafeStringView& table)
{
    return lazyRunTransaction([&](Handle& handle) {
        StatementInsert insert
        = StatementInsert().insertIntoTable(table).orReplace().columns(columns).values(
        BindParameter::bindParameters(columns.size()));
        if (!handle.prepare(insert)) {
            return false;
        }
        for (const OneRowValue& row : rows) {
            WCTRemedialAssert(columns.size() == row.size(),
                              "Number of values is not equal to number of columns",
                              handle.finalize();
                              return false;) handle.reset();
            handle.bindRow(row);
            if (!handle.step()) {
                handle.finalize();
                return false;
            }
        }
        handle.finalize();
        return true;
    });
}

bool HandleOperation::updateValues(const OneRowValue& values,
                                   const Columns& columns,
                                   const UnsafeStringView& table,
                                   const Expression& where)
{
    WCTRemedialAssert(columns.size() > 0, "Number of columns can not be zero", return false;)
    WCTRemedialAssert(columns.size() == values.size(),
                      "Number of values is not equal to number of columns",
                      return false;) StatementUpdate update
    = StatementUpdate().update(table);
    for (int i = 0; i < columns.size(); i++) {
        update.set(columns[i]).to(values[i]);
    }
    update.where(where);
    return getHandleHolder()->execute(update);
}

bool HandleOperation::deleteValues(const UnsafeStringView& table, const Expression& where)
{
    return getHandleHolder()->execute(StatementDelete().deleteFrom(table).where(where));
}

Value HandleOperation::selectValue(const ResultColumn& column, const UnsafeStringView& table)
{
    return getValueFromStatement(StatementSelect().select(column).from(table));
}

Value HandleOperation::selectValue(const ResultColumn& column,
                                   const UnsafeStringView& table,
                                   const Expression& where)
{
    return getValueFromStatement(StatementSelect().select(column).from(table).where(where));
}

OneColumnValue HandleOperation::selectOneColumn(const ResultColumn& column,
                                                const UnsafeStringView& table)
{
    return getOneColumnFromStatement(StatementSelect().select(column).from(table));
}

OneColumnValue HandleOperation::selectOneColumn(const ResultColumn& column,
                                                const UnsafeStringView& table,
                                                const Expression& where)
{
    return getOneColumnFromStatement(
    StatementSelect().select(column).from(table).where(where));
}

OneColumnValue HandleOperation::selectOneColumn(const ResultColumn& column,
                                                const UnsafeStringView& table,
                                                const Expression& where,
                                                const OrderingTerm& order)
{
    return getOneColumnFromStatement(
    StatementSelect().select(column).from(table).where(where).order(order));
}

OneColumnValue HandleOperation::selectOneColumn(const ResultColumn& column,
                                                const UnsafeStringView& table,
                                                const Expression& where,
                                                const OrderingTerm& order,
                                                const LiteralValue& limit)
{
    return getOneColumnFromStatement(
    StatementSelect().select(column).from(table).where(where).order(order).limit(limit));
}

OneRowValue HandleOperation::selectOneRow(const ResultColumns& columns,
                                          const UnsafeStringView& table)
{
    return getOneRowFromStatement(StatementSelect().select(columns).from(table));
}

OneRowValue HandleOperation::selectOneRow(const ResultColumns& columns,
                                          const UnsafeStringView& table,
                                          const Expression& where)
{
    return getOneRowFromStatement(StatementSelect().select(columns).from(table).where(where));
}

MultiRowsValue HandleOperation::selectAllRow(const ResultColumns& columns,
                                             const UnsafeStringView& table)
{
    return getAllRowsFromStatement(StatementSelect().select(columns).from(table));
}

MultiRowsValue HandleOperation::selectAllRow(const ResultColumns& columns,
                                             const UnsafeStringView& table,
                                             const Expression& where)
{
    return getAllRowsFromStatement(StatementSelect().select(columns).from(table).where(where));
}

MultiRowsValue HandleOperation::selectAllRow(const ResultColumns& columns,
                                             const UnsafeStringView& table,
                                             const Expression& where,
                                             const OrderingTerm& order)
{
    return getAllRowsFromStatement(
    StatementSelect().select(columns).from(table).where(where).order(order));
}

MultiRowsValue HandleOperation::selectAllRow(const ResultColumns& columns,
                                             const UnsafeStringView& table,
                                             const Expression& where,
                                             const OrderingTerm& order,
                                             const LiteralValue& limit)
{
    return getAllRowsFromStatement(
    StatementSelect().select(columns).from(table).where(where).order(order).limit(limit));
}

Value HandleOperation::getValueFromStatement(const Statement& statement, int index)
{
    Value result;
    RecyclableHandle handle = getHandleHolder();
    if (!handle->prepare(statement)) {
        return result;
    }
    if (handle->step() && !handle->done()) {
        result = handle->getValue(index);
    }
    handle->finalize();
    return result;
}

OneColumnValue
HandleOperation::getOneColumnFromStatement(const Statement& statement, int index)
{
    OneColumnValue result;
    RecyclableHandle handle = getHandleHolder();
    if (!handle->prepare(statement)) {
        return result;
    }
    result = handle->getOneColumn(index);
    handle->finalize();
    return result;
}

OneRowValue HandleOperation::getOneRowFromStatement(const Statement& statement)
{
    OneRowValue result;
    RecyclableHandle handle = getHandleHolder();
    if (!handle->prepare(statement)) {
        return result;
    }
    if (handle->step() && !handle->done()) {
        result = handle->getOneRow();
    }
    handle->finalize();
    return result;
}

MultiRowsValue HandleOperation::getAllRowsFromStatement(const Statement& statement)
{
    MultiRowsValue result;
    RecyclableHandle handle = getHandleHolder();
    if (!handle->prepare(statement)) {
        return result;
    }
    result = handle->getAllRows();
    handle->finalize();
    return result;
}

bool HandleOperation::runTransaction(TransactionCallback inTransaction)
{
    return getHandleHolder()->runTransaction([inTransaction](InnerHandle* innerHandle) {
        Handle handle = Handle(innerHandle);
        return inTransaction(handle);
    });
}

bool HandleOperation::lazyRunTransaction(TransactionCallback inTransaction)
{
    RecyclableHandle handleHolder = getHandleHolder();
    if (handleHolder->isInTransaction()) {
        Handle handle = Handle(handleHolder.get());
        return inTransaction(handle);
    } else {
        handleHolder->markErrorNotAllowedWithinTransaction();
        return runTransaction(inTransaction);
    }
}

bool HandleOperation::runPauseableTransactionWithOneLoop(TransactionCallbackForOneLoop inTransaction)
{
    return getHandleHolder()->runPauseableTransactionWithOneLoop(
    [inTransaction](InnerHandle* innerHandle, bool& stop, bool isNewTransaction) {
        Handle handle = Handle(innerHandle);
        return inTransaction(handle, stop, isNewTransaction);
    });
}

bool HandleOperation::runNestedTransaction(TransactionCallback inTransaction)
{
    return getHandleHolder()->runNestedTransaction([inTransaction](InnerHandle* innerHandle) {
        Handle handle = Handle(innerHandle);
        return inTransaction(handle);
    });
}

std::optional<bool> HandleOperation::tableExists(const UnsafeStringView& tableName)
{
    return getHandleHolder()->tableExists(tableName);
}

bool HandleOperation::dropTable(const UnsafeStringView& tableName)
{
    return getHandleHolder()->execute(StatementDropTable().dropTable(tableName).ifExists());
}

bool HandleOperation::dropIndex(const UnsafeStringView& indexName)
{
    return getHandleHolder()->execute(StatementDropIndex().dropIndex(indexName).ifExists());
}

} // namespace WCDB
