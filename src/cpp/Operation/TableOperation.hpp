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

#pragma once
#include "BaseOperation.hpp"
#include "Value.hpp"

namespace WCDB {

class TableOperation : public BaseOperation {
#pragma mark - Insert
public:
    /**
     @brief Execute inserting with multi rows of values.
     @note  It will run embedded transaction while rows.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    bool insertRows(const MultiRowsValue &rows, const Columns &columns);

    /**
     @brief Execute inserting with multi rows of values.
     It will replace the original row while they have same primary key or row id.
     @note  It will run embedded transaction while rows.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    bool insertOrReplaceRows(const MultiRowsValue &rows, const Columns &columns);

    /**
     @brief Execute inserting with multi rows of values.
     It will ignore the row while there already exists the same primary key or row id in current table.
     @note  It will run embedded transaction while rows.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    bool insertOrIgnoreRows(const MultiRowsValue &rows, const Columns &columns);

#pragma mark - Update
public:
    /**
     @brief Execute updating with one row of values.
     @return True if no error occurs.
     */
    bool updateRow(const OneRowValue &row,
                   const Columns &columns,
                   const Expression &where = Expression(),
                   const OrderingTerms &orders = OrderingTerms(),
                   const Expression &limit = Expression(),
                   const Expression &offset = Expression());

#pragma mark - delete
public:
    /**
     @brief Execute deleting.
     @return True if no error occurs.
     */
    bool deleteValues(const Expression &where = Expression(),
                      const OrderingTerms &orders = OrderingTerms(),
                      const Expression &limit = Expression(),
                      const Expression &offset = Expression());

#pragma mark - Select
public:
    /**
     @brief Get one value by specific selecting.
     */
    OptionalValue selectValue(const ResultColumn &column,
                              const Expression &where = Expression(),
                              const OrderingTerms &orders = OrderingTerms(),
                              const Expression &offset = Expression());

    /**
     @brief Get  one column of values by specific selecting.
     */
    OptionalOneColumn selectOneColumn(const ResultColumn &column,
                                      const Expression &where = Expression(),
                                      const OrderingTerms &orders = OrderingTerms(),
                                      const Expression &offset = Expression(),
                                      const Expression &limit = Expression());

    /**
     @brief Get  one row of values by specific selecting.
     */
    OptionalOneRow selectOneRow(const ResultColumns &columns,
                                const Expression &where = Expression(),
                                const OrderingTerms &orders = OrderingTerms(),
                                const Expression &offset = Expression());

    /**
     @brief Get  all row of values by specific selecting.
     */
    OptionalMultiRows selectAllRow(const ResultColumns &columns,
                                   const Expression &where = Expression(),
                                   const OrderingTerms &orders = OrderingTerms(),
                                   const Expression &limit = Expression(),
                                   const Expression &offset = Expression());

#pragma mark - Statement
public:
    /**
     @brief Get one value from specific statement.
     @param statement The statement to excute.
     @param index The index of result column, starting from 0.
     @return A value.
     */
    OptionalValue getValueFromStatement(const Statement &statement, int index = 0);

    /**
     @brief Get one column of value from specific statement.
     @param statement The statement to excute.
     @param index The index of result column, starting from 0.
     @return A value array.
     */
    OptionalOneRow getOneColumnFromStatement(const Statement &statement, int index = 0);

    /**
     @brief Get one row of value from specific statement.
     @param statement The statement to excute.
     @return A value array.
     */
    OptionalOneRow getOneRowFromStatement(const Statement &statement);

    /**
     @brief Get all rows of value from specific statement.
     @param statement The statement to excute.
     @return Two-dimensional array of values.
     */
    OptionalMultiRows getAllRowsFromStatement(const Statement &statement);

protected:
    virtual ~TableOperation() override = 0;
    virtual const UnsafeStringView &getTableName() const = 0;
};

} //namespace WCDB
