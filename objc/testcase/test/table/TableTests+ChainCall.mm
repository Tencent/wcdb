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

@interface TableTests_ChainCall : CRUDTestCase

@end

@implementation TableTests_ChainCall {
    WCTTable<TestCaseObject *> *_table;
}

- (void)setUp
{
    [super setUp];

    _table = [_database getTable:_tableName withClass:_cls];

    XCTAssertNotNil(_table);
}

- (void)test_insert_object
{
    TestCaseObject *object = [TestCaseObject objectWithId:(int) _preInserted.count];
    WCTInsert *insert = [_table prepareInsert];
    XCTAssertTrue([insert executeWithObject:object]);

    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    NSMutableArray<TestCaseObject *> *expected = [NSMutableArray arrayWithArray:_preInserted];
    [expected addObject:object];
    XCTAssertTrue([results isEqualToTestCaseObjects:expected]);
}

- (void)test_update_withObject
{
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    WCTUpdate *update = [_table prepareUpdate];
    XCTAssertTrue(([[update onProperties:{ TestCaseObject.variable2, TestCaseObject.variable3 }] executeWithObject:object]));
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
    WCTDelete *delete_ = [_table prepareDelete];
    XCTAssertTrue([delete_ execute]);
    NSMutableArray<TestCaseObject *> *results = [NSMutableArray arrayWithArray:[_database getObjectsOfClass:_cls fromTable:_tableName orderBy:TestCaseObject.variable1]];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)test_select
{
    WCTSelect *select = [_table prepareSelect];
    NSArray<TestCaseObject *> *objects = [select allObjects];
    XCTAssertTrue([objects isEqualToTestCaseObjects:_preInserted]);
}

- (void)test_rowSelect
{
    WCTRowSelect *rowSelect = [_table prepareRowSelect];
    WCTColumnsXRows *rows = [rowSelect allRows];
    XCTAssertTrue([rows isEqualToObjects:_preInserted
                          withComparator:^BOOL(WCTOneRow *lhs, TestCaseObject *rhs) {
                              if (lhs.count != 3) {
                                  return NO;
                              }
                              return lhs[0].numberValue.intValue == rhs.variable1 && [lhs[1].stringValue isEqualToString:rhs.variable2] && lhs[2].numberValue.doubleValue == rhs.variable3;
                          }]);
}

@end
