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
#import <WCDB/WCTExpr.h>
#import <WCDB/handle_statement.hpp>

@implementation WCTDelete {
    WCDB::StatementDelete _statement;
    int _changes;
}

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core andTableName:(NSString *)tableName
{
    if (self = [super initWithCore:core]) {
        if (tableName.length == 0) {
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Delete,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         @"Nil table name".UTF8String,
                                         &_error);
            return self;
        }
        _statement.deleteFrom(tableName.UTF8String);
    }
    return self;
}

- (instancetype)where:(const WCTCondition &)expr
{
    _statement.where(expr);
    return self;
}

- (instancetype)orderBy:(const WCTOrderByList &)orderList
{
    _statement.orderBy(orderList);
    return self;
}

- (instancetype)limit:(const WCTLimit &)limit
{
    _statement.limit(limit);
    return self;
}

- (instancetype)offset:(const WCTOffset &)offset
{
    _statement.offset(offset);
    return self;
}

- (BOOL)execute
{
    WCDB::ScopedTicker scopedTicker(_ticker);
    WCDB::RecyclableStatement statementHandle = _core->prepare(_statement, _error);
    if (!statementHandle) {
        return NO;
    }
    statementHandle->step();
    if (!statementHandle->isOK()) {
        _error = statementHandle->getError();
        return NO;
    }
    _changes = statementHandle->getChanges();
    return YES;
}

- (int)changes
{
    return _changes;
}

@end
