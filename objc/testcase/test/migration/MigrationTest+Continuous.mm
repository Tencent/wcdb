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

@interface MigrationTest_Continuous : MigrationTestCase

@end

@implementation MigrationTest_Continuous {
    int _count;

    WCTMigrationDatabase *_migrated;
    NSString *_migratedPath;
}

- (NSString *)tableNameAtIndex:(int)index
{
    if (index == 0) {
        return _tableName;
    }
    return [_tableName stringByAppendingFormat:@"_%d", index];
}

- (NSString *)migratedTableNameAtIndex:(int)index
{
    return [_tableName stringByAppendingFormat:@"_migrated_%d", index];
}

- (void)setUp
{
    [super setUp];

    _migratedPath = [self.recommendedPath stringByAppendingString:@"_migrated"];

    _migrated = [[WCTMigrationDatabase alloc] initWithPath:_migratedPath];

    _count = 5;

    for (int i = 0; i < 5; ++i) {
        if (i == 0) {
            continue;
        }
        NSString *tableName = [self tableNameAtIndex:i];
        XCTAssertTrue([_database createTableAndIndexes:tableName withClass:_cls]);
        XCTAssertTrue([_database insertObjects:_preInserted intoTable:tableName]);
    }
}

- (void)tearDown
{
    for (int i = 0; i < _count; ++i) {
        if (i != 0) {
            XCTAssertTrue([_database dropTable:[self tableNameAtIndex:i]]);
        }
        XCTAssertTrue([_migrated dropTable:[self migratedTableNameAtIndex:i]]);
    }

    [_migrated close:^{
      XCTAssertTrue([_migrated removeFiles]);
    }];

    _migrated = nil;

    [super tearDown];
}

- (void)doMigrationAtIndex:(int)index
{
    NSString *migratedTableName = [self migratedTableNameAtIndex:index];
    NSString *tableName = [self tableNameAtIndex:index];

    XCTAssertTrue([_migrated createTableAndIndexes:migratedTableName withClass:_cls]);

    WCTMigrationInfo *info = [[WCTMigrationInfo alloc] initWithTargetTable:migratedTableName fromSourceTable:tableName ofDatabase:_database];

    [_migrated setMigrationInfo:info];

    BOOL done;
    //start
    XCTAssertTrue([_migrated stepMigration:done]);
    //already attached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::DatabaseList) atIndex:1];
        XCTAssertEqual(schemas.count, 2);
        XCTAssertTrue([schemas[0].stringValue isEqualToString:@"main"]);
        XCTAssertTrue([schemas[1].stringValue isEqualToString:@"WCDBMigration"]);
    }

    //migration
    while ([_migrated stepMigration:done] && !done)
        ;
    XCTAssertTrue(done);

    //already detached
    {
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::DatabaseList)];
        XCTAssertEqual(schemas.count, 1);
    }

    //old table is already dropped
    WCTError *error;
    XCTAssertFalse([_database isTableExists:tableName withError:&error]);
    XCTAssertNil(error);

    //all data are migrated
    NSArray *objects = [_migrated getObjectsOfClass:_cls fromTable:migratedTableName];
    XCTAssertTrue([objects.sorted isEqualToTestCaseObjects:_preInserted.sorted]);
}

- (void)test_migration
{
    for (int i = 0; i < _count; ++i) {
        [self doMigrationAtIndex:i];
    }
}

@end
