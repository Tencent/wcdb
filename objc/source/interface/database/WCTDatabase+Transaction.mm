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

#import <WCDB/WCDB.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTDatabase (Transaction)

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction
{
    return _database->runTransaction([&inTransaction, self](WCDB::Handle *handle) {
        @autoreleasepool {
            WCTUnsafeHandle *unsafeHandle = [[WCTUnsafeHandle alloc] initWithDatabase:_database
                                                                            andHandle:handle];
            return inTransaction(unsafeHandle);
        }
    });
}

- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction
{
    return _database->runControllableTransaction([&inTransaction, self](WCDB::Handle *handle) -> bool {
        @autoreleasepool {
            WCTUnsafeHandle *unsafeHandle = [[WCTUnsafeHandle alloc] initWithDatabase:_database
                                                                            andHandle:handle];
            return inTransaction(unsafeHandle);
        }
    });
}

- (BOOL)runNestedTransaction:(WCTTransactionBlock)inTransaction
{
    return _database->runNestedTransaction([&inTransaction, self](WCDB::Handle *handle) {
        @autoreleasepool {
            WCTUnsafeHandle *unsafeHandle = [[WCTUnsafeHandle alloc] initWithDatabase:_database
                                                                            andHandle:handle];
            return inTransaction(unsafeHandle);
        }
    });
}

- (BOOL)begin
{
    return _database->execute(WCDB::StatementBegin::immediate);
}

- (BOOL)commit
{
    return _database->execute(WCDB::StatementCommit::default_);
}

- (BOOL)rollback
{
    return _database->execute(WCDB::StatementRollback::default_);
}

@end
