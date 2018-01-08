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
 
     BOOL committed = [database runTransaction:^BOOL(){
        BOOL result = [database insertObject:object into:tableName];
        return result;//return YES to commit transaction and return NO to rollback transaction.
     } event:^(WCTTransactionEvent event) {
        switch (event) {
            case WCTTransactionEventBeginFailed:
                //...
                break;
            case WCTTransactionEventCommitFailed:
                //...
                break;
            case WCTTransactionEventRollback:
                //...
                break;
            case WCTTransactionEventRollbackFailed:
                //...
                break;
        };
     }];

 @param inTransaction Operation inside transaction.
 @param onTransactionStateChanged State changed event.
 @return YES only if transaction is committed.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction event:(WCTTransactionEventBlock)onTransactionStateChanged;

/**
 @brief This interface is equivalent to [database runTransaction:transaction event:nil];
 @param inTransaction Operation inside transaction.
 @see runTransaction:event:
 @return YES only if transaction is committed.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief Separate interface of runTransaction:
 @warning You should call beginTransaction, commitTransaction, rollbackTransaction and all other operations in same thread. To do a cross-thread transaction, use WCTTransaction.
 @see WCTTransaction
 @return YES only if no error occurs.
 */
- (BOOL)beginTransaction;

/**
 @brief Separate interface of runTransaction:
 @warning You should call beginTransaction , commitTransaction , rollbackTransaction and all other operations in same thread. To do a cross-thread transaction, use WCTTransaction.
 @see WCTTransaction
 @return YES only if no error occurs.
 */
- (BOOL)commitTransaction;

/**
 @brief Separate interface of runTransaction:
 @warning You should call beginTransaction , commitTransaction , rollbackTransaction and all other operations in same thread. To do a cross-thread transaction, use WCTTransaction.
 @see WCTTransaction
 @return YES only if no error occurs.
 */
- (BOOL)rollbackTransaction;

@end
