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

#import "CRUDTestCase.h"

@interface InsertTests : CRUDTestCase

@end

@implementation InsertTests {
    WCTInsert<TestCaseObject *> *_insert;
}

- (void)setUp
{
    [super setUp];

    _insert = [[_database prepareInsert] intoTable:_tableName];

    XCTAssertNotNil(_insert);
}

- (void)tearDown
{
    _insert = nil;
    [super tearDown];
}

- (void)test_statement
{
    WCDB::StatementInsert &statement = [_insert onProperties:TestCaseObject.variable1].statement;
    NSString *expectedSQL = [NSString stringWithFormat:@"INSERT INTO %@(variable1) VALUES(?1)", _tableName];
    XCTAssertEqual(statement.getDescription(), expectedSQL.UTF8String);
}

- (void)test_insert_object
{
    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];
    XCTAssertTrue([_insert executeWithObject:object]);

    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected addObject:object];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_insert_or_replace_object
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue([[_insert orReplace] executeWithObject:object]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    expected[0] = object;
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_insert_object_onProperties
{
    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];
    object.isAutoIncrement = YES;
    XCTAssertTrue(([[_insert onProperties:{TestCaseObject.variable2, TestCaseObject.variable3}] executeWithObject:object]));
    XCTAssertEqual(object.lastInsertedRowID, object.variable1);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected addObject:object];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_insert_objects
{
    NSArray<TestCaseObject *> *objects = [TestCaseObject objectsWithCount:3 from:(int) _preInserted.count];
    XCTAssertTrue([_insert executeWithObjects:objects]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected addObjectsFromArray:objects];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

@end
