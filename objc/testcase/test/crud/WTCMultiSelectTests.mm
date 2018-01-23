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
#import "WTCMultiSelectObject.h"
#import "WTCMultiSelectObject+WCTTableCoding.h"

@interface WTCMultiSelectTests : WTCCRUDTestCase
@property(nonatomic, readonly) WCTMultiSelect* multiSelect;
@property(nonatomic, readonly) NSArray<WTCMultiSelectObject*>* preInsertedMultiSelectObjects;
@property(nonatomic, readonly) NSArray* preInsertedMultiObjects;
@end

@implementation WTCMultiSelectTests

- (void)setUp {
    [super setUp];
    XCTAssertTrue([self.database createTableAndIndexesOfName:WTCMultiSelectObject.Name withClass:WTCMultiSelectObject.class]);
    {
        WTCMultiSelectObject* object1 = [[WTCMultiSelectObject alloc] init];
        object1.variable1 = 1;
        object1.variable2 = @"multiObject1";
        WTCMultiSelectObject* object2 = [[WTCMultiSelectObject alloc] init];
        object2.variable1 = 2;
        object2.variable2 = @"multiObject2";        
        _preInsertedMultiSelectObjects = @[object1, object2];
    }
    {
        _preInsertedMultiObjects = @[
          @{
              WTCCRUDObject.Name:self.preInsertedObjects[0],
              WTCMultiSelectObject.Name:self.preInsertedMultiSelectObjects[0]
          },
          @{
              WTCCRUDObject.Name:self.preInsertedObjects[1],
              WTCMultiSelectObject.Name:self.preInsertedMultiSelectObjects[1]
          }
          ];
    }
    XCTAssertTrue([self.database insertObjects:_preInsertedMultiSelectObjects into:WTCMultiSelectObject.Name]);
    
    WCTProperty property1Table1 = WTCCRUDObject.variable1.inTable(WTCCRUDObject.Name);
    WCTProperty property2Table1 = WTCCRUDObject.variable2.inTable(WTCCRUDObject.Name);
    WCTProperty property1Table2 = WTCMultiSelectObject.variable1.inTable(WTCMultiSelectObject.Name);
    WCTProperty property2Table2 = WTCMultiSelectObject.variable2.inTable(WTCMultiSelectObject.Name);
    _multiSelect = [[self.database prepareSelectMultiObjectsOnProperties:{property1Table1, property2Table1, property1Table2, property2Table2} fromTables:@[WTCCRUDObject.Name, WTCMultiSelectObject.Name]] where:property1Table1 == property1Table2];
    XCTAssertNotNil(_multiSelect);    
}

- (void)tearDown {
    _multiSelect = nil;
    [super tearDown];
}

- (void)testBase {
    XCTAssertEqual(self.multiSelect.tag, self.database.tag);
    XCTAssertTrue([self.multiSelect.path isEqualToString:self.database.path]);
}

- (void)testMultiSelect {
    //When
    NSArray* results = self.multiSelect.allMultiObjects;
    XCTAssertTrue([results.sorted isEqual:self.preInsertedMultiObjects.sorted]);
}

- (void)testOrderedSelect {
    //When
    NSArray* results = [self.multiSelect orderBy:WTCCRUDObject.variable1.inTable(WTCCRUDObject.Name).asOrder(WCTOrderedDescending)].allMultiObjects;
    XCTAssertTrue([results.sorted isEqual:self.preInsertedMultiObjects.sorted.reversed]);
}

- (void)testLimitedSelect {
    //When
    NSArray* results = [self.multiSelect limit:1].allMultiObjects;
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results.sorted[0] isEqual:self.preInsertedMultiObjects.sorted[0]]);
}

- (void)testOffsetSelect {
    //When
    NSArray* results = [[self.multiSelect limit:1] offset:1].allMultiObjects;
    XCTAssertEqual(results.count, 1);
    XCTAssertTrue([results.sorted[0] isEqual:self.preInsertedMultiObjects.sorted[1]]);
}

- (void)testMultiSelectIteration {
    //When
    NSMutableArray* results = [[NSMutableArray alloc] init];
    while (NSObject* object = self.multiSelect.nextMultiObject) {
        [results addObject:object];
    }
    XCTAssertTrue([results.sorted isEqual:self.preInsertedMultiObjects.sorted]);
}

- (void)testMultiSelectFailed {
    WCTProperty property1Table1 = WTCCRUDObject.variable1.inTable(WTCCRUDObject.Name);
    WCTProperty property2Table1 = WTCCRUDObject.variable2.inTable(WTCCRUDObject.Name);
    WCTProperty property1Table2 = WTCMultiSelectObject.variable1.inTable(WTCMultiSelectObject.Name);
    WCTProperty property2Table2 = WTCMultiSelectObject.variable2.inTable(WTCMultiSelectObject.Name);
    _multiSelect = [self.database prepareSelectMultiObjectsOnProperties:{property1Table1, property2Table1, property1Table2, property2Table2} fromTables:@[]];
    XCTAssertNotNil(_multiSelect);
    
    XCTAssertNil(self.multiSelect.allMultiObjects);
}

@end
