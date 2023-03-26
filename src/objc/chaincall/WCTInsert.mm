//
// Created by sanhuazhang on 2019/05/02
//

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

#import "WCTInsert.h"
#import "Assertion.hpp"
#import "CaseInsensiveList.hpp"
#import "WCTChainCall+Private.h"
#import "WCTHandle+Private.h"
#import "WCTHandle+Transaction.h"
#import "WCTHandle.h"
#import "WCTORM.h"
#import "WCTTryDisposeGuard.h"

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
        if (_values.count > 1) {
            succeed = [_handle lazyRunTransaction:^BOOL(WCTHandle *handle) {
                WCDB_UNUSED(handle)
                return [self realExecute];
            }];
        } else {
            succeed = [self realExecute];
        }
        [self saveChangesAndError:succeed];
    }
    return succeed;
}

- (BOOL)realExecute
{
    Class cls = _values.firstObject.class;
    if (_properties.empty()) {
        _properties = [cls allProperties];
    }

    if (_statement.syntax().columns.empty()) {
        _statement
        .columns(_properties)
        .values(WCDB::BindParameter::bindParameters(_properties.size()));
    }

    std::vector<BOOL> autoIncrementsOfDefinitions;
    if (_statement.syntax().conflictAction != WCDB::Syntax::ConflictAction::Replace) {
        const auto &columnDefs = [cls objectRelationalMapping].getColumnDefs();
        for (const WCTProperty &property : _properties) {
            // auto increment?
            auto iter = columnDefs.caseInsensiveFind(property.getDescription());
            WCTRemedialAssert(iter != columnDefs.end(), "Related property is not found.", return NO;);
            autoIncrementsOfDefinitions.push_back(iter->second.syntax().isAutoIncrement());
        }
    }

    WCDB::Optional<BOOL> respondsToSetLastInsertedRowID;
    WCDB::Optional<BOOL> respondsToIsAutoIncrement;

    BOOL succeed = NO;
    if ([_handle prepare:_statement]) {
        succeed = YES;

        for (WCTObject *value in _values) {
            [_handle reset];
            int index = 1;

            for (const WCTProperty &property : _properties) {
                WCTAssert(autoIncrementsOfDefinitions.empty()
                          || autoIncrementsOfDefinitions.size() == _properties.size());
                if (autoIncrementsOfDefinitions.empty() || !autoIncrementsOfDefinitions[index - 1]) {
                    [_handle bindProperty:property
                                 ofObject:value
                                  toIndex:index];
                } else {
                    if (!respondsToIsAutoIncrement.succeed()) {
                        respondsToIsAutoIncrement = [value respondsToSelector:@selector(isAutoIncrement)];
                    }
                    WCTAssert(respondsToIsAutoIncrement.succeed());

                    BOOL isAutoIncrement = NO;
                    if (respondsToIsAutoIncrement.value()) {
                        isAutoIncrement = value.isAutoIncrement;
                    }

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

            // setup last inserted rowid
            if (!respondsToSetLastInsertedRowID.succeed()) {
                respondsToSetLastInsertedRowID = [value respondsToSelector:@selector(setLastInsertedRowID:)];
            }
            WCTAssert(respondsToSetLastInsertedRowID.succeed());
            if (respondsToSetLastInsertedRowID.value()) {
                [value setLastInsertedRowID:[_handle getLastInsertedRowID]];
            }
        }
        [_handle finalizeStatement];
    }
    WCTAssert(![_handle isPrepared]);
    return succeed;
}

@end
