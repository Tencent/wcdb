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

@interface WTCInsertTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTInsert *insert;
@end

@implementation WTCInsertTests

- (void)setUp
{
    [super setUp];
    _insert = [self.database prepareInsertObjectsOfClass:WTCCRUDObject.class into:WTCCRUDObject.Name];
    XCTAssertNotNil(_insert);
}

- (void)tearDown
{
    _insert = nil;
    [super tearDown];
}

- (void)testBase
{
    XCTAssertEqual(self.insert.tag, self.database.tag);
    XCTAssertTrue([self.insert.path isEqualToString:self.database.path]);
}

- (void)testInsert
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable1 = (int) self.preInsertedObjects.count + 1;
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.insert executeWithObjects:@[ object ]]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == object.variable1];
    XCTAssertNotNil(result);
    XCTAssertTrue([result isEqual:object]);
}

- (void)testAutoIncrementInsert
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    int expectedRowID = (int) self.preInsertedObjects.count + 1;
    object.isAutoIncrement = YES;
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.insert executeWithObjects:@[ object ]]);
    //Then
    XCTAssertEqual(object.lastInsertedRowID, expectedRowID);
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == expectedRowID];
    XCTAssertNotNil(result);
    XCTAssertEqual(result.variable1, expectedRowID);
    XCTAssertTrue([result.variable2 isEqualToString:object.variable2]);
}

- (void)testInsertOrReplace
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    int expectedReplacedRowID = 1;
    object.variable1 = expectedReplacedRowID;
    object.variable2 = self.name;
    _insert = [self.database prepareInsertOrReplaceObjectsOfClass:WTCCRUDObject.class into:WTCCRUDObject.Name];
    XCTAssertNotNil(_insert);
    //When
    XCTAssertTrue([self.insert executeWithObjects:@[ object ]]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == expectedReplacedRowID];
    XCTAssertNotNil(result);
    XCTAssertTrue([result.variable2 isEqualToString:self.name]);
}

- (void)testPartialInsert
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable1 = (int) self.preInsertedObjects.count + 1;
    object.variable2 = self.name;
    _insert = [self.database prepareInsertObjectsOnProperties:WTCCRUDObject.variable1 into:WTCCRUDObject.Name];
    XCTAssertNotNil(_insert);
    //When
    XCTAssertTrue([self.insert executeWithObjects:@[ object ]]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == object.variable1];
    XCTAssertNotNil(result);
    XCTAssertNil(result.variable2);
}

- (void)testPartialInsertOrReplace
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    int expectedReplacedRowID = 1;
    object.variable1 = expectedReplacedRowID;
    object.variable2 = self.name;
    _insert = [self.database prepareInsertOrReplaceObjectsOnProperties:{WTCCRUDObject.variable1, WTCCRUDObject.variable2} into:WTCCRUDObject.Name];
    XCTAssertNotNil(_insert);
    //When
    XCTAssertTrue([self.insert executeWithObjects:@[ object ]]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == expectedReplacedRowID];
    XCTAssertNotNil(result);
    XCTAssertTrue([result.variable2 isEqualToString:self.name]);
}

- (void)testInsertEmpty
{
    XCTAssertFalse([self.insert executeWithObjects:@[]]);
}

@end
