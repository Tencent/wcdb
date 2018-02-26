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

#import <Foundation/Foundation.h>
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTTable.h>

/**
 WCTTable(Convenient) category contains the combination of common operations on SQL.
 Although it's all glue code here, it works and it can bring you convenience. It's fairly straightforward so that no comment here.
 */
@interface WCTTable (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getObject;

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getObjectOrderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectOffset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition
                               offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                               offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
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
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset;

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

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                     offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition
                                     offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                     offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                     offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                                      where:(const WCDB::Expression &)condition
                                    orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      limit:(const WCDB::Expression &)limit
                                     offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Value
- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition
                                offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                         where:(const WCDB::Expression &)condition;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                        offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                         where:(const WCDB::Expression &)condition
                                        offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                        offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjects;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
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
- (BOOL)insertObject:(WCTObject *)object;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects;

- (BOOL)insertOrReplaceObject:(WCTObject *)object;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects;

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties;

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties;

#pragma mark - Update Properties With Object
- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Object
- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

#pragma mark - Update Properties With Row
- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Value
- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
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
                   orderBy:(const std::list<WCDB::OrderingTerm> &)orders;

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
