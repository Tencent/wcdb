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
#import "TestCaseConflictObject+WCTTableCoding.h"
#import "TestCaseConflictObject.h"

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
    XCTAssertTrue([_migrated createTableAndIndexes:_migratedTableName withClass:_cls]);

    //view created
    {
        NSString *viewName = [NSString stringWithFormat:@"WCDBUnioned_%@_%@", _migratedTableName, _tableName];
        WCTRowSelect *rowSelect = [[[_migrated prepareRowSelect] onResultColumn:WCTMaster.allResults.count() > 0] where:WCTMaster.name == viewName];
        WCDB::TableOrSubquery tempTable = WCDB::TableOrSubquery(WCTMaster.tempTableName).withSchema(WCDB::Schema::temp());
        rowSelect.statement.from(tempTable);
        WCTValue *exists = rowSelect.nextValue;
        XCTAssertTrue(exists.boolValue);
    }

    //already attached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList()) atIndex:1];
        NSMutableSet<NSString *> *schemaSet = [[NSMutableSet<NSString *> alloc] init];
        for (WCTValue *schema in schemas) {
            [schemaSet addObject:schema.stringValue];
        }
        XCTAssertEqual(schemaSet.count, 3);
        XCTAssertTrue([schemaSet containsObject:[self schemaNameForPath:_database.path]]);
    }

    //migration
    BOOL done;
    while ([_migrated stepMigration:done] && !done)
        ;
    XCTAssertTrue(done);

    //already detached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::databaseList())];
        XCTAssertEqual(schemas.count, 2);
    }

    //old table is already dropped
    WCTError *error;
    XCTAssertFalse([_database isTableExists:_tableName withError:&error]);
    XCTAssertNil(error);

    //view is already dropped
    {
        NSString *viewName = [NSString stringWithFormat:@"WCDBUnioned_%@_%@", _migratedTableName, _tableName];
        WCTRowSelect *rowSelect = [[[_migrated prepareRowSelect] onResultColumn:WCTMaster.allResults.count() == 0] where:WCTMaster.name == viewName];
        WCDB::TableOrSubquery tempTable = WCDB::TableOrSubquery(WCTMaster.tempTableName).withSchema(WCDB::Schema::temp());
        rowSelect.statement.from(tempTable);
        WCTValue *unexists = rowSelect.nextValue;
        XCTAssertTrue(unexists.boolValue);
    }

    //all data are migrated
    NSArray *objects = [_migrated getObjectsOfClass:_cls fromTable:_migratedTableName orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects isEqualToTestCaseObjects:_preInserted]);
}

- (void)test_insert_auto_increment
{
    XCTAssertTrue([_migrated createTableAndIndexes:_migratedTableName withClass:_cls]);

    int expectedInsertedRowID = (int) _preInserted.count;
    {
        WCTSequence *seq = [_database getObjectOnProperties:WCTSequence.seq fromTable:WCTSequence.tableName where:WCTSequence.name == _tableName];
        XCTAssertEqual(seq.seq, expectedInsertedRowID - 1);
    }

    BOOL done;
    //step but do not finish it.
    XCTAssertTrue([_migrated stepMigration:done]);
    XCTAssertFalse(done);

    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];
    object.isAutoIncrement = YES;
    XCTAssertTrue([_migrated insertObject:object intoTable:_migratedTableName]);
    XCTAssertEqual(object.lastInsertedRowID, expectedInsertedRowID);
}

- (void)test_migration_row_per_step
{
    XCTAssertTrue([_migrated createTableAndIndexes:_migratedTableName withClass:_cls]);

    int rowPerStep = rand() % 10;
    [_migrated setMigrateRowPerStep:rowPerStep];
    XCTAssertEqual(_migrated.migrateRowPerStep, rowPerStep);

    //migration
    BOOL done;
    XCTAssertTrue([_migrated stepMigration:done]);
    XCTAssertFalse(done);

    int count = [_database getValueOnResult:TestCaseObject.allResults.count() fromTable:_tableName].integer32Value;
    XCTAssertEqual(count, _preInserted.count - rowPerStep);
}

- (void)test_on_conflict_replace
{
    _cls = TestCaseConflictObject.class;
    XCTAssertTrue([_migrated createTableAndIndexes:_migratedTableName withClass:_cls]);

    NSString *expected = @"expected";
    int index = rand() % _preInserted.count;
    __block long long conflictRowID = -1;

    TestCaseConflictObject *object = [[TestCaseConflictObject alloc] init];
    object.isAutoIncrement = YES;
    object.variable2 = expected;
    object.variable3 = _preInserted[index].variable3;
    XCTAssertTrue([_migrated insertObject:object intoTable:_migratedTableName]);

    [_migrated setConflictCallback:^BOOL(WCTMigrationInfo *info, long long rowid) {
      conflictRowID = rowid;
      return YES;
    }];

    BOOL done = NO;
    while ([_migrated stepMigration:done] && !done)
        ;
    XCTAssertTrue(done);

    XCTAssertEqual(conflictRowID, _preInserted[index].variable1);

    TestCaseConflictObject *result = [_migrated getObjectOfClass:TestCaseConflictObject.class fromTable:_migratedTableName where:WCDB::Column::rowid() == conflictRowID];
    XCTAssertEqual(result.variable1, _preInserted[index].variable1);
    XCTAssertTrue([result.variable2 isEqualToString:expected]);
}

@end
