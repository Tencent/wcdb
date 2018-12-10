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
#import "ColumnCodingObject+WCTTableCoding.h"
#import "ColumnCodingObject.h"

@interface ColumnCodingTests : CRUDTestCase

@end

@implementation ColumnCodingTests

- (void)setUp
{
    [super setUp];
    self.tableClass = ColumnCodingObject.class;

    TestCaseAssertTrue([self createTable]);
    [self.database close];
}

- (void)test
{
    ColumnCodingObject* object = [[ColumnCodingObject alloc] init];
    object.int32Object = [[Integer32CodingObject alloc] initWithValue:[NSNumber randomInt32]];
    object.int64Object = [[Integer64CodingObject alloc] initWithValue:[NSNumber randomInt64]];
    object.doubleObject = [[DoubleCodingObject alloc] initWithValue:[NSNumber randomDouble]];
    object.stringObject = [[StringCodingObject alloc] initWithValue:[NSString randomString]];
    object.dataObject = [[DataCodingObject alloc] initWithValue:[NSData randomData]];

    BOOL result = [self checkObjects:@[ object ]
                        andInsertSQL:@"INSERT INTO main.testTable(int32Object, int64Object, doubleObject, stringObject, dataObject) VALUES(?1, ?2, ?3, ?4, ?5)"
                           withCount:1
            asExpectedAfterInsertion:^BOOL {
                return [self.database insertObject:object intoTable:self.tableName];
            }];
    TestCaseAssertTrue(result);
}

@end
