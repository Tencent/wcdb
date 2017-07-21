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

/**
 Trigger of error event in runTransaction:
 */
typedef NS_ENUM(int, WCTTransactionEvent) {
    /*
     * transaction failed with begin
     */
    WCTTransactionEventBeginFailed,
    /*
     * transaction failed with commit
     */
    WCTTransactionEventCommitFailed,
    /*
     * transaction rollback. It would be triggered when commiting failed or user returns NO to roll back
     */
    WCTTransactionEventRollback,
    /*
     * transaction failed with rollback
     */
    WCTTransactionEventRollbackFailed,
};

/**
 Transaction block
 */
typedef BOOL (^WCTTransactionBlock)();

/**
 Transaction event block
 */
typedef void (^WCTTransactionEventBlock)(WCTTransactionEvent event);

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
 
     BOOL commited = [transaction runTransaction:^BOOL(){
     BOOL result = [transaction insertObject:object into:tableName];
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
 @return YES only if transaction is commited.
 @see WCTTransactionBlock
 @see WCTTransactionEventBlock
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction event:(WCTTransactionEventBlock)onTransactionStateChanged;

/**
 @brief This interface is equivalent to [transaction runTransaction:transaction event:nil];
 @param inTransaction Operation inside transaction.
 @see runTransaction:event:
 @return YES only if transaction is commited.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction;

/**
 @brief It should be called after executing successfully
 @return the number of changes in the most recent call
 */
- (int)changes;

@end
