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

#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTDatabase+Transaction.h>
#import <WCDB/WCTTransaction.h>

@implementation WCTDatabase (Transaction)

- (WCTTransaction *)getTransaction
{
    return [[WCTTransaction alloc] initWithCore:_core];
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction
{
    return [self runTransaction:inTransaction event:nil];
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction event:(WCTTransactionEventBlock)onTransactionStateChanged
{
    WCDB::Database::TransactionEvent event = nullptr;
    if (onTransactionStateChanged) {
        event = [onTransactionStateChanged](WCDB::Database::TransactionEventType eventType) {
            onTransactionStateChanged((WCTTransactionEvent) eventType);
        };
    }
    WCDB::Error innerError;
    return _database->runTransaction([inTransaction](WCDB::Error &) -> bool {
        @autoreleasepool {
            return inTransaction();
        }
    },
                                     event, innerError);
}

- (BOOL)beginTransaction
{
    WCDB::Error innerError;
    return _database->begin(WCDB::StatementTransaction::Mode::Immediate, innerError);
}

- (BOOL)commitTransaction
{
    WCDB::Error innerError;
    return _database->commit(innerError);
}

- (BOOL)rollbackTransaction
{
    WCDB::Error innerError;
    return _database->rollback(innerError);
}

@end
