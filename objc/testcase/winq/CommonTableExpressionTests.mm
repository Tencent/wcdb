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

@interface CommonTableExpressionTests : BaseTestCase

@end

@implementation CommonTableExpressionTests {
    NSString* table;
    WCDB::Column column1;
    WCDB::Column column2;
    WCDB::StatementSelect select;
}

- (void)setUp
{
    [super setUp];
    table = @"testTable";
    column1 = WCDB::Column(@"testColumn1");
    column2 = WCDB::Column(@"testColumn2");
    select = WCDB::StatementSelect().select(1);
}

- (void)test_default_constructible
{
    WCDB::CommonTableExpression constructible;
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::CommonTableExpression().getType(), WCDB::SQL::Type::CommonTableExpression);
    TestCaseAssertEqual(WCDB::CommonTableExpression::type, WCDB::SQL::Type::CommonTableExpression);
}

- (void)test_common_table_expression
{
    auto testingSQL = WCDB::CommonTableExpression(table).column(column1).as(select);

    auto testingTypes = { WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable(testColumn1) AS(SELECT 1)");
}

- (void)test_common_table_expression_with_columns
{
    auto testingSQL = WCDB::CommonTableExpression(table).column(column1).column(column2).as(select);

    auto testingTypes = { WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable(testColumn1, testColumn2) AS(SELECT 1)");
}

- (void)test_common_table_expression_without_column
{
    auto testingSQL = WCDB::CommonTableExpression(table).as(select);

    auto testingTypes = { WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable AS(SELECT 1)");
}

@end
