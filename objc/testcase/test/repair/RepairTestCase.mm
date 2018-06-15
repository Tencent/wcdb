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

#import "RepairTestCase.h"

@implementation RepairTestCase

- (void)setUp
{
    [super setUp];

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _tableName1 = [self.className stringByAppendingString:@"_1"];
    _preInsertedObjects1 = [TestCaseObject objectsWithCount:1000];
    XCTAssertTrue([_database createTableAndIndexes:_tableName1 withClass:TestCaseObject.class]);
    XCTAssertTrue([_database insertObjects:_preInsertedObjects1 intoTable:_tableName1]);

    _tableName2 = [self.className stringByAppendingString:@"_2"];
    _preInsertedObjects2 = [TestCaseObject objectsWithCount:1000];
    XCTAssertTrue([_database createTableAndIndexes:_tableName2 withClass:TestCaseObject.class]);
    XCTAssertTrue([_database insertObjects:_preInsertedObjects2 intoTable:_tableName2]);
}

- (void)tearDown
{
    XCTAssertTrue([_database removeFiles]);

    [super tearDown];
}

@end
