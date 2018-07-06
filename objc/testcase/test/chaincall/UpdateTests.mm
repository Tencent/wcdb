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

@interface UpdateTests : CRUDTestCase

@end

@implementation UpdateTests {
    WCTPropertyList _properties;

    WCTUpdate<TestCaseObject *> *_update;
}

- (void)setUp
{
    [super setUp];

    _update = [[_database prepareUpdate] table:_tableName];

    _properties = { TestCaseObject.variable2, TestCaseObject.variable3 };
}

- (void)tearDown
{
    _update = nil;
    [super tearDown];
}

- (void)test_statement
{
    WCDB::StatementUpdate &statement = [_update onProperties:TestCaseObject.variable1].statement;
    NSString *expectedSQL = [NSString stringWithFormat:@"UPDATE main.%@ SET variable1 = ?1", _tableName];
    XCTAssertEqual(statement.getDescription(), expectedSQL.UTF8String);
}

- (void)test_update_withObject
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue(([[_update onProperties:_properties] executeWithObject:object]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    for (TestCaseObject *element in expected) {
        element.variable2 = object.variable2;
        element.variable3 = object.variable3;
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject_where
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue(([[[_update onProperties:_properties] where:_greaterThan0Condition] executeWithObject:object]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    for (TestCaseObject *element in expected) {
        if (element.variable1 > 0) {
            element.variable2 = object.variable2;
            element.variable3 = object.variable3;
        }
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject_orderBy_limit
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue(([[[[_update onProperties:_properties] orderBy:_descendingOrder] limit:_limit1] executeWithObject:object]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    expected[expected.count - 1].variable2 = object.variable2;
    expected[expected.count - 1].variable3 = object.variable3;
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject_limit
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue(([[[_update onProperties:_properties] limit:_limit1] executeWithObject:object]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    expected[0].variable2 = object.variable2;
    expected[0].variable3 = object.variable3;
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject_limit_offset
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue(([[[[_update onProperties:_properties] limit:_limit1] offset:_offset1] executeWithObject:object]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    expected[1].variable2 = object.variable2;
    expected[1].variable3 = object.variable3;
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withRow
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    WCTColumnCodingRow *row = [TestCaseObject rowWithObject:object onProperties:TestCaseObjectPropertiesVariable2 | TestCaseObjectPropertiesVariable3];
    XCTAssertTrue(([[_update onProperties:_properties] executeWithRow:row]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    for (TestCaseObject *element in expected) {
        element.variable2 = object.variable2;
        element.variable3 = object.variable3;
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withValue
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    WCTColumnCodingValue *value = (WCTColumnCodingValue *) object.variable2;
    XCTAssertTrue(([[_update onProperties:TestCaseObject.variable2] executeWithValue:value]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    for (TestCaseObject *element in expected) {
        element.variable2 = object.variable2;
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

@end
