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

@implementation WCTInsert {
    WCTProperties _properties;
    WCDB::StatementInsert _statement;
    NSArray<WCTObject *> *_values;
}

- (void)invalidate
{
    [super invalidate];
    _properties.clear();
    _values = nil;
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
    .values(WCDB::BindParameter((int) properties.size()));
    return self;
}

- (instancetype)values:(NSArray<WCTObject *> *)objects
{
    _values = objects;
    return self;
}

- (instancetype)value:(WCTObject *)object
{
    _values = object != nil ? @[ object ] : nil;
    return self;
}

- (BOOL)execute
{
    WCTUsedUpInvalidateGuard usedUpInvalidateGuard(self);
    if (_values.count == 0) {
        return YES;
    }

    const WCTProperties &properties = _properties.empty() ? [_values.firstObject.class allProperties] : _properties;

    if (_statement.syntax().columns.empty()) {
        _statement
        .columns(properties)
        .values(WCDB::BindParameter::bindParameters(properties.size()));
    }

    std::vector<bool> autoIncrements;
    for (const WCTProperty &property : properties) {
        const WCTColumnBinding &columnBinding = property.getColumnBinding();

        // auto increment?
        bool isAutoIncrement = false;
        if (_statement.syntax().switcher != WCDB::StatementInsert::SyntaxType::Switch::InsertOrReplace // not replace
        ) {
            for (const auto &constraint : columnBinding.columnDef.syntax().constraints) {
                if (constraint.switcher == WCDB::ColumnConstraint::SyntaxType::Switch::PrimaryKey) {
                    isAutoIncrement = constraint.autoIncrement;
                    break;
                }
            }
        }
        autoIncrements.push_back(isAutoIncrement);
    }

    WCTInnerAssert(autoIncrements.size() == properties.size());
    return [_handle runNestedTransaction:^BOOL(WCTHandle *handle) {
        int index = 1;
        BOOL canFillLastInsertedRowID = [self->_values.firstObject respondsToSelector:@selector(lastInsertedRowID)];
        for (WCTObject *value in self->_values) {
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
                return NO; // rollback
            }
            if (isAutoIncrement && canFillLastInsertedRowID) {
                value.lastInsertedRowID = [handle getLastInsertedRowID];
            }
            [handle reset];
        }
        [handle finalizeStatement];
        return YES;
    }];
}

@end
