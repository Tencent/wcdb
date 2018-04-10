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

#import <WCDB/WCTTable.h>

@interface WCTTable <ObjectType>
(Convenient)

#pragma mark - Get Object
    - (ObjectType) getObject;

- (ObjectType)getObjectWhere:(const WCDB::Expression &)condition;

- (ObjectType)getObjectOrderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (ObjectType)getObjectOffset:(const WCDB::Expression &)offset;

- (ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                      offset:(const WCDB::Expression &)offset;

- (ObjectType)getObjectOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

- (ObjectType)getObjectWhere:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                      offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (NSArray<ObjectType> *)getObjects;

- (NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition;

- (NSArray<ObjectType> *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray<ObjectType> *)getObjectsLimit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                 orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                   limit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                     limit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsLimit:(const WCDB::Expression &)limit
                                  offset:(const WCDB::Expression &)offset;

- (NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                 orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                   limit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                   limit:(const WCDB::Expression &)limit
                                  offset:(const WCDB::Expression &)offset;

- (NSArray<ObjectType> *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                     limit:(const WCDB::Expression &)limit
                                    offset:(const WCDB::Expression &)offset;

- (NSArray<ObjectType> *)getObjectsWhere:(const WCDB::Expression &)condition
                                 orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                   limit:(const WCDB::Expression &)limit
                                  offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Object
- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties;

- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties
                              where:(const WCDB::Expression &)condition;

- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties
                             offset:(const WCDB::Expression &)offset;

- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties
                              where:(const WCDB::Expression &)condition
                             offset:(const WCDB::Expression &)offset;

- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset;

- (ObjectType)getObjectOnProperties:(const WCTPropertyList &)properties
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          where:(const WCDB::Expression &)condition;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          limit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          where:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          where:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          where:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          where:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

- (NSArray<ObjectType> *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                          where:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                        offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Value
- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                        offset:(const WCDB::Expression &)offset;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

- (WCTValue *)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

#pragma mark - Insert
- (BOOL)insertObject:(ObjectType)object;

- (BOOL)insertObjects:(NSArray<ObjectType> *)objects;

- (BOOL)insertOrReplaceObject:(ObjectType)object;

- (BOOL)insertOrReplaceObjects:(NSArray<ObjectType> *)objects;

- (BOOL)insertObject:(ObjectType)object onProperties:(const WCTPropertyList &)properties;

- (BOOL)insertObjects:(NSArray<ObjectType> *)objects onProperties:(const WCTPropertyList &)properties;

- (BOOL)insertOrReplaceObject:(ObjectType)object onProperties:(const WCTPropertyList &)properties;

- (BOOL)insertOrReplaceObjects:(NSArray<ObjectType> *)objects onProperties:(const WCTPropertyList &)properties;

#pragma mark - Update Properties With Object
- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(ObjectType)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Properties With Row
- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTColumnCodingRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Value
- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTColumnCodingValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

#pragma mark - Delete
- (BOOL)deleteObjects;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition;

- (BOOL)deleteObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOffset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

@end
