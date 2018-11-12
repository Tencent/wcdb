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
#import <WCDB/WCTHandle+ChainCall.h>
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTHandle (ConvenientSelect)
#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextObject];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allObjects];
}

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCTResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                             orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orders:orders] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                             orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orders:orders] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orders:orders] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCTResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                             orders:(const WCDB::OrderingTerms &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allValues];
}

#pragma mark - Get One Value
- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orders:orders] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orders:orders] limit:1] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

- (WCTValue *)getValueOnResultColumn:(const WCTResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareRowSelect] onResultColumns:resultColumn] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenUsedUp] nextValue];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
{
    return [[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCTResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareRowSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
           intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] values:objects] invalidateWhenUsedUp] execute];
}

#pragma mark - Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                    intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                     intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] values:objects] invalidateWhenUsedUp] execute];
}

#pragma mark - Partial Insert
- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTProperties &)properties
           intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] intoTable:tableName] onProperties:properties] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] intoTable:tableName] onProperties:properties] values:objects] invalidateWhenUsedUp] execute];
}

#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] value:object] invalidateWhenUsedUp] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] values:objects] execute];
}

#pragma mark - Update Properties To Object
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

#pragma mark - Update Properties To Row
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

#pragma mark - Update Property To Value
- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
{
    return [[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

#pragma mark - Delete
- (BOOL)deleteFromTable:(NSString *)tableName
{
    return [[[[self prepareDelete] fromTable:tableName] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareDelete] fromTable:tableName] where:condition] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] fromTable:tableName] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] fromTable:tableName] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenUsedUp] execute];
}

@end
