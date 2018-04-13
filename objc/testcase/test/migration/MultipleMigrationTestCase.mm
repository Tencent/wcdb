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

@implementation MultipleMigrationTestCase

- (void)setUp
{
    [super setUp];

    _cls = TestCaseObject.class;

    _path1 = [self.recommendedPath stringByAppendingString:@"_source1"];
    _path2 = [self.recommendedPath stringByAppendingString:@"_source2"];
    _migratedPath = [self.recommendedPath stringByAppendingString:@"_migrated"];

    _table1 = [self.className stringByAppendingString:@"_table1"];
    _table2 = [self.className stringByAppendingString:@"_table2"];
    _table3 = [self.className stringByAppendingString:@"_table3"];
    _migratedTable = [self.className stringByAppendingString:@"_migrated"];

    _count = 5;
    _preInsertObjects1 = [TestCaseObject objectsWithCount:_count];
    _preInsertObjects2 = [TestCaseObject objectsWithCount:_count];
    _preInsertObjects3 = [TestCaseObject objectsWithCount:_count];

    _database1 = [[WCTDatabase alloc] initWithPath:_path1];
    _database2 = [[WCTDatabase alloc] initWithPath:_path2];
    WCTDatabase *database3 = [[WCTDatabase alloc] initWithPath:_migratedPath];

    XCTAssertTrue([_database1 createTableAndIndexes:_table1 withClass:_cls]);
    XCTAssertTrue([_database1 insertObjects:_preInsertObjects1 intoTable:_table1]);

    XCTAssertTrue([_database2 createTableAndIndexes:_table2 withClass:_cls]);
    XCTAssertTrue([_database2 insertObjects:_preInsertObjects2 intoTable:_table2]);

    XCTAssertTrue([database3 createTableAndIndexes:_table3 withClass:_cls]);
    XCTAssertTrue([database3 insertObjects:_preInsertObjects3 intoTable:_table3]);

    [database3 close];
    [database3 finalizeDatabase];

    WCTMigrationInfo *info1 = [[WCTMigrationInfo alloc] initWithTargetTable:_table1 fromSourceTable:_table1 ofDatabase:_path1];
    WCTMigrationInfo *info2 = [[WCTMigrationInfo alloc] initWithTargetTable:_table2 fromSourceTable:_table2 ofDatabase:_path2];
    WCTMigrationInfo *info3 = [[WCTMigrationInfo alloc] initWithTargetTable:_migratedTable fromSourceTable:_table3];
    _infos = @[ info1, info2, info3 ];
    _migrated = [[WCTMigrationDatabase alloc] initWithPath:_migratedPath andInfos:_infos];
    XCTAssertTrue([_migrated createTableAndIndexes:_table1 withClass:_cls]);
    XCTAssertTrue([_migrated createTableAndIndexes:_table2 withClass:_cls]);
    XCTAssertTrue([_migrated createTableAndIndexes:_migratedTable withClass:_cls]);
}

- (void)tearDown
{
    XCTAssertTrue([_database1 dropTable:_table1]);
    [_database1 close:^{
      XCTAssertTrue([_database1 removeFiles]);
    }];

    XCTAssertTrue([_database2 dropTable:_table2]);
    [_database2 close:^{
      XCTAssertTrue([_database2 removeFiles]);
    }];

    XCTAssertTrue([_migrated dropTable:_table3]);
    XCTAssertTrue([_migrated dropTable:_migratedTable]);
    [_migrated close:^{
      XCTAssertTrue([_migrated removeFiles]);
    }];

    [super tearDown];
}

- (NSString *)schemaNameForPath:(NSString *)path
{
    return [NSString stringWithFormat:@"WCDBMigration_%zu", std::hash<std::string>{}(path.UTF8String)];
}

@end
