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

@interface MultiSelectTests : CRUDTestCase

@end

@implementation MultiSelectTests {
    WCTMultiSelect *_multiSelect;
    NSString *_tableName2;
    NSArray<TestCaseObject *> *_preInserted2;
}

- (void)setUp
{
    [super setUp];

    _tableName2 = [_tableName stringByAppendingString:@"2"];

    XCTAssertTrue([_database createTableAndIndexes:_tableName2 withClass:_cls]);

    _preInserted2 = [TestCaseObject objectsWithCount:(int) _preInserted.count];

    XCTAssertTrue([_database insertObjects:_preInserted2 intoTable:_tableName2]);

    WCTPropertyList properties = [_cls AllProperties].inTable(_tableName);
    properties.addProperties([_cls AllProperties].inTable(_tableName2));

    _multiSelect = [[[[_database prepareMultiSelect] onProperties:properties] fromTables:@[ _tableName, _tableName2 ]] where:TestCaseObject.variable1.inTable(_tableName) == TestCaseObject.variable1.inTable(_tableName2)];

    XCTAssertNotNil(_multiSelect);
}

- (void)tearDown
{
    _tableName2 = nil;
    _preInserted2 = nil;
    _multiSelect = nil;
    [super tearDown];
}

- (void)test_multiSelect
{
    NSArray<WCTMultiObject *> *multiObjects = [_multiSelect allMultiObjects];
    XCTAssertEqual(multiObjects.count, _preInserted.count);
    for (NSUInteger i = 0; i < multiObjects.count; ++i) {
        TestCaseObject *first = (TestCaseObject *) [multiObjects[i] objectForKey:_tableName];
        XCTAssertTrue([first isEqualToObject:_preInserted[i]]);
        TestCaseObject *second = (TestCaseObject *) [multiObjects[i] objectForKey:_tableName2];
        XCTAssertTrue([second isEqualToObject:_preInserted2[i]]);
    }
}

- (void)test_multiSelect_nextMultiObject
{
    NSMutableArray<WCTMultiObject *> *multiObjects = [[NSMutableArray<WCTMultiObject *> alloc] init];
    WCTMultiObject *multiObject;
    while ((multiObject = [_multiSelect nextMultiObject])) {
        [multiObjects addObject:multiObject];
    }
    XCTAssertEqual(multiObjects.count, _preInserted.count);
    for (NSUInteger i = 0; i < multiObjects.count; ++i) {
        TestCaseObject *first = (TestCaseObject *) [multiObjects[i] objectForKey:_tableName];
        XCTAssertTrue([first isEqualToObject:_preInserted[i]]);
        TestCaseObject *second = (TestCaseObject *) [multiObjects[i] objectForKey:_tableName2];
        XCTAssertTrue([second isEqualToObject:_preInserted2[i]]);
    }
}

@end
