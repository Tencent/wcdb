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

#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTDatabase+Transaction.h>
#import <WCDB/WCTHandle+Private.h>

@implementation WCTDatabase (Transaction)

- (BOOL)isInTransaction
{
    return _database->isInTransaction();
}

- (BOOL)beginTransaction
{
    return _database->beginTransaction();
}

- (BOOL)commitOrRollbackTransaction
{
    return _database->commitOrRollbackTransaction();
}

- (void)rollbackTransaction
{
    return _database->rollbackTransaction();
}

- (BOOL)runTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction
{
    WCTRemedialAssert(inTransaction, "Transaction block can't be null.", return NO;);
    return _database->runTransaction([&inTransaction, self](WCDB::Handle *handle) -> bool {
        @autoreleasepool {
            WCTHandle *transactionHandle = [[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle];
            BOOL result = inTransaction(transactionHandle);
            [transactionHandle invalidate];
            return result;
        }
    });
}

- (BOOL)runPauseableTransactionWithOneLoop:(WCDB_NO_ESCAPE WCTTransactionBlockForOneLoop)inTransaction
{
    WCTRemedialAssert(inTransaction, "Transaction block can't be null.", return NO;);
    WCTHandle *transactionHandle = nil;
    BOOL ocStop = NO;
    bool ret = _database->runPauseableTransactionWithOneLoop([&inTransaction, self, transactionHandle, ocStop](WCDB::Handle *handle, bool &stop, bool isNewTransaction) mutable -> bool {
        @autoreleasepool {
            if(!transactionHandle){
                transactionHandle = [[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle];
            }
            BOOL result = inTransaction(transactionHandle, &ocStop, isNewTransaction);
            stop = ocStop;
            return result;
        }
    });
    if(transactionHandle){
        [transactionHandle invalidate];
    }
    return ret;
}

- (BOOL)beginNestedTransaction
{
    return _database->beginNestedTransaction();
}

- (BOOL)commitOrRollbackNestedTransaction
{
    return _database->commitOrRollbackNestedTransaction();
}

- (void)rollbackNestedTransaction
{
    return _database->rollbackNestedTransaction();
}

- (BOOL)runNestedTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction
{
    WCTRemedialAssert(inTransaction, "Transaction block can't be null.", return NO;);
    return _database->runNestedTransaction([&inTransaction, self](WCDB::Handle *handle) -> bool {
        @autoreleasepool {
            WCTHandle *transactionHandle = [[WCTHandle alloc] initWithDatabase:self andUnsafeHandle:handle];
            BOOL result = inTransaction(transactionHandle);
            [transactionHandle invalidate];
            return result;
        }
    });
}

@end
