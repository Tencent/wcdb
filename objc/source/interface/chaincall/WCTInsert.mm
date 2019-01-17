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
#import <WCDB/WCTHandle+Transaction.h>
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTTryDisposeGuard.h>

@implementation WCTInsert {
    WCTProperties _properties;
    WCDB::StatementInsert _statement;
    NSArray<WCTObject *> *_values;
}

- (WCDB::StatementInsert &)statement
{
    return _statement;
}

- (instancetype)orReplace
{
    _statement.orReplace();
    return self;
}

- (instancetype)intoTable:(NSString *)tableName
{
    _statement.insertIntoTable(tableName);
    return self;
}

- (instancetype)onProperties:(const WCTProperties &)properties
{
    _properties = properties;
    _statement.columns(properties)
    .values(WCDB::BindParameter::bindParameters(properties.size()));
    return self;
}

- (instancetype)values:(NSArray<WCTObject *> *)objects
{
    _values = objects;
    return self;
}

- (instancetype)value:(WCTObject *)object
{
    return [self values:object != nil ? @[ object ] : nil];
}

- (BOOL)execute
{
    WCTTryDisposeGuard tryDisposeGuard(self);
    BOOL succeed = YES;
    if (_values.count > 0) {
        Class cls = _values.firstObject.class;
        const WCTProperties &properties = _properties.empty() ? [cls allProperties] : _properties;

        if (_statement.syntax().columns.empty()) {
            _statement
            .columns(properties)
            .values(WCDB::BindParameter::bindParameters(properties.size()));
        }

        std::vector<bool> autoIncrements;
        const auto &columnDefs = [cls objectRelationalMapping].getColumnDefs();
        for (const WCTProperty &property : properties) {
            // auto increment?
            bool isAutoIncrement = false;
            if (!_statement.syntax().useConflictAction
                || _statement.syntax().conflictAction != WCDB::Syntax::ConflictAction::Replace // not replace
            ) {
                auto iter = columnDefs.find(property.getDescription());
                WCTRemedialAssert(iter != columnDefs.end(), "Unrelated property is found.", return NO;);
                isAutoIncrement = iter->second.syntax().isAutoIncrement();
            }
            autoIncrements.push_back(isAutoIncrement);
        }

        WCTInnerAssert(autoIncrements.size() == properties.size());
        if (_values.count > 1) {
            succeed = [_handle runNestedTransaction:^BOOL(WCTHandle *handle) {
                return [self realExecute:handle properties:properties autoIncrements:autoIncrements];
            }];
        } else {
            succeed = [self realExecute:_handle properties:properties autoIncrements:autoIncrements];
        }
    }
    return succeed;
}

- (BOOL)realExecute:(WCTHandle *)handle properties:(const WCTProperties &)properties autoIncrements:(const std::vector<bool> &)autoIncrements
{
    BOOL succeed = NO;
    if ([handle prepare:_statement]) {
        BOOL canFillLastInsertedRowID = [_values.firstObject respondsToSelector:@selector(lastInsertedRowID)];
        succeed = YES;
        for (WCTObject *value in _values) {
            int index = 1;
            BOOL isAutoIncrement = NO;
            for (const WCTProperty &property : properties) {
                if (!autoIncrements[index - 1] || !value.isAutoIncrement) {
                    [handle bindProperty:property
                                ofObject:value
                                 toIndex:index];
                } else {
                    [handle bindNullToIndex:index];
                    isAutoIncrement = YES;
                }
                ++index;
            }
            if (![handle step]) {
                succeed = NO;
                break;
            }
            if (isAutoIncrement && canFillLastInsertedRowID) {
                value.lastInsertedRowID = [handle getLastInsertedRowID];
            }
            [handle reset];
        }
        [handle finalizeStatement];
    }
    return succeed;
}

@end
