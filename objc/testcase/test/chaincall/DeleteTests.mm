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

@interface DeleteTests : CRUDTestCase

@end

@implementation DeleteTests {
    WCTDelete *_delete;
}

- (void)setUp
{
    [super setUp];

    _delete = [[_database prepareDelete] fromTable:_tableName];
}

- (void)tearDown
{
    _delete = nil;
    [super tearDown];
}

- (void)test_statement
{
    WCDB::StatementDelete &statement = _delete.statement;
    NSString *expectedSQL = [NSString stringWithFormat:@"DELETE FROM main.%@", _tableName];
    XCTAssertEqual(statement.getDescription(), expectedSQL.UTF8String);
}

- (void)test_delete
{
    XCTAssertTrue([_delete execute]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)test_delete_where
{
    XCTAssertTrue([[_delete where:_greaterThan0Condition] execute]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray array];
    for (TestCaseObject *object in _preInserted) {
        if (object.variable1 > 0) {
            continue;
        }
        [expected addObject:object];
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_delete_limit
{
    XCTAssertTrue([[_delete limit:_limit1] execute]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected removeObjectAtIndex:0];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_delete_orderBy_limit
{
    XCTAssertTrue([[[_delete orderBy:_descendingOrder] limit:_limit1] execute]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected removeLastObject];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_delete_limit_offset
{
    XCTAssertTrue([[[_delete limit:_limit1] offset:_offset1] execute]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected removeObjectAtIndex:1];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

@end
