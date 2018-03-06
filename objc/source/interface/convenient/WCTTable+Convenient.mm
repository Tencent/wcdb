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
#import <WCDB/WCTTable+Private.h>

@implementation WCTTable (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getObject
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOffset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] limit:1] orderBy:orders] nextObject];
}

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                 offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectWhere:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] orderBy:orders] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] orderBy:orders] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)properties
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextObject];
}

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] orderBy:orders] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] orderBy:orders] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] orderBy:orders] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] orderBy:orders] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] orderBy:orders] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ResultColumn &)resultColumn
                              where:(const WCDB::Expression &)condition
                            orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                              limit:(const WCDB::Expression &)limit
                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] orderBy:orders] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] orderBy:orders] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ResultColumn &)resultColumn
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumn:resultColumn] fromTable:_tableName] where:condition] orderBy:orders] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjects
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOffset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCDB::Expression &)offset
                                         offset:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCDB::Expression &)condition
                                        orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                          limit:(const WCDB::Expression &)limit
                                         offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] ofClass:_class] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
{
    return [[[[[WCTSelect alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] orderBy:orders] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] orderBy:orders] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)properties
                                                 where:(const WCDB::Expression &)condition
                                               orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                                 limit:(const WCDB::Expression &)limit
                                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onProperties:properties] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
{
    return [[[[[WCTRowSelect alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] orderBy:orders] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                   andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                    andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] orderBy:orders] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const std::list<WCDB::ResultColumn> &)resultColumns
                                where:(const WCDB::Expression &)condition
                              orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTRowSelect alloc] initWithDatabase:_database
                                     andRecyclableHandle:_database->flowOut()] onResultColumns:resultColumns] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] offset:offset] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
{
    return [[[[WCTInsert alloc] initWithDatabase:_database
                             andRecyclableHandle:_database->flowOut()] intoTable:_tableName] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
{
    return [[[[WCTInsert alloc] initWithDatabase:_database
                             andRecyclableHandle:_database->flowOut()] intoTable:_tableName] executeWithObjects:objects];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] intoTable:_tableName] orReplace] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] intoTable:_tableName] orReplace] executeWithObjects:objects];
}

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] intoTable:_tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties
{
    return [[[[[WCTInsert alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] intoTable:_tableName] onProperties:properties] executeWithObjects:objects];
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)properties
{
    return [[[[[[WCTInsert alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] orReplace] intoTable:_tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)properties
{
    return [[[[[[WCTInsert alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] orReplace] intoTable:_tableName] onProperties:properties] executeWithObjects:objects];
}

#pragma mark - Update Properties With Object
- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                    withObject:(WCTObject *)object
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Property With Object
- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] orderBy:orders] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Properties With Row
- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] orderBy:orders] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] orderBy:orders] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)properties
                       withRow:(WCTOneRow *)row
                         where:(const WCDB::Expression &)condition
                       orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:_tableName] onProperties:properties] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Properties With Value
- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
{
    return [[[[[WCTUpdate alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] orderBy:orders] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] orderBy:orders] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] orderBy:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] orderBy:orders] limit:limit] offset:offset] executeWithValue:value];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCDB::Expression &)condition
                     orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[[WCTUpdate alloc] initWithDatabase:_database
                                  andRecyclableHandle:_database->flowOut()] table:_tableName] onProperty:property] where:condition] orderBy:orders] limit:limit] offset:offset] executeWithValue:value];
}

#pragma mark - Delete
- (BOOL)deleteObjects
{
    return [[[[WCTDelete alloc] initWithDatabase:_database
                             andRecyclableHandle:_database->flowOut()] fromTable:_tableName] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] fromTable:_tableName] where:condition] execute];
}

- (BOOL)deleteObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] fromTable:_tableName] orderBy:orders] execute];
}

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] fromTable:_tableName] limit:limit] execute];
}

- (BOOL)deleteObjectsOffset:(const WCDB::Expression &)offset
{
    return [[[[[WCTDelete alloc] initWithDatabase:_database
                              andRecyclableHandle:_database->flowOut()] fromTable:_tableName] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const std::list<WCDB::OrderingTerm> &)orders
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] fromTable:_tableName] where:condition] orderBy:orders] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] fromTable:_tableName] where:condition] limit:limit] execute];
}

- (BOOL)deleteObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] fromTable:_tableName] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsLimit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTDelete alloc] initWithDatabase:_database
                               andRecyclableHandle:_database->flowOut()] fromTable:_tableName] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                     limit:(const WCDB::Expression &)limit
{
    return [[[[[[[WCTDelete alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTDelete alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] fromTable:_tableName] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsOrderBy:(const std::list<WCDB::OrderingTerm> &)orders
                       limit:(const WCDB::Expression &)limit
                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTDelete alloc] initWithDatabase:_database
                                andRecyclableHandle:_database->flowOut()] fromTable:_tableName] orderBy:orders] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCDB::Expression &)condition
                   orderBy:(const std::list<WCDB::OrderingTerm> &)orders
                     limit:(const WCDB::Expression &)limit
                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[[[WCTDelete alloc] initWithDatabase:_database
                                 andRecyclableHandle:_database->flowOut()] fromTable:_tableName] where:condition] orderBy:orders] limit:limit] offset:offset] execute];
}

@end
