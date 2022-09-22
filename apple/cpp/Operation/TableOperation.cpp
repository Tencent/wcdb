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

#include <WCDB/Assertion.hpp>
#include <WCDB/Handle.hpp>
#include <WCDB/InnerHandle.hpp>
#include <WCDB/TableOperation.hpp>

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
            return false;
        }
        for (const OneRowValue &row : rows) {
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
    };
    if (rows.size() == 0) {
        return true;
    } else if (rows.size() == 1) {
        Handle handle = Handle(getHandleHolder());
        return insertAction(handle);
    } else {
        Handle handle = Handle(getHandleHolder());
        return handle.lazyRunTransaction(insertAction);
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
            return false;
        }
        for (const OneRowValue &row : rows) {
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
    };
    if (rows.size() == 0) {
        return true;
    } else if (rows.size() == 1) {
        Handle handle = Handle(getHandleHolder());
        return insertAction(handle);
    } else {
        Handle handle = Handle(getHandleHolder());
        return handle.lazyRunTransaction(insertAction);
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
    RecyclableHandle handle = getHandleHolder();
    bool succeed = false;
    if ((succeed = handle->prepare(update))) {
        handle->bindRow(row);
        succeed = handle->step();
        handle->finalize();
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
    return getHandleHolder()->execute(delete_);
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
    Value result;
    RecyclableHandle handle = getHandleHolder();
    if (!handle->prepare(statement)) {
        return OptionalValue();
    }
    bool succeed = false;
    if ((succeed = handle->step()) && !handle->done()) {
        result = handle->getValue(index);
    }
    handle->finalize();
    return succeed ? result : OptionalValue();
}

OptionalOneColumn
TableOperation::getOneColumnFromStatement(const Statement &statement, int index)
{
    OptionalOneColumn result;
    RecyclableHandle handle = getHandleHolder();

    if (!handle->prepare(statement)) {
        return result;
    }
    result = handle->getOneColumn(index);
    handle->finalize();
    return result;
}

OptionalOneRow TableOperation::getOneRowFromStatement(const Statement &statement)
{
    OneRowValue result;
    RecyclableHandle handle = getHandleHolder();
    if (!handle->prepare(statement)) {
        return OptionalOneRow();
    }
    bool succeed = false;
    if ((succeed = handle->step()) && !handle->done()) {
        result = handle->getOneRow();
    }
    handle->finalize();
    return succeed ? result : OptionalOneRow();
}

OptionalMultiRows TableOperation::getAllRowsFromStatement(const Statement &statement)
{
    OptionalMultiRows result;
    RecyclableHandle handle = getHandleHolder();
    if (!handle->prepare(statement)) {
        return result;
    }
    result = handle->getAllRows();
    handle->finalize();
    return result;
}

} //namespace WCDB
