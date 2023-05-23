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

#import "Assertion.hpp"
#import "Macro.h"
#import "WCTHandle+Private.h"
#import "WCTHandle+Transaction.h"

@implementation WCTHandle (Transaction)

- (void)markErrorNotAllowedWithinTransaction
{
    WCTHandleAssert(return;);
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    handle->markErrorNotAllowedWithinTransaction();
}

- (BOOL)isInTransaction
{
    BOOL isInTransaction = NO;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        isInTransaction = handle->isInTransaction();
    }
    return isInTransaction;
}

- (BOOL)beginTransaction
{
    BOOL succeed = NO;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->beginTransaction();
    }
    return succeed;
}

- (BOOL)commitOrRollbackTransaction
{
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr && handle->isInTransaction(), "Commit or rollback transaction should be called only after beginTransaction called.", return NO;);
    return handle->commitOrRollbackTransaction();
}

- (void)rollbackTransaction
{
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr && handle->isInTransaction(), "Rollback transaction should be called only after beginTransaction called.", return;);
    handle->rollbackTransaction();
}

- (BOOL)runTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction
{
    WCTRemedialAssert(inTransaction, "Transaction block can't be null.", return NO;);
    BOOL succeed = NO;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->runTransaction([&inTransaction, self](WCDB::InnerHandle *) -> bool {
            @autoreleasepool {
                return inTransaction(self);
            }
        });
    }
    return succeed;
}

- (BOOL)runPausableTransactionWithOneLoop:(WCDB_NO_ESCAPE WCTTransactionBlockForOneLoop)inTransaction
{
    WCTRemedialAssert(inTransaction, "Transaction block can't be null.", return NO;);
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle == nullptr) {
        return NO;
    }
    BOOL ocStop = NO;
    bool ret = handle->runPausableTransactionWithOneLoop([&inTransaction, self, ocStop](WCDB::InnerHandle *, bool &stop, bool isNewTransaction) mutable -> bool {
        @autoreleasepool {
            BOOL result = inTransaction(self, &ocStop, isNewTransaction);
            stop = ocStop;
            return result;
        }
    });
    return ret;
}

@end
