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

#import <Foundation/Foundation.h>
#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTTransaction.h>

@interface WCTDatabase (Transaction)

/**
 @brief Generation a WCTTransaction object to do a transaction.
 @return WCTTransaction
 */
- (WCTTransaction *)getTransaction;

/**
 @brief Run a transaction in block.
 
     [database runTransaction:^void(){
        [database insertObject:object into:tableName];
     }, withError:&error];
     if (error.isOK) {
         //do sth
     }

 @param inTransaction Operation inside transaction.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction withError:(WCTError**)pError;

/**
 @brief Run a controllable transaction in block.
 
     BOOL committed = [database runControllableTransaction:^BOOL(){
         return [database insertObject:object into:tableName];
     }, withError: &error];
     if (committed) {
         // do sth
     }
 
 @param inTransaction Operation inside transaction.
 @see WCTControllableTransactionBlock
 */
- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction withError:(WCTError**)pError;

/**
 @brief Run an embedded transaction in block.
 
     [transaction runEmbeddedTransaction:^void(){
         [transaction insertObject:object into:tableName];
     }, withError: &error];
     if (error.isOK) {
         // do sth
     }
 
 @param inTransaction Operation inside transaction.
 @see WCTTransactionBlock
 */
- (BOOL)runEmbeddedTransaction:(WCTTransactionBlock)inTransaction withError:(WCTError**)pError;

/**
 @brief This interface is equivalent to [database runControllableTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 @see WCTControllableTransactionBlock
 */
- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction;

/**
 @brief This interface is equivalent to [database runEmbeddedTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 @see WCTTransactionBlock
 */
- (BOOL)runEmbeddedTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief This interface is equivalent to [database runTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief Separate interface of runTransaction:
 @warning You should call begin, commit, rollback and all other operations in same thread. To do a cross-thread transaction, use WCTTransaction.
 @see WCTTransaction
 @return YES only if no error occurs.
 */
- (BOOL)begin;

/**
 @brief Separate interface of runTransaction:
 @warning You should call begin , commit , rollback and all other operations in same thread. To do a cross-thread transaction, use WCTTransaction.
 @see WCTTransaction
 @return YES only if no error occurs.
 */
- (BOOL)commit;

/**
 @brief Separate interface of runTransaction:
 @warning You should call begin , commit , rollback and all other operations in same thread. To do a cross-thread transaction, use WCTTransaction.
 @see WCTTransaction
 @return YES only if no error occurs.
 */
- (BOOL)rollback;

@end
