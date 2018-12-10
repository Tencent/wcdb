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
#import <WCDB/Notifier.hpp>
#import <WCDB/WCTDatabase.h>
#import <WCDB/WCTError+Private.h>
#import <WCDB/WCTHandle+Private.h>
#import <WCDB/WCTHandle+Table.h>
#import <WCDB/WCTHandle+Transaction.h>
#import <WCDB/WCTORM.h>
#import <WCDB/WCTTable+Private.h>

@implementation WCTHandle (Table)

- (WCTOptional<BOOL, NO>)tableExists:(NSString *)tableName
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return nullptr;
    }
    std::pair<bool, bool> result = handle->tableExists(tableName);
    if (result.first) {
        return result.second;
    } else {
        return nullptr;
    }
}

- (BOOL)createTableAndIndexes:(NSString *)tableName
                    withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return NO;);
    return [self runNestedTransaction:^BOOL(WCTHandle *handle) {
        return [handle remapTable:tableName toClass:cls];
    }];
}

- (WCTTable *)getTable:(NSString *)tableName
             withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return nil;);
    return [[WCTTable alloc] initWithDatabase:self.database
                                         name:tableName
                                        class:cls];
}

- (BOOL)createVirtualTable:(NSString *)tableName
                 withClass:(Class<WCTTableCoding>)cls
{
    WCTRemedialAssert(tableName && cls, "Class or table name can't be null.", return NO;);
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->execute([cls objectRelationalMapping].generateVirtualCreateTableStatement(tableName));
}

- (BOOL)dropTable:(NSString *)tableName
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->execute(WCDB::StatementDropTable().dropTable(tableName));
}

- (BOOL)dropIndex:(NSString *)indexName
{
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    return handle->execute(WCDB::StatementDropIndex().dropIndex(indexName));
}

- (BOOL)remapTable:(NSString *)tableName toClass:(Class<WCTTableCoding>)cls
{
    WCTInnerAssert(tableName && cls);
    WCDB::Handle *handle = [self getOrGenerateHandle];
    if (!handle) {
        return NO;
    }
    const WCTBinding &binding = [cls objectRelationalMapping];
    std::pair<bool, bool> tableExists = handle->tableExists(tableName);
    if (!tableExists.first) {
        return NO;
    }
    if (tableExists.second) {
        auto pair = handle->getUnorderedColumns(WCDB::Schema::main(), tableName);
        if (!pair.first) {
            return NO;
        }
        std::set<WCDB::String> &columnNames = pair.second;
        std::list<const WCTColumnBinding *> columnBindingsToAdded;
        //Check whether the column names exists
        const auto &columnBindings = binding.getColumnBindings();
        for (const auto &columnBinding : columnBindings) {
            auto iter = columnNames.find(columnBinding.first);
            if (iter == columnNames.end()) {
                columnBindingsToAdded.push_back(&columnBinding.second);
            } else {
                columnNames.erase(iter);
            }
        }
        for (const WCDB::String &columnName : columnNames) {
            WCDB::Error error;
            error.setCode(WCDB::Error::Code::Mismatch);
            error.level = WCDB::Error::Level::Notice;
            error.message = "Skip column";
            error.infos.set("Table", tableName);
            error.infos.set("Column", columnName);
            error.infos.set("Path", self.database.path);
            WCDB::Notifier::shared()->notify(error);
        }
        //Add new column
        for (const WCTColumnBinding *columnBinding : columnBindingsToAdded) {
            if (!handle->execute(WCDB::StatementAlterTable().alterTable(tableName).addColumn(columnBinding->columnDef))) {
                return NO;
            }
        }
    } else {
        if (!handle->execute(binding.generateCreateTableStatement(tableName))) {
            return NO;
        }
    }
    for (const WCDB::StatementCreateIndex &statementCreateIndex : binding.generateCreateIndexStatements(tableName)) {
        if (!handle->execute(statementCreateIndex)) {
            return NO;
        }
    }
    return YES;
}

@end
