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

#import "WTCCRUDTestCase.h"

@interface WTCRowSelectTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTRowSelect *rowSelect;
@end

@implementation WTCRowSelectTests

- (void)setUp
{
    [super setUp];
    _rowSelect = [self.database prepareSelectRowsFromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(_rowSelect);
}

- (void)tearDown
{
    _rowSelect = nil;
    [super tearDown];
}

- (void)testBase
{
    XCTAssertEqual(self.rowSelect.tag, self.database.tag);
    XCTAssertTrue([self.rowSelect.path isEqualToString:self.database.path]);
}

- (void)testRowSelect
{
    //When
    WCTColumnsXRows *results = self.rowSelect.allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, self.preInsertedObjects.count);

    XCTAssertEqual(((NSNumber *) results[0][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:self.preInsertedObjects[0].variable2]);

    XCTAssertEqual(((NSNumber *) results[1][0]).intValue, self.preInsertedObjects[1].variable1);
    XCTAssertTrue([(NSString *) results[1][1] isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testConditionalRowSelect
{
    //When
    WCTColumnsXRows *results = [self.rowSelect where:WTCCRUDObject.variable1 == 1].allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, 1);

    XCTAssertEqual(((NSNumber *) results[0][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:self.preInsertedObjects[0].variable2]);
}

- (void)testOrderedRowSelect
{
    //When
    WCTColumnsXRows *results = [self.rowSelect orderBy:WTCCRUDObject.variable1.asOrder(WCTOrderedDescending)].allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, self.preInsertedObjects.count);

    XCTAssertEqual(((NSNumber *) results[0][0]).intValue, self.preInsertedObjects[1].variable1);
    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:self.preInsertedObjects[1].variable2]);

    XCTAssertEqual(((NSNumber *) results[1][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) results[1][1] isEqualToString:self.preInsertedObjects[0].variable2]);
}

- (void)testLimitedRowSelect
{
    //When
    WCTColumnsXRows *results = [self.rowSelect limit:1].allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, 1);

    XCTAssertEqual(((NSNumber *) results[0][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:self.preInsertedObjects[0].variable2]);
}

- (void)testOffsetRowSelect
{
    //When
    WCTColumnsXRows *results = [[self.rowSelect limit:1] offset:1].allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, 1);

    XCTAssertEqual(((NSNumber *) results[0][0]).intValue, self.preInsertedObjects[1].variable1);
    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testGroupBySelect
{
    //Give
    NSMutableArray<WTCCRUDObject *> *preInsertedObjects = [[NSMutableArray<WTCCRUDObject *> alloc] init];
    {
        WTCCRUDObject *object1 = [[WTCCRUDObject alloc] init];
        object1.variable1 = 3;
        object1.variable2 = @"object1";
        [preInsertedObjects addObject:object1];

        WTCCRUDObject *object2 = [[WTCCRUDObject alloc] init];
        object2.variable1 = 4;
        object2.variable2 = @"object2";
        [preInsertedObjects addObject:object2];
    }
    XCTAssertTrue([self.database insertObjects:preInsertedObjects into:WTCCRUDObject.Name]);

    _rowSelect = [self.database prepareSelectRowsOnResults:{WCDB::Column::All.count()} fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(_rowSelect);
    [[self.rowSelect groupBy:WTCCRUDObject.variable2] having:WTCCRUDObject.variable1 > 0];
    //When
    WCTOneColumn *results = self.rowSelect.allValues;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, 2);
    XCTAssertEqual(((NSNumber *) results[0]).intValue, 2);
    XCTAssertEqual(((NSNumber *) results[1]).intValue, 2);
}

- (void)testPartialRowSelect
{
    //Give
    _rowSelect = [self.database prepareSelectRowsOnResults:WTCCRUDObject.variable2 fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(_rowSelect);
    //When
    WCTColumnsXRows *results = self.rowSelect.allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, self.preInsertedObjects.count);

    XCTAssertTrue([(NSString *) results[0][0] isEqualToString:self.preInsertedObjects[0].variable2]);

    XCTAssertTrue([(NSString *) results[1][0] isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testRowSelectIteration
{
    //When
    NSMutableArray *results = [[NSMutableArray alloc] init];
    while (WCTOneRow *row = self.rowSelect.nextRow) {
        [results addObject:row];
    }
    //Then
    XCTAssertEqual(results.count, self.preInsertedObjects.count);

    XCTAssertEqual(((NSNumber *) results[0][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:self.preInsertedObjects[0].variable2]);

    XCTAssertEqual(((NSNumber *) results[1][0]).intValue, self.preInsertedObjects[1].variable1);
    XCTAssertTrue([(NSString *) results[1][1] isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testRowSelectValue
{
    //When
    WCTOneColumn *results = self.rowSelect.allValues;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, self.preInsertedObjects.count);
    XCTAssertEqual(((NSNumber *) results[0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertEqual(((NSNumber *) results[1]).intValue, self.preInsertedObjects[1].variable1);
}

- (void)testRowSelectValueIteration
{
    //When
    NSMutableArray *results = [[NSMutableArray alloc] init];
    while (WCTValue *row = self.rowSelect.nextValue) {
        [results addObject:row];
    }
    //Then
    XCTAssertEqual(results.count, self.preInsertedObjects.count);

    XCTAssertEqual(((NSNumber *) results[0]).intValue, self.preInsertedObjects[0].variable1);

    XCTAssertEqual(((NSNumber *) results[1]).intValue, self.preInsertedObjects[1].variable1);
}

- (void)testRowSelectMultiTables
{
    //Give
    NSString *table = WTCCRUDObject.Name;
    NSString *table2 = [NSString stringWithFormat:@"%@_2", WTCCRUDObject.Name];
    XCTAssertTrue([self.database createTableAndIndexesOfName:table2 withClass:WTCCRUDObject.class]);

    WTCCRUDObject *object1 = [[WTCCRUDObject alloc] init];
    object1.variable1 = 1;
    object1.variable2 = @"object3";
    XCTAssertTrue([self.database insertObject:object1 into:table2]);

    WCTProperty column1Table1 = WTCCRUDObject.variable1.inTable(table);
    WCTProperty column1Table2 = WTCCRUDObject.variable1.inTable(table2);
    WCTProperty column2Table1 = WTCCRUDObject.variable2.inTable(table);
    WCTProperty column2Table2 = WTCCRUDObject.variable2.inTable(table2);
    //When
    _rowSelect = [[self.database prepareSelectRowsOnResults:{column2Table1, column2Table2} fromTables:@[ table, table2 ]] where:column1Table1 == column1Table2];
    XCTAssertNotNil(_rowSelect);
    WCTColumnsXRows *results = self.rowSelect.allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, 1);

    XCTAssertTrue([(NSString *) results[0][0] isEqualToString:@"object1"]);

    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:@"object3"]);
}

- (void)testRowSelectAllMultiTables
{
    //Give
    NSString *table = WTCCRUDObject.Name;
    NSString *table2 = [NSString stringWithFormat:@"%@_2", WTCCRUDObject.Name];
    XCTAssertTrue([self.database createTableAndIndexesOfName:table2 withClass:WTCCRUDObject.class]);

    WTCCRUDObject *object1 = [[WTCCRUDObject alloc] init];
    object1.variable1 = 1;
    object1.variable2 = @"object3";
    XCTAssertTrue([self.database insertObject:object1 into:table2]);

    WCTProperty column1Table1 = WTCCRUDObject.variable1.inTable(table);
    WCTProperty column1Table2 = WTCCRUDObject.variable1.inTable(table2);
    //When
    _rowSelect = [[self.database prepareSelectRowsFromTables:@[ table, table2 ]] where:column1Table1 == column1Table2];
    XCTAssertNotNil(_rowSelect);
    WCTColumnsXRows *results = self.rowSelect.allRows;
    XCTAssertNotNil(results);
    //Then
    XCTAssertEqual(results.count, 1);

    XCTAssertEqual(((NSNumber *) results[0][0]).intValue, 1);
    XCTAssertTrue([(NSString *) results[0][1] isEqualToString:@"object1"]);

    XCTAssertEqual(((NSNumber *) results[0][2]).intValue, 1);
    XCTAssertTrue([(NSString *) results[0][3] isEqualToString:@"object3"]);
}

- (void)testRowSelectFailed
{
    _rowSelect = [self.database prepareSelectRowsFromTables:@[]];
    XCTAssertNil(self.rowSelect.allRows);
}

@end
