//
// Created by sanhuazhang on 2019/05/02
//

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

#import "WCTChainCall+Private.h"
#import "WCTDatabase+Convenient.h"
#import "WCTDelete.h"
#import "WCTInsert.h"
#import "WCTMultiSelect.h"
#import "WCTORM.h"
#import "WCTSelect.h"
#import "WCTSelectable+Private.h"
#import "WCTTable+ChainCall.h"
#import "WCTTable+Convenient.h"
#import "WCTTable+Private.h"
#import "WCTUpdate.h"

@implementation WCTTable (Convenient)

#pragma mark - Get Object
- (WCTObject *)getObject
{
    return [[[self prepareSelect] limit:1] firstObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
{
    return [[[[self prepareSelect] where:condition] limit:1] firstObject];
}

- (WCTObject *)getObjectOrders:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareSelect] orders:orders] limit:1] firstObject];
}

- (WCTObject *)getObjectOffset:(const WCDB::Expression &)offset
{
    return [[[[self prepareSelect] limit:1] offset:offset] firstObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                       orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] where:condition] limit:1] orders:orders] firstObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                       offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] where:condition] limit:1] offset:offset] firstObject];
}

- (WCTObject *)getObjectOrders:(const WCDB::OrderingTerms &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] orders:orders] limit:1] offset:offset] firstObject];
}

- (WCTObject *)getObjectWhere:(const WCDB::Expression &)condition
                       orders:(const WCDB::OrderingTerms &)orders
                       offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] where:condition] orders:orders] limit:1] offset:offset] firstObject];
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

- (NSArray /* <WCTObject*> */ *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
{
    return [[[self prepareSelect] orders:orders] allObjects];
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
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareSelect] where:condition] orders:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
                                           limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] orders:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOffset:(const WCDB::Expression &)offset
                                           limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] where:condition] orders:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrders:(const WCDB::OrderingTerms &)orders
                                           limit:(const WCDB::Expression &)limit
                                          offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] orders:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] where:condition] orders:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Object
- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] limit:1] firstObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:1] firstObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:1] firstObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] limit:1] offset:offset] firstObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:1] firstObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:1] offset:offset] firstObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:1] offset:offset] firstObject];
}

- (WCTObject *)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:1] offset:offset] firstObject];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
{
    return [[[self prepareSelect] onResultColumns:resultColumns] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] orders:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] where:condition] orders:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
{
    return [[[self prepareInsert] value:object] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
{
    return [[[self prepareInsert] values:objects] execute];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
{
    return [[[[self prepareInsert] orReplace] value:object] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
{
    return [[[[self prepareInsert] orReplace] values:objects] execute];
}

- (BOOL)insertOrIgnoreObject:(WCTObject *)object
{
    return [[[[self prepareInsert] orIgnore] value:object] execute];
}

- (BOOL)insertOrIgnoreObjects:(NSArray<WCTObject *> *)objects
{
    return [[[[self prepareInsert] orIgnore] values:objects] execute];
}

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTProperties &)properties
{
    return [[[[self prepareInsert] onProperties:properties] value:object] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties
{
    return [[[[self prepareInsert] onProperties:properties] values:objects] execute];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTProperties &)properties
{
    return [[[[[self prepareInsert] orReplace] onProperties:properties] value:object] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties
{
    return [[[[[self prepareInsert] orReplace] onProperties:properties] values:objects] execute];
}

- (BOOL)insertOrIgnoreObject:(WCTObject *)object onProperties:(const WCTProperties &)properties
{
    return [[[[[self prepareInsert] orIgnore] onProperties:properties] value:object] execute];
}

- (BOOL)insertOrIgnoreObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties
{
    return [[[[[self prepareInsert] orIgnore] onProperties:properties] values:objects] execute];
}

#pragma mark - Update Properties To Object
- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
{
    return [[[[self prepareUpdate] set:properties] toObject:object] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareUpdate] set:properties] toObject:object] where:condition] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] set:properties] toObject:object] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:properties] toObject:object] orders:orders] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] set:properties] toObject:object] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:properties] toObject:object] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                toObject:(WCTObject *)object
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:properties] toObject:object] where:condition] orders:orders] limit:limit] offset:offset] execute];
}

#pragma mark - Update Properties To Row
- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
{
    return [[[[self prepareUpdate] set:properties] toRow:row] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareUpdate] set:properties] toRow:row] where:condition] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] set:properties] toRow:row] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:properties] toRow:row] orders:orders] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] set:properties] toRow:row] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:properties] toRow:row] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperties:(const WCTProperties &)properties
                   toRow:(WCTColumnCodingRow *)row
                   where:(const WCDB::Expression &)condition
                  orders:(const WCDB::OrderingTerms &)orders
                   limit:(const WCDB::Expression &)limit
                  offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:properties] toRow:row] where:condition] orders:orders] limit:limit] offset:offset] execute];
}

#pragma mark - Update Properties With Value
- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
{
    return [[[[self prepareUpdate] set:property] toValue:value] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareUpdate] set:property] toValue:value] where:condition] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareUpdate] set:property] toValue:value] limit:limit] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:property] toValue:value] where:condition] limit:limit] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] set:property] toValue:value] orders:orders] limit:limit] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareUpdate] set:property] toValue:value] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] set:property] toValue:value] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:property] toValue:value] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] set:property] toValue:value] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)updateProperty:(const WCTProperty &)property
               toValue:(WCTColumnCodingValue *)value
                 where:(const WCDB::Expression &)condition
                orders:(const WCDB::OrderingTerms &)orders
                 limit:(const WCDB::Expression &)limit
                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] set:property] toValue:value] where:condition] orders:orders] limit:limit] offset:offset] execute];
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

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
{
    return [[[self prepareDelete] orders:orders] execute];
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

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
                      limit:(const WCDB::Expression &)limit
{
    return [[[[self prepareDelete] orders:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[self prepareDelete] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                    orders:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareDelete] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsOrders:(const WCDB::OrderingTerms &)orders
                      limit:(const WCDB::Expression &)limit
                     offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareDelete] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                    orders:(const WCDB::OrderingTerms &)orders
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] where:condition] orders:orders] limit:limit] offset:offset] execute];
}

#pragma mark - Get Value
- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).orders(orders).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                              offset:(const WCDB::Expression &)offset
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).orders(orders).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).orders(orders).limit(1).offset(offset)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [_database getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).orders(orders).limit(1).offset(offset)];
}

#pragma mark - Get Row
- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).orders(orders).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              offset:(const WCDB::Expression &)offset
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).orders(orders).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).orders(orders).limit(1).offset(offset)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [_database getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).orders(orders).limit(1).offset(offset)];
}

#pragma mark - Get Column
- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    where:(const WCDB::Expression &)condition
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).orders(orders)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    limit:(const WCDB::Expression &)limit
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    where:(const WCDB::Expression &)condition
                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).orders(orders)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    where:(const WCDB::Expression &)condition
                                    limit:(const WCDB::Expression &)limit
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).orders(orders).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    where:(const WCDB::Expression &)condition
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).orders(orders).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).orders(orders).limit(limit).offset(offset)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    where:(const WCDB::Expression &)condition
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                    where:(const WCDB::Expression &)condition
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [_database getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(_tableName).where(condition).orders(orders).limit(limit).offset(offset)];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).orders(orders)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      limit:(const WCDB::Expression &)limit
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).orders(orders)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).orders(orders).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).orders(orders).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).orders(orders).limit(limit).offset(offset)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [_database getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(_tableName).where(condition).orders(orders).limit(limit).offset(offset)];
}

@end
