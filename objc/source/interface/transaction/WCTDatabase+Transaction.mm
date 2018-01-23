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
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTTransaction.h>

@implementation WCTDatabase (Transaction)

- (WCTTransaction *)getTransaction
{
    return [[WCTTransaction alloc] initWithCore:_core];
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction withError:(WCTError **)pError
{
    WCDB::Error error;
    bool result = _database->runTransaction([inTransaction](WCDB::Error &) {
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
    bool result = _database->runControllableTransaction([inTransaction](WCDB::Error &) -> bool {
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
    bool result = _database->runEmbeddedTransaction([inTransaction](WCDB::Error &) {
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

- (BOOL)runControllableTransaction:(WCTControllableTransactionBlock)inTransaction
{
    return [self runControllableTransaction:inTransaction withError:nil];
}

- (BOOL)runEmbeddedTransaction:(WCTTransactionBlock)inTransaction
{
    return [self runEmbeddedTransaction:inTransaction withError:nil];
}

- (BOOL)runTransaction:(WCTTransactionBlock)inTransaction
{
    return [self runTransaction:inTransaction withError:nil];
}

- (BOOL)begin
{
    WCDB::Error innerError;
    return _database->begin(WCDB::StatementTransaction::Mode::Immediate, innerError);
}

- (BOOL)beginWithMode:(WCTTransactionMode)mode
{
    WCDB::Error innerError;
    return _database->begin((WCDB::StatementTransaction::Mode) mode, innerError);
}

- (BOOL)commit
{
    WCDB::Error innerError;
    return _database->commit(innerError);
}

- (BOOL)rollback
{
    WCDB::Error innerError;
    return _database->rollback(innerError);
}

@end
