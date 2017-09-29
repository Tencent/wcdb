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
#import <WCDB/handle_statement.hpp>
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
    return _core->runEmbeddedTransaction([self, cls, tableName](WCDB::Error &error) -> bool {
        WCDB::Error innerError;
        const WCTBinding *binding = [cls objectRelationalMappingForWCDB];
        bool isTableExists = _core->isTableExists(tableName.UTF8String, innerError);
        if (isTableExists) {
            //Get all column names
            std::list<std::string> columnNameList;
            {
                WCDB::RecyclableStatement statementHandle = _core->prepare(WCDB::StatementPragma()
                                                                               .pragma(WCDB::Pragma::TableInfo, tableName.UTF8String),
                                                                           error);
                if (!statementHandle) {
                    return NO;
                }
                while (statementHandle->step()) {
                    columnNameList.push_back(statementHandle->getValue<(WCDB::ColumnType) WCTColumnTypeString>(1));
                }

                if (!statementHandle->isOK()) {
                    error = statementHandle->getError();
                    return NO;
                }
            }
            //Check whether the column names exists
            WCTColumnBindingMap columnBindingMap = binding->getColumnBindingMap();
            for (const std::string &columnName : columnNameList) {
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
                                     .alter(tableName.UTF8String)
                                     .addColumn(iter.second->getColumnDef()),
                                 error)) {
                    return NO;
                }
            }
        } else {
            if (!_core->exec(binding->generateCreateTableStatement(tableName.UTF8String), error)) {
                return NO;
            }
        }
        const std::shared_ptr<WCTIndexBindingMap> &indexBindingMap = binding->getIndexBindingMap();
        if (indexBindingMap) {
            for (const auto &indexBinding : *indexBindingMap.get()) {
                _core->exec(indexBinding.second->generateCreateIndexStatement(tableName.UTF8String), innerError);
            }
        }
        return YES;
    },
                                         error);
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
    return _core->runEmbeddedTransaction([self, cls, tableName](WCDB::Error &error) -> bool {
        const WCTBinding *binding = [cls objectRelationalMappingForWCDB];
        return _core->exec(binding->generateVirtualCreateTableStatement(tableName.UTF8String), error);
    },
                                         error);
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefList:(const WCTColumnDefList &)columnDefList andError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementCreateTable()
                           .create(tableName.UTF8String, columnDefList),
                       error);
}

- (BOOL)createTableOfName:(NSString *)tableName withColumnDefList:(const WCTColumnDefList &)columnDefList andConstraintList:(const WCTTableConstraintList &)constraintList andError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementCreateTable()
                           .create(tableName.UTF8String, columnDefList, constraintList),
                       error);
}

- (BOOL)dropTableOfName:(NSString *)tableName withError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementDropTable().drop(tableName.UTF8String), error);
}

- (BOOL)isTableExists:(NSString *)tableName withError:(WCDB::Error &)error
{
    return _core->isTableExists(tableName.UTF8String, error);
}

- (BOOL)addColumn:(const WCDB::ColumnDef &)columnDef forTable:(NSString *)tableName withError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementAlterTable()
                           .alter(tableName.UTF8String)
                           .addColumn(columnDef),
                       error);
}

- (BOOL)createIndexOfName:(NSString *)indexName withIndexList:(const WCTIndexList &)indexList forTable:(NSString *)tableName andError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementCreateIndex()
                           .create(indexName.UTF8String)
                           .on(tableName.UTF8String, indexList),
                       error);
}

- (BOOL)dropIndexOfName:(NSString *)indexName withError:(WCDB::Error &)error
{
    return _core->exec(WCDB::StatementDropIndex()
                           .drop(indexName.UTF8String),
                       error);
}

@end
