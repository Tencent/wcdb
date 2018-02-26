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
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTSelectBase+Private.h>
#import <WCDB/WCTSelectBase.h>

@implementation WCTSelectBase

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core
{
    if (self = [super initWithCore:core]) {
        _prepared = NO;
    }
    return self;
}

- (BOOL)lazyPrepare
{
    if (_error.isOK()) {
        if (!_prepared) {
            _prepared = YES;
            _statementHandle = _core->prepare(_statement, _error);
            return _error.isOK();
        }
        return YES;
    }
    return NO;
}

- (instancetype)where:(const WCDB::Expression &)condition
{
    _statement.where(condition);
    return self;
}

- (instancetype)orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    _statement.orderBy(orders);
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

- (instancetype)groupBy:(const std::list<WCDB::Expression> &)groups
{
    _statement.groupBy(groups);
    return self;
}

- (instancetype)having:(const WCDB::Expression &)having
{
    _statement.having(having);
    return self;
}

- (BOOL)next
{
    if (_statementHandle->step()) {
        return YES;
    }
    [self finalize];
    return NO;
}

- (BOOL)finalize
{
    bool result = true;
    if (_statementHandle) {
        result = _statementHandle->finalize();
        _statementHandle = nullptr;
    }
    return result;
}

@end
