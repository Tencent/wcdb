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
#import <WCDB/WCTInterface.h>
#import <WCDB/WCTDeclare.h>

typedef NS_ENUM(int, WCTTransactionEvent) {
    WCTTransactionEventBeginFailed = 0,
    WCTTransactionEventCommitFailed = 1,
    WCTTransactionEventRollback = 2,
    WCTTransactionEventRollbackFailed = 3,
};
typedef BOOL (^WCTTransactionBlock)();
typedef void (^WCTTransactionEventBlock)(WCTTransactionEvent event);

@interface WCTTransaction : WCTInterface

/**
 Begin this transaction.

 @return YES only if no error occurs.
 */
- (BOOL)begin;

/**
 Commit this transaction.

 @return YES only if no error occurs.
 */
- (BOOL)commit;

/**
 Rollback this transaction.

 @return YES only if no error occurs.
 */
- (BOOL)rollback;

/**
 Run a transaction in block.
 
 BOOL commited = [database runTransaction:^BOOL(){
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
 @return YES only if transaction is commited.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction event:(WCTTransactionEventBlock)onTransactionStateChanged;

/**
 This interface is equivalent to [transaction runTransaction:transaction event:nil];
 
 @param inTransaction Operation inside transaction.
 @return YES only if transaction is commited.
 */
- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction;

/**
 More detailed error messages.
 
 @return nil or error.isOK==YES if no error occurs.
 */
- (WCTError*)error;

/**
 After enabling, you can call the [cost] interface to get time consuming.
 Note that you should call it before calling all other interface, otherwise [cost] will return an inaccurate result.
 See [cost] also.
 
 @param enabled enabled
 */
- (void)setStaticticsEnabled:(BOOL)enabled;

/**
 The time consuming. You can call it to profile the performance.
 See [setStaticticsEnabled:] also.
 
 @return Time in seconds
 */
- (double)cost;

@end
