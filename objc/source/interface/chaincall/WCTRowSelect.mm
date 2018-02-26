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

#import <WCDB/HandleStatement.hpp>
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTRowSelect+Private.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelectBase+Private.h>

@implementation WCTRowSelect

- (instancetype)initWithCore:(const std::shared_ptr<WCDB::CoreBase> &)core
            andResultColumns:(const std::list<WCDB::ResultColumn> &)resultColumns
                  fromTables:(NSArray<NSString *> *)tableNames
                  isDistinct:(BOOL)isDistinct
{
    if (self = [super initWithCore:core]) {
        std::list<WCDB::TableOrSubquery> tableOrSubquerys;
        for (NSString *tableName in tableNames) {
            tableOrSubquerys.push_back(tableName.UTF8String);
        }
        if (isDistinct) {
            _statement.distinct();
        }
        _statement.select(resultColumns).from(tableOrSubquerys);
    }
    return self;
}

- (WCTColumnsXRows *)allRows
{
    if ([self lazyPrepare]) {
        NSMutableArray *allRows = [NSMutableArray array];
        NSMutableArray *row = nil;
        while ([self next]) {
            row = [NSMutableArray array];
            if ([self extractValueToRow:row]) {
                [allRows addObject:row];
            } else {
                return nil;
            }
        }
        return _error.isOK() ? allRows : nil;
    }
    return nil;
}

- (WCTOneRow *)nextRow
{
    if ([self lazyPrepare] && [self next]) {
        NSMutableArray *row = [NSMutableArray array];
        if ([self extractValueToRow:row]) {
            return row;
        }
    }
    return nil;
}

- (WCTOneColumn *)allValues
{
    if ([self lazyPrepare]) {
        NSMutableArray *allValues = [NSMutableArray array];
        WCTValue *value = nil;
        while ([self next]) {
            value = [self extractValue];
            if (!value) {
                value = [NSNull null];
            }
            [allValues addObject:value];
        }
        return _error.isOK() ? allValues : nil;
    }
    return nil;
}

- (WCTValue *)nextValue
{
    if ([self lazyPrepare] && [self next]) {
        return [self extractValue];
    }
    return nil;
}

@end
