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
#import <WCDB/WCTInterface+Convenient.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTRowSelect+Private.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect+Private.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTUpdate+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTInterface (Convenient)

#pragma mark - Get Object
- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                    offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition
                                   orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition
                                    offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                   orderBy:(const WCTOrderByList &)orderList
                                    offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] orderBy:orderList] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOfClass:(Class)cls
                                 fromTable:(NSString *)tableName
                                     where:(const WCTCondition &)condition
                                   orderBy:(const WCTOrderByList &)orderList
                                    offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextObject];
}

#pragma mark Get Part Of Object
- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                      offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition
                                     orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:1] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition
                                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                     orderBy:(const WCTOrderByList &)orderList
                                      offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:1] offset:offset] nextObject];
}

- (id /* WCTObject* */)getOneObjectOnResults:(const WCTResultList &)resultList
                                   fromTable:(NSString *)tableName
                                       where:(const WCTCondition &)condition
                                     orderBy:(const WCTOrderByList &)orderList
                                      offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextObject];
}

#pragma mark - Get One Row
- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                           offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition
                          orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:1] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition
                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                          orderBy:(const WCTOrderByList &)orderList
                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:1] offset:offset] nextRow];
}

- (WCTOneRow *)getOneRowOnResults:(const WCTResultList &)resultList
                        fromTable:(NSString *)tableName
                            where:(const WCTCondition &)condition
                          orderBy:(const WCTOrderByList &)orderList
                           offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextRow];
}

#pragma mark - Get One Column
- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
{
    return [[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCTLimit &)limit
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                offset:(const WCTOffset &)offset
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                                 limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                                offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList
                                offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] orderBy:orderList] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneColumnOnResult:(const WCTResult &)result
                             fromTable:(NSString *)tableName
                                 where:(const WCTCondition &)condition
                               orderBy:(const WCTOrderByList &)orderList
                                 limit:(const WCTLimit &)limit
                                offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Distinct Column
- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
{
    WCTResultList resultList = {result};
    return [[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] orderBy:orderList] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] orderBy:orderList] limit:limit] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] orderBy:orderList] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] orderBy:orderList] limit:limit] offset:offset] allValues];
}

- (WCTOneColumn *)getOneDistinctColumnOnResult:(const WCTResult &)result
                                     fromTable:(NSString *)tableName
                                         where:(const WCTCondition &)condition
                                       orderBy:(const WCTOrderByList &)orderList
                                         limit:(const WCTLimit &)limit
                                        offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allValues];
}

#pragma mark - Get One Value
- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition
                                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition
                                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                  orderBy:(const WCTOrderByList &)orderList
                                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] orderBy:orderList] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneValueOnResult:(const WCTResult &)result
                                fromTable:(NSString *)tableName
                                    where:(const WCTCondition &)condition
                                  orderBy:(const WCTOrderByList &)orderList
                                   offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:{result} fromTable:tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextValue];
}

#pragma mark - Get One Distinct Value
- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
{
    WCTResultList resultList = {result};
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] orderBy:orderList] limit:1] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] orderBy:orderList] limit:1] offset:offset] nextValue];
}

- (id /* WCTValue* */)getOneDistinctValueOnResult:(const WCTResult &)result
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset
{
    WCTResultList resultList = {result};
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList.distinct() fromTable:tableName] where:condition] orderBy:orderList] limit:1] offset:offset] nextValue];
}

#pragma mark - Get Objects
- (NSArray /* <WCTObject*> */ *)getAllObjectsOfClass:(Class)cls
                                           fromTable:(NSString *)tableName
{
    return [[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCTLimit &)limit
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                           offset:(const WCTOffset &)offset
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                            limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                           offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] orderBy:orderList] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] orderBy:orderList] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOfClass:(Class)cls
                                        fromTable:(NSString *)tableName
                                            where:(const WCTCondition &)condition
                                          orderBy:(const WCTOrderByList &)orderList
                                            limit:(const WCTLimit &)limit
                                           offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:[cls AllProperties] fromTable:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Part Of Objects
- (NSArray /* <WCTObject*> */ *)getAllObjectsOnResults:(const WCTResultList &)resultList
                                             fromTable:(NSString *)tableName
{
    return [[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCTLimit &)limit
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                             offset:(const WCTOffset &)offset
{
    return [[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                              limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                             offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList
                                             offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:limit] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

- (NSArray /* <WCTObject*> */ *)getObjectsOnResults:(const WCTResultList &)resultList
                                          fromTable:(NSString *)tableName
                                              where:(const WCTCondition &)condition
                                            orderBy:(const WCTOrderByList &)orderList
                                              limit:(const WCTLimit &)limit
                                             offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allObjects];
}

#pragma mark - Get Rows
- (WCTColumnsXRows *)getAllRowsOnResults:(const WCTResultList &)resultList
                               fromTable:(NSString *)tableName
{
    return [[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCTLimit &)limit
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                               offset:(const WCTOffset &)offset
{
    return [[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                                limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                               offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList
                               offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:limit] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                               offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] orderBy:orderList] limit:limit] offset:offset] allRows];
}

- (WCTColumnsXRows *)getRowsOnResults:(const WCTResultList &)resultList
                            fromTable:(NSString *)tableName
                                where:(const WCTCondition &)condition
                              orderBy:(const WCTOrderByList &)orderList
                                limit:(const WCTLimit &)limit
                               offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] allRows];
}

#pragma mark - Insert
- (BOOL)insertObject:(WCTObject *)object
                into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:object.class andTableName:tableName andReplaceFlag:NO] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertObjects:(NSArray<WCTObject *> *)objects
                 into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:objects.firstObject.class andTableName:tableName andReplaceFlag:NO] executeWithObjects:objects];
    }
    return NO;
}

- (BOOL)insertOrReplaceObject:(WCTObject *)object
                         into:(NSString *)tableName
{
    if (object) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:object.class andTableName:tableName andReplaceFlag:YES] executeWithObjects:@[ object ]];
    }
    return NO;
}

- (BOOL)insertOrReplaceObjects:(NSArray<WCTObject *> *)objects
                          into:(NSString *)tableName
{
    if (objects) {
        return [[[WCTInsert alloc] initWithCore:_core andClass:objects.firstObject.class andTableName:tableName andReplaceFlag:YES] executeWithObjects:objects];
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
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                  withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                   offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Property With Object
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                  withObject:(WCTObject *)object
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                   offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
               withObject:(WCTObject *)object
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithObject:object];
}

#pragma mark - Update Properties With Row
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                onProperties:(const WCTPropertyList &)propertyList
                     withRow:(WCTOneRow *)row
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                   offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithRow:row];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
             onProperties:(const WCTPropertyList &)propertyList
                  withRow:(WCTOneRow *)row
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithRow:row];
}

#pragma mark - Update Property With Value
- (BOOL)updateAllRowsInTable:(NSString *)tableName
                  onProperty:(const WCTProperty &)property
                   withValue:(WCTValue *)value
{
    return [[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCTLimit &)limit
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                   offset:(const WCTOffset &)offset
{
    return [[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

- (BOOL)updateRowsInTable:(NSString *)tableName
               onProperty:(const WCTProperty &)property
                withValue:(WCTValue *)value
                    where:(const WCTCondition &)condition
                  orderBy:(const WCTOrderByList &)orderList
                    limit:(const WCTLimit &)limit
                   offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTUpdate alloc] initWithCore:_core andProperties:{property} andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] executeWithRow:@[ value ]];
}

#pragma mark - Delete
- (BOOL)deleteAllObjectsFromTable:(NSString *)tableName
{
    return [[[WCTDelete alloc] initWithCore:_core andTableName:tableName] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] orderBy:orderList] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCTLimit &)limit
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                        offset:(const WCTOffset &)offset
{
    return [[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] orderBy:orderList] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] orderBy:orderList] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] orderBy:orderList] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] orderBy:orderList] limit:limit] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] orderBy:orderList] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] orderBy:orderList] limit:limit] offset:offset] execute];
}

- (BOOL)deleteObjectsFromTable:(NSString *)tableName
                         where:(const WCTCondition &)condition
                       orderBy:(const WCTOrderByList &)orderList
                         limit:(const WCTLimit &)limit
                        offset:(const WCTOffset &)offset
{
    return [[[[[[[WCTDelete alloc] initWithCore:_core andTableName:tableName] where:condition] orderBy:orderList] limit:limit] offset:offset] execute];
}

@end
