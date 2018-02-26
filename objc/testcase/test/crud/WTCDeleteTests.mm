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

@interface WTCDeleteTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTDelete *delete_;
@end

@implementation WTCDeleteTests

- (void)setUp
{
    [super setUp];
    _delete_ = [self.database prepareDeleteFromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(_delete_);
}

- (void)tearDown
{
    [super tearDown];
    _delete_ = nil;
}

- (void)testBase
{
    XCTAssertEqual(self.delete_.tag, self.database.tag);
    XCTAssertTrue([self.delete_.path isEqualToString:self.database.path]);
}

- (void)testDelete
{
    //When
    XCTAssertTrue([self.delete_ execute]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 0);
}

- (void)testConditionalDelete
{
    //When
    XCTAssertTrue([[self.delete_ where:WTCCRUDObject.variable1 == 2] execute]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testOrderedDelete
{
    //When
    XCTAssertTrue([[[self.delete_ orderBy:WTCCRUDObject.variable1.asOrder(WCTOrderedDescending)] limit:1] execute]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testLimitedDelete
{
    //When
    XCTAssertTrue([[self.delete_ limit:1] execute]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[1]]);
}

- (void)testOffsetDelete
{
    //When
    XCTAssertTrue([[[self.delete_ limit:1] offset:1] execute]);
    //Then
    NSArray<WTCCRUDObject *> *results = [self.database getObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testDeleteChanges
{
    //When
    XCTAssertTrue([self.delete_ execute]);
    //Then
    XCTAssertEqual(self.delete_.changes, self.preInsertedObjects.count);
}

@end
