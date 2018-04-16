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

#import "MigrationTestCase.h"

@implementation MigrationTestCase

- (void)setUp
{
    [super setUp];

    _count = 100;

    int max = _count - 1;

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _cls = TestCaseObject.class;

    _tableName = self.className;

    _preInserted = [TestCaseObject objectsWithCount:_count];

    XCTAssertTrue([_database createTableAndIndexes:_tableName withClass:_cls]);

    XCTAssertTrue([_database insertObjects:_preInserted intoTable:_tableName]);

    _greaterThan0Condition = TestCaseObject.variable1 > 0;

    _removeBothEndCondition = TestCaseObject.variable1 > 0 && TestCaseObject.variable1 < max;

    _ascendingOrder = TestCaseObject.variable1.asOrder(WCTOrderedAscending);

    _descendingOrder = TestCaseObject.variable1.asOrder(WCTOrderedDescending);

    _limit1 = 1;

    _offset1 = 1;
}

- (void)tearDown
{
    XCTAssertTrue([_database dropTable:_tableName]);

    [_database close:^{
      XCTAssertTrue([_database removeFiles]);
    }];

    _database = nil;

    _cls = nil;

    _tableName = nil;

    _preInserted = nil;

    [super tearDown];
}

- (NSString *)schemaNameForPath:(NSString *)path
{
    return [NSString stringWithFormat:@"WCDBMigration_%zu", std::hash<std::string>{}(path.UTF8String)];
}

@end
