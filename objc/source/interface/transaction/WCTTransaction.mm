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
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTTransaction+Private.h>
#import <WCDB/WCTTransaction.h>

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
    return _transaction->begin(WCDB::StatementBegin::Transaction::Immediate, _error);
}

- (BOOL)begin:(WCTTransactionMode)mode
{
    return _transaction->begin((WCDB::StatementBegin::Transaction) mode, _error);
}

- (BOOL)commit
{
    return _transaction->commit(_error);
}

- (BOOL)rollback
{
    return _transaction->rollback(_error);
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction withError:(WCTError **)pError
{
    WCDB::Error error;
    bool result = _transaction->runTransaction([inTransaction](WCDB::Error &) {
        @autoreleasepool {
            inTransaction();
        }
    },
                                               error);
    if (pError) {
        *pError = [WCTError errorWithWCDBError:error];
    }
    return result;
}

- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction withError:(WCTError **)pError
{
    WCDB::Error error;
    bool result = _transaction->runControllableTransaction([inTransaction](WCDB::Error &) -> bool {
        @autoreleasepool {
            return inTransaction();
        }
    },
                                                           error);
    if (pError) {
        *pError = [WCTError errorWithWCDBError:error];
    }
    return result;
}

- (BOOL)runEmbeddedTransaction:(WCTTransactionBlock)inTransaction withError:(WCTError **)pError
{
    WCDB::Error error;
    bool result = _transaction->runEmbeddedTransaction([inTransaction](WCDB::Error &) {
        @autoreleasepool {
            inTransaction();
        }
    },
                                                       error);
    if (pError) {
        *pError = [WCTError errorWithWCDBError:error];
    }
    return result;
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction
{
    return [self runTransaction:inTransaction withError:nil];
}

- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction
{
    return [self runControllableTransaction:inTransaction withError:nil];
}

- (BOOL)runEmbeddedTransaction:(WCTTransactionBlock)inTransaction
{
    return [self runEmbeddedTransaction:inTransaction withError:nil];
}

- (int)changes
{
    return _transaction->getChanges();
}

- (WCTError *)error
{
    if (_error.isOK()) {
        return nil;
    }
    return [WCTError errorWithWCDBError:_error];
}

@end
