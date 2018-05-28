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

#import "MultipleMigrationTestCase.h"

@interface MultipleMigrationTests : MultipleMigrationTestCase

@end

@implementation MultipleMigrationTests

- (void)test_migration
{
    //already attached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList()) atIndex:1];
        XCTAssertEqual(schemas.count, 4);
        NSMutableSet *schemaSet = [NSMutableSet set];
        for (WCTValue *schema in schemas) {
            [schemaSet addObject:schema.stringValue];
        }
        NSSet *expectedSchemaSet = [NSSet setWithObjects:@"main", @"temp", [self schemaNameForPath:_database1.path], [self schemaNameForPath:_database2.path], nil];
        XCTAssertTrue([schemaSet isEqualToSet:expectedSchemaSet]);
    }
}

- (void)test_interrupt
{
    BOOL done;
    XCTAssertTrue([_migrated stepMigration:done]);

    [_migrated close];
    [_migrated finalizeDatabase];
    _migrated = nil;

    _migrated = [[WCTMigrationDatabase alloc] initWithPath:_migratedPath andInfos:_infos];
    {
        NSArray<TestCaseObject *> *objects = [_migrated getObjectsOfClass:TestCaseObject.class fromTable:_table1];
        XCTAssertTrue([_preInsertObjects1 isEqualToTestCaseObjects:objects]);
    }
    {
        NSArray<TestCaseObject *> *objects = [_migrated getObjectsOfClass:TestCaseObject.class fromTable:_migratedTable2];
        XCTAssertTrue([_preInsertObjects2 isEqualToTestCaseObjects:objects]);
    }
    {
        NSArray<TestCaseObject *> *objects = [_migrated getObjectsOfClass:TestCaseObject.class fromTable:_migratedTable3];
        XCTAssertTrue([_preInsertObjects3 isEqualToTestCaseObjects:objects]);
    }
}

- (void)test_migrated
{
    BOOL done;
    __block BOOL tableTested = NO;
    NSMutableSet *tables = [NSMutableSet setWithObjects:_table1, _migratedTable2, _migratedTable3, nil];
    [_migrated setTableMigratedCallback:^(WCTMigrationInfo *info) {
      if ([tables containsObject:info.targetTable]) {
          tableTested = YES;
      }
    }];
    while ([_migrated stepMigration:done] && !done)
        ;
    XCTAssertTrue(done);
    XCTAssertTrue(tableTested);
}

- (void)check_migrated
{
    //already detached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList())];
        XCTAssertEqual(schemas.count, 2);
    }

    //old table is already dropped
    WCTError *error;
    XCTAssertFalse([_database1 isTableExists:_table1 withError:&error]);
    XCTAssertNil(error);
    XCTAssertFalse([_database2 isTableExists:_table2 withError:&error]);
    XCTAssertNil(error);
    XCTAssertFalse([_migrated isTableExists:_table3 withError:&error]);
    XCTAssertNil(error);

    //all data are migrated
    NSArray *objects1 = [_migrated getObjectsOfClass:_cls fromTable:_table1 orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects1 isEqualToTestCaseObjects:_preInsertObjects1]);

    NSArray *objects2 = [_migrated getObjectsOfClass:_cls fromTable:_migratedTable2 orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects2 isEqualToTestCaseObjects:_preInsertObjects2]);

    NSArray *objects3 = [_migrated getObjectsOfClass:_cls fromTable:_migratedTable3 orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects3 isEqualToTestCaseObjects:_preInsertObjects3]);
}

- (void)test_async_migration
{
    __block NSMutableSet *expectedMigratedTable = [NSMutableSet setWithObjects:_table1, _migratedTable2, _migratedTable3, nil];
    __block BOOL migrated = NO;

    __block NSCondition *condition = [[NSCondition alloc] init];
    [_migrated setTableMigratedCallback:^(WCTMigrationInfo *_Nullable info) {
      [condition lock];
      [expectedMigratedTable removeObject:info.targetTable];
      [condition unlock];
    }];

    [_migrated asyncMigrationWhenStepped:^BOOL(WCTMigrationState state, BOOL result) {
      if (state == WCTMigrationStateDone && result) {
          [condition lock];
          migrated = YES;
          [condition signal];
          [condition unlock];
      }
      return YES;
    }];
    [condition lock];
    while (!migrated) {
        [condition waitUntilDate:[NSDate distantFuture]];
    }
    [condition unlock];
    XCTAssertTrue(migrated);
    XCTAssertEqual(expectedMigratedTable.count, 0);
    [self check_migrated];
}

@end
