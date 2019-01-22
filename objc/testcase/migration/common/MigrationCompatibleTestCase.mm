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

#import "MigrationCompatibleTestCase.h"

@interface MigrationCompatibleTestCase ()

@property (nonatomic, readonly) NSString* schemaName;

@end

@implementation MigrationCompatibleTestCase {
    NSString* _schemaName;
}

- (void)setUp
{
    [super setUp];

    TestCaseAssertTrue([[self.table getObjectsOrders:MigrationObject.identifier.asOrder(WCTOrderedAscending)] isEqualToArray:self.objects]);
}

- (NSString*)schemaName
{
    @synchronized(self) {
        if (_schemaName == nil) {
            if (self.isCrossDatabaseMigration) {
                _schemaName = [NSString stringWithFormat:@"WCDBMigration_%u", WCDB::String(self.sourcePath).hash()];
            } else {
                _schemaName = @"main";
            }
        }
        return _schemaName;
    }
}

- (void)doTestInsertAutoIncrement
{
    MigrationObject* newObject = [self.random autoIncrementMigrationObjectWithIdentifier:self.objects.lastObject.identifier + 1];
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[
        @"BEGIN IMMEDIATE",
        [NSString stringWithFormat:@"INSERT INTO %@.testSourceTable(identifier, content) VALUES(?1, ?2)", self.schemaName],
        [NSString stringWithFormat:@"INSERT INTO main.testTable(content, identifier) SELECT content, identifier FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
        [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
        @"COMMIT"
    ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table insertObject:newObject];
      }];
}

- (void)doTestInsertOrReplace
{
    MigrationObject* newObject = [self.random migrationObjectWithIdentifier:self.objects.lastObject.identifier];
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeLastObject];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[
        @"BEGIN IMMEDIATE",
        [NSString stringWithFormat:@"INSERT OR REPLACE INTO %@.testSourceTable(identifier, content) VALUES(?1, ?2)", self.schemaName],
        [NSString stringWithFormat:@"INSERT OR REPLACE INTO main.testTable(content, identifier) SELECT content, identifier FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
        [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
        @"COMMIT"
    ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table insertOrReplaceObject:newObject];
      }];
}

- (void)doTestInsertFailedWithConflict
{
    MigrationObject* newObject = [self.random migrationObjectWithIdentifier:self.objects.lastObject.identifier];

    TestCaseAssertFalse([self.table insertObject:newObject]);
}

- (void)doTestLimitedDelete
{
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeObjectAtIndex:2];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table deleteObjectsWhere:MigrationObject.identifier > 1 orders:MigrationObject.identifier.asOrder(WCTOrderedAscending) limit:1 offset:1];
      }];
}

- (void)doTestLimitedUpdate
{
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    MigrationObject* secondObject = [expectedObjects objectAtIndex:2];
    MigrationObject* newSecondObject = [self.random migrationObjectWithIdentifier:secondObject.identifier];
    [expectedObjects setObject:newSecondObject atIndexedSubscript:2];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"UPDATE %@.testSourceTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table updateProperties:MigrationObject.content toObject:newSecondObject where:MigrationObject.identifier > 1 orders:MigrationObject.identifier.asOrder(WCTOrderedAscending) limit:1 offset:1];
      }];
}

- (void)doTestSelect
{
    NSString* sql = @"SELECT identifier, content FROM temp.WCDBUnioned_testTable WHERE identifier == 1";

    [self doTestObjects:@[ self.objects.firstObject ]
                 andSQL:sql
            bySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                return [self.table getObjectsWhere:MigrationObject.identifier == 1];
            }];
}

- (void)doTestDropTable
{
    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DROP TABLE IF EXISTS %@.testSourceTable", self.schemaName],
                                  @"DROP TABLE IF EXISTS main.testTable",
                                  @"COMMIT" ];

    [self doTestSQLs:sqls
         inOperation:^BOOL {
             return [self.database dropTable:self.tableName];
         }];
}

- (void)doTestSubqueryWithinDelete
{
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeLastObject];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))", self.schemaName],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table deleteObjectsWhere:MigrationObject.identifier.table(self.tableName).schema(WCDB::Schema::main()).in(WCDB::StatementSelect().select(MigrationObject.identifier.max()).from(self.tableName))];
      }];
}

- (void)doTestSubqueryWithinUpdate
{
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    MigrationObject* newObject = [self.random migrationObjectWithIdentifier:expectedObjects.lastObject.identifier];
    [expectedObjects removeLastObject];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"UPDATE %@.testSourceTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))", self.schemaName],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table updateProperties:MigrationObject.content toObject:newObject where:MigrationObject.identifier.table(self.tableName).schema(WCDB::Schema::main()).in(WCDB::StatementSelect().select(MigrationObject.identifier.max()).from(self.tableName))];
      }];
}

@end
