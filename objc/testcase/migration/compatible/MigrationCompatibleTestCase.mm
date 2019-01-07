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

    TestCaseAssertTrue([[self.table getObjects] isEqualToArray:self.objects]);
}

- (NSString*)schemaName
{
    if (!_schemaName) {
        if (self.isCrossDatabaseMigration) {
            _schemaName = [NSString stringWithFormat:@"WCDBMigration_%u", WCDB::String(self.sourcePath).hash()];
        } else {
            _schemaName = @"main";
        }
    }
    return _schemaName;
}

- (void)doTestInsertAutoIncrement
{
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.isAutoIncrement = YES;
    newObject.identifier = self.objects.lastObject.identifier + 1;
    newObject.content = self.random.string;
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[
        @"BEGIN IMMEDIATE",
        [NSString stringWithFormat:@"INSERT INTO %@.testSourceTable(identifier, content) VALUES(?1, ?2)", self.schemaName],
        [NSString stringWithFormat:@"INSERT INTO main.testTable(rowid, content, identifier) SELECT rowid, content, identifier FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
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
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = self.objects.lastObject.identifier;
    newObject.content = self.random.string;
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeLastObject];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[
        @"BEGIN IMMEDIATE",
        [NSString stringWithFormat:@"INSERT OR REPLACE INTO %@.testSourceTable(identifier, content) VALUES(?1, ?2)", self.schemaName],
        [NSString stringWithFormat:@"INSERT OR REPLACE INTO main.testTable(rowid, content, identifier) SELECT rowid, content, identifier FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
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
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = self.objects.lastObject.identifier;
    newObject.content = self.random.string;

    TestCaseAssertFalse([self.table insertObject:newObject]);
}

- (void)doTestLimitedDelete
{
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeObjectAtIndex:2];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table deleteObjectsWhere:TestCaseObject.identifier > 1 orders:TestCaseObject.identifier.asOrder(WCTOrderedAscending) limit:1 offset:1];
      }];
}

- (void)doTestLimitedUpdate
{
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    TestCaseObject* secondObject = [expectedObjects objectAtIndex:2];
    TestCaseObject* newSecondObject = [[TestCaseObject alloc] init];
    newSecondObject.identifier = secondObject.identifier;
    newSecondObject.content = self.random.string;
    [expectedObjects setObject:newSecondObject atIndexedSubscript:2];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"UPDATE %@.testSourceTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:newSecondObject where:TestCaseObject.identifier > 1 orders:TestCaseObject.identifier.asOrder(WCTOrderedAscending) limit:1 offset:1];
      }];
}

- (void)doTestSelect
{
    NSString* sql = @"SELECT identifier, content FROM temp.WCDBUnioned_testTable WHERE identifier == 1";

    [self doTestObjects:@[ self.objects.firstObject ]
                 andSQL:sql
            bySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                return [self.table getObjectsWhere:TestCaseObject.identifier == 1];
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
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeLastObject];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))", self.schemaName],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table deleteObjectsWhere:TestCaseObject.identifier.table(self.tableName).schema(WCDB::Schema::main()).in(WCDB::StatementSelect().select(TestCaseObject.identifier.max()).from(self.tableName))];
      }];
}

- (void)doTestSubqueryWithinUpdate
{
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = expectedObjects.lastObject.identifier;
    newObject.content = self.random.string;
    [expectedObjects removeLastObject];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"UPDATE %@.testSourceTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))", self.schemaName],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable WHERE temp.WCDBUnioned_testTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable))",
                                  @"COMMIT" ];

    [self doTestObjects:expectedObjects
                andSQLs:sqls
      afterModification:^BOOL {
          return [self.table updateProperties:TestCaseObject.content toObject:newObject where:TestCaseObject.identifier.table(self.tableName).schema(WCDB::Schema::main()).in(WCDB::StatementSelect().select(TestCaseObject.identifier.max()).from(self.tableName))];
      }];
}

@end
