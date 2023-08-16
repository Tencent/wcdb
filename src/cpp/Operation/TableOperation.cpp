//
// Created by qiuwenchen on 2022/9/2.
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

#include "TableOperation.hpp"
#include "Assertion.hpp"
#include "Handle.hpp"
#include "InnerHandle.hpp"

#define GetHandleOrReturnValue(writeHint, value)                               \
    RecyclableHandle handle = getHandleHolder(writeHint);                      \
    if (handle == nullptr) {                                                   \
        return value;                                                          \
    }

namespace WCDB {

TableOperation::~TableOperation() = default;

bool TableOperation::insertRows(const MultiRowsValue &rows, const Columns &columns)
{
    auto insertAction = [&](Handle &handle) {
        StatementInsert insert = StatementInsert()
                                 .insertIntoTable(getTableName())
                                 .columns(columns)
                                 .values(BindParameter::bindParameters(columns.size()));
        if (!handle.prepare(insert)) {
            assignErrorToDatabase(handle.getError());
            return false;
        }
        for (const OneRowValue &row : rows) {
            WCTRemedialAssert(columns.size() == row.size(),
                              "Number of values is not equal to number of columns",
                              handle.finalize();
                              return false;) handle.reset();
            handle.bindRow(row);
            if (!handle.step()) {
                assignErrorToDatabase(handle.getError());
                handle.finalize();
                return false;
            }
        }
        handle.finalize();
        return true;
    };
    GetHandleOrReturnValue(true, false);
    Handle newHandle = Handle(handle);
    if (rows.size() == 0) {
        return true;
    } else if (rows.size() == 1) {
        return insertAction(newHandle);
    } else {
        bool succeed = newHandle.lazyRunTransaction(insertAction);
        if (!succeed) {
            assignErrorToDatabase(newHandle.getError());
        }
        return succeed;
    }
}

bool TableOperation::insertOrReplaceRows(const MultiRowsValue &rows, const Columns &columns)
{
    auto insertAction = [&](Handle &handle) {
        StatementInsert insert = StatementInsert()
                                 .insertIntoTable(getTableName())
                                 .orReplace()
                                 .columns(columns)
                                 .values(BindParameter::bindParameters(columns.size()));
        if (!handle.prepare(insert)) {
            assignErrorToDatabase(handle.getError());
            return false;
        }
        for (const OneRowValue &row : rows) {
            WCTRemedialAssert(columns.size() == row.size(),
                              "Number of values is not equal to number of columns",
                              handle.finalize();
                              return false;) handle.reset();
            handle.bindRow(row);
            if (!handle.step()) {
                assignErrorToDatabase(handle.getError());
                handle.finalize();
                return false;
            }
        }
        handle.finalize();
        return true;
    };
    GetHandleOrReturnValue(true, false);
    Handle newHandle = Handle(handle);
    if (rows.size() == 0) {
        return true;
    } else if (rows.size() == 1) {
        return insertAction(newHandle);
    } else {
        bool succeed = newHandle.lazyRunTransaction(insertAction);
        if (!succeed) {
            assignErrorToDatabase(newHandle.getError());
        }
        return succeed;
    }
}

bool TableOperation::insertOrIgnoreRows(const MultiRowsValue &rows, const Columns &columns)
{
    auto insertAction = [&](Handle &handle) {
        StatementInsert insert = StatementInsert()
                                 .insertIntoTable(getTableName())
                                 .orIgnore()
                                 .columns(columns)
                                 .values(BindParameter::bindParameters(columns.size()));
        if (!handle.prepare(insert)) {
            assignErrorToDatabase(handle.getError());
            return false;
        }
        for (const OneRowValue &row : rows) {
            WCTRemedialAssert(columns.size() == row.size(),
                              "Number of values is not equal to number of columns",
                              handle.finalize();
                              return false;) handle.reset();
            handle.bindRow(row);
            if (!handle.step()) {
                assignErrorToDatabase(handle.getError());
                handle.finalize();
                return false;
            }
        }
        handle.finalize();
        return true;
    };
    GetHandleOrReturnValue(true, false);
    Handle newHandle = Handle(handle);
    if (rows.size() == 0) {
        return true;
    } else if (rows.size() == 1) {
        return insertAction(newHandle);
    } else {
        bool succeed = newHandle.lazyRunTransaction(insertAction);
        if (!succeed) {
            assignErrorToDatabase(newHandle.getError());
        }
        return succeed;
    }
}

bool TableOperation::updateRow(const OneRowValue &row,
                               const Columns &columns,
                               const Expression &where,
                               const OrderingTerms &orders,
                               const Expression &limit,
                               const Expression &offset)
{
    WCTRemedialAssert(columns.size() > 0, "Number of columns can not be zero", return false;)
    WCTRemedialAssert(columns.size() == row.size(),
                      "Number of values in this row is not equal to number of columns",
                      return false;) StatementUpdate update
    = StatementUpdate().update(getTableName());
    for (int i = 0; i < columns.size(); i++) {
        update.set(columns[i]).to(WCDB::BindParameter(i + 1));
    }
    configStatement(update, where, orders, limit, offset);
    GetHandleOrReturnValue(true, false);
    bool succeed = false;
    if ((succeed = handle->prepare(update))) {
        handle->bindRow(row);
        succeed = handle->step();
        handle->finalize();
    }
    if (!succeed) {
        assignErrorToDatabase(handle->getError());
    }
    return succeed;
}

bool TableOperation::deleteValues(const Expression &where,
                                  const OrderingTerms &orders,
                                  const Expression &limit,
                                  const Expression &offset)
{
    StatementDelete delete_ = StatementDelete().deleteFrom(getTableName());
    configStatement(delete_, where, orders, limit, offset);
    GetHandleOrReturnValue(true, false);
    bool succeed = handle->execute(delete_);
    if (!succeed) {
        assignErrorToDatabase(handle->getError());
    }
    return succeed;
}

OptionalValue TableOperation::selectValue(const ResultColumn &column,
                                          const Expression &where,
                                          const OrderingTerms &orders,
                                          const Expression &offset)
{
    auto select = StatementSelect().select(column).from(getTableName());
    configStatement(select, where, orders, Expression(1), offset);
    return getValueFromStatement(select);
}

OptionalOneColumn TableOperation::selectOneColumn(const ResultColumn &column,
                                                  const Expression &where,
                                                  const OrderingTerms &orders,
                                                  const Expression &limit,
                                                  const Expression &offset)
{
    auto select = StatementSelect().select(column).from(getTableName());
    configStatement(select, where, orders, limit, offset);
    return getOneColumnFromStatement(select);
}

OptionalOneRow TableOperation::selectOneRow(const ResultColumns &columns,
                                            const Expression &where,
                                            const OrderingTerms &orders,
                                            const Expression &offset)
{
    auto select = StatementSelect().select(columns).from(getTableName());
    configStatement(select, where, orders, Expression(1), offset);
    return getOneRowFromStatement(select);
}

OptionalMultiRows TableOperation::selectAllRow(const ResultColumns &columns,
                                               const Expression &where,
                                               const OrderingTerms &orders,
                                               const Expression &limit,
                                               const Expression &offset)
{
    auto select = StatementSelect().select(columns).from(getTableName());
    configStatement(select, where, orders, limit, offset);
    return getAllRowsFromStatement(select);
}

OptionalValue TableOperation::getValueFromStatement(const Statement &statement, int index)
{
    OptionalValue result;
    GetHandleOrReturnValue(false, result);
    if (!handle->prepare(statement)) {
        assignErrorToDatabase(handle->getError());
        return result;
    }
    bool succeed = false;
    if ((succeed = handle->step()) && !handle->done()) {
        result = handle->getValue(index);
    }
    handle->finalize();
    if (!succeed) {
        assignErrorToDatabase(handle->getError());
    }
    return result;
}

OptionalOneColumn
TableOperation::getOneColumnFromStatement(const Statement &statement, int index)
{
    OptionalOneColumn result;
    GetHandleOrReturnValue(false, result);

    if (!handle->prepare(statement)) {
        assignErrorToDatabase(handle->getError());
        return result;
    }
    result = handle->getOneColumn(index);
    handle->finalize();
    if (!result.succeed()) {
        assignErrorToDatabase(handle->getError());
    }
    return result;
}

OptionalOneRow TableOperation::getOneRowFromStatement(const Statement &statement)
{
    OptionalOneRow result;
    GetHandleOrReturnValue(false, result);
    if (!handle->prepare(statement)) {
        assignErrorToDatabase(handle->getError());
        return result;
    }
    bool succeed = false;
    if ((succeed = handle->step()) && !handle->done()) {
        result = handle->getOneRow();
    }
    handle->finalize();
    if (!succeed) {
        assignErrorToDatabase(handle->getError());
    }
    return result;
}

OptionalMultiRows TableOperation::getAllRowsFromStatement(const Statement &statement)
{
    OptionalMultiRows result;
    GetHandleOrReturnValue(false, false);
    if (!handle->prepare(statement)) {
        assignErrorToDatabase(handle->getError());
        return result;
    }
    result = handle->getAllRows();
    handle->finalize();
    if (!result.succeed()) {
        assignErrorToDatabase(handle->getError());
    }
    return result;
}

} //namespace WCDB
