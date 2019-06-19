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
    WCTOptionalBool result = nullptr;
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (handle != nullptr) {
        bool succeed, exists;
        std::tie(succeed, exists) = handle->tableExists(tableName);
        if (succeed) {
            result.reset(exists);
        }
    }
    return result;
}

- (BOOL)createTable:(NSString *)tableName
          withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName != nil && cls != nil, "Class or table name can't be null.", return NO;);
    return [self remapTable:tableName toClass:cls];
}

- (WCTTable *)getTable:(NSString *)tableName
             withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName != nil && cls != nil, "Class or table name can't be null.", return nil;);
    return [[WCTTable alloc] initWithDatabase:self.database
                                         name:tableName
                                        class:cls];
}

- (BOOL)createVirtualTable:(NSString *)tableName
                 withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName != nil && cls != nil, "Class or table name can't be null.", return NO;);
    return [self execute:[cls objectRelationalMapping].generateCreateVirtualTableStatement(tableName)];
}

- (BOOL)dropTable:(NSString *)tableName
{
    return [self execute:WCDB::StatementDropTable().dropTable(tableName).ifExists()];
}

- (BOOL)dropIndex:(NSString *)indexName
{
    return [self execute:WCDB::StatementDropIndex().dropIndex(indexName).ifExists()];
}

- (BOOL)remapTable:(NSString *)tableName toClass:(Class<WCTTableCoding>)cls
{
    WCTAssert(tableName != nil && cls != nil);
    // TODO: check the constraints are as expected here.
    return [self lazyRunTransaction:^BOOL(WCTHandle *nsHandle) {
        WCDB::Handle *handle = [nsHandle getOrGenerateHandle];
        WCTAssert(handle != nullptr);
        bool succeed, exists;
        std::tie(succeed, exists) = handle->tableExists(tableName);
        if (!succeed) {
            return NO;
        }
        const WCTBinding &binding = [cls objectRelationalMapping];
        if (exists) {
            std::set<WCDB::StringView> columnNames;
            std::tie(succeed, columnNames) = handle->getColumns(tableName);
            if (!succeed) {
                return NO;
            }
            //Check whether the column names exists
            const auto &columnDefs = binding.getColumnDefs();
            for (const auto &columnDef : columnDefs) {
                auto iter = columnNames.find(columnDef.first);
                if (iter == columnNames.end()) {
                    //Add new column
                    if (!handle->execute(WCDB::StatementAlterTable().alterTable(tableName).addColumn(columnDef.second))) {
                        return NO;
                    }
                } else {
                    columnNames.erase(iter);
                }
            }
            for (const auto &columnName : columnNames) {
                WCDB::Error error(WCDB::Error::Code::Mismatch, WCDB::Error::Level::Notice, "Skip column");
                error.infos.insert_or_assign("Table", WCDB::StringView(tableName));
                error.infos.insert_or_assign("Column", WCDB::StringView(columnName));
                error.infos.insert_or_assign(WCDB::ErrorStringKeyPath, handle->getPath());
                WCDB::Notifier::shared().notify(error);
            }
        } else {
            if (!handle->execute(binding.generateCreateTableStatement(tableName))) {
                return NO;
            }
        }
        std::list<WCDB::StatementCreateIndex> createIndexStatements;
        std::list<WCDB::StatementDropIndex> dropIndexStatements;
        std::tie(createIndexStatements, dropIndexStatements) = binding.generateIndexStatements(tableName, !exists);
        for (const WCDB::StatementCreateIndex &statement : createIndexStatements) {
            if (!handle->execute(statement)) {
                return NO;
            }
        }
        for (const WCDB::StatementDropIndex &statement : dropIndexStatements) {
            if (!handle->execute(statement)) {
                return NO;
            }
        }
        return YES;
    }];
}

@end
