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

#include "BaseOperation.hpp"
#include "Value.hpp"

namespace WCDB {

class HandleOperation : public BaseOperation {
#pragma mark - Insert
public:
    /**
     @brief Execute inserting with multi rows of values. 
     @note  It will run embedded transaction while rows.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    bool insertRows(const MultiRowsValue &rows, const Columns &columns, const UnsafeStringView &table);

    /**
     @brief Execute inserting with multi rows of values.
     It will replace the original row while they have same primary key or row id.
     @note  It will run embedded transaction while rows.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    bool insertOrReplaceRows(const MultiRowsValue &rows,
                             const Columns &columns,
                             const UnsafeStringView &table);

    /**
     @brief Execute inserting with multi rows of values.
     It will ignore the row while there already exists the same primary key or row id in current table.
     @note  It will run embedded transaction while rows.size>1. The embedded transaction means that it will run a transaction if it's not in other transaction, otherwise it will be executed within the existing transaction.
     @return True if no error occurs.
     */
    bool insertOrIgnoreRows(const MultiRowsValue &rows,
                            const Columns &columns,
                            const UnsafeStringView &table);

#pragma mark - Update
public:
    /**
     @brief Execute updating with one row of values.
     @return True if no error occurs.
     */
    bool updateRow(const OneRowValue &row,
                   const Columns &columns,
                   const UnsafeStringView &table,
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
    bool deleteValues(const UnsafeStringView &table,
                      const Expression &where = Expression(),
                      const OrderingTerms &orders = OrderingTerms(),
                      const Expression &limit = Expression(),
                      const Expression &offset = Expression());

#pragma mark - Select
public:
    /**
     @brief Get one value by specific selecting.
     */
    OptionalValue selectValue(const ResultColumn &column,
                              const UnsafeStringView &table,
                              const Expression &where = Expression(),
                              const OrderingTerms &orders = OrderingTerms(),
                              const Expression &offset = Expression());

    /**
     @brief Get  one column of values by specific selecting.
     */
    OptionalOneColumn selectOneColumn(const ResultColumn &column,
                                      const UnsafeStringView &table,
                                      const Expression &where = Expression(),
                                      const OrderingTerms &orders = OrderingTerms(),
                                      const Expression &limit = Expression(),
                                      const Expression &offset = Expression());

    /**
     @brief Get  one row of values by specific selecting.
     */
    OptionalOneRow selectOneRow(const ResultColumns &columns,
                                const UnsafeStringView &table,
                                const Expression &where = Expression(),
                                const OrderingTerms &orders = OrderingTerms(),
                                const Expression &offset = Expression());

    /**
     @brief Get  all row of values by specific selecting.
     */
    OptionalMultiRows selectAllRow(const ResultColumns &columns,
                                   const UnsafeStringView &table,
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

    /**
     @brief Execute a statement directly.
     @warning You should firstly use `-[WCTHandle finalizeStatement]` to finalize the previous statement prepared by `-[WCTHandle prepare:]`.
     @return YES if no error occurs.
     */
    bool execute(const Statement &statement);

#pragma mark - Transaction
public:
    /**
     Triggered when a new transaction is begun successfully.
     */
    typedef std::function<bool(Handle &)> TransactionCallback;

    bool runTransaction(TransactionCallback inTransaction);

    /**
     Triggered when a new transaction is begun successfully or the main thread isn't suspended due to the current transaction.
     */
    typedef std::function<bool(Handle &, bool &, bool)> TransactionCallbackForOneLoop;

    /**
     @brief Run a pausable transaction in block.
     Firstly, WCDB will begin a transaction and call the block. After the block is finished, WCDB will check whether the main thread is suspended due to the current transaction. If not, it will call the block again; if it is, it will temporarily commit the current transaction. Once database operations in main thread are finished, WCDB will rebegin a new transaction in the current thread and call the block. This process will be repeated until the second parameter of the block is specified as YES, or some error occurs during the transaction.
     You can use pausable transaction to do some long term database operations, such as data cleaning or data migration, and avoid to block the main thread.
     
         database.runPausableTransactionWithOneLoop([&](Handle& handle, bool &stop, bool isNewTraction) {
             if(isNewTraction) {
                 // Do some initialization for new transaction.
             }
             
             // Perform a small amount of data processing.
             
             if( Error occurs ) {
                 return false;
             }
             if( All database operations are finished ) {
                 stop = true;
             }
             return true;
         });
     
     @param inTransaction Operation inside transaction for one loop.
     @see   `TransactionCallbackForOneLoop`
     @return True only if all transactions are committed.
     */
    bool runPausableTransactionWithOneLoop(TransactionCallbackForOneLoop inTransaction);

    /**
     @brief Run a transaction in block.
     @param inTransaction Operation inside transaction.
     @return True only if current nest transaction is committed successfully.
     */
    bool runNestedTransaction(TransactionCallback inTransaction);

protected:
    bool lazyRunTransaction(TransactionCallback inTransaction);

#pragma mark - Table
public:
    /**
     @brief Check the existence of the table.
     @param tableName The name of the table to be checked.
     @return result.
     */
    Optional<bool> tableExists(const UnsafeStringView &tableName);

    /**
     @brief Drop table if exists.
     @param tableName The name of the table to be dropped.
     @return True only if no error occurs.
     */
    bool dropTable(const UnsafeStringView &tableName);

    /**
     @brief Drop index if not exists.
     @param indexName The name of the index to be dropped.
     @return True only if no error occurs.
     */
    bool dropIndex(const UnsafeStringView &indexName);

protected:
    void notifyError(Error &error);
    void assertCondition(bool condition);
    virtual ~HandleOperation() override = 0;
};

} //namespace WCDB
