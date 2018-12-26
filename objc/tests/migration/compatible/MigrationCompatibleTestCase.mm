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

@implementation MigrationCompatibleTestCase

- (void)setUp
{
    [super setUp];

    NSMutableArray<NSString*>* sqls = [NSMutableArray array];
    if (self.isCrossDatabaseMigration) {
        _schemaName = [NSString stringWithFormat:@"WCDBMigration_%u", WCDB::String(self.sourcePath).hash()];
        [sqls addObject:[NSString stringWithFormat:@"ATTACH \'%@\' AS %@", self.sourcePath, self.schemaName]];
    } else {
        _schemaName = @"main";
    }

    [sqls addObjectsFromArray:@[
        [NSString stringWithFormat:@"PRAGMA %@.table_info('testSourceTable')", self.schemaName],
        [NSString stringWithFormat:@"CREATE TEMP VIEW IF NOT EXISTS WCDBUnioned_testTable_testOriginTable AS SELECT rowid AS rowid, content, identifier FROM main.testTable %@ SELECT rowid AS rowid, content, identifier FROM %@.testSourceTable ORDER BY rowid ASC", self.isCrossDatabaseMigration ? @"UNION" : @"UNION ALL", self.schemaName],
        @"PRAGMA main.database_list",
        @"SELECT identifier, content FROM temp.WCDBUnioned_testTable_testOriginTable",
    ]];
    BOOL result = [self checkObjects:self.objects
                             andSQLs:sqls
               asExpectedBySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                   return [self.table getObjects];
               }];
    TestCaseAssertTrue(result);
}

- (void)doTestInsertAutoIncrement
{
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.isAutoIncrement = YES;
    newObject.identifier = self.objects.lastObject.identifier + 1;
    newObject.content = [NSString randomString];
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[
        @"BEGIN IMMEDIATE",
        [NSString stringWithFormat:@"INSERT INTO %@.testSourceTable(identifier, content) VALUES(?1, ?2)", self.schemaName],
        [NSString stringWithFormat:@"INSERT INTO main.testTable(rowid, content, identifier) SELECT rowid, content, identifier FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
        [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid == ?1", self.schemaName],
        @"COMMIT"
    ];

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:sqls
         asExpectedAfterModification:^BOOL {
             return [self.table insertObject:newObject];
         }];
    TestCaseAssertTrue(result);
}

- (void)doTestInsertOrReplace
{
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = self.objects.lastObject.identifier;
    newObject.content = [NSString randomString];
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

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:sqls
         asExpectedAfterModification:^BOOL {
             return [self.table insertOrReplaceObject:newObject];
         }];
    TestCaseAssertTrue(result);
}

- (void)doTestInsertFailedWithConflict
{
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = self.objects.lastObject.identifier;
    newObject.content = [NSString randomString];

    TestCaseAssertFalse([self.table insertObject:newObject]);
}

- (void)doTestLimitedDelete
{
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeObjectAtIndex:2];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
                                  @"COMMIT" ];

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:sqls
         asExpectedAfterModification:^BOOL {
             return [self.table deleteObjectsWhere:TestCaseObject.identifier > 1 orders:TestCaseObject.identifier.asOrder(WCTOrderedAscending) limit:1 offset:1];
         }];
    TestCaseAssertTrue(result);
}

- (void)doTestLimitedUpdate
{
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    TestCaseObject* secondObject = [expectedObjects objectAtIndex:2];
    TestCaseObject* newSecondObject = [[TestCaseObject alloc] init];
    newSecondObject.identifier = secondObject.identifier;
    newSecondObject.content = [NSString randomString];
    [expectedObjects setObject:newSecondObject atIndexedSubscript:2];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"UPDATE %@.testSourceTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)", self.schemaName],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE identifier > 1 ORDER BY identifier ASC LIMIT 1 OFFSET 1)",
                                  @"COMMIT" ];

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:sqls
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:newSecondObject where:TestCaseObject.identifier > 1 orders:TestCaseObject.identifier.asOrder(WCTOrderedAscending) limit:1 offset:1];
         }];
    TestCaseAssertTrue(result);
}

- (void)doTestSelect
{
    NSString* sql = @"SELECT identifier, content FROM temp.WCDBUnioned_testTable_testOriginTable WHERE identifier == 1";

    BOOL result = [self checkObjects:@[ self.objects.firstObject ]
                              andSQL:sql
               asExpectedBySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                   return [self.table getObjectsWhere:TestCaseObject.identifier == 1];
               }];
    TestCaseAssertTrue(result);
}

- (void)doTestDropTable
{
    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DROP TABLE IF EXISTS %@.testSourceTable", self.schemaName],
                                  @"DROP TABLE IF EXISTS main.testTable",
                                  @"COMMIT" ];

    BOOL result = [self checkAllSQLs:sqls
               asExpectedInOperation:^BOOL {
                   return [self.database dropTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

- (void)doTestSubqueryWithinDelete
{
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeLastObject];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"DELETE FROM %@.testSourceTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE temp.WCDBUnioned_testTable_testOriginTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable_testOriginTable))", self.schemaName],
                                  @"DELETE FROM main.testTable WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE temp.WCDBUnioned_testTable_testOriginTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable_testOriginTable))",
                                  @"COMMIT" ];

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:sqls
         asExpectedAfterModification:^BOOL {
             return [self.table deleteObjectsWhere:TestCaseObject.identifier.table(self.tableName).schema(WCDB::Schema::main()).in(WCDB::StatementSelect().select(TestCaseObject.identifier.max()).from(self.tableName))];
         }];
    TestCaseAssertTrue(result);
}

- (void)doTestSubqueryWithinUpdate
{
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = expectedObjects.lastObject.identifier;
    newObject.content = [NSString randomString];
    [expectedObjects removeLastObject];
    [expectedObjects addObject:newObject];

    NSArray<NSString*>* sqls = @[ @"BEGIN IMMEDIATE",
                                  [NSString stringWithFormat:@"UPDATE %@.testSourceTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE temp.WCDBUnioned_testTable_testOriginTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable_testOriginTable))", self.schemaName],
                                  @"UPDATE main.testTable SET content = ?1 WHERE rowid IN(SELECT rowid FROM temp.WCDBUnioned_testTable_testOriginTable WHERE temp.WCDBUnioned_testTable_testOriginTable.identifier IN(SELECT max(identifier) FROM temp.WCDBUnioned_testTable_testOriginTable))",
                                  @"COMMIT" ];

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:sqls
         asExpectedAfterModification:^BOOL {
             return [self.table updateProperties:TestCaseObject.content toObject:newObject where:TestCaseObject.identifier.table(self.tableName).schema(WCDB::Schema::main()).in(WCDB::StatementSelect().select(TestCaseObject.identifier.max()).from(self.tableName))];
         }];
    TestCaseAssertTrue(result);
}

@end
