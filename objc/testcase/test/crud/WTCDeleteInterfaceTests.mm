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

@interface WTCDeleteInterfaceTests : WTCCRUDTestCase

@end

@implementation WTCDeleteInterfaceTests

- (void)testDelete
{
    //When
    XCTAssertTrue([self.database deleteObjectsFromTable:WTCCRUDObject.Name]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testConditionalDelete
{
    //When
    XCTAssertTrue([self.database deleteObjectsFromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 2]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testOrderedDelete
{
    //When
    XCTAssertTrue([self.database deleteObjectsFromTable:WTCCRUDObject.Name orderBy:WTCCRUDObject.variable1.asOrder(WCTOrderedDescending) limit:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testLimitedDelete
{
    //When
    XCTAssertTrue([self.database deleteObjectsFromTable:WTCCRUDObject.Name limit:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[1]]);
}

- (void)testOffsetDelete
{
    //When
    XCTAssertTrue([self.database deleteObjectsFromTable:WTCCRUDObject.Name limit:1 offset:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testTableDelete
{
    //Give
    WCTTable *table = [self.database getTableOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class];
    //When
    XCTAssertTrue([table deleteObjects]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testTableConditionalDelete
{
    //Give
    WCTTable *table = [self.database getTableOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class];
    //When
    XCTAssertTrue([table deleteObjectsWhere:WTCCRUDObject.variable1 == 2]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testTableOrderedDelete
{
    //Give
    WCTTable *table = [self.database getTableOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class];
    //When
    XCTAssertTrue([table deleteObjectsOrderBy:WTCCRUDObject.variable1.asOrder(WCTOrderedDescending) limit:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testTableLimitedDelete
{
    //Give
    WCTTable *table = [self.database getTableOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class];
    //When
    XCTAssertTrue([table deleteObjectsLimit:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[1]]);
}

- (void)testTableOffsetDelete
{
    //Give
    WCTTable *table = [self.database getTableOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class];
    //When
    XCTAssertTrue([table deleteObjectsLimit:1 offset:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

@end
