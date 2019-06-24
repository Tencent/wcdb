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

    self.expectMode = DatabaseTestCaseExpectSomeSQLs;

    TestCaseAssertTrue([[self.table getObjectsOrders:MigrationObject.identifier.asOrder(WCTOrderedAscending)] isEqualToArray:self.objects]);
}

- (NSString*)schemaName
{
    @synchronized(self) {
        if (_schemaName == nil) {
            if (self.isCrossDatabaseMigration) {
                _schemaName = [NSString stringWithFormat:@"wcdb_migration_%u", WCDB::UnsafeStringView(self.sourcePath).hash()];
            } else {
                _schemaName = @"main";
            }
        }
        return _schemaName;
    }
}

- (void)doTestInsert
{
    MigrationObject* newObject = [self.random migrationObjectWithIdentifier:self.objects.lastObject.identifier + 1];
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects addObject:newObject];

    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    [sqls addObject:@"BEGIN IMMEDIATE"];
    [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@.%@(identifier, content) VALUES(?1, ?2)", self.schemaName, self.sourceTable]];
    [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@.%@ WHERE rowid == ?1", self.schemaName, self.sourceTable]];
    if (self.mode == MigrationObjectORMModeNormal) {
        [sqls addObject:@"INSERT INTO main.testTable(rowid, identifier, content) VALUES((SELECT max(rowid) + 1 FROM temp.wcdb_union_testTable), ?1, ?2)"];
    } else {
        [sqls addObject:[NSString stringWithFormat:@"INSERT INTO main.testTable(rowid, identifier, content) VALUES(?%d, ?1, ?2)", SQLITE_MAX_VARIABLE_NUMBER]];
    }
    [sqls addObject:@"COMMIT"];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table insertObject:newObject];
      }];
}

- (void)doTestInsertAutoIncrement
{
    MigrationObject* newObject = [self.random autoIncrementMigrationObjectWithIdentifier:self.objects.lastObject.identifier + 1];
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects addObject:newObject];

    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    [sqls addObject:@"BEGIN IMMEDIATE"];
    [sqls addObject:[NSString stringWithFormat:@"INSERT INTO %@.%@(identifier, content) VALUES(?1, ?2)", self.schemaName, self.sourceTable]];
    [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@.%@ WHERE rowid == ?1", self.schemaName, self.sourceTable]];
    [sqls addObject:[NSString stringWithFormat:@"INSERT INTO main.testTable(rowid, identifier, content) VALUES(?%d, ?1, ?2)", SQLITE_MAX_VARIABLE_NUMBER]];
    [sqls addObject:@"COMMIT"];

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

    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    [sqls addObject:@"BEGIN IMMEDIATE"];
    [sqls addObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO %@.%@(identifier, content) VALUES(?1, ?2)", self.schemaName, self.sourceTable]];
    [sqls addObject:[NSString stringWithFormat:@"DELETE FROM %@.%@ WHERE rowid == ?1", self.schemaName, self.sourceTable]];
    [sqls addObject:[NSString stringWithFormat:@"INSERT OR REPLACE INTO main.testTable(rowid, identifier, content) VALUES(?%d, ?1, ?2)", SQLITE_MAX_VARIABLE_NUMBER]];
    [sqls addObject:@"COMMIT"];

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
                                  [NSString stringWithFormat:@"DELETE FROM %@.%@ WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName, self.sourceTable],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
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
                                  [NSString stringWithFormat:@"UPDATE %@.%@ SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName, self.sourceTable],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table updateProperties:MigrationObject.content toObject:newSecondObject where:MigrationObject.identifier > 1 orders:MigrationObject.identifier.asOrder(WCTOrderedAscending) limit:1 offset:1];
      }];
}

- (void)doTestSelect
{
    NSString* sql = @"SELECT identifier, content FROM temp.wcdb_union_testTable WHERE identifier == 1 ORDER BY rowid ASC";

    [self doTestObjects:@[ self.objects.firstObject ]
                 andSQL:sql
            bySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                return [self.table getObjectsWhere:MigrationObject.identifier == 1];
            }];
}

- (void)doTestDropTable
{
    {
        NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                      @"DROP TABLE IF EXISTS main.testTable",
                                      [NSString stringWithFormat:@"DELETE FROM %@.%@", self.schemaName, self.sourceTable],
                                      @"COMMIT" ];

        [self doTestSQLs:sqls
             inOperation:^BOOL {
                 return [self.database dropTable:self.tableName];
             }];
    }
}

- (void)doTestSubqueryWithinDelete
{
    NSMutableArray<MigrationObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeLastObject];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DELETE FROM %@.%@ WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable))", self.schemaName, self.sourceTable],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable))",
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
                                  [NSString stringWithFormat:@"UPDATE %@.%@ SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable))", self.schemaName, self.sourceTable],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.wcdb_union_testTable WHERE temp.wcdb_union_testTable.identifier IN(SELECT max(identifier) FROM temp.wcdb_union_testTable))",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table updateProperties:MigrationObject.content toObject:newObject where:MigrationObject.identifier.table(self.tableName).schema(WCDB::Schema::main()).in(WCDB::StatementSelect().select(MigrationObject.identifier.max()).from(self.tableName))];
      }];
}

@end
