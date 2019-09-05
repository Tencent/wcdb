//
// Created by sanhuazhang on 2019/05/02
//

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

#import <WCDB/Assertion.hpp>
#import <WCDB/Core.h>
#import <WCDB/WCTDatabase+Table.h>
#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTTable+Private.h>

@implementation WCTTable

@synthesize tableName = _tableName;
@synthesize tableClass = _tableClass;
@synthesize database = _database;

- (instancetype)initWithDatabase:(WCTDatabase *)database
                       tableName:(NSString *)tableName
                      tableClass:(Class<WCTTableCoding>)tableClass
{
    WCTAssert(database != nil && tableClass != nil && tableName != nil);
    if (self = [super init]) {
        _database = database;
        _tableName = tableName;
        _tableClass = tableClass;
    }
    return self;
}

- (NSString *)indexWithSuffix:(NSString *)indexSuffix
{
    return [_tableName stringByAppendingString:indexSuffix];
}

- (BOOL)dropIndexWithSuffix:(NSString *)indexSuffix
{
    WCTRemedialAssert(indexSuffix != nil, "Index suffix can't be null.", return NO;);
    return [_database dropIndex:[_tableName stringByAppendingString:indexSuffix]];
}

- (WCTDatabase *)database
{
    return _database;
}

- (WCTError *)error
{
    return _database.error;
}

@end
