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

@interface AggregateFunctionTests : BaseTestCase

@end

@implementation AggregateFunctionTests {
    WCDB::Expression expression1;
    WCDB::Expression expression2;
}

- (void)setUp
{
    [super setUp];
    expression1 = 1;
    expression2 = 2;
}

- (void)test
{
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::avg(expression1), @"avg(1)");
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::count(expression1), @"count(1)");
    TestCaseAssertSQLEqual(WCDB::ResultColumnAll().count(), @"count(*)");
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::groupConcat(expression1), @"group_concat(1)");
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::groupConcat(expression1, expression2), @"group_concat(1, 2)");
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::max(expression1), @"max(1)");
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::min(expression1), @"min(1)");
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::sum(expression1), @"sum(1)");
    TestCaseAssertSQLEqual(WCDB::AggregateFunction::total(expression1), @"total(1)");
}

- (void)test_column_operable
{
    WCDB::Column column(@"testColumn");
    TestCaseAssertSQLEqual(column.avg(), @"avg(testColumn)");
    TestCaseAssertSQLEqual(column.count(), @"count(testColumn)");
    TestCaseAssertSQLEqual(column.groupConcat(), @"group_concat(testColumn)");
    TestCaseAssertSQLEqual(column.groupConcat(expression1), @"group_concat(testColumn, 1)");
    TestCaseAssertSQLEqual(column.max(), @"max(testColumn)");
    TestCaseAssertSQLEqual(column.min(), @"min(testColumn)");
    TestCaseAssertSQLEqual(column.sum(), @"sum(testColumn)");
    TestCaseAssertSQLEqual(column.total(), @"total(testColumn)");
}

- (void)test_expression_operable
{
    WCDB::Expression column = WCDB::Column(@"testColumn");
    TestCaseAssertSQLEqual(column.avg(), @"avg(testColumn)");
    TestCaseAssertSQLEqual(column.count(), @"count(testColumn)");
    TestCaseAssertSQLEqual(column.groupConcat(), @"group_concat(testColumn)");
    TestCaseAssertSQLEqual(column.groupConcat(expression1), @"group_concat(testColumn, 1)");
    TestCaseAssertSQLEqual(column.max(), @"max(testColumn)");
    TestCaseAssertSQLEqual(column.min(), @"min(testColumn)");
    TestCaseAssertSQLEqual(column.sum(), @"sum(testColumn)");
    TestCaseAssertSQLEqual(column.total(), @"total(testColumn)");
}

@end
