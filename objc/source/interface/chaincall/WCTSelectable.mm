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

#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTSelectable+Private.h>

@implementation WCTSelectable

- (WCDB::StatementSelect &)statement
{
    return _statement;
}

- (BOOL)lazyPrepare
{
    if (![_handle isPrepared]) {
        return [_handle prepare:_statement];
    }
    return YES;
}

- (instancetype)distinct
{
    _statement.distinct();
    return self;
}

- (instancetype)where:(const WCDB::Expression &)condition
{
    _statement.where(condition);
    return self;
}

- (instancetype)orders:(const WCDB::OrderingTerms &)orders
{
    _statement.orders(orders);
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

- (instancetype)groups:(const WCDB::Expressions &)groups
{
    _statement.groups(groups);
    return self;
}

- (instancetype)having:(const WCDB::Expression &)having
{
    _statement.having(having);
    return self;
}

@end
