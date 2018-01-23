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

@interface WTCUpdateInterfaceTests : WTCCRUDTestCase

@end

@implementation WTCUpdateInterfaceTests

- (void)testUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.database updateRowsInTable:WTCCRUDObject.Name onProperties:WTCCRUDObject.variable2 withObject:object]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
    XCTAssertTrue([results[1].variable2 isEqualToString:self.name]);
}

- (void)testConditionalUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.database updateRowsInTable:WTCCRUDObject.Name onProperties:WTCCRUDObject.variable2 withObject:object where:WTCCRUDObject.variable1 == 1]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 1];
    XCTAssertNotNil(result);
    XCTAssertTrue([result.variable2 isEqualToString:self.name]);
}

- (void)testOrderedUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.database updateRowsInTable:WTCCRUDObject.Name onProperties:WTCCRUDObject.variable2 withObject:object orderBy:WTCCRUDObject.variable2.asOrder(WCTOrderedDescending) limit:1]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 2];
    XCTAssertNotNil(result);
    XCTAssertTrue([result.variable2 isEqualToString:self.name]);
}

- (void)testLimitedUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.database updateRowsInTable:WTCCRUDObject.Name onProperties:WTCCRUDObject.variable2 withObject:object limit:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
    XCTAssertTrue([results[1].variable2 isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testOffsetUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.database updateRowsInTable:WTCCRUDObject.Name onProperties:WTCCRUDObject.variable2 withObject:object limit:1 offset:1]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.preInsertedObjects[0].variable2]);
    XCTAssertTrue([results[1].variable2 isEqualToString:self.name]);
}

@end
