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

#import <WCDB/WCTCoding.h>
#import <WCDB/WCTCore+Private.h>
#import <WCDB/WCTInterface+Table.h>
#import <WCDB/WCTORM.h>
#import <WCDB/utility.hpp>

@implementation WCTInterface (Table)

- (BOOL)createTableAndIndexesOfName:(NSString *)tableName withClass:(Class)cls andError:(WCDB::Error &)error
{
    if (![cls conformsToProtocol:@protocol(WCTTableCoding)]) {
        WCDB::Error::ReportInterface(_core->getTag(),
                                     _core->getPath(),
                                     WCDB::Error::InterfaceOperation::Table,
                                     WCDB::Error::InterfaceCode::ORM,
                                     [NSString stringWithFormat:@"%@ should conform to protocol WCTTableCoding", NSStringFromClass(cls)].UTF8String,
                                     &error);
        return NO;
    }
    _core->runEmbeddedTransaction([self, cls, tableName](WCDB::Error &error) {
        WCDB::Error innerError;
        const WCTBinding *binding = [cls objectRelationalMappingForWCDB];
        bool isTableExists = _core->isTableExists(tableName.UTF8String, innerError);
        if (isTableExists) {
            //Get all column names
            std::list<std::string> columnNames;
            {
                WCDB::RecyclableStatement handleStatement = _core->prepare(WCDB::StatementPragma()
                                                                               .pragma(WCDB::Pragma::TableInfo, tableName.UTF8String),
                                                                           error);
                if (!error.isOK()) {
                    return;
                }
                while (handleStatement->step()) {
                    columnNames.push_back(handleStatement->getValue<WCTColumnTypeString>(1));
                }

                if (!handleStatement->isOK()) {
                    error = handleStatement->getError();
                    return;
                }
            }
            //Check whether the column names exists
            auto columnBindingMap = binding->getColumnBindingMap();
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
                if (!_core->exec(WCDB::StatementAlterTable()
                                     .alterTable(tableName.UTF8String)
                                     .addColumn(iter.second->columnDef),
                                 error)) {
                    return;
                }
            }
        } else {
            if (!_core->exec(binding->generateCreateTableStatement(tableName.UTF8String), error)) {
                return;
            }
        }
        for (const auto &statementCreateIndex : binding->generateCreateIndexStatements(tableName.UTF8String)) {
            if (!_core->exec(statementCreateIndex, error)) {
                return;
            }
        }
    },
                                  error);
    return error.isOK();
}

- (BOOL)createVirtualTableOfName:(NSString *)tableName withClass:(Class)cls andError:(WCDB::Error &)error
{
    if (![cls conformsToProtocol:@protocol(WCTTableCoding)]) {
        WCDB::Error::ReportInterface(_core->getTag(),
                                     _core->getPath(),
                                     WCDB::Error::InterfaceOperation::Table,
                                     WCDB::Error::InterfaceCode::ORM,
                                     [NSString stringWithFormat:@"%@ should conform to protocol WCTTableCoding", NSStringFromClass(cls)].UTF8String,
                                     &error);
        return NO;
    }
    _core->runEmbeddedTransaction([self, cls, tableName](WCDB::Error &error) {
        const WCTBinding *binding = [cls objectRelationalMappingForWCDB];
        _core->exec(binding->generateVirtualCreateTableStatement(tableName.UTF8String), error);
    },
                                  error);
    return error.isOK();
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefs:(const std::list<WCDB::ColumnDef> &)columnDefs andError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementCreateTable()
                           .createTable(tableName.UTF8String)
                           .define(columnDefs),
                       error);
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefs:(const std::list<WCDB::ColumnDef> &)columnDefs andConstraints:(const std::list<WCDB::TableConstraint> &)constraints andError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementCreateTable()
                           .createTable(tableName.UTF8String)
                           .define(columnDefs)
                           .addTableConstraints(constraints),
                       error);
}

- (BOOL)dropTableOfName:(NSString *)tableName withError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementDropTable().dropTable(tableName.UTF8String), error);
}

- (BOOL)isTableExists:(NSString *)tableName withError:(WCDB::Error &)error
{
    return _core->isTableExists(tableName.UTF8String, error);
}

- (BOOL)addColumn:(const WCDB::ColumnDef &)columnDef forTable:(NSString *)tableName withError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementAlterTable()
                           .alterTable(tableName.UTF8String)
                           .addColumn(columnDef),
                       error);
}

- (BOOL)createIndexOfName:(NSString *)indexName withIndexedColumns:(const std::list<WCDB::IndexedColumn> &)indexedColumns forTable:(NSString *)tableName andError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementCreateIndex()
                           .createIndex(indexName.UTF8String)
                           .on(tableName.UTF8String)
                           .indexedBy(indexedColumns),
                       error);
}

- (BOOL)dropIndexOfName:(NSString *)indexName withError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementDropIndex()
                           .dropIndex(indexName.UTF8String),
                       error);
}

@end
