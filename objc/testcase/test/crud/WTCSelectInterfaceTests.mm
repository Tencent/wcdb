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

@interface WTCSelectInterfaceTests : WTCCRUDTestCase

@end

@implementation WTCSelectInterfaceTests

- (void)testSelect
{
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertTrue([results.sorted isEqualToArray:self.preInsertedObjects.sorted]);
}

- (void)testConditionalSelect
{
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 2];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:@"object2"]);
}

- (void)testOrderedSelect
{
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name orderBy:WTCCRUDObject.variable2.asOrder(WCTOrderedDescending)];
    XCTAssertNotNil(results);
    XCTAssertTrue([results isEqualToArray:self.preInsertedObjects.sorted.reversed]);
}

- (void)testLimitedSelect
{
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name limit:1];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects.sorted[0]]);
}

- (void)testOffsetSelect
{
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name limit:1 offset:1];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects.sorted[1]]);
}

- (void)testPartialSelect
{
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOnProperties:WTCCRUDObject.variable2 fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, self.preInsertedObjects.count);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.preInsertedObjects[0].variable2]);
    XCTAssertTrue([results[1].variable2 isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testTableSelect
{
    //Give
    WCTTable *table = [self.database getTableOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class];
    XCTAssertNotNil(table);
    //When
    NSArray<WTCCRUDObject *> *results = [table getObjects];
    XCTAssertNotNil(results);
    XCTAssertTrue([results.sorted isEqualToArray:self.preInsertedObjects.sorted]);
}

- (void)testRowSelect
{
    //When
    WCTColumnsXRows *rows = [self.database getRowsOnResults:{WCDB::Column::All} fromTable:WTCCRUDObject.Name];
    //Then
    XCTAssertEqual(rows.count, self.preInsertedObjects.count);
    XCTAssertEqual(((NSNumber *) rows[0][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) rows[0][1] isEqualToString:self.preInsertedObjects[0].variable2]);
    XCTAssertEqual(((NSNumber *) rows[1][0]).intValue, self.preInsertedObjects[1].variable1);
    XCTAssertTrue([(NSString *) rows[1][1] isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testConditionalRowSelect
{
    //When
    WCTColumnsXRows *rows = [self.database getRowsOnResults:{WCDB::Column::All} fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 1];
    //Then
    XCTAssertEqual(rows.count, 1);
    XCTAssertEqual(((NSNumber *) rows[0][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) rows[0][1] isEqualToString:self.preInsertedObjects[0].variable2]);
}

- (void)testOrderedRowSelect
{
    //When
    WCTColumnsXRows *rows = [self.database getRowsOnResults:{WCDB::Column::All} fromTable:WTCCRUDObject.Name orderBy:WTCCRUDObject.variable1.asOrder(WCTOrderedDescending)];
    //Then
    XCTAssertEqual(rows.count, self.preInsertedObjects.count);
    XCTAssertEqual(((NSNumber *) rows[0][0]).intValue, self.preInsertedObjects[1].variable1);
    XCTAssertTrue([(NSString *) rows[0][1] isEqualToString:self.preInsertedObjects[1].variable2]);
    XCTAssertEqual(((NSNumber *) rows[1][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) rows[1][1] isEqualToString:self.preInsertedObjects[0].variable2]);
}

- (void)testLimitedRowSelect
{
    //When
    WCTColumnsXRows *rows = [self.database getRowsOnResults:{WCDB::Column::All} fromTable:WTCCRUDObject.Name limit:1];
    //Then
    XCTAssertEqual(rows.count, 1);
    XCTAssertEqual(((NSNumber *) rows[0][0]).intValue, self.preInsertedObjects[0].variable1);
    XCTAssertTrue([(NSString *) rows[0][1] isEqualToString:self.preInsertedObjects[0].variable2]);
}

- (void)testOffsetRowSelect
{
    //When
    WCTColumnsXRows *rows = [self.database getRowsOnResults:{WCDB::Column::All} fromTable:WTCCRUDObject.Name limit:1 offset:1];
    //Then
    XCTAssertEqual(rows.count, 1);
    XCTAssertEqual(((NSNumber *) rows[0][0]).intValue, self.preInsertedObjects[1].variable1);
    XCTAssertTrue([(NSString *) rows[0][1] isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testPartialRowSelect
{
    //When
    WCTColumnsXRows *rows = [self.database getRowsOnResults:WTCCRUDObject.variable2 fromTable:WTCCRUDObject.Name];
    //Then
    XCTAssertEqual(rows.count, self.preInsertedObjects.count);
    XCTAssertTrue([(NSString *) rows[0][0] isEqualToString:self.preInsertedObjects[0].variable2]);
    XCTAssertTrue([(NSString *) rows[1][0] isEqualToString:self.preInsertedObjects[1].variable2]);
}

@end
