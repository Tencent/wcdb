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

#import <WCDB/WCDB.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTHandle+Private.h>

@implementation WCTDatabase (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                              fromTable:(NSString *)tableName
                                  where:(const WCDB::Expression &)condition
                                orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                     fromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                          fromTable:(NSString *)tableName
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] orderBy:orders] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] ofClass:cls] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                             fromTable:(NSString *)tableName
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] orderBy:orders] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
                into:(NSString *)tableName
{
    return [[[[WCTInsert alloc] initWithDatabase:_database
                             andRecyclableHandle:_database->flowOut()] intoTable:tableName] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
                 into:(NSString *)tableName
{
    return [[[[WCTInsert alloc] initWithDatabase:_database
                             andRecyclableHandle:_database->flowOut()] intoTable:tableName] executeWithObjects:objects];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
                         into:(NSString *)tableName
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] orReplace] intoTable:tableName] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                          into:(NSString *)tableName
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] orReplace] intoTable:tableName] executeWithObjects:objects];
}

- (BOOL)insertObject:(WCTObject *)object
        onProperties:(const WCTPropertyList &)properties
                into:(NSString *)tableName
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] intoTable:tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] intoTable:tableName] onProperties:properties] executeWithObjects:objects];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    return [[[[[[WCTInsert alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] orReplace] intoTable:tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties into:(NSString *)tableName
{
    return [[[[[[WCTInsert alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] orReplace] intoTable:tableName] onProperties:properties] executeWithObjects:objects];
}

#pragma mark - Update Properties With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Property With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Properties With Row
- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] orderBy:orders] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] orderBy:orders] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)properties
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Property With Value
- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] orderBy:orders] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] orderBy:orders] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] orderBy:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] orderBy:orders] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithValue:value];
}

#pragma mark - Delete
- (BOOL)deleteObjectsFromTable:(NSString *)tableName
{
    return [[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
{
    return [[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] where:condition] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] orderBy:orders] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit
{
    return [[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] where:condition] orderBy:orders] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] where:condition] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] where:condition] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] orderBy:orders] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTDelete alloc] initWithDatabase:_database andRecyclableHandle:_database->flowOut()] where:condition] orderBy:orders] limit:limit] offset:offset] execute];
}

@end
