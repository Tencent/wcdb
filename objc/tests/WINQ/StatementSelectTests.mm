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

@interface StatementSelectTests : WINQTestCase

@end

@implementation StatementSelectTests {
    WCDB::CommonTableExpressions commonTableExpressions;
    WCDB::ResultColumns resultColumns;
    WCDB::TablesOrSubqueries tablesOrSubqueries;
    WCDB::Join join;
    WCDB::Expression condition;
    WCDB::Expressions groups;
    WCDB::Expression having;
    NSString* window1;
    WCDB::WindowDef windowDef1;
    NSString* window2;
    WCDB::WindowDef windowDef2;
    WCDB::Expressions values;
    WCDB::OrderingTerms orderingTerms;
    WCDB::Expression limit;
    WCDB::Expression limitParameter;
}

- (void)setUp
{
    [super setUp];
    commonTableExpressions = {
        WCDB::CommonTableExpression(@"testTable1").columns(WCDB::Column(@"testColumn1")).as(WCDB::StatementSelect().select(1)),
        WCDB::CommonTableExpression(@"testTable2").columns(WCDB::Column(@"testColumn2")).as(WCDB::StatementSelect().select(2)),
    };
    resultColumns = {
        1,
        2,
    };
    tablesOrSubqueries = {
        @"testTable1",
        @"testTable2",
    };
    join = WCDB::Join().table(@"testTable");
    condition = 1;
    groups = {
        1,
        2,
    };
    orderingTerms = {
        1,
        2,
    };
    having = 1;
    window1 = @"testWindow1";
    windowDef1 = WCDB::WindowDef().order(1);
    window2 = @"testWindow2";
    windowDef2 = WCDB::WindowDef().order(2);
    values = {
        1,
        2,
    };
    limit = 1;
    limitParameter = 2;
}

- (void)test_default_constructible
{
    WCDB::StatementSelect constructible __attribute((unused));
}

- (void)test_select
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2");
}

- (void)test_with_select
{
    auto testingSQL = WCDB::StatementSelect().with(commonTableExpressions).select(resultColumns);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"WITH testTable1(testColumn1) AS(SELECT 1), testTable2(testColumn2) AS(SELECT 2) SELECT 1, 2");
}

- (void)test_with_recursive_select
{
    auto testingSQL = WCDB::StatementSelect().with(commonTableExpressions).recursive().select(resultColumns);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"WITH RECURSIVE testTable1(testColumn1) AS(SELECT 1), testTable2(testColumn2) AS(SELECT 2) SELECT 1, 2");
}

- (void)test_distinct_select
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).distinct();

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT DISTINCT 1, 2");
}

- (void)test_select_from_tables_or_subqueries
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).from(tablesOrSubqueries);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 FROM main.testTable1, main.testTable2");
}

- (void)test_select_from_join
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).from(join);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 FROM main.testTable");
}

- (void)test_select_with_condition
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).where(condition);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 WHERE 1");
}

- (void)test_select_with_groups
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).group(groups);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 GROUP BY 1, 2");
}

- (void)test_select_with_having
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).group(groups).having(having);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 GROUP BY 1, 2 HAVING 1");
}

- (void)test_select_with_window
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).window(window1).as(windowDef1);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::WindowDef, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 WINDOW testWindow1 AS ORDER BY 1");
}

- (void)test_select_with_windows
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).window(window1).as(windowDef1).window(window2).as(windowDef2);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::WindowDef, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::WindowDef, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 WINDOW testWindow1 AS ORDER BY 1, testWindow2 AS ORDER BY 2");
}

- (void)test_values
{
    auto testingSQL = WCDB::StatementSelect().values(values);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"VALUES(1, 2)");
}

- (void)test_compound_multiple_select
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).group(groups).union_().select(resultColumns).where(condition);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 GROUP BY 1, 2 UNION SELECT 1, 2 WHERE 1");
}

- (void)test_compound_union
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).union_().values(values);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 UNION VALUES(1, 2)");
}

- (void)test_compound_union_all
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).unionAll().values(values);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 UNION ALL VALUES(1, 2)");
}

- (void)test_compound_intersect
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).intersect().values(values);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 INTERSECT VALUES(1, 2)");
}

- (void)test_compound_except
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).except().values(values);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 EXCEPT VALUES(1, 2)");
}

- (void)test_order
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).order(orderingTerms);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 ORDER BY 1, 2");
}

- (void)test_limit
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).limit(limit);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 LIMIT 1");
}

- (void)test_limit_length
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).limit(limit, limitParameter);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 LIMIT 1, 2");
}

- (void)test_limit_offset
{
    auto testingSQL = WCDB::StatementSelect().select(resultColumns).limit(limit).offset(limitParameter);

    auto testingTypes = { WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"SELECT 1, 2 LIMIT 1 OFFSET 2");
}

@end
