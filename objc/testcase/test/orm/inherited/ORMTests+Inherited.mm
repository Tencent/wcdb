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

#import "ORMInheritedTestCaseObject+WCTTableCoding.h"
#import "ORMInheritedTestCaseObject.h"
#import "ORMTestCase.h"

@interface ORMTests_Inherited : ORMTestCase

@end

@implementation ORMTests_Inherited {
    WCTDatabase *_database;

    NSString *_tableName;

    Class _cls;
}

- (void)setUp
{
    [super setUp];

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _tableName = self.className;

    _cls = ORMInheritedTestCaseObject.class;
}

- (void)tearDown
{
    XCTAssertTrue([_database removeFiles]);

    _database = nil;

    [super tearDown];
}

- (void)test_inherited
{
    NSString *createTableSQL = [NSString stringWithFormat:@"CREATE TABLE IF NOT EXISTS main.%@(variable1 INTEGER, variable2 TEXT, variable3 REAL, variable4 TEXT, variable5 REAL)", _tableName];
    [_expectedSQLs addObject:createTableSQL];
    NSString *insertSQL = [NSString stringWithFormat:@"INSERT INTO main.%@(variable1, variable2, variable3, variable4, variable5) VALUES(?1, ?2, ?3, ?4, ?5)", _tableName];
    [_expectedSQLs addObject:insertSQL];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:_cls]);

    ORMInheritedTestCaseObject *object = [ORMInheritedTestCaseObject objectWithId:1];
    XCTAssertTrue([_database insertObject:object intoTable:_tableName]);

    ORMInheritedTestCaseObject *result = [_database getObjectOfClass:_cls fromTable:_tableName];
    XCTAssertTrue([result isEqualToObject:object]);
}

@end
