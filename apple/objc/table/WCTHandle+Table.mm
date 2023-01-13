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
#import <WCDB/CoreConst.h>
#import <WCDB/Notifier.hpp>
#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTHandle+Table.h>
#import <WCDB/WCTHandle+Transaction.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTTable+Private.h>

@implementation WCTHandle (Table)

- (WCTOptionalBool)tableExists:(NSString *)tableName
{
    WCTOptionalBool exists;
    WCDB::InnerHandle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        exists = handle->tableExists(tableName);
    }
    return exists;
}

- (BOOL)createTable:(NSString *)tableName
          withClass:(Class<WCTTableCoding>)tableClass
{
    WCTRemedialAssert(tableName != nil && tableClass != nil, "Class or table name can't be null.", return NO;);
    return [self remapTable:tableName toClass:tableClass];
}

- (WCTTable *)getTable:(NSString *)tableName
             withClass:(Class<WCTTableCoding>)tableClass
{
    WCTRemedialAssert(tableName != nil && tableClass != nil, "Class or table name can't be null.", return nil;);
    return [[WCTTable alloc] initWithDatabase:_database
                                    tableName:tableName
                                   tableClass:tableClass];
}

- (BOOL)createVirtualTable:(NSString *)tableName
                 withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName != nil && cls != nil, "Class or table name can't be null.", return NO;);
    const WCTBinding &binding = [cls objectRelationalMapping];
    WCDB::InnerHandle *innerHandle = [self getOrGenerateHandle];
    if (innerHandle == nullptr) {
        return NO;
    }
    return binding.createVirtualTable(tableName, innerHandle);
}

- (BOOL)dropTable:(NSString *)tableName
{
    return [self execute:WCDB::StatementDropTable().dropTable(tableName).ifExists()];
}

- (BOOL)dropIndex:(NSString *)indexName
{
    return [self execute:WCDB::StatementDropIndex().dropIndex(indexName).ifExists()];
}

- (BOOL)remapTable:(NSString *)tableName toClass:(Class<WCTTableCoding>)tableClass
{
    WCTAssert(tableName != nil && tableClass != nil);
    WCTAssert([(id) tableClass respondsToSelector:@selector(objectRelationalMapping)] && [(id) tableClass respondsToSelector:@selector(allProperties)]);
    const WCTBinding &binding = [tableClass objectRelationalMapping];
    WCDB::InnerHandle *innerHandle = [self getOrGenerateHandle];
    if (innerHandle == nullptr) {
        return NO;
    }
    return binding.createTable(tableName, innerHandle);
}

@end
