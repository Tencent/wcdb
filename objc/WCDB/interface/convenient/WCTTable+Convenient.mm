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
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTInsert+Private.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTRowSelect+Private.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect+Private.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTTable+Convenient.h>
#import <WCDB/WCTTable+Private.h>
#import <WCDB/WCTUpdate+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTTable (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getOneObject
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectWhere:(const WCTCondition &)condition
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOrderBy:(const WCDB::OrderList &)order
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] orderBy:order] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOffset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectWhere:(const WCTCondition &)condition
                                 orderBy:(const WCDB::OrderList &)order
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] limit:1] orderBy:order] nextObject];
}

- (id /* WCTObject* */)getOneObjectWhere:(const WCTCondition &)condition
                                  offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOrderBy:(const WCDB::OrderList &)order
                                    offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] orderBy:order] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectWhere:(const WCTCondition &)condition
                                 orderBy:(const WCDB::OrderList &)order
                                  offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] orderBy:order] limit:1] offset:offset] nextObject];
}

#pragma mark - Get Part Of Object
- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                       where:(const WCTCondition &)condition
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                     orderBy:(const WCDB::OrderList &)order
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:order] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                       where:(const WCTCondition &)condition
                                     orderBy:(const WCDB::OrderList &)order
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:order] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                       where:(const WCTCondition &)condition
                                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                     orderBy:(const WCDB::OrderList &)order
                                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:order] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                       where:(const WCTCondition &)condition
                                     orderBy:(const WCDB::OrderList &)order
                                      offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:order] limit:1] offset:offset] nextObject];
}

#pragma mark - Get One Row
- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                            where:(const WCTCondition &)condition
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                          orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:orderList] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                           offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                            where:(const WCTCondition &)condition
                          orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:orderList] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                            where:(const WCTCondition &)condition
                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                          orderBy:(const WCTOrderByList &)orderList
                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:orderList] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                            where:(const WCTCondition &)condition
                          orderBy:(const WCTOrderByList &)orderList
                           offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
{
    return [[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                               orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 limit:(const WCTLimit &)limit
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                offset:(const WCTOffset &)offset
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
                                 limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
                                offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                               orderBy:(const WCTOrderByList &)orderList
                                offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] orderBy:orderList] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
{
    WCTResultList resultList = {result};
    return [[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                       orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                       orderBy:(const WCTOrderByList &)orderList
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] orderBy:orderList] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                    where:(const WCTCondition &)condition
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                    where:(const WCTCondition &)condition
                                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                    where:(const WCTCondition &)condition
                                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                  orderBy:(const WCTOrderByList &)orderList
                                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] orderBy:orderList] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                    where:(const WCTCondition &)condition
                                  orderBy:(const WCTOrderByList &)orderList
                                   offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:_tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextValue];
}

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                            where:(const WCTCondition &)condition
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                          orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                            where:(const WCTCondition &)condition
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] orderBy:orderList] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:_tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getAllObjects
{
    return [[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderList &)order
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] orderBy:order] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCTLimit &)limit
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOffset:(const WCTOffset &)offset
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
                                        orderBy:(const WCDB::OrderList &)order
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] orderBy:order] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
                                          limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
                                         offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderList &)order
                                            limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] orderBy:order] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderList &)order
                                           offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] orderBy:order] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsLimit:(const WCTOffset &)offset
                                         offset:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
                                        orderBy:(const WCDB::OrderList &)order
                                          limit:(const WCTLimit &)limit
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] orderBy:order] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
                                        orderBy:(const WCDB::OrderList &)order
                                         offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] orderBy:order] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
                                          limit:(const WCTLimit &)limit
                                         offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOrderBy:(const WCDB::OrderList &)order
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] orderBy:order] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsWhere:(const WCTCondition &)condition
                                        orderBy:(const WCDB::OrderList &)order
                                          limit:(const WCTLimit &)limit
                                         offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName] where:condition] orderBy:order] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getAllObjectsOnResults:(const WCTResultList &)resultList
{
    return [[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                            orderBy:(const WCDB::OrderList &)order
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:order] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              limit:(const WCTLimit &)limit
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                             offset:(const WCTOffset &)offset
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCDB::OrderList &)order
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:order] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
                                              limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
                                             offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                            orderBy:(const WCDB::OrderList &)order
                                              limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:order] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                            orderBy:(const WCDB::OrderList &)order
                                             offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:order] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCDB::OrderList &)order
                                              limit:(const WCTLimit &)limit
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:order] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCDB::OrderList &)order
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:order] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                            orderBy:(const WCDB::OrderList &)order
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:order] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCDB::OrderList &)order
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:order] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getAllRowsOnResults:(const WCTResultList &)resultList
{
    return [[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                              orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:orderList] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                limit:(const WCTLimit &)limit
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                               offset:(const WCTOffset &)offset
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:orderList] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
                                limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
                               offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:orderList] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                              orderBy:(const WCTOrderByList &)orderList
                               offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:orderList] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:orderList] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                               offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:orderList] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] orderBy:orderList] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:_cls andTableName:_tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:_cls andTableName:_tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:_cls andTableName:_tableName andReplaceFlag:YES] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:_cls andTableName:_tableName andReplaceFlag:YES] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertObject:(WCTObject *)object onProperties:(const WCTPropertyList &)propertyList
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)propertyList
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object onProperties:(const WCTPropertyList &)propertyList
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects onProperties:(const WCTPropertyList &)propertyList
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

#pragma mark - Update Properties With Object
- (BOOL)updateAllRowsOnProperties:(const WCTPropertyList &)propertyList
                       withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                       orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                        offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                    withObject:(WCTObject *)object
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Property With Object
- (BOOL)updateAllRowsOnProperty:(const WCTProperty &)property
                     withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                      offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
                       limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const WCTOrderByList &)orderList
                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Properties With Row
- (BOOL)updateAllRowsOnProperties:(const WCTPropertyList &)propertyList
                          withRow:(WCTOneRow *)row
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                       orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                        offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] orderBy:orderList] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsOnProperties:(const WCTPropertyList &)propertyList
                       withRow:(WCTOneRow *)row
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Properties With Value
- (BOOL)updateAllRowsOnProperty:(const WCTProperty &)property
                      withValue:(WCTValue *)value
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                      offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
                       limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const WCTOrderByList &)orderList
                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] orderBy:orderList] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsOnProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
                       where:(const WCTCondition &)condition
                     orderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

#pragma mark - Delete
- (BOOL)deleteAllObjects
{
    return [[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] execute];
}

- (BOOL)deleteObjectsOrderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] orderBy:orderList] execute];
}

- (BOOL)deleteObjectsLimit:(const WCTLimit &)limit
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] limit:limit] execute];
}

- (BOOL)deleteObjectsOffset:(const WCTOffset &)offset
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
                   orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] orderBy:orderList] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
                     limit:(const WCTLimit &)limit
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] limit:limit] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
                    offset:(const WCTOffset &)offset
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] offset:offset] execute];
}

- (BOOL)deleteObjectsOrderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] orderBy:orderList] limit:limit] execute];
}

- (BOOL)deleteObjectsOrderBy:(const WCTOrderByList &)orderList
                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] orderBy:orderList] offset:offset] execute];
}

- (BOOL)deleteObjectsLimit:(const WCTLimit &)limit
                    offset:(const WCTOffset &)offset
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
                   orderBy:(const WCTOrderByList &)orderList
                     limit:(const WCTLimit &)limit
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
                   orderBy:(const WCTOrderByList &)orderList
                    offset:(const WCTOffset &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] orderBy:orderList] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
                     limit:(const WCTLimit &)limit
                    offset:(const WCTOffset &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsOrderBy:(const WCTOrderByList &)orderList
                       limit:(const WCTLimit &)limit
                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] orderBy:orderList] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsWhere:(const WCTCondition &)condition
                   orderBy:(const WCTOrderByList &)orderList
                     limit:(const WCTLimit &)limit
                    offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTDelete alloc] initWithCore:_core andTableName:_tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] execute];
}

@end
