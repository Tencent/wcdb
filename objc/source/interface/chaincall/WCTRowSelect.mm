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
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUsedUpInvalidateGuard.h>

@implementation WCTRowSelect

- (instancetype)onResultColumns:(const WCTResultColumns &)resultColumns
{
    _statement.select(resultColumns);
    return self;
}

- (instancetype)fromTable:(NSString *)tableName
{
    WCTRemedialAssert(tableName != nil, @"Table name can't be null.", return self;);
    return [self fromTables:@[ tableName ]];
}

- (instancetype)fromTables:(NSArray<NSString *> *)tableNames
{
    WCTRemedialAssert(tableNames != nil, @"Table names can't be null.", return self;);
    WCDB::TablesOrSubqueries tables;
    for (NSString *tableName in tableNames) {
        tables.push_back(tableName);
    }
    _statement.from(tables);
    return self;
}

- (WCTColumnsXRows *)allRows
{
    WCTUsedUpInvalidateGuard usedUpInvalidateGuard(self);
    if (![self lazyPrepare]) {
        return nil;
    }
    WCTColumnsXRows *rows = [_handle allRows];
    [_handle finalizeStatement];
    return rows;
}

- (WCTOneRow *)nextRow
{
    WCTUsedUpInvalidateGuard usedUpInvalidateGuard(self);
    if (![self lazyPrepare]) {
        return nil;
    }
    BOOL done = NO;
    if (![_handle step:done] || done) {
        [_handle finalizeStatement];
        return nil;
    }
    return [_handle getRow];
}

- (WCTOneColumn *)allValues
{
    return [self allValuesAtIndex:0];
}

- (WCTOneColumn *)allValuesAtIndex:(int)index
{
    WCTUsedUpInvalidateGuard usedUpInvalidateGuard(self);
    if (![self lazyPrepare]) {
        return nil;
    }
    WCTOneColumn *column = [_handle allValuesAtIndex:index];
    [_handle finalizeStatement];
    return column;
}

- (WCTValue *)nextValue
{
    return [self nextValueAtIndex:0];
}

- (WCTValue *)nextValueAtIndex:(int)index
{
    WCTUsedUpInvalidateGuard usedUpInvalidateGuard(self);
    if (![self lazyPrepare]) {
        return nil;
    }
    BOOL done = NO;
    if (![_handle step:done] || done) {
        [_handle finalizeStatement];
        return nil;
    }
    return [_handle getValueAtIndex:index];
}

@end
