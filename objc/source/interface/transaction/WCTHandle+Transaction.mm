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

#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTHandle (Transaction)

- (BOOL)beginTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->beginTransaction();
}

- (BOOL)commitOrRollbackTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr, "Commit or rollback transaction should be called only after beginTransaction called.", return NO;);
    return handle->commitOrRollbackTransaction();
}

- (void)rollbackTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr, "Rollback transaction should be called only after beginTransaction called.", return;);
    handle->rollbackTransaction();
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->runTransaction([&inTransaction, self](WCDB::Handle *) -> bool {
        @autoreleasepool {
            return inTransaction(self);
        }
    });
}

- (BOOL)beginNestedTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->beginNestedTransaction();
}

- (BOOL)commitOrRollbackNestedTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr, "Commit or rollback transaction should be called only after beginTransaction called.", return NO;);
    return handle->commitOrRollbackNestedTransaction();
}

- (void)rollbackNestedTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    WCTRemedialAssert(handle != nullptr, "Rollback transaction should be called only after beginTransaction called.", return;);
    return handle->rollbackNestedTransaction();
}

- (BOOL)runNestedTransaction:(WCTTransactionBlock)inTransaction
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->runNestedTransaction([&inTransaction, self](WCDB::Handle *) -> bool {
        @autoreleasepool {
            return inTransaction(self);
        }
    });
}

@end
