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

@interface MigrationTests_SameDatabase : MigrationTestCase

@end

@implementation MigrationTests_SameDatabase {
    WCTMigrationDatabase *_migrated;
    NSString *_migratedTableName;
    WCTMigrationInfo *_info;
}

- (void)setUp
{
    [super setUp];

    _migratedTableName = [_tableName stringByAppendingString:@"_migrated"];

    [_database close];
    [_database finalizeDatabase];
    _database = nil;

    _info = [[WCTMigrationInfo alloc] initWithTargetTable:_migratedTableName
                                          fromSourceTable:_tableName];

    _migrated = [[WCTMigrationDatabase alloc] initWithPath:self.recommendedPath
                                                   andInfo:_info];
    _database = _migrated;

    XCTAssertTrue([_migrated createTableAndIndexes:_migratedTableName withClass:_cls]);
}

- (void)tearDown
{
    XCTAssertTrue([_migrated dropTable:_migratedTableName]);

    [_migrated close:^{
      XCTAssertTrue([_migrated removeFiles]);
    }];

    _migrated = nil;

    _info = nil;

    _migratedTableName = nil;
    [super tearDown];
}

- (void)test_migration
{
    //view created
    {
        NSString *viewName = [NSString stringWithFormat:@"WCDBUnioned_%@_%@", _migratedTableName, _tableName];
        WCTRowSelect *rowSelect = [[[_migrated prepareRowSelect] onResultColumn:WCTMaster.allResults.count() > 0] where:WCTMaster.name == viewName];
        WCDB::TableOrSubquery tempTable = WCDB::TableOrSubquery(WCTMaster.tempTableName).withSchema(WCDB::Schema::temp());
        rowSelect.statement.from(tempTable);
        WCTValue *exists = rowSelect.nextValue;
        XCTAssertTrue(exists.boolValue);
    }

    //migration
    BOOL done;
    while ([_migrated stepMigration:done] && !done)
        ;
    XCTAssertTrue(done);

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

@end
