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

#import "MigrationTestCase.h"

@interface TableMigrationTests : MigrationTestCase

@end

@implementation TableMigrationTests

- (void)setUp
{
    self.isCrossDatabaseMigration = NO;

    [super setUp];

    BOOL result = [self checkObjects:self.objects
                             andSQLs:@[ @"PRAGMA main.table_info('testOriginTable')",
                                        @"CREATE TEMP VIEW IF NOT EXISTS WCDBUnioned_testTable_testOriginTable AS SELECT rowid, content, identifier FROM main.testTable UNION ALL SELECT rowid, content, identifier FROM main.testOriginTable ORDER BY rowid ASC",
                                        @"PRAGMA main.database_list",
                                        @"SELECT identifier, content FROM temp.WCDBUnioned_testTable_testOriginTable" ]
               asExpectedBySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                   return [self.table getObjects];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_insert_auto_increment
{
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.isAutoIncrement = YES;
    newObject.identifier = self.objects.lastObject.identifier + 1;
    newObject.content = [NSString randomString];
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects addObject:newObject];

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"INSERT INTO main.testOriginTable(identifier, content) VALUES(?1, ?2)", @"INSERT INTO main.testTable(rowid, content, identifier) SELECT rowid, content, identifier FROM main.testOriginTable WHERE rowid == ?1", @"DELETE FROM main.testOriginTable WHERE rowid == ?1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             return [self.table insertObject:newObject];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_insert_or_replace
{
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = self.objects.lastObject.identifier;
    newObject.content = [NSString randomString];
    NSMutableArray<TestCaseObject*>* expectedObjects = [NSMutableArray arrayWithArray:self.objects];
    [expectedObjects removeLastObject];
    [expectedObjects addObject:newObject];

    BOOL result = [self checkObjects:expectedObjects
                             andSQLs:@[ @"BEGIN IMMEDIATE", @"INSERT OR REPLACE INTO main.testOriginTable(identifier, content) VALUES(?1, ?2)", @"INSERT OR REPLACE INTO main.testTable(rowid, content, identifier) SELECT rowid, content, identifier FROM main.testOriginTable WHERE rowid == ?1", @"DELETE FROM main.testOriginTable WHERE rowid == ?1", @"COMMIT" ]
         asExpectedAfterModification:^BOOL {
             return [self.table insertOrReplaceObject:newObject];
         }];
    TestCaseAssertTrue(result);
}

- (void)test_insert_failed_with_conflict
{
    TestCaseObject* newObject = [[TestCaseObject alloc] init];
    newObject.identifier = self.objects.lastObject.identifier;
    newObject.content = [NSString randomString];

    TestCaseAssertFalse([self.table insertObject:newObject]);
}

- (void)test_limited_delete
{
}

- (void)test_limited_update
{
}

- (void)test_select
{
    BOOL result = [self checkObjects:@[ self.objects.firstObject ]
                              andSQL:@"SELECT identifier, content FROM temp.WCDBUnioned_testTable_testOriginTable WHERE temp.WCDBUnioned_testTable_testOriginTable.identifier IN(SELECT min(identifier) FROM temp.WCDBUnioned_testTable_testOriginTable)"
               asExpectedBySelecting:^NSArray<NSObject<WCTTableCoding>*>* {
                   return [self.table getObjectsWhere:TestCaseObject.identifier.table(self.tableName).in(WCDB::StatementSelect().select(TestCaseObject.identifier.min()).from(self.tableName))];
               }];
    TestCaseAssertTrue(result);
}

- (void)test_drop_table
{
    BOOL result = [self checkAllSQLs:@[ @"BEGIN IMMEDIATE", @"DROP TABLE IF EXISTS main.testTable", @"DROP TABLE IF EXISTS main.testOriginTable", @"COMMIT" ]
               asExpectedInOperation:^BOOL {
                   return [self.database dropTable:self.tableName];
               }];
    TestCaseAssertTrue(result);
}

@end
