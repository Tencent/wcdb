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
#import <WCDB/WCTCommon.h>
#import <WCDB/WCTDelete+Private.h>
#import <WCDB/WCTDelete.h>
#import <WCDB/WCTInsert+Private.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTInterface+Convenient.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTRowSelect+Private.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect+Private.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTUpdate+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTInterface (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
{
    return [[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                             offset:(const WCDB::Expression &)offset
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] orderBy:orders] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      limit:(const WCDB::Expression &)limit
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] orderBy:orders] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] orderBy:orders] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] orderBy:orders] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:{resultColumn} fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:1] offset:offset] nextValue];
}

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
                                        offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                        offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                        offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] orderBy:orders] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                        offset:(const WCDB::Expression &)offset
{
    std::list<WCDB::ResultColumn> resultColumns = {resultColumn};
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:YES] where:condition] orderBy:orders] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
{
    return [[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
{
    return [[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database andProperties:properties fromTable:tableName isDistinct:NO] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
{
    return [[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
{
    return [[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] orderBy:orders] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database andResultColumns:resultColumns fromTables:tableName ? @[ tableName ] : @[] isDistinct:NO] where:condition] orderBy:orders] limit:limit] offset:offset] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
                into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:[object.class AllProperties] andTableName:tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
                 into:(NSString *)tableName
{
    if (objects.count > 0) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:[objects.firstObject.class AllProperties] andTableName:tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
                         into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:[object.class AllProperties] andTableName:tableName andReplaceFlag:YES] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                          into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:[objects.firstObject.class AllProperties] andTableName:tableName andReplaceFlag:YES] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:properties andTableName:tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:properties andTableName:tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:properties andTableName:tableName andReplaceFlag:YES] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithDatabase:_database andProperties:properties andTableName:tableName andReplaceFlag:YES] executeWithObjects:objects];
    }
    return NO;
}

#pragma mark - Update Properties With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Property With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Properties With Row
- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
{
    return [[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] orderBy:orders] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] orderBy:orders] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:properties andTableName:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Property With Value
- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
{
    return [[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] orderBy:orders] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] orderBy:orders] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] orderBy:orders] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] orderBy:orders] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] orderBy:orders] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database andProperties:{property} andTableName:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

#pragma mark - Delete
- (BOOL)deleteObjectsFromTable:(NSString *)tableName
{
    return [[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
{
    return [[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] where:condition] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] orderBy:orders] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit
{
    return [[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] where:condition] orderBy:orders] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] where:condition] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] where:condition] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] orderBy:orders] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTDelete alloc] initWithDatabase:_database andTableName:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] execute];
}

@end
