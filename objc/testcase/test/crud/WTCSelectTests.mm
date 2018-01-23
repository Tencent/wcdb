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

@interface WTCSelectTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTSelect *select;
@end

@implementation WTCSelectTests

- (void)setUp
{
    [super setUp];
    _select = [self.database prepareSelectObjectsOfClass:WTCCRUDObject.class fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(_select);
}

- (void)tearDown
{
    _select = nil;
    [super tearDown];
}

- (void)testBase
{
    XCTAssertEqual(self.select.tag, self.database.tag);
    XCTAssertTrue([self.select.path isEqualToString:self.database.path]);
}

- (void)testSelect
{
    NSArray<WTCCRUDObject *> *results = self.select.allObjects;
    XCTAssertNotNil(results);
    XCTAssertTrue([results.sorted isEqual:self.preInsertedObjects.sorted]);
}

- (void)testConditionalSelect
{
    NSArray<WTCCRUDObject *> *results = [self.select where:WTCCRUDObject.variable1 == 2].allObjects;
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0].variable2 isEqualToString:@"object2"]);
}

- (void)testOrderedSelect
{
    NSArray<WTCCRUDObject *> *results = [self.select orderBy:WTCCRUDObject.variable2.asOrder(WCTOrderedDescending)].allObjects;
    XCTAssertNotNil(results);
    XCTAssertTrue([results isEqual:self.preInsertedObjects.reversed]);
}

- (void)testLimitedSelect
{
    NSArray<WTCCRUDObject *> *results = [self.select limit:1].allObjects;
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[0]]);
}

- (void)testOffsetSelect
{
    NSArray<WTCCRUDObject *> *results = [[self.select limit:1] offset:1].allObjects;
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results[0] isEqual:self.preInsertedObjects[1]]);
}

- (void)testPartialSelect
{
    _select = [self.database prepareSelectObjectsOnProperties:WTCCRUDObject.variable2 fromTable:WTCCRUDObject.Name];
    XCTAssertNotNil(_select);

    NSArray<WTCCRUDObject *> *results = self.select.allObjects;
    XCTAssertNotNil(results);
    XCTAssertEqual(results.count, self.preInsertedObjects.count);
    XCTAssertEqual(results[0].variable1, 0);
    XCTAssertTrue([results[0].variable2 isEqualToString:self.preInsertedObjects[0].variable2]);
    XCTAssertEqual(results[1].variable1, 0);
    XCTAssertTrue([results[1].variable2 isEqualToString:self.preInsertedObjects[1].variable2]);
}

- (void)testSelectIteration
{
    NSMutableArray<WTCCRUDObject *> *results = [[NSMutableArray<WTCCRUDObject *> alloc] init];
    while (WTCCRUDObject *object = [self.select nextObject]) {
        [results addObject:object];
    }
    XCTAssertTrue([results isEqual:self.preInsertedObjects]);
}

@end
