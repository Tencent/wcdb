//
// Created by sanhuazhang on 2019/05/02
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

#import "Macro.h"
#import "WCTDeclaration.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Triggered when a new transaction is begun successfully.
 */
typedef BOOL (^WCTTransactionBlock)(WCTHandle* /*handle*/);

/**
 Triggered when a new transaction is begun successfully or the main thread isn't suspended due to the current transaction.
 */
typedef BOOL (^WCTTransactionBlockForOneLoop)(WCTHandle* /*handle*/, BOOL* /*stop*/, BOOL /*isNewTransaction*/);

@protocol WCTTransactionProtocol
@required
/**
 @brief Check whether the current database has begun a transaction in the current thread with `sqlite3_get_autocommit`.
 @return YES if current database has begun a transaction in the current thread.
 */
- (BOOL)isInTransaction;

/**
 @brief Separate interface of `-[WCTTransactionProtocol runTransaction:]`.
 @warning You should call `beginTransaction`, `commitOrRollbackTransaction`, `rollbackTransaction` and all other operations in same thread.
 @return YES only if no error occurs.
 */
- (BOOL)beginTransaction;

/**
 @brief Separate interface of `-[WCTTransactionProtocol runTransaction:]`.
 It will commit current transaction if no error occurs during the transaction, otherwise it will rollback current transaction.
 @warning You should call `beginTransaction`, `commitOrRollbackTransaction`, `rollbackTransaction` and all other operations in same thread.
 @return YES only if current transaction is committed successfully.
 */
- (BOOL)commitOrRollbackTransaction;

/**
 @brief Separate interface of `-[WCTTransactionProtocol runTransaction:]`.
 @warning You should call `beginTransaction`, `commitOrRollbackTransaction`, `rollbackTransaction` and all other operations in same thread.
 */
- (void)rollbackTransaction;

/**
 @brief Run a transaction in block. Transaction supports nesting.
 @param inTransaction Operation inside transaction.
 @return YES only if current transaction is committed successfully.
 */
- (BOOL)runTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction;

/**
 @brief Run a pausable transaction in block.
 Firstly, WCDB will begin a transaction and call the block. 
 After the block is finished, WCDB will check whether the main thread is suspended due to the current transaction.
 If not, it will call the block again; if it is, it will temporarily commit the current transaction.
 Once database operations in main thread are finished, WCDB will rebegin a new transaction in the current thread and call the block.
 This process will be repeated until the second parameter of the block is specified as true, or some error occurs during the transaction.
 You can use pausable transaction to do some long term database operations, such as data cleaning or data migration, and avoid to block the main thread.
 
     BOOL allCommited = [database runPausableTransactionWithOneLoop:^BOOL(WCTHandle *handle, BOOL *stop, BOOL isNewTransaction) {
         if(isNewTransaction) {
             // Do some initialization for new transaction.
         }
     
         // Perform a small amount of data processing.
     
         if( Error occurs ) {
             return NO;
         }
         if( All database operations are finished ) {
             *stop = YES;
         }
         return YES;
     }];
 
 @param inTransaction Operation inside transaction for one loop.
 @see   `WCTTransactionBlockForOneLoop`
 @return YES only if all transactions are committed.
 */
- (BOOL)runPausableTransactionWithOneLoop:(WCDB_NO_ESCAPE WCTTransactionBlockForOneLoop)inTransaction;
@end

NS_ASSUME_NONNULL_END
