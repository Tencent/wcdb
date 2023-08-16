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

#import "Assertion.hpp"
#import "WCTChainCall+Private.h"
#import "WCTDatabase+ChainCall.h"
#import "WCTDatabase+Convenient.h"
#import "WCTDatabase+Handle.h"
#import "WCTDelete.h"
#import "WCTHandle+Private.h"
#import "WCTHandle.h"
#import "WCTInsert.h"
#import "WCTMultiSelect.h"
#import "WCTORM.h"
#import "WCTSelect.h"
#import "WCTUpdate.h"

@implementation WCTDatabase (Convenient)
#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] offset:offset] firstObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] offset:offset] firstObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] offset:offset] firstObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] firstObject];
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
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] allObjects];
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
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] allObjects];
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
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] allObjects];
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
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] offset:offset] allObjects];
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
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] offset:offset] firstObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] firstObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] offset:offset] firstObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] offset:offset] firstObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] firstObject];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
{
    return [[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
           intoTable:(NSString *)tableName
{
    return [[[[self prepareInsert] intoTable:tableName] value:object] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)tableName
{
    return [[[[self prepareInsert] intoTable:tableName] values:objects] execute];
}

#pragma mark - Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                    intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] orReplace] intoTable:tableName] value:object] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                     intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] orReplace] intoTable:tableName] values:objects] execute];
}

#pragma mark - Insert or Ignore
- (BOOL)insertOrIgnoreObject:(WCTObject *)object
                   intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] orIgnore] intoTable:tableName] value:object] execute];
}

- (BOOL)insertOrIgnoreObjects:(NSArray<WCTObject *> *)objects
                    intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] orIgnore] intoTable:tableName] values:objects] execute];
}

#pragma mark - Partial Insert
- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTProperties &)properties
           intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] onProperties:properties] value:object] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] onProperties:properties] values:objects] execute];
}

#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] value:object] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] values:objects] execute];
}

#pragma mark - Partial Insert or Ignore
- (BOOL)insertOrIgnoreObject:(WCTObject *)object onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orIgnore] intoTable:tableName] onProperties:properties] value:object] execute];
}

- (BOOL)insertOrIgnoreObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orIgnore] intoTable:tableName] onProperties:properties] values:objects] execute];
}

#pragma mark - Update Properties To Object
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
{
    return [[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] offset:offset] execute];
}

#pragma mark - Update Properties To Row
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
{
    return [[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] offset:offset] execute];
}

#pragma mark - Update Property To Value
- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
{
    return [[[[[self prepareUpdate] table:tableName] set:property] toValue:value] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] offset:offset] execute];
}

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
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[self prepareDelete] fromTable:tableName] limit:limit] offset:offset] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] offset:offset] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] execute];
}

#pragma mark - Get Value
- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                              offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(1).offset(offset)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(1).offset(offset)];
}

#pragma mark - Get Row
- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                              offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(1).offset(offset)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(1).offset(offset)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
                              orders:(const WCDB::OrderingTerms &)orders
                              offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(1).offset(offset)];
}

#pragma mark - Get Column
- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                    limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(limit)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(limit).offset(offset)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                   orders:(const WCDB::OrderingTerms &)orders
                                    limit:(const WCDB::Expression &)limit
                                   offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(limit).offset(offset)];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid())];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(limit)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(limit).offset(offset)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).order(WCDB::OrderingTerm::ascendingRowid()).limit(limit).offset(offset)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
                                     orders:(const WCDB::OrderingTerms &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(limit).offset(offset)];
}

#pragma mark - Get From Statement
- (WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement
{
    WCTValue *result = nil;
    WCTHandle *handle = [self getHandle];
    [handle setWriteHint:NO];
    if ([handle prepare:statement]) {
        if ([handle step]) {
            result = [handle extractValueAtIndex:0];
        }
        [handle finalizeStatement];
    }
    [handle invalidate];
    return result;
}

- (WCTOneRow *)getRowFromStatement:(const WCDB::Statement &)statement
{
    WCTOneRow *result = nil;
    WCTHandle *handle = [self getHandle];
    [handle setWriteHint:NO];
    if ([handle prepare:statement]) {
        if ([handle step]) {
            result = [handle extractRow];
        }
        [handle finalizeStatement];
    };
    [handle invalidate];
    return result;
}

- (WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement
{
    WCTOneColumn *result = nil;
    WCTHandle *handle = [self getHandle];
    [handle setWriteHint:NO];
    if ([handle prepare:statement]) {
        result = [handle allValues];
        [handle finalizeStatement];
    }
    [handle invalidate];
    return result;
}

- (WCTColumnsXRows *)getRowsFromStatement:(const WCDB::Statement &)statement
{
    WCTColumnsXRows *result = nil;
    WCTHandle *handle = [self getHandle];
    [handle setWriteHint:NO];
    if ([handle prepare:statement]) {
        result = [handle allRows];
        [handle finalizeStatement];
    }
    [handle invalidate];
    return result;
}

@end
