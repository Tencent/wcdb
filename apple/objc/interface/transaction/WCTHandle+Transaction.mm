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

#import <WCDB/Assertion.hpp>
#import <WCDB/Macro.h>
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTHandle+Transaction.h>

@implementation WCTHandle (Transaction)

- (void)markErrorNotAllowedWithinTransaction
{
    WCTHandleAssert(return;);
    WCDB::Handle *handle = [self getOrGenerateHandle];
    handle->markErrorNotAllowedWithinTransaction();
}

- (BOOL)isInTransaction
{
    BOOL isInTransaction = NO;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        isInTransaction = handle->isInTransaction();
    }
    return isInTransaction;
}

- (BOOL)beginTransaction
{
    BOOL succeed = NO;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->beginTransaction();
    }
    return succeed;
}

- (BOOL)commitOrRollbackTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr && handle->isInTransaction(), "Commit or rollback transaction should be called only after beginTransaction called.", return NO;);
    return handle->commitOrRollbackTransaction();
}

- (void)rollbackTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr && handle->isInTransaction(), "Rollback transaction should be called only after beginTransaction called.", return;);
    handle->rollbackTransaction();
}

- (BOOL)runTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction
{
    WCTRemedialAssert(inTransaction, "Transaction block can't be null.", return NO;);
    BOOL succeed = NO;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->runTransaction([&inTransaction, self](WCDB::Handle *) -> bool {
            @autoreleasepool {
                return inTransaction(self);
            }
        });
    }
    return succeed;
}

- (BOOL)beginNestedTransaction
{
    BOOL succeed = NO;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->beginNestedTransaction();
    }
    return succeed;
}

- (BOOL)commitOrRollbackNestedTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr && handle->isInTransaction(), "Commit or rollback transaction should be called only after beginTransaction called.", return NO;);
    return handle->commitOrRollbackNestedTransaction();
}

- (void)rollbackNestedTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr && handle->isInTransaction(), "Rollback transaction should be called only after beginTransaction called.", return;);
    return handle->rollbackNestedTransaction();
}

- (BOOL)runNestedTransaction:(WCDB_NO_ESCAPE WCTTransactionBlock)inTransaction
{
    WCTRemedialAssert(inTransaction, "Transaction block can't be null.", return NO;);
    BOOL succeed = NO;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        succeed = handle->runNestedTransaction([&inTransaction, self](WCDB::Handle *) -> bool {
            @autoreleasepool {
                return inTransaction(self);
            }
        });
    }
    return succeed;
}

@end
