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

#import <WCDB/WCTCommon.h>

NS_ASSUME_NONNULL_BEGIN

@protocol WCTConvenientSelect
#pragma mark - Get Object
- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName
                                           where:(const WCDB::Expression &)condition;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName
                                           order:(const WCDB::OrderingTerms &)orders;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName
                                          offset:(const WCDB::Expression &)offset;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName
                                           where:(const WCDB::Expression &)condition
                                           order:(const WCDB::OrderingTerms &)orders;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName
                                           where:(const WCDB::Expression &)condition
                                          offset:(const WCDB::Expression &)offset;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName
                                           order:(const WCDB::OrderingTerms &)orders
                                          offset:(const WCDB::Expression &)offset;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                       fromTable:(NSString *)tableName
                                           where:(const WCDB::Expression &)condition
                                           order:(const WCDB::OrderingTerms &)orders
                                          offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     where:(const WCDB::Expression &)condition;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     order:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     where:(const WCDB::Expression &)condition
                                                     order:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     where:(const WCDB::Expression &)condition
                                                     limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     order:(const WCDB::OrderingTerms &)orders
                                                     limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     limit:(const WCDB::Expression &)limit
                                                    offset:(const WCDB::Expression &)offset;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     where:(const WCDB::Expression &)condition
                                                     order:(const WCDB::OrderingTerms &)orders
                                                     limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     where:(const WCDB::Expression &)condition
                                                     limit:(const WCDB::Expression &)limit
                                                    offset:(const WCDB::Expression &)offset;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     order:(const WCDB::OrderingTerms &)orders
                                                     limit:(const WCDB::Expression &)limit
                                                    offset:(const WCDB::Expression &)offset;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                                 fromTable:(NSString *)tableName
                                                     where:(const WCDB::Expression &)condition
                                                     order:(const WCDB::OrderingTerms &)orders
                                                     limit:(const WCDB::Expression &)limit
                                                    offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientPartialSelect
#pragma mark - Get Part Of Object
- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName
                                                   where:(const WCDB::Expression &)condition;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName
                                                   order:(const WCDB::OrderingTerms &)orders;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName
                                                  offset:(const WCDB::Expression &)offset;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName
                                                   where:(const WCDB::Expression &)condition
                                                   order:(const WCDB::OrderingTerms &)orders;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName
                                                   where:(const WCDB::Expression &)condition
                                                  offset:(const WCDB::Expression &)offset;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName
                                                   order:(const WCDB::OrderingTerms &)orders
                                                  offset:(const WCDB::Expression &)offset;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                               fromTable:(NSString *)tableName
                                                   where:(const WCDB::Expression &)condition
                                                   order:(const WCDB::OrderingTerms &)orders
                                                  offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             where:(const WCDB::Expression &)condition;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             order:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             where:(const WCDB::Expression &)condition
                                                             order:(const WCDB::OrderingTerms &)orders;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             where:(const WCDB::Expression &)condition
                                                             limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             order:(const WCDB::OrderingTerms &)orders
                                                             limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             limit:(const WCDB::Expression &)limit
                                                            offset:(const WCDB::Expression &)offset;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             where:(const WCDB::Expression &)condition
                                                             order:(const WCDB::OrderingTerms &)orders
                                                             limit:(const WCDB::Expression &)limit;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             where:(const WCDB::Expression &)condition
                                                             limit:(const WCDB::Expression &)limit
                                                            offset:(const WCDB::Expression &)offset;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             order:(const WCDB::OrderingTerms &)orders
                                                             limit:(const WCDB::Expression &)limit
                                                            offset:(const WCDB::Expression &)offset;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                         fromTable:(NSString *)tableName
                                                             where:(const WCDB::Expression &)condition
                                                             order:(const WCDB::OrderingTerms &)orders
                                                             limit:(const WCDB::Expression &)limit
                                                            offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientRowSelect
#pragma mark - Get One Row
- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTOneRow *)getRowOnResults:(const WCDB::ResultColumns &)resultColumns
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Column
- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                       order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

- (nullable WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                       order:(const WCDB::OrderingTerms &)orders
                                       limit:(const WCDB::Expression &)limit
                                      offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Value
- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

- (nullable WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                  order:(const WCDB::OrderingTerms &)orders
                                 offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         order:(const WCDB::OrderingTerms &)orders;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (nullable WCTColumnsXRows *)getRowsOnResults:(const WCDB::ResultColumns &)resultColumns
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         order:(const WCDB::OrderingTerms &)orders
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

@end

#pragma mark -
@protocol WCTConvenientInsert
#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
           intoTable:(NSString *)tableName;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
            intoTable:(NSString *)tableName;
@end

#pragma mark -
@protocol WCTConvenientInsertOrReplace
#pragma mark - Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                    intoTable:(NSString *)tableName;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                     intoTable:(NSString *)tableName;
@end

#pragma mark -
@protocol WCTConvenientPartialInsert
#pragma mark - Partial Insert
- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTProperties &)properties
           intoTable:(NSString *)tableName;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
         onProperties:(const WCTProperties &)properties
            intoTable:(NSString *)tableName;
@end

#pragma mark -
@protocol WCTConvenientPartialInsertOrReplace
#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                 onProperties:(const WCTProperties &)properties
                    intoTable:(NSString *)tableName;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                  onProperties:(const WCTProperties &)properties
                     intoTable:(NSString *)tableName;
@end

#pragma mark -
@protocol WCTConvenientUpdate
#pragma mark - Update Properties With Object
- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientRowUpdate
#pragma mark - Update Properties With Row
- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTProperties &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Value
- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
              order:(const WCDB::OrderingTerms &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientDelete
#pragma mark - Delete
- (BOOL)deleteFromTable:(NSString *)tableName;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition;

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  order:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  order:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                  order:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  order:(const WCDB::OrderingTerms &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientStatement
#pragma mark - Statement
- (nullable WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement;

- (nullable WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement atIndex:(int)index;

- (nullable WCTOneRow *)getRowFromStatement:(const WCDB::Statement &)statement;

- (nullable WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement;

- (nullable WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement atIndex:(int)index;

- (nullable WCTColumnsXRows *)getRowsFromStatement:(const WCDB::Statement &)statement;

- (nullable id /* WCTObject* */)getObjectOfClass:(Class)cls
                                   fromStatement:(const WCDB::Statement &)statement;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                             fromStatement:(const WCDB::Statement &)statement;

- (nullable id /* WCTObject* */)getObjectOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                           fromStatement:(const WCDB::Statement &)statement;

- (nullable NSArray /* <WCTObject*> */ *)getObjectsOnResultColumns:(const WCDB::ResultColumns &)resultColumns
                                                     fromStatement:(const WCDB::Statement &)statement;

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object;

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object
   onProperties:(const WCTProperties &)properties;

- (BOOL)execute:(const WCDB::Statement &)statement
      withValue:(WCTColumnCodingValue *)value;

- (BOOL)execute:(const WCDB::Statement &)statement
        withRow:(WCTOneRow *)row;

@end

NS_ASSUME_NONNULL_END
