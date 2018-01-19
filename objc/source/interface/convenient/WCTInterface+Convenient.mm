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

#import <WCDB/WCTCoding.h>
#import <WCDB/WCTDeclare.h>
#import <WCDB/WCTDelete+Private.h>
#import <WCDB/WCTDelete.h>
#import <WCDB/WCTInsert+Private.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTInterface+Convenient.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTRowSelect+Private.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect+Private.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTUpdate+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTInterface (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                    offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                   orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCDB::OrderList &)orderList
                                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orderList] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCDB::Expression &)condition
                                   orderBy:(const WCDB::OrderList &)orderList
                                    offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:1] offset:offset] nextObject];
}

#pragma mark Get Part Of Object
- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCDB::OrderList &)orderList
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] orderBy:orderList] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getObjectOnProperties:(const WCTPropertyList &)propertyList
                                   fromTable:(NSString *)tableName
                                       where:(const WCDB::Expression &)condition
                                     orderBy:(const WCDB::OrderList &)orderList
                                      offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:1] offset:offset] nextObject];
}

#pragma mark - Get One Row
- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                           offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition
                          orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:1] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition
                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCDB::OrderList &)orderList
                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getRowOnResults:(const WCDB::ColumnResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCDB::Expression &)condition
                          orderBy:(const WCDB::OrderList &)orderList
                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
{
    return [[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCDB::Expression &)limit
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                offset:(const WCDB::Expression &)offset
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getColumnOnResult:(const WCDB::ColumnResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCDB::Expression &)condition
                               orderBy:(const WCDB::OrderList &)orderList
                                 limit:(const WCDB::Expression &)limit
                                offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
{
    WCDB::ColumnResultList resultList = {result};
    return [[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCDB::Expression &)limit
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] orderBy:orderList] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getDistinctColumnOnResult:(const WCDB::ColumnResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCDB::Expression &)condition
                                       orderBy:(const WCDB::OrderList &)orderList
                                         limit:(const WCDB::Expression &)limit
                                        offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] orderBy:orderList] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCDB::OrderList &)orderList
                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getValueOnResult:(const WCDB::ColumnResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCDB::Expression &)condition
                                  orderBy:(const WCDB::OrderList &)orderList
                                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:result fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:1] offset:offset] nextValue];
}

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                           offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                           offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
                                           offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] orderBy:orderList] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getDistinctValueOnResult:(const WCDB::ColumnResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
                                           offset:(const WCDB::Expression &)offset
{
    WCDB::ColumnResultList resultList = {result};
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:YES] where:condition] orderBy:orderList] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                           fromTable:(NSString *)tableName
{
    return [[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCDB::Expression &)condition
                                          orderBy:(const WCDB::OrderList &)orderList
                                            limit:(const WCDB::Expression &)limit
                                           offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andProperties:[cls AllProperties] fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                             fromTable:(NSString *)tableName
{
    return [[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCDB::Expression &)limit
{
    return [[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                              limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnProperties:(const WCTPropertyList &)propertyList
                                          fromTable:(NSString *)tableName
                                              where:(const WCDB::Expression &)condition
                                            orderBy:(const WCDB::OrderList &)orderList
                                              limit:(const WCDB::Expression &)limit
                                             offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andProperties:propertyList fromTable:tableName isDistinct:NO] where:condition] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                               fromTable:(NSString *)tableName
{
    return [[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] orderBy:orderList] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCDB::ColumnResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCDB::Expression &)condition
                              orderBy:(const WCDB::OrderList &)orderList
                                limit:(const WCDB::Expression &)limit
                               offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andColumnResultList:resultList fromTables:tableName?@[tableName]:@[] isDistinct:NO] where:condition] orderBy:orderList] limit:limit] offset:offset] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
                into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:[object.class AllProperties] andTableName:tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
                 into:(NSString *)tableName
{
    if (objects.count > 0) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:[objects.firstObject.class AllProperties] andTableName:tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
                         into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:[object.class AllProperties] andTableName:tableName andReplaceFlag:YES] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                          into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:[objects.firstObject.class AllProperties] andTableName:tableName andReplaceFlag:YES] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:tableName andReplaceFlag:YES] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)propertyList into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:tableName andReplaceFlag:YES] executeWithObjects:objects];
    }
    return NO;
}

#pragma mark - Update Properties With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                  withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Property With Object
- (BOOL)updateRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Properties With Row
- (BOOL)updateRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                     withRow:(WCTOneRow *)row
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Property With Value
- (BOOL)updateRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCDB::Expression &)condition
                  orderBy:(const WCDB::OrderList &)orderList
                    limit:(const WCDB::Expression &)limit
                   offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

#pragma mark - Delete
- (BOOL)deleteAllObjectsFromTable:(NSString *)tableName
{
    return [[[WCTDelete alloc] initWithCore:_core andTableName:tableName] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] orderBy:orderList] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] orderBy:orderList] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] orderBy:orderList] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] orderBy:orderList] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCDB::Expression &)condition
                       orderBy:(const WCDB::OrderList &)orderList
                         limit:(const WCDB::Expression &)limit
                        offset:(const WCDB::Expression &)offset
{
    return [[[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] execute];
}

@end
