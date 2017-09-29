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

#import <WCDB/WCTInterface+Table.h>
#import <WCDB/WCTTransaction+Private.h>
#import <WCDB/WCTTransaction+Table.h>

@implementation WCTTransaction (Table)

- (BOOL)createTableAndIndexesOfName:(NSString *)tableName withClass:(Class<WCTTableCoding>)cls
{
    return [self createTableAndIndexesOfName:tableName withClass:cls andError:_error];
}

- (BOOL)createVirtualTableOfName:(NSString *)tableName withClass:(Class<WCTTableCoding>)cls
{
    return [self createVirtualTableOfName:tableName withClass:cls andError:_error];
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefList:(const WCTColumnDefList &)columnDefList
{
    return [self createTableOfName:tableName withColumnDefList:columnDefList andError:_error];
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefList:(const WCTColumnDefList &)columnDefList andConstraintList:(const WCTTableConstraintList &)constraintList
{
    return [self createTableOfName:tableName withColumnDefList:columnDefList andConstraintList:constraintList andError:_error];
}

- (BOOL)isTableExists:(NSString *)tableName
{
    return [self isTableExists:tableName withError:_error];
}

- (BOOL)dropTableOfName:(NSString *)tableName
{
    return [self dropTableOfName:tableName withError:_error];
}

- (BOOL)createIndexOfName:(NSString *)indexName withIndexList:(const WCTIndexList &)indexList forTable:(NSString *)tableName
{
    return [self createIndexOfName:indexName withIndexList:indexList forTable:tableName andError:_error];
}

- (BOOL)dropIndexOfName:(NSString *)indexName
{
    return [self dropIndexOfName:indexName withError:_error];
}

- (BOOL)addColumn:(const WCTColumnDef &)columnDef forTable:(NSString *)tableName
{
    return [self addColumn:columnDef forTable:tableName withError:_error];
}

@end
