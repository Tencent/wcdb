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

#import <WCDB/WCTDataBase+Table.h>
#import <WCDB/WCTTable+DataBase.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTInterface+Table.h>

@implementation WCTDataBase(Table)

- (BOOL)createTableAndIndexesOfName:(NSString*)tableName withClass:(Class<WCTTableCoding>)cls
{
    WCDB::Error innerError;
    return [self createTableAndIndexesOfName:tableName withClass:cls andError:innerError];
}

- (WCTTable*)getTableOfName:(NSString*)tableName withClass:(Class<WCTTableCoding>)cls
{
    WCDB::Error innerError;
    if (![self isTableExists:tableName withError:innerError]) {
        return nil;
    }
    return [[WCTTable alloc] initWithCore:_core andTableName:tableName andClass:cls];
}

- (BOOL)createVirtualTableOfName:(NSString*)tableName usingModule:(NSString*)moduleName withClass:(Class<WCTTableCoding>)cls
{
    WCDB::Error innerError;
    return [self createVirtualTableOfName:tableName usingModule:moduleName withClass:cls andError:innerError];
}

- (BOOL)createTableOfName:(NSString*)tableName withColumnDefList:(const WCTColumnDefList&)columnDefList
{
    WCDB::Error innerError;
    return [self createTableOfName:tableName withColumnDefList:columnDefList andError:innerError];
}

- (BOOL)createTableOfName:(NSString*)tableName withColumnDefList:(const WCTColumnDefList&)columnDefList andConstraintList:(const WCTTableConstraintList&)constraintList
{
    WCDB::Error innerError;
    return [self createTableOfName:tableName withColumnDefList:columnDefList andConstraintList:constraintList andError:innerError];
}

- (BOOL)isTableExists:(NSString*)tableName
{
    WCDB::Error innerError;
    return [self isTableExists:tableName withError:innerError];
}

- (BOOL)dropTableOfName:(NSString*)tableName
{
    WCDB::Error innerError;
    return [self dropTableOfName:tableName withError:innerError];
}

- (BOOL)createIndexOfName:(NSString*)indexName withIndexList:(const WCTIndexList&)indexList forTable:(NSString*)tableName
{
    WCDB::Error innerError;
    return [self createIndexOfName:indexName withIndexList:indexList forTable:tableName andError:innerError];
}

- (BOOL)dropIndexOfName:(NSString*)indexName
{
    WCDB::Error innerError;
    return [self dropIndexOfName:indexName withError:innerError];
}

- (BOOL)addColumn:(const WCTColumnDef&)columnDef forTable:(NSString*)tableName
{
    WCDB::Error innerError;
    return [self addColumn:columnDef forTable:tableName withError:innerError];
}

@end
