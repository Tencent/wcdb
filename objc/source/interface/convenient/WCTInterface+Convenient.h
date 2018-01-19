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
#import <WCDB/WCTInterface.h>

/**
 WCTInterface(Convenient) category contains the combination of common operations on SQL.
 Although it's all glue code here, it works and it can bring you convenience. It's fairly straightforward so that no comment here.
 */
@interface WCTInterface (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                    offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                   orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                    offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCDB::OrderList &)orderList
                                    offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                   orderBy:(const WCDB::OrderList &)orderList
                                    offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCDB::OrderList &)orderList
                                      offset:(const WCDB::Expression &)offset;

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const WCDB::OrderList &)orderList
                                      offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName;

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition;

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                           offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition
                          orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition
                           offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCDB::OrderList &)orderList
                           offset:(const WCDB::Expression &)offset;

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition
                          orderBy:(const WCDB::OrderList &)orderList
                           offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Value
- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                   offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                  orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                   offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCDB::OrderList &)orderList
                                   offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                  orderBy:(const WCDB::OrderList &)orderList
                                   offset:(const WCDB::Expression &)offset;

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                           offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
                                           offset:(const WCDB::Expression &)offset;

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
                                           offset:(const WCDB::Expression &)offset;

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                           fromTable:(NSString *)tableName;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset;

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                             fromTable:(NSString *)tableName;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset;

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                               fromTable:(NSString *)tableName;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCDB::OrderList &)orderList;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset;

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
                into:(NSString *)tableName;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
                 into:(NSString *)tableName;

- (BOOL)insertOrReplaceObject:(WCTObject *)object
                         into:(NSString *)tableName;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                          into:(NSString *)tableName;

- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTPropertyList &)propertyList
                into:(NSString *)tableName;

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
         onProperties:(const WCTPropertyList &)propertyList
                 into:(NSString *)tableName;

- (BOOL)insertOrReplaceObject:(WCTObject *)object
                 onProperties:(const WCTPropertyList &)propertyList
                         into:(NSString *)tableName;

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                  onProperties:(const WCTPropertyList &)propertyList
                          into:(NSString *)tableName;

#pragma mark - Update Properties With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                  withObject:(WCTObject *)object;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

#pragma mark - Update Properties With Row
- (BOOL)updateRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                     withRow:(WCTOneRow *)row;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

#pragma mark - Update Property With Value
- (BOOL)updateRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset;

#pragma mark - Delete
- (BOOL)deleteAllObjectsFromTable:(NSString *)tableName;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset;

@end
