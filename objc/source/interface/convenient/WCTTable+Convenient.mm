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

#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTDelete.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTTable+ChainCall.h>
#import <WCDB/WCTTable+Convenient.h>
#import <WCDB/WCTTable+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTTable (Convenient)

#pragma mark - Get Object
- (WCTObject *)getObject
{
    return [[[[self prepareSelect] limit:1] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
{
    return [[[[[self prepareSelect] where:condition] limit:1] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOrders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] orders:orders] limit:1] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOffset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                       orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] where:condition] limit:1] orders:orders] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                       offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOrders:(const WCDB::OrderingTerms &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                       orders:(const WCDB::OrderingTerms &)orders
                       offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjects
{
    return [[[self prepareSelect] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
{
    return [[[[self prepareSelect] where:condition] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareSelect] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOffset:(const WCDB::Expression &)offset
{
    return [[[[self prepareSelect] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] where:condition] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] where:condition] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
                                           limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)offset
                                         offset:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
                                           limit:(const WCDB::Expression &)limit
                                          offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

#pragma mark - Get Part Of Object
- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] limit:1] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:1] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:1] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:1] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] orders:orders] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumn] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                             orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] orders:orders] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                             orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] orders:orders] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] orders:orders] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

#pragma mark - Get One Value
- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] orders:orders] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] orders:orders] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumns] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] orders:orders] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] orders:orders] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] orders:orders] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
{
    return [[[[self prepareInsert] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
{
    return [[[[self prepareInsert] values:objects] invalidateWhenUsedUp] execute];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
{
    return [[[[[self prepareInsert] orReplace] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
{
    return [[[[[self prepareInsert] orReplace] values:objects] invalidateWhenUsedUp] execute];
}

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTProperties &)properties
{
    return [[[[[self prepareInsert] onProperties:properties] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties
{
    return [[[[[self prepareInsert] onProperties:properties] values:objects] invalidateWhenUsedUp] execute];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTProperties &)properties
{
    return [[[[[[self prepareInsert] orReplace] onProperties:properties] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties
{
    return [[[[[[self prepareInsert] orReplace] onProperties:properties] values:objects] invalidateWhenUsedUp] execute];
}

#pragma mark - Update Properties To Object
- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
{
    return [[[[[self prepareUpdate] set:properties] toObject:object] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:properties] toObject:object] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:properties] toObject:object] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:properties] toObject:object] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:properties] toObject:object] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

#pragma mark - Update Properties To Row
- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
{
    return [[[[[self prepareUpdate] set:properties] toRow:row] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:properties] toRow:row] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:properties] toRow:row] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:properties] toRow:row] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:properties] toRow:row] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

#pragma mark - Update Properties With Value
- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
{
    return [[[[[self prepareUpdate] set:property] toValue:value] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareUpdate] set:property] toValue:value] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:property] toValue:value] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:property] toValue:value] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:property] toValue:value] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:property] toValue:value] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] set:property] toValue:value] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:property] toValue:value] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:property] toValue:value] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] set:property] toValue:value] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

#pragma mark - Delete
- (BOOL)deleteObjects
{
    return [[[self prepareDelete] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
{
    return [[[[self prepareDelete] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareDelete] orders:orders] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
{
    return [[[[self prepareDelete] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsOffset:(const WCDB::Expression &)offset
{
    return [[[[self prepareDelete] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
                      limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareDelete] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                    orders:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
                      limit:(const WCDB::Expression &)limit
                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                    orders:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

@end
