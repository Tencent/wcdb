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

#import "WINQTestCase.h"
#import <WCDB/WCDB.h>

@interface FTS3FunctionTests : WINQTestCase

@end

@implementation FTS3FunctionTests {
    WCDB::Expression expression;
}

- (void)setUp
{
    [super setUp];
    expression = WCDB::Column(@"testColumn");
}

- (void)test
{
    WINQAssertEqual(WCDB::FTS3Function::offsets(expression), @"offsets(testColumn)");
    WINQAssertEqual(WCDB::FTS3Function::snippet(expression, "testStart", "testEnd", "testEllipses", 0, 0), @"snippet(testColumn, 'testStart', 'testEnd', 'testEllipses', 0, 0)");
    WINQAssertEqual(WCDB::FTS3Function::snippet(expression), @"snippet(testColumn, '<b>', '</b>', '<b>...</b>', -1, -15)");
    WINQAssertEqual(WCDB::FTS3Function::matchinfo(expression), @"matchinfo(testColumn)");
}

- (void)test_column_operable
{
    WCDB::Column column(@"testColumn");
    WINQAssertEqual(column.snippet("testStart", "testEnd", "testEllipses", 0, 0), @"snippet(testColumn, 'testStart', 'testEnd', 'testEllipses', 0, 0)");
    WINQAssertEqual(column.snippet(), @"snippet(testColumn, '<b>', '</b>', '<b>...</b>', -1, -15)");
    WINQAssertEqual(column.matchinfo(), @"matchinfo(testColumn)");
}

- (void)test_expression_operable
{
    WCDB::Expression column = WCDB::Column(@"testColumn");
    WINQAssertEqual(column.snippet("testStart", "testEnd", "testEllipses", 0, 0), @"snippet(testColumn, 'testStart', 'testEnd', 'testEllipses', 0, 0)");
    WINQAssertEqual(column.snippet(), @"snippet(testColumn, '<b>', '</b>', '<b>...</b>', -1, -15)");
    WINQAssertEqual(column.matchinfo(), @"matchinfo(testColumn)");
}

@end
