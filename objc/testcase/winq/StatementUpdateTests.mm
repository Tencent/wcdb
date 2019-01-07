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

@interface StatementUpdateTests : BaseTestCase

@end

@implementation StatementUpdateTests {
    WCDB::With with;
    WCDB::QualifiedTable table;
    WCDB::Column column;
    WCDB::Columns columns;
    WCDB::Expression expression1;
    WCDB::Expression expression2;
    WCDB::Expression condition;
    WCDB::OrderingTerm orderingTerm1;
    WCDB::OrderingTerm orderingTerm2;
    WCDB::OrderingTerms orderingTerms;
    WCDB::Expression limit;
    WCDB::Expression limitParameter;
}

- (void)setUp
{
    [super setUp];
    with = WCDB::With().table(@"testTable").as(WCDB::StatementSelect().select(1));
    table = @"testTable";
    column = WCDB::Column(@"testColumn");
    columns = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
    expression1 = 1;
    expression2 = 2;
    condition = 3;
    orderingTerm1 = 1;
    orderingTerm2 = 2;
    orderingTerms = {
        orderingTerm1,
        orderingTerm2,
    };
    limit = 1;
    limitParameter = 2;
}

- (void)test_default_constructible
{
    WCDB::StatementUpdate constructible __attribute((unused));
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementUpdate().getType(), WCDB::SQL::Type::UpdateSTMT);
    TestCaseAssertEqual(WCDB::StatementUpdate::type, WCDB::SQL::Type::UpdateSTMT);
}

- (void)test_update
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1");
}

- (void)test_update_with
{
    auto testingSQL = WCDB::StatementUpdate().with(with).update(table).set(column).to(expression1);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::WithClause, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"WITH testTable AS(SELECT 1) UPDATE main.testTable SET testColumn = 1");
}

- (void)test_update_or_rollback
{
    auto testingSQL = WCDB::StatementUpdate().update(table).orRollback().set(column).to(expression1);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE OR ROLLBACK main.testTable SET testColumn = 1");
}

- (void)test_update_or_abort
{
    auto testingSQL = WCDB::StatementUpdate().update(table).orAbort().set(column).to(expression1);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE OR ABORT main.testTable SET testColumn = 1");
}

- (void)test_update_or_replace
{
    auto testingSQL = WCDB::StatementUpdate().update(table).orReplace().set(column).to(expression1);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE OR REPLACE main.testTable SET testColumn = 1");
}

- (void)test_update_or_fail
{
    auto testingSQL = WCDB::StatementUpdate().update(table).orFail().set(column).to(expression1);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE OR FAIL main.testTable SET testColumn = 1");
}

- (void)test_update_or_ignore
{
    auto testingSQL = WCDB::StatementUpdate().update(table).orIgnore().set(column).to(expression1);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE OR IGNORE main.testTable SET testColumn = 1");
}

- (void)test_update_multiple
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1).set(columns).to(expression2);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1, (testColumn1, testColumn2) = 2");
}

- (void)test_update_with_condition
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1).where(condition);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1 WHERE 3");
}

- (void)test_update_with_seperated_orders
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1).order(orderingTerm1).order(orderingTerm2).limit(limit);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1 ORDER BY 1, 2 LIMIT 1");
}

- (void)test_update_with_orders
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1).orders(orderingTerms).limit(limit);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1 ORDER BY 1, 2 LIMIT 1");
}

- (void)test_update_with_limit
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1).limit(limit);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1 LIMIT 1");
}

- (void)test_update_with_limit_length
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1).limit(limit, limitParameter);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1 LIMIT 1, 2");
}

- (void)test_update_with_limit_offset
{
    auto testingSQL = WCDB::StatementUpdate().update(table).set(column).to(expression1).limit(limit).offset(limitParameter);

    auto testingTypes = { WCDB::SQL::Type::UpdateSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"UPDATE main.testTable SET testColumn = 1 LIMIT 1 OFFSET 2");
}

@end
