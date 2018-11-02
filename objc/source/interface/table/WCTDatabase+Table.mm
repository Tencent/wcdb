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
#import <WCDB/Interface.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTTable+Private.h>
#import <WCDB/WCTUnsafeHandle+Private.h>

@implementation WCTDatabase (Table)

- (BOOL)tableExists:(NSString *)tableName
{
    return _database->tableExists(tableName).second;
}

- (BOOL)tableExists:(NSString *)tableName
          withError:(WCTError **)error
{
    auto result = _database->tableExists(tableName);
    if (error) {
        if (result.first) {
            *error = nil;
        } else {
            *error = [[WCTError alloc] initWithError:_database->getThreadedError()];
        }
    }
    return result.second;
}

- (BOOL)createTableAndIndexes:(NSString *)tableName
                    withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return NO;);
    return [self runNestedTransaction:^BOOL(WCTHandle *handle) {
        return [handle rebindTable:tableName toClass:cls];
    }];
}

- (WCTTable *)getTable:(NSString *)tableName
             withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return nil;);
    return [[WCTTable alloc] initWithCore:self
                             andTableName:tableName
                                 andClass:cls];
}

- (BOOL)createVirtualTable:(NSString *)tableName
                 withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return NO;);
    return _database->execute([cls objectRelationalMapping].generateVirtualCreateTableStatement(tableName));
}

- (BOOL)dropTable:(NSString *)tableName
{
    return _database->execute(WCDB::StatementDropTable().dropTable(tableName));
}

- (BOOL)dropIndex:(NSString *)indexName
{
    return _database->execute(WCDB::StatementDropIndex().dropIndex(indexName));
}

@end
