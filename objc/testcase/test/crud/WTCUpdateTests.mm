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

@interface WTCUpdateTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTUpdate *update;
@end

@implementation WTCUpdateTests

- (void)setUp
{
    [super setUp];
    _update = [self.database prepareUpdateTable:WTCCRUDObject.Name onProperties:WTCCRUDObject.variable2];
    XCTAssertNotNil(_update);
}

- (void)tearDown
{
    _update = nil;
    [super tearDown];
}

- (void)testBase
{
    XCTAssertEqual(self.update.tag, self.database.tag);
    XCTAssertTrue([self.update.path isEqualToString:self.database.path]);
}

- (void)testUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.update executeWithObject:object]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    for (WTCCRUDObject *result : results) {
        XCTAssertTrue([result.variable2 isEqualToString:self.name]);
    }
}

- (void)testConditionalUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([[self.update where:WTCCRUDObject.variable1] executeWithObject:object]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 1];
    XCTAssertNotNil(result);
    XCTAssertTrue([self.name isEqualToString:result.variable2]);
}

- (void)testOrderedUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([[[self.update orderBy:WTCCRUDObject.variable1.asOrder(WCTOrderedDescending)] limit:1] executeWithObject:object]);
    //Then
    WTCCRUDObject *result = [self.database getObjectOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name where:WTCCRUDObject.variable1 == 2];
    XCTAssertNotNil(result);
    XCTAssertTrue([self.name isEqualToString:result.variable2]);
}

- (void)testLimitedUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([[self.update limit:1] executeWithObject:object]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.name]);
    XCTAssertTrue([results[1] isEqual:self.preInsertedObjects[1]]);
}

- (void)testOffsetUpdate
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([[[self.update limit:1] offset:1] executeWithObject:object]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
    XCTAssertTrue([results[1].variable2 isEqualToString:self.name]);
}

- (void)testUpdateChanges
{
    //Give
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;
    //When
    XCTAssertTrue([self.update executeWithObject:object]);
    //Then
    XCTAssertEqual(self.update.changes, self.preInsertedObjects.count);
}

- (void)testUpdateFailed
{
    WTCCRUDObject *object = [[WTCCRUDObject alloc] init];
    object.variable2 = self.name;

    _update = [self.database prepareUpdateTable:@"" onProperties:WTCCRUDObject.variable2];
    XCTAssertNotNil(_update);
    XCTAssertFalse([self.update executeWithObject:object]);

    _update = [self.database prepareUpdateTable:WTCCRUDObject.Name onProperties:{}];
    XCTAssertNotNil(_update);
    XCTAssertFalse([self.update executeWithObject:object]);
}

@end
