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
#import <WCDB/WCTChainCall+Private.h>
#import <WCDB/WCTDatabase+ChainCall.h>
#import <WCDB/WCTDatabase+Convenient.h>
#import <WCDB/WCTDatabase+Handle.h>
#import <WCDB/WCTDelete.h>
#import <WCDB/WCTHandle.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTDatabase (Convenient)
#pragma mark - Get Object
- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
                                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
                                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
                                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
                                                  where:(const WCDB::Expression &)condition
                                                 orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
                                                  where:(const WCDB::Expression &)condition
                                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
                                                 orders:(const WCDB::OrderingTerms &)orders
                                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOfClass:(Class)cls
                                              fromTable:(NSString *)tableName
                                                  where:(const WCDB::Expression &)condition
                                                 orders:(const WCDB::OrderingTerms &)orders
                                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

#pragma mark - Get Objects
- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] ofClass:cls] fromTable:tableName] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                             orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                             orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOfClass:(Class)cls
                                                          fromTable:(NSString *)tableName
                                                              where:(const WCDB::Expression &)condition
                                                             orders:(const WCDB::OrderingTerms &)orders
                                                              limit:(const WCDB::Expression &)limit
                                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] ofClass:cls] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

#pragma mark - Get Part Of Object
- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
                                                          where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
                                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
                                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
                                                          where:(const WCDB::Expression &)condition
                                                         orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
                                                          where:(const WCDB::Expression &)condition
                                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
                                                         orders:(const WCDB::OrderingTerms &)orders
                                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

- (WCTOptionalObject /* ObjectType* */)getObjectOnResultColumns:(const WCTResultColumns &)resultColumns
                                                      fromTable:(NSString *)tableName
                                                          where:(const WCDB::Expression &)condition
                                                         orders:(const WCDB::OrderingTerms &)orders
                                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:1] offset:offset] invalidateWhenDispose] nextObject];
}

#pragma mark - Get Part Of Objects
- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
{
    return [[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                     orders:(const WCDB::OrderingTerms &)orders
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

- (WCTOptionalObjects /* NSArray<ObjectType> * */)getObjectsOnResultColumns:(const WCTResultColumns &)resultColumns
                                                                  fromTable:(NSString *)tableName
                                                                      where:(const WCDB::Expression &)condition
                                                                     orders:(const WCDB::OrderingTerms &)orders
                                                                      limit:(const WCDB::Expression &)limit
                                                                     offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareSelect] onResultColumns:resultColumns] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] allObjects];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
           intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] value:object] invalidateWhenDispose] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)tableName
{
    return [[[[[self prepareInsert] intoTable:tableName] values:objects] invalidateWhenDispose] execute];
}

#pragma mark - Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                    intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] value:object] invalidateWhenDispose] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                     intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] orReplace] intoTable:tableName] values:objects] invalidateWhenDispose] execute];
}

#pragma mark - Partial Insert
- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTProperties &)properties
           intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] intoTable:tableName] onProperties:properties] value:object] invalidateWhenDispose] execute];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[self prepareInsert] intoTable:tableName] onProperties:properties] values:objects] invalidateWhenDispose] execute];
}

#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] value:object] invalidateWhenDispose] execute];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTProperties &)properties intoTable:(NSString *)tableName
{
    return [[[[[[[self prepareInsert] orReplace] intoTable:tableName] onProperties:properties] values:objects] invalidateWhenDispose] execute];
}

#pragma mark - Update Properties To Object
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
           toObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:properties] toObject:object] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

#pragma mark - Update Properties To Row
- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
{
    return [[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
      setProperties:(const WCTProperties &)properties
              toRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:properties] toRow:row] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

#pragma mark - Update Property To Value
- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
{
    return [[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)updateTable:(NSString *)tableName
        setProperty:(const WCTProperty &)property
            toValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
             orders:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[[self prepareUpdate] table:tableName] set:property] toValue:value] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

#pragma mark - Delete
- (BOOL)deleteFromTable:(NSString *)tableName
{
    return [[[[self prepareDelete] fromTable:tableName] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
{
    return [[[[[self prepareDelete] fromTable:tableName] where:condition] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[self prepareDelete] fromTable:tableName] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[self prepareDelete] fromTable:tableName] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] where:condition] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[self prepareDelete] fromTable:tableName] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                 orders:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[self prepareDelete] fromTable:tableName] where:condition] orders:orders] limit:limit] offset:offset] invalidateWhenDispose] execute];
}

#pragma mark - Get Value
- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(1)];
}

- (WCTValue *)getValueOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
{
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(1)];
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
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(1).offset(offset)];
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
    return [self getValueFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(1).offset(offset)];
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
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(1)];
}

- (WCTOneRow *)getRowOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                           fromTable:(NSString *)tableName
                               where:(const WCDB::Expression &)condition
{
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(1)];
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
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(1).offset(offset)];
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
    return [self getRowFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(1).offset(offset)];
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
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName)];
}

- (WCTOneColumn *)getColumnOnResultColumn:(const WCDB::ResultColumn &)resultColumn
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
{
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition)];
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
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(limit)];
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
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(limit)];
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
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).limit(limit).offset(offset)];
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
    return [self getColumnFromStatement:WCDB::StatementSelect().select(resultColumn).from(tableName).where(condition).limit(limit).offset(offset)];
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
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName)];
}

- (WCTColumnsXRows *)getRowsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                  fromTable:(NSString *)tableName
                                      where:(const WCDB::Expression &)condition
{
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition)];
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
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(limit)];
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
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(limit)];
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
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).limit(limit).offset(offset)];
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
    return [self getRowsFromStatement:WCDB::StatementSelect().select(resultColumns).from(tableName).where(condition).limit(limit).offset(offset)];
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
    do {
        if (![handle prepare:statement]
            || ![handle step]) {
            break;
        }
        result = [handle extractValueAtIndex:0];
    } while (false);
    [handle finalizeStatement];
    [handle invalidate];
    return result;
}

- (WCTOneRow *)getRowFromStatement:(const WCDB::Statement &)statement
{
    WCTOneRow *result = nil;
    WCTHandle *handle = [self getHandle];
    do {
        if (![handle prepare:statement]
            || ![handle step]) {
            break;
        }
        result = [handle extractRow];
    } while (false);
    [handle finalizeStatement];
    [handle invalidate];
    return result;
}

- (WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement
{
    WCTOneColumn *result = nil;
    WCTHandle *handle = [self getHandle];
    do {
        if (![handle prepare:statement]) {
            break;
        }
        result = [handle allValues];
    } while (false);
    [handle finalizeStatement];
    [handle invalidate];
    return result;
}

- (WCTColumnsXRows *)getRowsFromStatement:(const WCDB::Statement &)statement
{
    WCTColumnsXRows *result = nil;
    WCTHandle *handle = [self getHandle];
    do {
        if (![handle prepare:statement]) {
            break;
        }
        result = [handle allRows];
    } while (false);
    [handle finalizeStatement];
    [handle invalidate];
    return result;
}

@end
