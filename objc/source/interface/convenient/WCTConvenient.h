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

@protocol WCTConvenientSelect
#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientPartialSelect
#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientRowSelect
#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                        offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Value
- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                        offset:(const WCDB::Expression &)offset;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
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
        onProperties:(const WCTPropertyList &)properties
           intoTable:(NSString *)tableName;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
         onProperties:(const WCTPropertyList &)properties
            intoTable:(NSString *)tableName;
@end

#pragma mark -
@protocol WCTConvenientPartialInsertOrReplace
#pragma mark - Partial Insert or Replace
- (BOOL)insertOrReplaceObject:(WCTObject *)object
                 onProperties:(const WCTPropertyList &)properties
                    intoTable:(NSString *)tableName;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                  onProperties:(const WCTPropertyList &)properties
                     intoTable:(NSString *)tableName;
@end

#pragma mark -
@protocol WCTConvenientUpdate
#pragma mark - Update Properties With Object
- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
         withObject:(WCTObject *)object
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientRowUpdate
#pragma mark - Update Properties With Row
- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
       onProperties:(const WCTPropertyList &)properties
            withRow:(WCTColumnCodingRow *)row
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
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
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
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
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
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
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
              limit:(const WCDB::Expression &)limit
             offset:(const WCDB::Expression &)offset;

- (BOOL)updateTable:(NSString *)tableName
         onProperty:(const WCTProperty &)property
          withValue:(WCTColumnCodingValue *)value
              where:(const WCDB::Expression &)condition
            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
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
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;

- (BOOL)deleteFromTable:(NSString *)tableName
                  where:(const WCDB::Expression &)condition
                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                  limit:(const WCDB::Expression &)limit
                 offset:(const WCDB::Expression &)offset;
@end

#pragma mark -
@protocol WCTConvenientStatement
#pragma mark - Statement
- (WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement;

- (WCTValue *)getValueFromStatement:(const WCDB::Statement &)statement atIndex:(int)index;

- (WCTOneRow *)getRowFromStatement:(const WCDB::Statement &)statement;

- (WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement;

- (WCTOneColumn *)getColumnFromStatement:(const WCDB::Statement &)statement atIndex:(int)index;

- (WCTColumnsXRows *)getRowsFromStatement:(const WCDB::Statement &)statement;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                          fromStatement:(const WCDB::Statement &)statement;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                    fromStatement:(const WCDB::Statement &)statement;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                               fromStatement:(const WCDB::Statement &)statement;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                         fromStatement:(const WCDB::Statement &)statement;

- (BOOL)execute:(const WCDB::Statement &)statement
     withObject:(WCTObject *)object;

- (BOOL)execute:(const WCDB::Statement &)statement
      withObject:(WCTObject *)object
    onProperties:(const WCTPropertyList &)properties;

- (BOOL)execute:(const WCDB::Statement &)statement
      withValue:(WCTColumnCodingValue *)value;

- (BOOL)execute:(const WCDB::Statement &)statement
        withRow:(WCTOneRow *)row;

@end
