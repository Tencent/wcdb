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

#import "ORMTestCase.h"

@implementation ORMTestCase

- (void)setUp
{
    [super setUp];

    _tested = NO;

    _tableName = self.className;

    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];

    _expectedSQLs = [[NSMutableArray alloc] init];

    [WCTDatabase globalTraceSQL:^(NSString *sql) {
        if (_expectedSQLs.count > 0 && [sql isEqualToString:_expectedSQLs[0]]) {
            [_expectedSQLs removeObjectAtIndex:0];
            if (_expectedSQLs.count == 0) {
                _tested = YES;
            }
        }
    }];
}

- (void)tearDown
{
    XCTAssertTrue(_tested);

    [WCTDatabase globalTraceSQL:nil];

    XCTAssertTrue([_database removeFiles]);

    _database = nil;

    _expectedSQLs = nil;

    [super tearDown];
}

@end
