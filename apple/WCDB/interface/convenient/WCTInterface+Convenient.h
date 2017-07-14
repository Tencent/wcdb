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
- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName;

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition;

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                    offset:(const WCTOffset &)offset;

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition
                                   orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition
                                    offset:(const WCTOffset &)offset;

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCTOrderByList &)orderList
                                    offset:(const WCTOffset &)offset;

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition
                                   orderBy:(const WCTOrderByList &)orderList
                                    offset:(const WCTOffset &)offset;

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName;

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition;

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                      offset:(const WCTOffset &)offset;

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition
                                     orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition
                                      offset:(const WCTOffset &)offset;

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCTOrderByList &)orderList
                                      offset:(const WCTOffset &)offset;

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition
                                     orderBy:(const WCTOrderByList &)orderList
                                      offset:(const WCTOffset &)offset;

#pragma mark - Get One Row
- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName;

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition;

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCTOrderByList &)orderList;

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                           offset:(const WCTOffset &)offset;

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition
                          orderBy:(const WCTOrderByList &)orderList;

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition
                           offset:(const WCTOffset &)offset;

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCTOrderByList &)orderList
                           offset:(const WCTOffset &)offset;

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition
                          orderBy:(const WCTOrderByList &)orderList
                           offset:(const WCTOffset &)offset;

#pragma mark - Get One Column
- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                                 limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                                offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList
                                offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset;

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                        offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                         limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                        offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList
                                        offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                        offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset;

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset;

#pragma mark - Get One Value
- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName;

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition;

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                   offset:(const WCTOffset &)offset;

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition
                                  orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition
                                   offset:(const WCTOffset &)offset;

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCTOrderByList &)orderList
                                   offset:(const WCTOffset &)offset;

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition
                                  orderBy:(const WCTOrderByList &)orderList
                                   offset:(const WCTOffset &)offset;

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                           offset:(const WCTOffset &)offset;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                           offset:(const WCTOffset &)offset;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset;

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset;

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getAllObjectsOfClass:(Class)cls
                                           fromTable:(NSString *)tableName;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                            limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                           offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset;

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getAllObjectsOnResults:(const WCTResultList &)resultList
                                             fromTable:(NSString *)tableName;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                             offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                              limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                             offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList
                                             offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList
                                             offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset;

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset;

#pragma mark - Get Rows
- (WCTColumnsXRows *)getAllRowsOnResults:(const WCTResultList &)resultList
                               fromTable:(NSString *)tableName;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCTLimit &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                               offset:(const WCTOffset &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                                limit:(const WCTLimit &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                               offset:(const WCTOffset &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList
                               offset:(const WCTOffset &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                               offset:(const WCTOffset &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset;

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset;

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
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                  withObject:(WCTObject *)object;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

#pragma mark - Update Property With Object
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

#pragma mark - Update Properties With Row
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                     withRow:(WCTOneRow *)row;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

#pragma mark - Update Property With Value
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset;

#pragma mark - Delete
- (BOOL)deleteAllObjectsFromTable:(NSString *)tableName;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCTLimit &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                        offset:(const WCTOffset &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                        offset:(const WCTOffset &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset;

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset;

@end
