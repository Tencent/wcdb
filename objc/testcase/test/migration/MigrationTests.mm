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

@interface MigrationTests : MigrationTestCase

@end

@implementation MigrationTests {
    WCTMigrationDatabase *_migrated;
    NSString *_migratedTableName;
    WCTMigrationInfo *_info;
    NSString *_migratedPath;
}

- (void)setUp
{
    [super setUp];

    _migratedPath = [self.recommendedPath stringByAppendingString:@"_migrated"];

    _migratedTableName = [_tableName stringByAppendingString:@"_migrated"];

    _info = [[WCTMigrationInfo alloc] initWithTargetTable:_migratedTableName fromSourceTable:_tableName ofDatabase:_database.path];

    _migrated = [[WCTMigrationDatabase alloc] initWithPath:_migratedPath
                                                   andInfo:_info];

    XCTAssertTrue([_migrated createTableAndIndexes:_migratedTableName withClass:_cls]);
}

- (void)tearDown
{
    XCTAssertTrue([_migrated dropTable:_migratedTableName]);

    [_migrated close:^{
      XCTAssertTrue([_migrated removeFiles]);
    }];
    [_migrated finalizeDatabase];

    _migrated = nil;

    _info = nil;

    _migratedTableName = nil;
    [super tearDown];
}

- (void)test_migration
{
    BOOL done;
    //start
    XCTAssertTrue([_migrated stepMigration:done]);
    //already attached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList()) atIndex:1];
        XCTAssertEqual(schemas.count, 2);
        XCTAssertTrue([schemas[0].stringValue isEqualToString:@"main"]);
        XCTAssertTrue([schemas[1].stringValue isEqualToString:[self schemaNameForPath:_database.path]]);
    }

    //migration
    while ([_migrated stepMigration:done] && !done)
        ;
    XCTAssertTrue(done);

    //already detached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList())];
        XCTAssertEqual(schemas.count, 1);
    }

    //old table is already dropped
    WCTError *error;
    XCTAssertFalse([_database isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);

    //all data are migrated
    NSArray *objects = [_migrated getObjectsOfClass:_cls fromTable:_migratedTableName orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects isEqualToTestCaseObjects:_preInserted]);
}

- (void)test_insert_auto_increment_before_started
{
    [_migrated purge];

    __block BOOL tested = NO;
    [WCTStatistics SetGlobalSQLTrace:^(NSString *sql) {
      NSString *expectedSQL = [NSString stringWithFormat:@"INSERT INTO %@.%@(variable1, variable2, variable3) VALUES(?1, ?2, ?3)", [self schemaNameForPath:_database.path], _tableName];
      if ([sql isEqualToString:expectedSQL]) {
          tested = YES;
      }
    }];

    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];
    object.isAutoIncrement = YES;
    XCTAssertTrue([_migrated insertObject:object intoTable:_migratedTableName]);

    XCTAssertTrue(tested);
    [WCTStatistics SetGlobalSQLTrace:nil];

    NSArray<TestCaseObject *> *objects = [_migrated getObjectsOfClass:_cls fromTable:_migratedTableName];
    XCTAssertEqual(objects.count, _preInserted.count + 1);
    NSArray<TestCaseObject *> *subobjects = [objects subarrayWithRange:NSMakeRange(0, _preInserted.count)];
    XCTAssertTrue([subobjects isEqualToTestCaseObjects:_preInserted]);
    XCTAssertTrue([objects[_preInserted.count] isEqualToObject:object]);
}

- (void)test_insert_auto_increment_after_started
{
    {
        WCTSequence *seq = [_database getObjectOnProperties:WCTSequence.seq fromTable:WCTSequence.tableName where:WCTSequence.name == _tableName];
        XCTAssertEqual(seq.seq, _preInserted.count - 1);
    }

    //start
    BOOL done;
    XCTAssertTrue([_migrated stepMigration:done]);

    {
        WCTSequence *seq = [_migrated getObjectOnProperties:WCTSequence.seq fromTable:WCTSequence.tableName where:WCTSequence.name == _migratedTableName];
        XCTAssertEqual(seq.seq, _preInserted.count - 1);
    }

    [_migrated purge];
    __block BOOL tested = NO;
    [WCTStatistics SetGlobalSQLTrace:^(NSString *sql) {
      NSString *expectedSQL = [NSString stringWithFormat:@"INSERT INTO main.%@(variable1, variable2, variable3) VALUES(?1, ?2, ?3)", _migratedTableName];
      if ([sql isEqualToString:expectedSQL]) {
          tested = YES;
      }
    }];

    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];
    object.isAutoIncrement = YES;
    XCTAssertTrue([_migrated insertObject:object intoTable:_migratedTableName]);

    XCTAssertTrue(tested);
    [WCTStatistics SetGlobalSQLTrace:nil];

    NSArray<TestCaseObject *> *objects = [_migrated getObjectsOfClass:_cls fromTable:_migratedTableName];
    XCTAssertEqual(objects.count, _preInserted.count + 1);
    NSArray<TestCaseObject *> *subobjects = [objects subarrayWithRange:NSMakeRange(0, _preInserted.count)];
    XCTAssertTrue([subobjects isEqualToTestCaseObjects:_preInserted]);
    XCTAssertTrue([objects[_preInserted.count] isEqualToObject:object]);
}

@end
