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

#import <WCDB/WCTCoding.h>
#import <WCDB/WCTDelete+Private.h>
#import <WCDB/WCTDelete.h>
#import <WCDB/WCTInsert+Private.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTInterface+ChainCall.h>
#import <WCDB/WCTMultiSelect+Private.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTRowSelect+Private.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect+Private.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTUpdate+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTInterface (ChainCall)

- (WCTInsert *)prepareInsertObjectsOfClass:(Class)cls into:(NSString *)tableName
{
    return [[WCTInsert alloc] initWithDatabase:_database andProperties:[cls AllProperties] andTableName:tableName andReplaceFlag:NO];
}

- (WCTInsert *)prepareInsertOrReplaceObjectsOfClass:(Class)cls into:(NSString *)tableName
{
    return [[WCTInsert alloc] initWithDatabase:_database andProperties:[cls AllProperties] andTableName:tableName andReplaceFlag:YES];
}

- (WCTInsert *)prepareInsertObjectsOnProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    return [[WCTInsert alloc] initWithDatabase:_database andProperties:properties andTableName:tableName andReplaceFlag:NO];
}

- (WCTInsert *)prepareInsertOrReplaceObjectsOnProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    return [[WCTInsert alloc] initWithDatabase:_database andProperties:properties andTableName:tableName andReplaceFlag:YES];
}

- (WCTDelete *)prepareDeleteFromTable:(NSString *)tableName
{
    return [[WCTDelete alloc] initWithDatabase:_database andTableName:tableName];
}

- (WCTUpdate *)prepareUpdateTable:(NSString *)tableName onProperties:(const WCTPropertyList &)properties
{
    return [[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName];
}

- (WCTSelect *)prepareSelectObjectsOfClass:(Class)cls fromTable:(NSString *)tableName
{
    return [[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO];
}

- (WCTSelect *)prepareSelectObjectsOfClass:(Class)cls fromTable:(NSString *)tableName isDistinct:(BOOL)isDistinct
{
    return [[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:isDistinct];
}

- (WCTSelect *)prepareSelectObjectsOnProperties:(const WCTPropertyList &)properties fromTable:(NSString *)tableName
{
    return [[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO];
}

- (WCTSelect *)prepareSelectObjectsOnProperties:(const WCTPropertyList &)properties fromTable:(NSString *)tableName isDistinct:(BOOL)isDistinct
{
    return [[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:isDistinct];
}

- (WCTRowSelect *)prepareSelectRowsFromTable:(NSString *)tableName
{
    return [[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{WCDB::ResultColumn::All} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns fromTable:(NSString *)tableName
{
    return [[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns fromTable:(NSString *)tableName isDistinct:(BOOL)isDistinct
{
    return [[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:isDistinct];
}

- (WCTRowSelect *)prepareSelectRowsFromTables:(NSArray<NSString *> *)tableNames
{
    return [[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{WCDB::ResultColumn::All} fromTables:tableNames isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns fromTables:(NSArray<NSString *> *)tableNames
{
    return [[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableNames isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns fromTables:(NSArray<NSString *> *)tableNames isDistinct:(BOOL)isDistinct
{
    return [[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableNames isDistinct:isDistinct];
}

- (WCTMultiSelect *)prepareSelectMultiObjectsOnProperties:(const WCTPropertyList &)properties fromTables:(NSArray<NSString *> *)tableNames
{
    return [[WCTMultiSelect alloc] initWithDatabase:_database andProperties:properties fromTables:tableNames isDistinct:NO];
}

- (WCTMultiSelect *)prepareSelectMultiObjectsOnProperties:(const WCTPropertyList &)properties fromTables:(NSArray<NSString *> *)tableNames isDistinct:(BOOL)isDistinct
{
    return [[WCTMultiSelect alloc] initWithDatabase:_database andProperties:properties fromTables:tableNames isDistinct:isDistinct];
}

@end
