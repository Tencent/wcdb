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
}

- (NSArray<TestCaseObject *> *)insertObjectsOfCount:(int)count intoTable:(NSString *)tableName
{
    return [self insertObjectsOfCount:count from:0 intoTable:tableName];
}

- (NSArray<TestCaseObject *> *)insertObjectsOfCount:(int)count from:(int)offset intoTable:(NSString *)tableName
{
    NSArray<TestCaseObject *> *objects = [TestCaseObject objectsWithCount:count from:offset];
    if ([_database createTableAndIndexes:tableName withClass:TestCaseObject.class] && [_database insertOrReplaceObjects:objects intoTable:tableName]) {
        return objects;
    }
    return nil;
}

- (void)tearDown
{
    XCTAssertTrue([_database removeFiles]);

    [super tearDown];
}

@end
