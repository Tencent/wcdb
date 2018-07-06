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
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTDatabase (ConvenientSelect)
#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] autoFinalizeImmediately] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
{
    return [[[[self prepareSelect] ofClass:cls] fromTable:tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

@end

#pragma mark -
@implementation WCTDatabase (ConvenientPartialSelect)
#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
{
    return [[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] autoFinalizeImmediately] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
{
    return [[[[self prepareSelect] onProperties:properties] fromTable:tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareSelect] onProperties:properties] fromTable:tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[self prepareSelect] onProperties:properties] fromTable:tableName] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] onProperties:properties] fromTable:tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

@end

#pragma mark -
@implementation WCTDatabase (ConvenientRowSelect)
#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] orderBy:orders] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] limit:1] offset:offset] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:1] offset:offset] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allRows];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientInsert)
#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
           intoTable:(NSString *)tableName
{
    return [[[self prepareInsert] intoTable:tableName] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)tableName
{
    return [[[self prepareInsert] intoTable:tableName] executeWithObjects:objects];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientInsertOrReplace)
#pragma mark - Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                    intoTable:(NSString *)tableName
{
    return [[[[self prepareInsert] orReplace] intoTable:tableName] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                     intoTable:(NSString *)tableName
{
    return [[[[self prepareInsert] orReplace] intoTable:tableName] executeWithObjects:objects];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientPartialInsert)
#pragma mark - Partial Insert
- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTPropertyList &)properties
           intoTable:(NSString *)tableName
{
    return [[[[self prepareInsert] intoTable:tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties intoTable:(NSString *)tableName
{
    return [[[[self prepareInsert] intoTable:tableName] onProperties:properties] executeWithObjects:objects];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientPartialInsertOrReplace)
#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] executeWithObjects:objects];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientUpdate)
#pragma mark - Update Properties With Object
- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
{
    return [[[[self prepareUpdate] table:tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] table:tableName] onProperties:properties] limit:limit] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:properties] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:properties] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:properties] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientRowUpdate)
#pragma mark - Update Properties With Row
- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
{
    return [[[[self prepareUpdate] table:tableName] onProperties:properties] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] table:tableName] onProperties:properties] limit:limit] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:properties] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:properties] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:properties] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Property With Value
- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
{
    return [[[[self prepareUpdate] table:tableName] onProperties:property] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareUpdate] table:tableName] onProperties:property] where:condition] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] table:tableName] onProperties:property] limit:limit] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:property] where:condition] limit:limit] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:property] orderBy:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] table:tableName] onProperties:property] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:property] where:condition] orderBy:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:property] where:condition] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] onProperties:property] orderBy:orders] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] onProperties:property] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithValue:value];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientDelete)
#pragma mark - Delete
- (BOOL)deleteFromTable:(NSString *)tableName
{
    return [[[self prepareDelete] fromTable:tableName] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
{
    return [[[[self prepareDelete] fromTable:tableName] where:condition] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareDelete] fromTable:tableName] limit:limit] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] fromTable:tableName] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareDelete] fromTable:tableName] limit:limit] offset:offset] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] where:condition] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] execute];
}
@end

#pragma mark -
@implementation WCTDatabase (ConvenientStatement)
#pragma mark - Statement
- (WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement atIndex:(int)index
{
    WCTHandle *handle = [self getHandle];
    WCTValue *value = [handle getValueFromStatement:statement atIndex:index];
    [handle finalizeDatabase];
    return value;
}

- (WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement atIndex:(int)index
{
    WCTHandle *handle = [self getHandle];
    WCTOneColumn *column = [handle getColumnFromStatement:statement atIndex:index];
    [handle finalizeDatabase];
    return column;
}

- (WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    WCTValue *value = [handle getValueFromStatement:statement];
    [handle finalizeDatabase];
    return value;
}

- (WCTOneRow *)getRowFromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    WCTOneRow *row = [handle getRowFromStatement:statement];
    [handle finalizeDatabase];
    return row;
}

- (WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    WCTOneColumn *column = [handle getColumnFromStatement:statement];
    [handle finalizeDatabase];
    return column;
}

- (WCTColumnsXRows *)getRowsFromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    WCTColumnsXRows *rows = [handle getRowsFromStatement:statement];
    [handle finalizeDatabase];
    return rows;
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                          fromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    id object = [handle getObjectOfClass:cls
                           fromStatement:statement];
    [handle finalizeDatabase];
    return object;
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                    fromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    NSArray *objects = [handle getObjectOfClass:cls
                                  fromStatement:statement];
    [handle finalizeDatabase];
    return objects;
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                               fromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    id object = [handle getObjectOnProperties:properties
                                fromStatement:statement];
    [handle finalizeDatabase];
    return object;
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                         fromStatement:(const WCDB::Statement &)statement
{
    WCTHandle *handle = [self getHandle];
    WCTColumnsXRows *rows = [handle getObjectsOnProperties:properties
                                             fromStatement:statement];
    [handle finalizeDatabase];
    return rows;
}

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object
{
    WCTHandle *handle = [self getHandle];
    BOOL result = [handle execute:statement
                       withObject:object];
    [handle finalizeDatabase];
    return result;
}

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object
   onProperties:(const WCTPropertyList &)properties
{
    WCTHandle *handle = [self getHandle];
    BOOL result = [handle execute:statement
                       withObject:object
                     onProperties:properties];
    [handle finalizeDatabase];
    return result;
}

- (BOOL)execute:(const WCDB::Statement &)statement
      withValue:(WCTColumnCodingValue *)value
{
    WCTHandle *handle = [self getHandle];
    BOOL result = [handle execute:statement
                        withValue:value];
    [handle finalizeDatabase];
    return result;
}

- (BOOL)execute:(const WCDB::Statement &)statement
        withRow:(WCTOneRow *)row
{
    WCTHandle *handle = [self getHandle];
    BOOL result = [handle execute:statement
                          withRow:row];
    [handle finalizeDatabase];
    return result;
}

@end
