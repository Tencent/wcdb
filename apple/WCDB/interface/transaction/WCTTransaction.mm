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
#import <WCDB/WCTTransaction+Private.h>
#import <WCDB/WCTTransaction.h>
#import <WCDB/database.hpp>
#import <WCDB/transaction.hpp>

@implementation WCTTransaction

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core
{
    if (self = [super initWithCore:core]) {
        if (_core->getType() == WCDB::CoreType::Database) {
            _core = ((WCDB::Database *) _core.get())->getTransaction(_error);
        }
        if (_core) {
            _transaction = (WCDB::Transaction *) _core.get();
        }
    }
    return self;
}

- (BOOL)begin
{
    @synchronized(self)
    {
        BOOL result = _transaction->begin(WCDB::StatementTransaction::Mode::Immediate, _error);
        if (result && _ticker) {
            _ticker->tick();
        }
        return result;
    }
}

- (BOOL)commit
{
    @synchronized(self)
    {
        BOOL result = _transaction->commit(_error);
        if (result && _ticker) {
            _ticker->pause();
        }
        _changes = _transaction->getChanges();
        return result;
    }
}

- (BOOL)rollback
{
    @synchronized(self)
    {
        BOOL result = _transaction->rollback(_error);
        if (_ticker) {
            _ticker->pause();
        }
        return result;
    }
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction
{
    return [self runTransaction:inTransaction event:nil];
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction event:(WCTTransactionEventBlock)onTransactionStateChanged
{
    @synchronized(self)
    {
        WCDB::Database::TransactionEvent event = nullptr;
        if (onTransactionStateChanged) {
            event = [onTransactionStateChanged](WCDB::Database::TransactionEventType eventType) {
                onTransactionStateChanged((WCTTransactionEvent) eventType);
            };
        }
        WCDB::Error innerError;
        BOOL result = _transaction->runTransaction([inTransaction](WCDB::Error &) -> bool {
            @autoreleasepool {
                return inTransaction();
            }
        },
                                                   event, innerError);
        _changes = _transaction->getChanges();
        return result;
    }
}

- (int)changes
{
    @synchronized(self)
    {
        return _changes;
    }
}

@end
