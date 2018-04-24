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

@interface SelectTests : CRUDTestCase

@end

@implementation SelectTests {
    WCTSelect<TestCaseObject *> *_select;
}

- (void)setUp
{
    [super setUp];

    _select = [[_database prepareSelect] fromTable:_tableName];

    XCTAssertNotNil(_select);
}

- (void)tearDown
{
    _select = nil;
    [super tearDown];
}

- (void)test_statement
{
    WCDB::StatementSelect &statement = _select.statement;
    statement.select(WCDB::ResultColumn::all());
    NSString *expectedSQL = [NSString stringWithFormat:@"SELECT * FROM %@", _tableName];
    XCTAssertEqual(statement.getDescription(), expectedSQL.UTF8String);
}

- (void)test_select
{
    NSArray<TestCaseObject *> *objects = [[_select ofClass:_cls] allObjects];
    XCTAssertTrue([objects isEqualToTestCaseObjects:_preInserted]);
}

- (void)test_select_onProperties
{
    NSArray<TestCaseObject *> *objects = [[_select onProperties:{TestCaseObject.variable1, TestCaseObject.variable2}] allObjects];
    XCTAssertTrue([objects isEqualToTestCaseObjects:_preInserted onProperties:TestCaseObjectPropertiesVariable1 | TestCaseObjectPropertiesVariable2]);
}

- (void)test_select_where
{
    NSArray<TestCaseObject *> *objects = [[[_select ofClass:_cls] where:_greaterThan0Condition] allObjects];
    NSMutableArray<TestCaseObject *> *result = [NSMutableArray arrayWithArray:_preInserted];
    [result removeObjectAtIndex:0];
    XCTAssertTrue([objects isEqualToTestCaseObjects:result]);
}

- (void)test_select_orderBy
{
    NSArray<TestCaseObject *> *objects = [[[_select ofClass:_cls] orderBy:_descendingOrder] allObjects];
    NSArray<TestCaseObject *> *result = _preInserted.reversed;
    XCTAssertTrue([objects isEqualToTestCaseObjects:result]);
}

- (void)test_select_limit
{
    NSArray<TestCaseObject *> *objects = [[[_select ofClass:_cls] limit:_limit1] allObjects];
    NSMutableArray<TestCaseObject *> *result = [NSMutableArray<TestCaseObject *> arrayWithObject:[_preInserted objectAtIndex:0]];
    XCTAssertTrue([objects isEqualToTestCaseObjects:result]);
}

- (void)test_select_limit_offset
{
    NSArray<TestCaseObject *> *objects = [[[[_select ofClass:_cls] limit:_limit1] offset:_offset1] allObjects];
    NSMutableArray<TestCaseObject *> *result = [NSMutableArray<TestCaseObject *> arrayWithObject:_preInserted[1]];
    XCTAssertTrue([objects isEqualToTestCaseObjects:result]);
}

- (void)test_select_nextObject
{
    [_select ofClass:_cls];
    TestCaseObject *object;
    int index = 0;
    while (object = [_select nextObject]) {
        TestCaseObject *expected = _preInserted[index++];
        XCTAssertTrue([object isEqualToObject:expected]);
    }
    XCTAssertEqual(index, _preInserted.count);
}

@end
