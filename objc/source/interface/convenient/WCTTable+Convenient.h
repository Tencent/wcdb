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
- (id /* WCTObject* */)getOneObject;

- (id /* WCTObject* */)getOneObjectWhere:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getOneObjectOrderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getOneObjectOffset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getOneObjectWhere:(const WCDB::Expression &)condition
                                 orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getOneObjectWhere:(const WCDB::Expression &)condition
                                  offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getOneObjectOrderBy:(const WCDB::OrderList &)orderList
                                    offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getOneObjectWhere:(const WCDB::Expression &)condition
                                 orderBy:(const WCDB::OrderList &)orderList
                                  offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList;

- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList
                                       where:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList
                                     orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList
                                     orderBy:(const WCDB::OrderList &)orderList
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getOneObjectOnProperties:(const WCTPropertyList &)propertyList
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const WCDB::OrderList &)orderList
                                      offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Row
- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList;

- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList
                            where:(const WCDB::Expression &)condition;

- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList
                          orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList
                           offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList
                            where:(const WCDB::Expression &)condition
                          orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList
                            where:(const WCDB::Expression &)condition
                           offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList
                          orderBy:(const WCDB::OrderList &)orderList
                           offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getOneRowOnResults:(const WCDB::ColumnResultList &)resultList
                            where:(const WCDB::Expression &)condition
                          orderBy:(const WCDB::OrderList &)orderList
                           offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Column
- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                               orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                               orderBy:(const WCDB::OrderList &)orderList
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCDB::ColumnResult &)result
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                       orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                       orderBy:(const WCDB::OrderList &)orderList
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Value
- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result;

- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result
                                    where:(const WCDB::Expression &)condition;

- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result
                                  orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result
                                   offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result
                                    where:(const WCDB::Expression &)condition
                                  orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result
                                    where:(const WCDB::Expression &)condition
                                   offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result
                                  orderBy:(const WCDB::OrderList &)orderList
                                   offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getOneValueOnResult:(const WCDB::ColumnResult &)result
                                    where:(const WCDB::Expression &)condition
                                  orderBy:(const WCDB::OrderList &)orderList
                                   offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                            where:(const WCDB::Expression &)condition;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                          orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                           offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                            where:(const WCDB::Expression &)condition
                                           offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                          orderBy:(const WCDB::OrderList &)orderList
                                           offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
                                           offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjects;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const WCDB::OrderList &)orderList
                                          limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const WCDB::OrderList &)orderList
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              where:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                            orderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              where:(const WCDB::Expression &)condition
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              where:(const WCDB::Expression &)condition
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (WCTColumnsXRows *)getAllRowsOnResults:(const WCDB::ColumnResultList &)resultList;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                where:(const WCDB::Expression &)condition;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                              orderBy:(const WCDB::OrderList &)orderList;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects;

- (BOOL)insertOrReplaceObject:(WCTObject *)object;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects;

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTPropertyList &)propertyList;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)propertyList;

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)propertyList;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)propertyList;

#pragma mark - Update Properties With Object
- (BOOL)updateAllRowsOnProperties:(const WCTPropertyList &)propertyList
                       withObject:(WCTObject *)object;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                       orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Object
- (BOOL)updateAllRowsOnProperty:(const WCTProperty &)property
                     withObject:(WCTObject *)object;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

#pragma mark - Update Properties With Row
- (BOOL)updateAllRowsOnProperties:(const WCTPropertyList &)propertyList
                          withRow:(WCTOneRow *)row;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                       orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Value
- (BOOL)updateAllRowsOnProperty:(const WCTProperty &)property
                      withValue:(WCTValue *)value;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

#pragma mark - Delete
- (BOOL)deleteAllObjects;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition;

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderList &)orderList;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOffset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const WCDB::OrderList &)orderList
                     limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsOrderBy:(const WCDB::OrderList &)orderList
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const WCDB::OrderList &)orderList
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset;

@end
