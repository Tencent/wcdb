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
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTInterface.h>

typedef NS_ENUM(int, WCTTransactionMode) {
    WCTTransactionModeDeferred = (WCTTransactionMode) WCDB::StatementTransaction::Mode::Deferred,
    WCTTransactionModeImmediate = (WCTTransactionMode) WCDB::StatementTransaction::Mode::Immediate,
    WCTTransactionModeExclusive = (WCTTransactionMode) WCDB::StatementTransaction::Mode::Exclusive
};

/**
 Controllable Transaction block
 */
typedef BOOL (^WCTControllableTransactionBlock)();

/**
 Transaction block
 */
typedef void (^WCTTransactionBlock)();

/**
 Transaction
 */
@interface WCTTransaction : WCTInterface

/**
 @brief Begin this transaction.
 @return YES only if no error occurs.
 */
- (BOOL)begin;

/**
 @brief Begin this transaction.
 @return YES only if no error occurs.
 */
- (BOOL)beginWithMode:(WCTTransactionMode)mode;

/**
 @brief Commit this transaction.
 @return YES only if no error occurs.
 */
- (BOOL)commit;

/**
 @brief Rollback this transaction.
 @return YES only if no error occurs.
 */
- (BOOL)rollback;

/**
 @brief Run a transaction in block.
 
     [transaction runTransaction:^void(){
         [transaction insertObject:object into:tableName];
     }, withError: &error];
     if (error.isOK) {
         // do sth
     }

 @param inTransaction Operation inside transaction.
 @see WCTTransactionBlock
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction withError:(WCTError **)pError;

/**
 @brief Run a controllable transaction in block.
 
     BOOL committed = [transaction runControllableTransaction:^BOOL(){
         return [transaction insertObject:object into:tableName];
     }, withError: &error];
     if (committed) {
         // do sth
     }
 
 @param inTransaction Operation inside transaction.
 @see WCTControllableTransactionBlock
 */
- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction withError:(WCTError **)pError;

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
- (BOOL)runEmbeddedTransaction:(WCTTransactionBlock)inTransaction withError:(WCTError **)pError;

/**
 @brief This interface is equivalent to [transaction runTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 @see WCTTransactionBlock
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief This interface is equivalent to [transaction runControllableTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 @see WCTControllableTransactionBlock
 */
- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction;

/**
 @brief This interface is equivalent to [transaction runEmbeddedTransaction:transaction withError:nil];
 @param inTransaction Operation inside transaction.
 @see WCTTransactionBlock
 */
- (BOOL)runEmbeddedTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief It should be called after executing successfully
 @return the number of changes in the most recent call
 */
- (int)changes;

@end
