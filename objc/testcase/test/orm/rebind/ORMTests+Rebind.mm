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

#import "ORMRebindNew+WCTTableCoding.h"
#import "ORMRebindNew.h"
#import "ORMRebindOld+WCTTableCoding.h"
#import "ORMRebindOld.h"
#import "ORMTestCase.h"

@interface ORMTests_Rebind : ORMTestCase

@end

@implementation ORMTests_Rebind

- (void)test_create_table
{
    {
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS %@(variable1 INTEGER PRIMARY KEY, variable2 REAL, variable3 TEXT)", _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    {
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX IF NOT EXISTS %@_index_1 ON %@(variable3)", _tableName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    {
        NSString *expectedSQL = [NSString stringWithFormat:@"ALTER TABLE %@ ADD COLUMN variable4 TEXT", _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    {
        NSString *expectedSQL = [NSString stringWithFormat:@"CREATE INDEX IF NOT EXISTS %@_index_2 ON %@(variable4)", _tableName, _tableName];
        [_expectedSQLs addObject:expectedSQL];
    }
    XCTAssertTrue([_database createTableAndIndexes:_tableName
                                         withClass:ORMRebindOld.class]);
    XCTAssertTrue([_database createTableAndIndexes:_tableName
                                         withClass:ORMRebindNew.class]);
}

- (void)test_insert
{
    XCTAssertTrue([_database createTableAndIndexes:_tableName
                                         withClass:ORMRebindOld.class]);

    ORMRebindOld *oldObject = [[ORMRebindOld alloc] init];
    oldObject.variable1 = 1;
    oldObject.variable2 = 1.0;
    oldObject.variable3 = @"oldObject";
    XCTAssertTrue([_database insertObject:oldObject intoTable:_tableName]);

    XCTAssertTrue([_database createTableAndIndexes:_tableName
                                         withClass:ORMRebindNew.class]);

    ORMRebindNew *newObject = [[ORMRebindNew alloc] init];
    newObject.variable1 = 1;
    newObject.renamedVariable2 = 2.0;
    newObject.variable4 = @"newObject";

    NSString *expectedSQL = [NSString stringWithFormat:@"INSERT OR REPLACE INTO %@(variable1, variable2, variable4) VALUES(?1, ?2, ?3)", _tableName];
    [_expectedSQLs addObject:expectedSQL];

    XCTAssertTrue([_database insertOrReplaceObject:newObject intoTable:_tableName]);

    ORMRebindNew *result = [_database getObjectOfClass:ORMRebindNew.class fromTable:_tableName];
    XCTAssertTrue([result isEqualToObject:newObject]);
}

@end
