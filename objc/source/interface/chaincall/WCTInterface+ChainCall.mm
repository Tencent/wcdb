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
    return [[WCTInsert alloc] initWithCore:_core andProperties:[cls AllProperties] andTableName:tableName andReplaceFlag:NO];
}

- (WCTInsert *)prepareInsertOrReplaceObjectsOfClass:(Class)cls into:(NSString *)tableName
{
    return [[WCTInsert alloc] initWithCore:_core andProperties:[cls AllProperties] andTableName:tableName andReplaceFlag:YES];
}

- (WCTInsert *)prepareInsertObjectsOnProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName
{
    return [[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:tableName andReplaceFlag:NO];
}

- (WCTInsert *)prepareInsertOrReplaceObjectsOnProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName
{
    return [[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:tableName andReplaceFlag:YES];
}

- (WCTDelete *)prepareDeleteFromTable:(NSString *)tableName
{
    return [[WCTDelete alloc] initWithCore:_core andTableName:tableName];
}

- (WCTUpdate *)prepareUpdateTable:(NSString *)tableName onProperties:(const WCTPropertyList &)propertyList
{
    return [[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName];
}

- (WCTSelect *)prepareSelectObjectsOfClass:(Class)cls fromTable:(NSString *)tableName
{
    return [[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO];
}

- (WCTSelect *)prepareSelectObjectsOfClass:(Class)cls fromTable:(NSString *)tableName isDistinct:(BOOL)isDistinct
{
    return [[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:isDistinct];
}

- (WCTSelect *)prepareSelectObjectsOnProperties:(const WCTPropertyList &)propertyList fromTable:(NSString *)tableName
{
    return [[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO];
}

- (WCTSelect *)prepareSelectObjectsOnProperties:(const WCTPropertyList &)propertyList fromTable:(NSString *)tableName isDistinct:(BOOL)isDistinct
{
    return [[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:isDistinct];
}

- (WCTRowSelect *)prepareSelectRowsFromTable:(NSString *)tableName
{
    return [[WCTRowSelect alloc] initWithCore:_core andColumnResultList:{WCDB::Column::All} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCDB::ColumnResultList &)resultList fromTable:(NSString *)tableName
{
    return [[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCDB::ColumnResultList &)resultList fromTable:(NSString *)tableName isDistinct:(BOOL)isDistinct
{
    return [[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName ? @[ tableName ] : @[] isDistinct:isDistinct];
}

- (WCTRowSelect *)prepareSelectRowsFromTables:(NSArray<NSString *> *)tableNames
{
    return [[WCTRowSelect alloc] initWithCore:_core andColumnResultList:{WCDB::Column::All} fromTables:tableNames isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCDB::ColumnResultList &)resultList fromTables:(NSArray<NSString *> *)tableNames
{
    return [[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableNames isDistinct:NO];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCDB::ColumnResultList &)resultList fromTables:(NSArray<NSString *> *)tableNames isDistinct:(BOOL)isDistinct
{
    return [[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableNames isDistinct:isDistinct];
}

- (WCTMultiSelect *)prepareSelectMultiObjectsOnProperties:(const WCTPropertyList &)propertyList fromTables:(NSArray<NSString *> *)tableNames
{
    return [[WCTMultiSelect alloc] initWithCore:_core andPropertyList:propertyList fromTables:tableNames isDistinct:NO];
}

- (WCTMultiSelect *)prepareSelectMultiObjectsOnProperties:(const WCTPropertyList &)propertyList fromTables:(NSArray<NSString *> *)tableNames isDistinct:(BOOL)isDistinct
{
    return [[WCTMultiSelect alloc] initWithCore:_core andPropertyList:propertyList fromTables:tableNames isDistinct:isDistinct];
}

@end
