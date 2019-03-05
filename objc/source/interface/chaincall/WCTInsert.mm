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
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTHandle+Transaction.h>
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTTryDisposeGuard.h>

@implementation WCTInsert {
    WCTProperties _properties;
    WCDB::StatementInsert _statement;
    NSArray<WCTObject *> *_values;
    std::vector<bool> _autoIncrements;
    WCTOptionalBool _canFillLastInsertedRowID;
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
        if (_properties.empty()) {
            _properties = [cls allProperties];
        }

        if (_statement.syntax().columns.empty()) {
            _statement
            .columns(_properties)
            .values(WCDB::BindParameter::bindParameters(_properties.size()));
        }

        _autoIncrements.clear();
        if (_statement.syntax().conflictAction != WCDB::Syntax::ConflictAction::Replace) {
            const auto &columnDefs = [cls objectRelationalMapping].getColumnDefs();
            for (const WCTProperty &property : _properties) {
                // auto increment?
                auto iter = columnDefs.find(property.getDescription());
                WCTRemedialAssert(iter != columnDefs.end(), "Unrelated property is found.", return NO;);
                _autoIncrements.push_back(iter->second.syntax().isAutoIncrement());
            }
        }

        _canFillLastInsertedRowID = nullptr;
        if (_values.count > 1) {
            succeed = [_handle runNestedTransaction:^BOOL(WCTHandle *handle) {
                [handle enableLazyNestedTransaction:YES];
                BOOL succeed = [self realExecute];
                [handle enableLazyNestedTransaction:NO];
                return succeed;
            }];
        } else {
            succeed = [self realExecute];
        }
    }
    return succeed;
}

- (BOOL)realExecute
{
    BOOL succeed = NO;
    if ([_handle prepare:_statement]) {
        WCTOptionalBool isAutoIncrement = nullptr;
        succeed = YES;
        for (WCTObject *value in _values) {
            int index = 1;
            for (const WCTProperty &property : _properties) {
                WCTInnerAssert(_autoIncrements.empty() || _autoIncrements.size() == _properties.size());
                if (_autoIncrements.empty() || !_autoIncrements[index - 1]) {
                    [_handle bindProperty:property
                                 ofObject:value
                                  toIndex:index];
                } else {
                    if (isAutoIncrement.failed()) {
                        isAutoIncrement = value.isAutoIncrement;
                    }
                    WCTInnerAssert(!isAutoIncrement.failed());
                    if (isAutoIncrement) {
                        [_handle bindNullToIndex:index];
                    } else {
                        [_handle bindProperty:property
                                     ofObject:value
                                      toIndex:index];
                    }
                }
                ++index;
            }
            if (![_handle step]) {
                succeed = NO;
                break;
            }
            if (!_autoIncrements.empty()) {
                if (_canFillLastInsertedRowID.failed()) {
                    _canFillLastInsertedRowID = [_values.firstObject respondsToSelector:@selector(lastInsertedRowID)];
                }
                WCTInnerAssert(!_canFillLastInsertedRowID.failed());
                if (_canFillLastInsertedRowID) {
                    if (isAutoIncrement.failed()) {
                        isAutoIncrement = value.isAutoIncrement;
                    }
                    WCTInnerAssert(!isAutoIncrement.failed());
                    if (isAutoIncrement) {
                        value.lastInsertedRowID = [_handle getLastInsertedRowID];
                    }
                }
            }
            [_handle reset];
        }
        [_handle finalizeStatement];
    }
    return succeed;
}

@end
