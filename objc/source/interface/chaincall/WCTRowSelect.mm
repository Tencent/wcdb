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

#import <WCDB/WCDB.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTRowSelect

- (instancetype)initWithDatabase:(const std::shared_ptr<WCDB::Database> &)database
             andRecyclableHandle:(const WCDB::RecyclableHandle &)handle
{
    if (self = [super initWithDatabase:database
                   andRecyclableHandle:handle]) {
        _statement.select(WCDB::ResultColumn::All);
    }
    return self;
}

- (instancetype)onResultColumn:(const WCDB::ResultColumn &)resultColumn
{
    _statement.select(resultColumn);
    return self;
}

- (instancetype)onResultColumns:(const std::list<WCDB::ResultColumn> &)resultColumns
{
    _statement.select(resultColumns);
    return self;
}

- (instancetype)fromTable:(NSString *)tableName
{
    _statement.from(tableName.UTF8String);
    return self;
}

- (instancetype)fromTables:(NSArray<NSString *> *)tableNames
{
    std::list<WCDB::TableOrSubquery> tables;
    for (NSString *tableName in tableNames) {
        tables.push_back(tableName.UTF8String);
    }
    _statement.from(tables);
    return self;
}

- (WCTColumnsXRows *)allRows
{
    if (![self lazyPrepare]) {
        return nil;
    }
    NSMutableArray *rows = [NSMutableArray array];
    bool done;
    while (_handle->step(done) && !done) {
        [rows addObject:[self getRow]];
    }
    _handle->finalize();
    return done ? rows : nil;
}

- (WCTOneRow *)nextRow
{
    if (![self lazyPrepare]) {
        return nil;
    }
    bool done;
    if (!_handle->step(done) || done) {
        _handle->finalize();
        return nil;
    }
    return [self getRow];
}

- (WCTOneColumn *)allValues
{
    if (![self lazyPrepare]) {
        return nil;
    }
    NSMutableArray *columns = [[NSMutableArray alloc] init];
    bool done;
    while (_handle->step(done) && !done) {
        NSObject *value = [self getValueAtIndex:0];
        [columns addObject:value ? value : [NSNull null]];
    }
    _handle->finalize();
    return done ? columns : nil;
}

- (WCTValue *)nextValue
{
    if (![self lazyPrepare]) {
        return nil;
    }
    bool done;
    if (!_handle->step(done) || done) {
        _handle->finalize();
        return nil;
    }
    return [self getValueAtIndex:0];
}

@end
