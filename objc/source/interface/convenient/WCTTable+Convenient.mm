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

#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTTable+Private.h>

@implementation WCTTable (Convenient)

#pragma mark - Get Object
- (WCTObject *)getObject
{
    return [[[[self prepareSelect] autoFinalizeImmediately] limit:1] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
{
    return [[[[[self prepareSelect] autoFinalizeImmediately] where:condition] limit:1] nextObject];
}

- (WCTObject *)getObjectOrderBy:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] autoFinalizeImmediately] order:orders] limit:1] nextObject];
}

- (WCTObject *)getObjectOffset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] autoFinalizeImmediately] limit:1] offset:offset] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                        order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] autoFinalizeImmediately] where:condition] limit:1] order:orders] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                       offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] autoFinalizeImmediately] where:condition] limit:1] offset:offset] nextObject];
}

- (WCTObject *)getObjectOrderBy:(const WCDB::OrderingTerms &)orders
                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] autoFinalizeImmediately] order:orders] limit:1] offset:offset] nextObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                        order:(const WCDB::OrderingTerms &)orders
                       offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] where:condition] order:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjects
{
    return [[self prepareSelect] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
{
    return [[[self prepareSelect] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderingTerms &)orders
{
    return [[[self prepareSelect] order:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)limit
{
    return [[[self prepareSelect] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOffset:(const WCDB::Expression &)offset
{
    return [[[self prepareSelect] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          order:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareSelect] where:condition] order:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] order:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)offset
                                         offset:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          order:(const WCDB::OrderingTerms &)orders
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] where:condition] order:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] order:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          order:(const WCDB::OrderingTerms &)orders
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] where:condition] order:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Object
- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
{
    return [[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] limit:1] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] limit:1] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] order:orders] limit:1] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] limit:1] offset:offset] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] order:orders] limit:1] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] limit:1] offset:offset] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] order:orders] limit:1] offset:offset] nextObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] order:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
{
    return [[[self prepareSelect] onResultColumns:resultColumns] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    order:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] order:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] order:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    order:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] order:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    order:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] order:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    order:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] order:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    order:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] order:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
{
    return [[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                         order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] order:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] order:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                         order:(const WCDB::OrderingTerms &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] order:orders] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumns] where:condition] order:orders] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
{
    return [[[self prepareRowSelect] onResultColumns:resultColumn] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              order:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumn] order:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumn] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] order:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              order:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] order:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              order:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] order:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              order:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] order:orders] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              order:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] where:condition] order:orders] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
{
    return [[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] where:condition] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] order:orders] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] limit:1] offset:offset] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] where:condition] order:orders] limit:1] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] where:condition] limit:1] offset:offset] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         order:(const WCDB::OrderingTerms &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] order:orders] limit:1] offset:offset] nextValue];
}

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] autoFinalizeImmediately] onResultColumns:resultColumn] where:condition] order:orders] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
{
    return [[[self prepareRowSelect] onResultColumns:resultColumns] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                where:(const WCDB::Expression &)condition
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                order:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumns] order:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareRowSelect] onResultColumns:resultColumns] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                where:(const WCDB::Expression &)condition
                                order:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] order:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                order:(const WCDB::OrderingTerms &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] order:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                where:(const WCDB::Expression &)condition
                                order:(const WCDB::OrderingTerms &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] order:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                order:(const WCDB::OrderingTerms &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] order:orders] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                where:(const WCDB::Expression &)condition
                                order:(const WCDB::OrderingTerms &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] where:condition] order:orders] limit:limit] offset:offset] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
{
    return [[self prepareInsert] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
{
    return [[self prepareInsert] executeWithObjects:objects];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
{
    return [[[self prepareInsert] orReplace] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
{
    return [[[self prepareInsert] orReplace] executeWithObjects:objects];
}

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTProperties &)properties
{
    return [[[self prepareInsert] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties
{
    return [[[self prepareInsert] onProperties:properties] executeWithObjects:objects];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTProperties &)properties
{
    return [[[[self prepareInsert] orReplace] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties
{
    return [[[[self prepareInsert] orReplace] onProperties:properties] executeWithObjects:objects];
}

#pragma mark - Update Properties With Object
- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
{
    return [[[self prepareUpdate] onProperties:properties] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
{
    return [[[[self prepareUpdate] onProperties:properties] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareUpdate] onProperties:properties] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] onProperties:properties] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] onProperties:properties] order:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareUpdate] onProperties:properties] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] onProperties:properties] where:condition] order:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] onProperties:properties] order:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] onProperties:properties] where:condition] order:orders] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Properties With Row
- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
{
    return [[[self prepareUpdate] onProperties:properties] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
{
    return [[[[self prepareUpdate] onProperties:properties] where:condition] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareUpdate] onProperties:properties] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] onProperties:properties] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] onProperties:properties] order:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareUpdate] onProperties:properties] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] onProperties:properties] where:condition] order:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] onProperties:properties] order:orders] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTProperties &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         order:(const WCDB::OrderingTerms &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] onProperties:properties] where:condition] order:orders] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Properties With Value
- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
{
    return [[[self prepareUpdate] onProperties:property] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
{
    return [[[[self prepareUpdate] onProperties:property] where:condition] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareUpdate] onProperties:property] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] onProperties:property] where:condition] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] onProperties:property] order:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareUpdate] onProperties:property] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] onProperties:property] where:condition] order:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] onProperties:property] where:condition] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] onProperties:property] order:orders] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       order:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] onProperties:property] where:condition] order:orders] limit:limit] offset:offset] executeWithValue:value];
}

#pragma mark - Delete
- (BOOL)deleteObjects
{
    return [[self prepareDelete] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
{
    return [[[self prepareDelete] where:condition] execute];
}

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderingTerms &)orders
{
    return [[[self prepareDelete] order:orders] execute];
}

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
{
    return [[[self prepareDelete] limit:limit] execute];
}

- (BOOL)deleteObjectsOffset:(const WCDB::Expression &)offset
{
    return [[[self prepareDelete] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareDelete] where:condition] limit:limit] execute];
}

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareDelete] order:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[self prepareDelete] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     order:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] where:condition] order:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareDelete] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderingTerms &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareDelete] order:orders] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     order:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] where:condition] order:orders] limit:limit] offset:offset] execute];
}

@end
