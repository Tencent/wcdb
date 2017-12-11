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
#import <WCDB/WCTDelete+Private.h>
#import <WCDB/WCTDelete.h>
#import <WCDB/WCTExpr.h>
#import <WCDB/WCTInsert+Private.h>
#import <WCDB/WCTInsert.h>
#import <WCDB/WCTMultiSelect+Private.h>
#import <WCDB/WCTMultiSelect.h>
#import <WCDB/WCTProperty.h>
#import <WCDB/WCTResult.h>
#import <WCDB/WCTRowSelect+Private.h>
#import <WCDB/WCTRowSelect.h>
#import <WCDB/WCTSelect+Private.h>
#import <WCDB/WCTSelect.h>
#import <WCDB/WCTTable+ChainCall.h>
#import <WCDB/WCTTable+Private.h>
#import <WCDB/WCTUpdate+Private.h>
#import <WCDB/WCTUpdate.h>

@implementation WCTTable (ChainCall)

- (WCTInsert *)prepareInsertObjects
{
    return [[WCTInsert alloc] initWithCore:_core andClass:_cls andTableName:_tableName andReplaceFlag:NO];
}

- (WCTInsert *)prepareInsertOrReplaceObjects
{
    return [[WCTInsert alloc] initWithCore:_core andClass:_cls andTableName:_tableName andReplaceFlag:YES];
}

- (WCTInsert *)prepareInsertObjectsOnProperties:(const WCTPropertyList &)propertyList
{
    return [[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName andReplaceFlag:NO];
}

- (WCTInsert *)prepareInsertOrReplaceObjectsOnProperties:(const WCTPropertyList &)propertyList
{
    return [[WCTInsert alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName andReplaceFlag:YES];
}

- (WCTDelete *)prepareDelete
{
    return [[WCTDelete alloc] initWithCore:_core andTableName:_tableName];
}

- (WCTUpdate *)prepareUpdateOnProperties:(const WCTPropertyList &)propertyList
{
    return [[WCTUpdate alloc] initWithCore:_core andProperties:propertyList andTableName:_tableName];
}

- (WCTSelect *)prepareSelectObjects
{
    return [[WCTSelect alloc] initWithCore:_core andResults:[_cls AllProperties] fromTable:_tableName];
}

- (WCTSelect *)prepareSelectObjectsOnResults:(const WCTResultList &)resultList
{
    return [[WCTSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName];
}

- (WCTRowSelect *)prepareSelectRowsOnResults:(const WCTResultList &)resultList
{
    return [[WCTRowSelect alloc] initWithCore:_core andResults:resultList fromTable:_tableName];
}

@end
