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
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTSelectBase+Private.h>
#import <WCDB/WCTSelectBase.h>
#import <WCDB/handle_statement.hpp>

@implementation WCTSelectBase

- (instancetype)initWithResultList:(const WCTResultList &)resultList fromTables:(NSArray<NSString *> *)tableNames
{
    if (self = [super init]) {
        if (resultList.size() == 0) {
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Select,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         [NSString stringWithFormat:@"Selecting nothing from %@ is invalid", tableNames].UTF8String,
                                         &_error);
            return self;
        }
        if (tableNames.count == 0) {
            WCDB::Error::ReportInterface(_core->getTag(),
                                         _core->getPath(),
                                         WCDB::Error::InterfaceOperation::Select,
                                         WCDB::Error::InterfaceCode::Misuse,
                                         @"Empty table".UTF8String,
                                         &_error);
            return self;
        }
        WCDB::SubqueryList subqueryList;
        for (NSString *tableName in tableNames) {
            subqueryList.push_back(tableName.UTF8String);
        }
        _statement.select(resultList, resultList.isDistinct()).from(subqueryList);
    }
    return self;
}

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

- (instancetype)where:(const WCTCondition &)condition
{
    _statement.where(condition);
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

- (instancetype)groupBy:(const WCTGroupByList &)groupByList
{
    WCDB::ExprList exprList;
    for (const WCTGroupBy &groupBy : groupByList) {
        exprList.push_back(groupBy);
    }
    _statement.groupBy(exprList);
    return self;
}

- (instancetype)having:(const WCTHaving &)having
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

- (void)finalize
{
    _statementHandle = nullptr;
}

@end
