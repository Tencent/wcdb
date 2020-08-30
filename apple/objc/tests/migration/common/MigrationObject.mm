//
// Created by sanhuazhang on 2019/05/02
//

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

#import "MigrationObject.h"
#import "MigrationObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation MigrationObject

WCDB_IMPLEMENTATION(MigrationObject)
WCDB_SYNTHESIZE(identifier)
WCDB_SYNTHESIZE(content)

@synthesize isAutoIncrement;
@synthesize lastInsertedRowID;

+ (WCDB::StatementCreateTable)statementForCreatingSourceTable:(NSString*)tableName withMode:(MigrationObjectORMMode)mode
{
    switch (mode) {
    case MigrationObjectORMModeNormal:
        return [MigrationObject statementForCreatingNormalTable:tableName];
    case MigrationObjectORMModePrimaryKey:
        return [MigrationObject statementForCreatingPrimaryKeyTable:tableName];
    case MigrationObjectORMModeAutoIncrement:
        return [MigrationObject statementForCreatingAutoIncrementTable:tableName];
    case MigrationObjectORMModeMissColumn:
        return [MigrationObject statementForCreatingMissColumnTable:tableName];
    }
}

+ (WCDB::StatementCreateTable)statementForCreatingTargetTable:(NSString*)tableName withMode:(MigrationObjectORMMode)mode
{
    switch (mode) {
    case MigrationObjectORMModeNormal:
    case MigrationObjectORMModeMissColumn:
        return [MigrationObject statementForCreatingNormalTable:tableName];
    case MigrationObjectORMModePrimaryKey:
        return [MigrationObject statementForCreatingPrimaryKeyTable:tableName];
    case MigrationObjectORMModeAutoIncrement:
        return [MigrationObject statementForCreatingAutoIncrementTable:tableName];
    }
}

+ (WCDB::StatementCreateTable)statementForCreatingNormalTable:(NSString*)tableName
{
    return MigrationObject.objectRelationalMapping.generateCreateTableStatement(tableName);
}

+ (WCDB::StatementCreateTable)statementForCreatingPrimaryKeyTable:(NSString*)tableName
{
    WCDB::StatementCreateTable statement = MigrationObject.objectRelationalMapping.generateCreateTableStatement(tableName);
    for (auto& columnDef : statement.syntax().columnDefs) {
        if (columnDef.column.name == MigrationObject.identifier.getDescription()) {
            WCDB::ColumnConstraint constraint;
            constraint.primaryKey().order(WCDB::Order::ASC);
            columnDef.constraints.push_back(constraint.syntax());
            break;
        }
    }
    return statement;
}

+ (WCDB::StatementCreateTable)statementForCreatingAutoIncrementTable:(NSString*)tableName
{
    WCDB::StatementCreateTable statement = MigrationObject.objectRelationalMapping.generateCreateTableStatement(tableName);
    for (auto& columnDef : statement.syntax().columnDefs) {
        if (columnDef.column.name == MigrationObject.identifier.getDescription()) {
            WCDB::ColumnConstraint constraint;
            constraint.primaryKey().order(WCDB::Order::ASC).autoIncrement();
            columnDef.constraints.push_back(constraint.syntax());
            break;
        }
    }
    return statement;
}

+ (WCDB::StatementCreateTable)statementForCreatingMissColumnTable:(NSString*)tableName
{
    WCDB::StatementCreateTable statement = MigrationObject.objectRelationalMapping.generateCreateTableStatement(tableName);
    auto iterater = statement.syntax().columnDefs.begin();
    while (iterater != statement.syntax().columnDefs.end()) {
        if (iterater->column.name == MigrationObject.content.getDescription()) {
            statement.syntax().columnDefs.erase(iterater);
            break;
        }
        iterater++;
    }
    return statement;
}

+ (instancetype)objectWithIdentifier:(int)identifier andContent:(NSString*)content
{
    MigrationObject* object = [[MigrationObject alloc] init];
    object.identifier = identifier;
    object.content = content;
    return object;
}

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    MigrationObject* other = (MigrationObject*) object;
    return self.identifier == other.identifier && [NSObject isObject:self.content nilEqualToObject:other.content];
}

- (NSUInteger)hash
{
    NSMutableData* data = [NSMutableData data];
    [data appendBytes:&(_identifier) length:sizeof(_identifier)];
    [data appendData:[_content dataUsingEncoding:NSUTF8StringEncoding]];
    return data.hash;
}

@end
