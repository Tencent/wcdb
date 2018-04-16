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
        WCTOneColumn *schemas = [_migrated getColumnFromStatement:WCDB::StatementPragma().pragma(WCDB::Pragma::DatabaseList) atIndex:1];
        XCTAssertEqual(schemas.count, 3);
        NSSet *schemaSet = [NSSet setWithObjects:schemas[0].stringValue, schemas[1].stringValue, schemas[2].stringValue, nil];
        NSSet *expectedSchemaSet = [NSSet setWithObjects:@"main", [self schemaNameForPath:_database1.path], [self schemaNameForPath:_database2.path], nil];
        XCTAssertTrue([schemaSet isEqualToSet:expectedSchemaSet]);
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
    XCTAssertFalse([_database1 isTableExists:_table1 withError:&error]);
    XCTAssertNil(error);
    XCTAssertFalse([_database2 isTableExists:_table2 withError:&error]);
    XCTAssertNil(error);
    XCTAssertFalse([_migrated isTableExists:_table3 withError:&error]);
    XCTAssertNil(error);

    //all data are migrated
    NSArray *objects1 = [_migrated getObjectsOfClass:_cls fromTable:_table1 orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects1 isEqualToTestCaseObjects:_preInsertObjects1]);

    NSArray *objects2 = [_migrated getObjectsOfClass:_cls fromTable:_table2 orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects2 isEqualToTestCaseObjects:_preInsertObjects2]);

    NSArray *objects3 = [_migrated getObjectsOfClass:_cls fromTable:_migratedTable orderBy:TestCaseObject.variable1];
    XCTAssertTrue([objects3 isEqualToTestCaseObjects:_preInsertObjects3]);
}

@end
