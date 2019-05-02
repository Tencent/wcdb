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
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTDelete.h>
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTTryDisposeGuard.h>

@implementation WCTDelete {
    WCDB::StatementDelete _statement;
}

- (WCDB::StatementDelete &)statement
{
    return _statement;
}

- (instancetype)fromTable:(NSString *)tableName
{
    _statement.deleteFrom(tableName);
    return self;
}

- (instancetype)where:(const WCDB::Expression &)condition
{
    _statement.where(condition);
    return self;
}

- (instancetype)orders:(const WCDB::OrderingTerms &)orders
{
    _statement.order(orders);
    return self;
}

- (instancetype)limit:(const WCDB::Expression &)limit
{
    _statement.limit(limit);
    return self;
}

- (instancetype)offset:(const WCDB::Expression &)offset
{
    _statement.offset(offset);
    return self;
}

- (BOOL)execute
{
    WCTTryDisposeGuard tryDisposeGuard(self);
    return [_handle execute:_statement];
}

@end
