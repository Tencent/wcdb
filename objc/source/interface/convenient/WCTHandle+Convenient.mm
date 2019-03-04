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
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTSelectable+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTHandle (Convenient)
#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

- (id /* WCTObject* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                      fromTable:(NSString *)tableName
                                          where:(const WCDB::Expression &)condition
                                         orders:(const WCDB::OrderingTerms &)orders
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] finalizeStatementWhenDispose] nextObject];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                fromTable:(NSString *)tableName
                                                    where:(const WCDB::Expression &)condition
                                                   orders:(const WCDB::OrderingTerms &)orders
                                                    limit:(const WCDB::Expression &)limit
                                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] allObjects];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
           intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] value:object] finalizeStatementWhenDispose] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] values:objects] finalizeStatementWhenDispose] execute];
}

#pragma mark - Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                    intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] value:object] finalizeStatementWhenDispose] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                     intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] values:objects] finalizeStatementWhenDispose] execute];
}

#pragma mark - Partial Insert
- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTProperties &)properties
           intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] intoTable:tableName] onProperties:properties] value:object] finalizeStatementWhenDispose] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] intoTable:tableName] onProperties:properties] values:objects] finalizeStatementWhenDispose] execute];
}

#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] value:object] finalizeStatementWhenDispose] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] values:objects] finalizeStatementWhenDispose] execute];
}

#pragma mark - Update Properties To Object
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

#pragma mark - Update Properties To Row
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

#pragma mark - Update Property To Value
- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
{
    return [[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

#pragma mark - Delete
- (BOOL)deleteFromTable:(NSString *)tableName
{
    return [[[[self prepareDelete] fromTable:tableName] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareDelete] fromTable:tableName] where:condition] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] fromTable:tableName] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] fromTable:tableName] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] finalizeStatementWhenDispose] execute];
}

#pragma mark - Get Value
- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(1)];
}

- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(1)];
}

- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
                                    orders:(const WCDB::OrderingTerms &)orders
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(1)];
}

- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
                                    offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(1).offset(offset)];
}

- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                    orders:(const WCDB::OrderingTerms &)orders
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(1)];
}

- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                    offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(1).offset(offset)];
}

- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
                                    orders:(const WCDB::OrderingTerms &)orders
                                    offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(1).offset(offset)];
}

- (WCTOptionalValue)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                    orders:(const WCDB::OrderingTerms &)orders
                                    offset:(const WCDB::Expression &)offset
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(1).offset(offset)];
}

#pragma mark - Get Row
- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(1)];
}

- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(1)];
}

- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(1)];
}

- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(1).offset(offset)];
}

- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(1)];
}

- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(1).offset(offset)];
}

- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(1).offset(offset)];
}

- (WCTOptionalRow)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 orders:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(1).offset(offset)];
}

#pragma mark - Get Column
- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                      orders:(const WCDB::OrderingTerms &)orders
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(limit)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      orders:(const WCDB::OrderingTerms &)orders
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                       limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(limit)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                      orders:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(limit)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(limit).offset(offset)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      orders:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(limit)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                      orders:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).orders(orders).limit(limit).offset(offset)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(limit).offset(offset)];
}

- (WCTOptionalColumn)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      orders:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).orders(orders).limit(limit).offset(offset)];
}

#pragma mark - Get Rows
- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(limit)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(limit)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(limit)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(limit).offset(offset)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(limit)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).orders(orders).limit(limit).offset(offset)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(limit).offset(offset)];
}

- (WCTOptionalColumnsXRows)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           orders:(const WCDB::OrderingTerms &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).orders(orders).limit(limit).offset(offset)];
}

#pragma mark - Get From Statement
- (WCTOptionalValue)getValueFromStatement:(const WCDB::Statement &)statement
{
    WCTOptionalValue result = nil;
    if ([self prepare:statement]) {
        if ([self step]) {
            result = [self extractValueAtIndex:0];
        }
        [self finalizeStatement];
    }
    return result;
}

- (WCTOptionalRow)getRowFromStatement:(const WCDB::Statement &)statement
{
    WCTOptionalRow result = nil;
    if ([self prepare:statement]) {
        if ([self step]) {
            result = [self extractRow];
        }
        [self finalizeStatement];
    }
    return result;
}

- (WCTOptionalColumn)getColumnFromStatement:(const WCDB::Statement &)statement
{
    WCTOptionalColumn result = nil;
    if ([self prepare:statement]) {
        result = [self allValues];
        [self finalizeStatement];
    }
    return result;
}

- (WCTOptionalColumnsXRows)getRowsFromStatement:(const WCDB::Statement &)statement
{
    WCTOptionalColumnsXRows result = nil;
    if ([self prepare:statement]) {
        result = [self allRows];
        [self finalizeStatement];
    }
    return result;
}

@end
