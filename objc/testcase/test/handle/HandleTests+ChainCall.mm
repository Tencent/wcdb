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

@interface HandleTests_ChainCall : CRUDTestCase

@end

@implementation HandleTests_ChainCall {
    WCTHandle *_handle;
}

- (void)setUp
{
    [super setUp];

    _handle = [_database getHandle];

    XCTAssertNotNil(_handle);
}

- (void)tearDown
{
    [_handle finalizeHandle];
    _handle = nil;

    [super tearDown];
}

- (void)test_insert_object
{
    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];
    WCTInsert *insert = [[_handle prepareInsert] intoTable:_tableName];
    XCTAssertTrue([insert executeWithObject:object]);

    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected addObject:object];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    WCTUpdate *update = [[_handle prepareUpdate] table:_tableName];
    XCTAssertTrue(([[update onProperties:{TestCaseObject.variable2, TestCaseObject.variable3}] executeWithObject:object]));
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    for (TestCaseObject *element in expected) {
        element.variable2 = object.variable2;
        element.variable3 = object.variable3;
    }
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_delete
{
    WCTDelete *delete_ = [[_handle prepareDelete] fromTable:_tableName];
    XCTAssertTrue([delete_ execute]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)test_select
{
    WCTSelect *select = [[_handle prepareSelect] fromTable:_tableName];
    NSArray<TestCaseObject *> *objects = [[select ofClass:_cls] allObjects];
    XCTAssertTrue([objects isEqualToTestCaseObjects:_preInserted]);
}

- (void)test_rowSelect
{
    WCTRowSelect *rowSelect = [_handle prepareRowSelect];
    WCTColumnsXRows *rows = [[rowSelect fromTable:_tableName] allRows];
    XCTAssertTrue([rows isEqualToObjects:_preInserted
                          withComparator:^BOOL(WCTOneRow *lhs, TestCaseObject *rhs) {
                            if (lhs.count != 3) {
                                return NO;
                            }
                            return lhs[0].integer32Value == rhs.variable1 && [lhs[1].stringValue isEqualToString:rhs.variable2] && lhs[2].doubleValue == rhs.variable3;
                          }]);
}

- (void)test_multiSelect
{
    NSString *tableName2 = [_tableName stringByAppendingString:@"2"];

    XCTAssertTrue([_database createTableAndIndexes:tableName2 withClass:_cls]);

    NSArray<TestCaseObject *> *preInserted2 = [TestCaseObject objectsWithCount:(int) _preInserted.count];

    XCTAssertTrue([_database insertObjects:preInserted2 intoTable:tableName2]);

    WCTPropertyList properties = [_cls allProperties].inTable(_tableName);
    properties.addProperties([_cls allProperties].inTable(tableName2));

    WCTMultiSelect *multiSelect = [[[[_database prepareMultiSelect] onProperties:properties] fromTables:@[ _tableName, tableName2 ]] where:TestCaseObject.variable1.inTable(_tableName) == TestCaseObject.variable1.inTable(tableName2)];

    XCTAssertNotNil(multiSelect);

    NSArray<WCTMultiObject *> *multiObjects = [multiSelect allMultiObjects];
    XCTAssertEqual(multiObjects.count, _preInserted.count);
    for (NSUInteger i = 0; i < multiObjects.count; ++i) {
        TestCaseObject *first = (TestCaseObject *) [multiObjects[i] objectForKey:_tableName];
        XCTAssertTrue([first isEqualToObject:_preInserted[i]]);
        TestCaseObject *second = (TestCaseObject *) [multiObjects[i] objectForKey:tableName2];
        XCTAssertTrue([second isEqualToObject:preInserted2[i]]);
    }
}

@end
