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

#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTDatabase+Table.h>
#import <WCDB/WCTInterface+Table.h>
#import <WCDB/WCTTable+Database.h>

@implementation WCTDatabase (Table)

- (BOOL)createTableAndIndexesOfName:(NSString *)tableName withClass:(Class<WCTTableCoding>)cls
{
    WCDB::Error innerError;
    return [self createTableAndIndexesOfName:tableName withClass:cls andError:innerError];
}

- (WCTTable *)getTableOfName:(NSString *)tableName withClass:(Class<WCTTableCoding>)cls
{
    WCDB::Error innerError;
    if ([self isTableExists:tableName withError:innerError]) {
        return [[WCTTable alloc] initWithCore:_core andTableName:tableName andClass:cls];
    }
    return nil;
}

- (BOOL)createVirtualTableOfName:(NSString *)tableName withClass:(Class<WCTTableCoding>)cls
{
    WCDB::Error innerError;
    return [self createVirtualTableOfName:tableName withClass:cls andError:innerError];
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefs:(const std::list<WCDB::ColumnDef> &)columnDefs
{
    WCDB::Error innerError;
    return [self createTableOfName:tableName withColumnDefs:columnDefs andError:innerError];
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefs:(const std::list<WCDB::ColumnDef> &)columnDefs andConstraints:(const std::list<WCDB::TableConstraint> &)constraints
{
    WCDB::Error innerError;
    return [self createTableOfName:tableName withColumnDefs:columnDefs andConstraints:constraints andError:innerError];
}

- (BOOL)isTableExists:(NSString *)tableName
{
    WCDB::Error innerError;
    return [self isTableExists:tableName withError:innerError];
}

- (BOOL)dropTableOfName:(NSString *)tableName
{
    WCDB::Error innerError;
    return [self dropTableOfName:tableName withError:innerError];
}

- (BOOL)createIndexOfName:(NSString *)indexName withIndexedColumns:(const std::list<WCDB::IndexedColumn> &)indexedColumns forTable:(NSString *)tableName
{
    WCDB::Error innerError;
    return [self createIndexOfName:indexName withIndexedColumns:indexedColumns forTable:tableName andError:innerError];
}

- (BOOL)dropIndexOfName:(NSString *)indexName
{
    WCDB::Error innerError;
    return [self dropIndexOfName:indexName withError:innerError];
}

- (BOOL)addColumn:(const WCDB::ColumnDef &)columnDef forTable:(NSString *)tableName
{
    WCDB::Error innerError;
    return [self addColumn:columnDef forTable:tableName withError:innerError];
}

@end
