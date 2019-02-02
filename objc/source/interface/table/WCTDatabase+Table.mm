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
#import <WCDB/WCTDatabase+Private.h>
#import <WCDB/WCTDatabase+Transaction.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTHandle+Table.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTTable+Private.h>

@implementation WCTDatabase (Table)

- (WCTOptional<BOOL, NO>)tableExists:(NSString *)tableName
{
    WCTDatabaseAssert(return nullptr;);
    WCTOptional<BOOL, NO> result = nullptr;
    bool succeed, exists;
    std::tie(succeed, exists) = _database->tableExists(tableName);
    if (succeed) {
        result = exists;
    }
    return result;
}

- (BOOL)createTableAndIndexes:(NSString *)tableName
                    withClass:(Class<WCTTableCoding>)cls
{
    WCTDatabaseAssert(return NO;);
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return NO;);
    return [self runNestedTransaction:^BOOL(WCTHandle *handle) {
        return [handle remapTable:tableName toClass:cls];
    }];
}

- (WCTTable *)getTable:(NSString *)tableName
             withClass:(Class<WCTTableCoding>)cls
{
    WCTDatabaseAssert(return nil;);
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return nil;);
    return [[WCTTable alloc] initWithDatabase:self name:tableName class:cls];
}

- (BOOL)createVirtualTable:(NSString *)tableName
                 withClass:(Class<WCTTableCoding>)cls
{
    WCTDatabaseAssert(return NO;);
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return NO;);
    return _database->execute([cls objectRelationalMapping].generateCreateVirtualTableStatement(tableName));
}

- (BOOL)dropTable:(NSString *)tableName
{
    WCTDatabaseAssert(return NO;);
    return _database->execute(WCDB::StatementDropTable().dropTable(tableName).ifExists());
}

- (BOOL)dropIndex:(NSString *)indexName
{
    WCTDatabaseAssert(return NO;);
    return _database->execute(WCDB::StatementDropIndex().dropIndex(indexName).ifExists());
}

@end
