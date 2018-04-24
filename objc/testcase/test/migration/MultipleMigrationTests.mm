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
    BOOL done;
    //start
    XCTAssertTrue([_migrated stepMigration:done]);
    //already attached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList()) atIndex:1];
        XCTAssertEqual(schemas.count, 3);
        NSSet *schemaSet = [NSSet setWithObjects:schemas[0].stringValue, schemas[1].stringValue, schemas[2].stringValue, nil];
        NSSet *expectedSchemaSet = [NSSet setWithObjects:@"main", [self schemaNameForPath:_database1.path], [self schemaNameForPath:_database2.path], nil];
        XCTAssertTrue([schemaSet isEqualToSet:expectedSchemaSet]);
    }
}

- (void)test_interrupt
{
    BOOL done;
    //start
    XCTAssertTrue([_migrated stepMigration:done]);
    XCTAssertTrue([_migrated stepMigration:done]);
    XCTAssertTrue([_migrated stepMigration:done]);

    [_migrated close];
    [_migrated finalizeDatabase];
    _migrated = nil;

    _migrated = [[WCTMigrationDatabase alloc] initWithPath:_migratedPath andInfos:_infos];
    NSString *migratingTable = [_migrated getValueOnResult:WCDB::Column("value") fromTable:@"WCDBKV" where:WCDB::Column("key") == "WCDBMigrating"].stringValue;
    XCTAssertTrue([migratingTable isEqualToString:_table1] || [migratingTable isEqualToString:_migratedTable2] || [migratingTable isEqualToString:_migratedTable3]);
}

- (void)test_check_table_migration_done
{
    BOOL done;
    __block BOOL tested = NO;
    __block NSMutableSet *tables = [NSMutableSet setWithObjects:_table1, _migratedTable2, _migratedTable3, nil];
    //start
    XCTAssertTrue([_migrated stepMigration:done onTableMigrated:nil]);
    while ([_migrated stepMigration:done
                    onTableMigrated:^(WCTMigrationInfo *info) {
                      XCTAssertTrue([tables containsObject:info.targetTable]);
                      [tables removeObject:info.targetTable];
                      if (tables.count == 0) {
                          tested = YES;
                      }
                    }] &&
           !done)
        ;
    XCTAssertTrue(done);
    XCTAssertTrue(tested);
}

- (void)check_migrated
{
    //already detached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList())];
        XCTAssertEqual(schemas.count, 1);
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

- (void)test_async_migration_with_interval
{
    __block BOOL done = NO;
    __block BOOL succeed = NO;
    NSDate *before = [NSDate date];
    __block NSCondition *condition = [[NSCondition alloc] init];
    [_migrated asyncMigrationWithInterval:1.0
                          onTableMigrated:nil
                               onMigrated:^(BOOL result) {
                                 succeed = result;
                                 [condition lock];
                                 done = YES;
                                 [condition signal];
                                 [condition unlock];
                               }];
    [condition lock];
    while (!done) {
        [condition wait];
    }
    [condition unlock];
    XCTAssertTrue(succeed);
    NSDate *after = [NSDate date];
    NSTimeInterval interval = [after timeIntervalSinceDate:before];
    XCTAssertGreaterThan(interval, 3.0);
}

- (void)test_async_migration
{
    __block BOOL tested = NO;
    __block BOOL done = NO;
    __block BOOL succeed = NO;
    __block NSCondition *condition = [[NSCondition alloc] init];
    __block NSMutableSet *tables = [NSMutableSet setWithObjects:_table1, _migratedTable2, _migratedTable3, nil];
    [_migrated asyncMigrationOnStepped:^BOOL(WCTMigrationInfo *_Nullable, BOOL) {
      return YES;
    }
        onTableMigrated:^(WCTMigrationInfo *info) {
          XCTAssertTrue([tables containsObject:info.targetTable]);
          [tables removeObject:info.targetTable];
          if (tables.count == 0) {
              tested = YES;
          }
        }
        onMigrated:^(BOOL result) {
          succeed = result;
          [condition lock];
          done = YES;
          [condition signal];
          [condition unlock];
        }];
    [condition lock];
    while (!done) {
        [condition wait];
    }
    [condition unlock];
    XCTAssertTrue(succeed);
    XCTAssertTrue(tested);
}

@end
