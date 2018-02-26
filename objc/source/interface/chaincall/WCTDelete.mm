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
#import <WCDB/WCTDelete.h>

@implementation WCTDelete {
    WCDB::StatementDelete _statement;
    int _changes;
}

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andTableName:(NSString *)tableName
{
    if (self = [super initWithCore:core]) {
        _statement.deleteFrom(tableName.UTF8String);
    }
    return self;
}

- (instancetype)where:(const WCDB::Expression &)expr
{
    _statement.where(expr);
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

- (BOOL)execute
{
    WCDB::RecyclableStatement handleStatement = _core->prepare(_statement, _error);
    if (!handleStatement) {
        return NO;
    }
    handleStatement->step();
    if (!handleStatement->isOK()) {
        _error = handleStatement->getError();
        return NO;
    }
    _changes = handleStatement->getChanges();
    return YES;
}

- (int)changes
{
    return _changes;
}

@end
