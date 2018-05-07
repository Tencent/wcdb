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

@interface RowSelects : CRUDTestCase

@end

@implementation RowSelects {
    WCTRowSelect *_rowSelect;
}

- (void)setUp
{
    [super setUp];

    _rowSelect = [_database prepareRowSelect];

    XCTAssertNotNil(_rowSelect);
}

- (void)tearDown
{
    _rowSelect = nil;
    [super tearDown];
}

- (void)test_rowSelect
{
    WCTColumnsXRows *rows = [[_rowSelect fromTable:_tableName] allRows];
    XCTAssertTrue([rows isEqualToObjects:_preInserted
                          withComparator:^BOOL(WCTOneRow *lhs, TestCaseObject *rhs) {
                            if (lhs.count != 3) {
                                return NO;
                            }
                            return lhs[0].numberValue.intValue == rhs.variable1 && [lhs[1].stringValue isEqualToString:rhs.variable2] && lhs[2].numberValue.doubleValue == rhs.variable3;
                          }]);
}

- (void)test_rowSelect_onResults
{
    WCTColumnsXRows *rows = [[[_rowSelect fromTable:_tableName] onResultColumns:{TestCaseObject.variable1, TestCaseObject.variable3}] allRows];
    XCTAssertTrue([rows isEqualToObjects:_preInserted
                          withComparator:^BOOL(WCTOneRow *lhs, TestCaseObject *rhs) {
                            if (lhs.count != 2) {
                                return NO;
                            }
                            return lhs[0].numberValue.intValue == rhs.variable1 && lhs[1].numberValue.doubleValue == rhs.variable3;
                          }]);
}

- (void)test_rowSelect_distinct
{
    NSString *value = @"distinct";
    XCTAssertTrue([_database updateTable:_tableName onProperty:TestCaseObject.variable2 withValue:value]);
    WCTOneColumn *column = [[[[_rowSelect onResultColumns:TestCaseObject.variable2] distinct] fromTable:_tableName] allValues];
    XCTAssertEqual(column.count, 1);
    XCTAssertTrue([column[0].stringValue isEqualToString:value]);
}

- (void)test_rowSelect_fromTables
{
    NSString *tableName2 = [_tableName stringByAppendingString:@"2"];
    XCTAssertTrue([_database createTableAndIndexes:tableName2 withClass:_cls]);
    TestCaseObject *object = [TestCaseObject objectWithId:0];
    XCTAssertTrue([_database insertObject:object intoTable:tableName2]);
    WCTColumnsXRows *rows = [[_rowSelect fromTables:@[ _tableName, tableName2 ]] allRows];
    XCTAssertEqual(rows.count, _preInserted.count);
    for (int i = 0; i < rows.count; ++i) {
        XCTAssertEqual(rows[i].count, 6);
        XCTAssertEqual(rows[i][0].numberValue.intValue, _preInserted[i].variable1);
        XCTAssertTrue([rows[i][1].stringValue isEqualToString:_preInserted[i].variable2]);
        XCTAssertEqual(rows[i][2].numberValue.doubleValue, _preInserted[i].variable3);
        XCTAssertEqual(rows[i][3].numberValue.intValue, object.variable1);
        XCTAssertTrue([rows[i][4].stringValue isEqualToString:object.variable2]);
        XCTAssertEqual(rows[i][5].numberValue.doubleValue, object.variable3);
    }
}

- (void)test_rowSelect_nextRow
{
    [_rowSelect fromTable:_tableName];
    NSMutableArray *rows = [[NSMutableArray alloc] init];
    WCTOneRow *row;
    while ((row = [_rowSelect nextRow])) {
        [rows addObject:row];
    }
    XCTAssertTrue([rows isEqualToObjects:_preInserted
                          withComparator:^BOOL(WCTOneRow *lhs, TestCaseObject *rhs) {
                            if (lhs.count != 3) {
                                return NO;
                            }
                            return lhs[0].numberValue.intValue == rhs.variable1 && [lhs[1].stringValue isEqualToString:rhs.variable2] && lhs[2].numberValue.doubleValue == rhs.variable3;
                          }]);
}

- (void)test_rowSelect_allValues
{
    WCTOneColumn *column = [[[_rowSelect fromTable:_tableName] onResultColumns:TestCaseObject.variable3] allValues];
    XCTAssertTrue([column isEqualToObjects:_preInserted
                            withComparator:^BOOL(WCTValue *lhs, TestCaseObject *rhs) {
                              return lhs.numberValue.doubleValue == rhs.variable3;
                            }]);
}

- (void)test_rowSelect_nextValue
{
    [[_rowSelect fromTable:_tableName] onResultColumns:TestCaseObject.variable3];
    NSMutableArray *column = [[NSMutableArray alloc] init];
    WCTValue *value;
    while ((value = [_rowSelect nextValue])) {
        [column addObject:value];
    }
    XCTAssertTrue([column isEqualToObjects:_preInserted
                            withComparator:^BOOL(WCTValue *lhs, TestCaseObject *rhs) {
                              return lhs.numberValue.doubleValue == rhs.variable3;
                            }]);
}

- (void)test_rowSelect_groupBy_having
{
    XCTAssertTrue([_database updateTable:_tableName onProperty:TestCaseObject.variable3 withValue:@(1)]);
    XCTAssertTrue([_database updateTable:_tableName onProperty:TestCaseObject.variable3 withValue:@(2) where:TestCaseObject.variable1 > 0]);

    WCDB::Expression groupBy = TestCaseObject.variable3;
    WCDB::Expression having = TestCaseObject.variable3.count() == 1;
    WCTOneColumn *column = [[[[[_rowSelect onResultColumns:TestCaseObject.variable3] fromTable:_tableName] groupBy:groupBy] having:having] allValues];

    XCTAssertEqual(column.count, 1);
    XCTAssertEqual(column[0].numberValue.doubleValue, 1);
}

@end
