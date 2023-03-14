//
// Created by sanhuazhang on 2019/05/02
//

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

#import "WINQAssertion.h"

@interface FTSFunctionTests : BaseTestCase

@end

@implementation FTSFunctionTests {
    WCDB::Expression expression;
}

- (void)setUp
{
    [super setUp];
    expression = WCDB::Column(@"testColumn");
}

- (void)test
{
    TestCaseAssertSQLEqual(WCDB::FTSFunction::offsets(expression), @"offsets(testColumn)");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::fts3Snippet(expression, "testStart", "testEnd", "testEllipses", 0, 0), @"snippet(testColumn, 'testStart', 'testEnd', 'testEllipses', 0, 0)");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::fts3Snippet(expression), @"snippet(testColumn, '<b>', '</b>', '<b>...</b>', -1, -15)");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::matchinfo(expression), @"matchinfo(testColumn)");

    TestCaseAssertSQLEqual(WCDB::FTSFunction::bm25(expression), @"bm25(testColumn)");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::bm25(expression).arguments({ 5, 10 }), @"bm25(testColumn, 5, 10)");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::highlight(expression), @"highlight(testColumn, 0, '<b>', '</b>')");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::highlight(expression, 1, "testStart", "testEnd"), @"highlight(testColumn, 1, 'testStart', 'testEnd')");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::fts5Snippet(expression, 0, "testStart", "testEnd", "testEllipses", 0), @"snippet(testColumn, 0, 'testStart', 'testEnd', 'testEllipses', 0)");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::fts5Snippet(expression), @"snippet(testColumn, 0, '<b>', '</b>', '<b>...</b>', 64)");
    TestCaseAssertSQLEqual(WCDB::FTSFunction::substringMatchInfo(expression, 1, ",;"), @"substring_match_info(testColumn, 1, ',;')");
}

- (void)test_column_operable
{
    WCDB::Column column(@"testColumn");
    TestCaseAssertSQLEqual(column.fts3Snippet("testStart", "testEnd", "testEllipses", 0, 0), @"snippet(testColumn, 'testStart', 'testEnd', 'testEllipses', 0, 0)");
    TestCaseAssertSQLEqual(column.fts3Snippet(), @"snippet(testColumn, '<b>', '</b>', '<b>...</b>', -1, -15)");
    TestCaseAssertSQLEqual(column.offsets(), @"offsets(testColumn)");
    TestCaseAssertSQLEqual(column.matchinfo(), @"matchinfo(testColumn)");

    TestCaseAssertSQLEqual(column.bm25(), @"bm25(testColumn)");
    TestCaseAssertSQLEqual(column.bm25().arguments({ 5, 10 }), @"bm25(testColumn, 5, 10)");
    TestCaseAssertSQLEqual(column.highlight(), @"highlight(testColumn, 0, '<b>', '</b>')");
    TestCaseAssertSQLEqual(column.highlight(1, "testStart", "testEnd"), @"highlight(testColumn, 1, 'testStart', 'testEnd')");
    TestCaseAssertSQLEqual(column.fts5Snippet(0, "testStart", "testEnd", "testEllipses", 0), @"snippet(testColumn, 0, 'testStart', 'testEnd', 'testEllipses', 0)");
    TestCaseAssertSQLEqual(column.fts5Snippet(), @"snippet(testColumn, 0, '<b>', '</b>', '<b>...</b>', 64)");
    TestCaseAssertSQLEqual(column.substringMatchInfo(1, ",;"), @"substring_match_info(testColumn, 1, ',;')");
}

- (void)test_expression_operable
{
    WCDB::Expression column = WCDB::Column(@"testColumn");
    TestCaseAssertSQLEqual(column.fts3Snippet("testStart", "testEnd", "testEllipses", 0, 0), @"snippet(testColumn, 'testStart', 'testEnd', 'testEllipses', 0, 0)");
    TestCaseAssertSQLEqual(column.fts3Snippet(), @"snippet(testColumn, '<b>', '</b>', '<b>...</b>', -1, -15)");
    TestCaseAssertSQLEqual(column.offsets(), @"offsets(testColumn)");
    TestCaseAssertSQLEqual(column.matchinfo(), @"matchinfo(testColumn)");

    TestCaseAssertSQLEqual(column.bm25(), @"bm25(testColumn)");
    TestCaseAssertSQLEqual(column.bm25().arguments({ 5, 10 }), @"bm25(testColumn, 5, 10)");
    TestCaseAssertSQLEqual(column.highlight(), @"highlight(testColumn, 0, '<b>', '</b>')");
    TestCaseAssertSQLEqual(column.highlight(1, "testStart", "testEnd"), @"highlight(testColumn, 1, 'testStart', 'testEnd')");
    TestCaseAssertSQLEqual(column.fts5Snippet(0, "testStart", "testEnd", "testEllipses", 0), @"snippet(testColumn, 0, 'testStart', 'testEnd', 'testEllipses', 0)");
    TestCaseAssertSQLEqual(column.fts5Snippet(), @"snippet(testColumn, 0, '<b>', '</b>', '<b>...</b>', 64)");
    TestCaseAssertSQLEqual(column.substringMatchInfo(1, ",;"), @"substring_match_info(testColumn, 1, ',;')");
}

@end
