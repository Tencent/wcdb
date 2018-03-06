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

#import <WCDB/WCDB.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTTable+Private.h>

@implementation WCTDatabase (Table)

- (BOOL)createTableAndIndexesOfName:(NSString *)tableName
                          withClass:(Class<WCTTableCoding>)cls
{
    return _database->runNestedTransaction([self, cls, tableName](WCDB::Handle *handle) {
        std::string table = tableName.UTF8String;
        const WCTBinding *binding = [cls objectRelationalMappingForWCDB];
        std::pair<bool, bool> isTableExists = _database->isTableExists(table);
        if (!isTableExists.first) {
            return;
        }
        if (isTableExists.second) {
            //Get all column names
            std::list<std::string> columnNames;
            {
                if (!handle->prepare(WCDB::StatementPragma().pragma(WCDB::Pragma::TableInfo, table))) {
                    return;
                }
                bool done;
                while (handle->step(done) && !done) {
                    columnNames.push_back(handle->getText(1));
                }
                if (!done) {
                    handle->finalize();
                    return;
                }
            }
            //Check whether the column names exists
            std::map<std::string, std::shared_ptr<WCTColumnBinding>, WCDB::String::CaseInsensiveComparator> columnBindingMap = binding->getColumnBindingMap();
            for (const std::string &columnName : columnNames) {
                auto iter = columnBindingMap.find(columnName);
                if (iter == columnBindingMap.end()) {
                    WCDB::Error::Warning([NSString stringWithFormat:@"Skip column named [%s] for table [%s]", columnName.c_str(), tableName.UTF8String].UTF8String);
                } else {
                    columnBindingMap.erase(iter);
                }
            }
            //Add new column
            for (const auto &iter : columnBindingMap) {
                if (!handle->execute(WCDB::StatementAlterTable().alterTable(table).addColumn(iter.second->columnDef))) {
                    return;
                }
            }
        } else {
            if (!handle->execute(binding->generateCreateTableStatement(tableName.UTF8String))) {
                return;
            }
        }
        for (const WCDB::StatementCreateIndex &statementCreateIndex : binding->generateCreateIndexStatements(table)) {
            if (!handle->execute(statementCreateIndex)) {
                return;
            }
        }
    });
}

- (WCTTable *)getTableOfName:(NSString *)tableName
                   withClass:(Class<WCTTableCoding>)cls
{
    return [[WCTTable alloc] initWithDatabase:_database
                                 andTableName:tableName
                                     andClass:cls];
}

- (BOOL)createVirtualTableOfName:(NSString *)tableName
                       withClass:(Class<WCTTableCoding>)cls
{
    return _database->execute([cls objectRelationalMappingForWCDB]->generateVirtualCreateTableStatement(tableName.UTF8String));
}

- (BOOL)dropTableOfName:(NSString *)tableName
{
    return _database->execute(WCDB::StatementDropTable().dropTable(tableName.UTF8String));
}

- (BOOL)dropIndexOfName:(NSString *)indexName
{
    return _database->execute(WCDB::StatementDropIndex().dropIndex(indexName.UTF8String));
}

@end
