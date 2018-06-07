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
#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTRowSelect

- (instancetype)onResultColumns:(const std::list<WCDB::ResultColumn> &)resultColumns
{
    _statement.select(resultColumns);
    return self;
}

- (instancetype)onResultColumn:(const WCDB::ResultColumn &)resultColumn
{
    _statement.select(resultColumn);
    return self;
}

- (instancetype)fromTable:(NSString *)tableName
{
    WCTRemedialAssert(tableName, "Table name can't be null.", return self;);
    _statement.from(tableName.cppString);
    return self;
}

- (instancetype)fromTables:(NSArray<NSString *> *)tableNames
{
    WCTRemedialAssert(tableNames, "Table names can't be null.", return self;);
    std::list<WCDB::TableOrSubquery> tables;
    for (NSString *tableName in tableNames) {
        tables.push_back(tableName.cppString);
    }
    _statement.from(tables);
    return self;
}

- (WCTColumnsXRows *)allRows
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
    WCTColumnsXRows *rows = [super allRows];
    [self doAutoFinalize:!rows];
    return rows;
}

- (WCTOneRow *)nextRow
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
    BOOL done = NO;
    WCTOneRow *row = [self nextRowOrDone:done];
    if (!row || _finalizeImmediately) {
        [self doAutoFinalize:!done];
    }
    return row;
}

- (WCTOneColumn *)allValues
{
    return [self allValuesAtIndex:0];
}

- (WCTOneColumn *)allValuesAtIndex:(int)index
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
    WCTOneColumn *column = [super allValuesAtIndex:index];
    [self doAutoFinalize:!column];
    return column;
}

- (WCTValue *)nextValue
{
    return [self nextValueAtIndex:0];
}

- (WCTValue *)nextValueAtIndex:(int)index
{
    if (![self lazyPrepare]) {
        [self doAutoFinalize:YES];
        return nil;
    }
    BOOL done = NO;
    WCTValue *value = [self nextValueAtIndex:index orDone:done];
    if (!value || _finalizeImmediately) {
        [self doAutoFinalize:!done];
    }
    return value;
}

- (BOOL)lazyPrepare
{
    if (_statement.isResultColumnsNotSet()) {
        _statement.select(WCDB::ResultColumn::all());
    }
    return [super lazyPrepare];
}

@end
