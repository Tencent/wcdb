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

@interface ExpressionTests : BaseTestCase

@end

@implementation ExpressionTests {
    WCDB::LiteralValue literalValue;
    WCDB::BindParameter bindParameter;
    WCDB::Schema schema;
    NSString* table;
    WCDB::Column column;
    WCDB::Expression expression1;
    WCDB::Expression expression2;
    WCDB::Expression expression3;
    WCDB::Expression expression4;
    WCDB::Expression expression5;
    WCDB::Expression expression6;
    NSString* function;
    WCDB::ColumnType columnType;
    WCDB::StatementSelect select;
    WCDB::Expressions expressions;
    WCDB::RaiseFunction raiseFunction;
    NSString* windowFunction;
    WCDB::Filter filter;
    WCDB::WindowDef windowDef;
    NSString* window;
}

- (void)setUp
{
    [super setUp];
    literalValue = 1;
    bindParameter = WCDB::BindParameter(1);
    schema = @"testSchema";
    table = @"testTable";
    column = WCDB::Column(@"testColumn");
    expression1 = 1;
    expression2 = 2;
    expression3 = 3;
    expression4 = 4;
    expression5 = 5;
    expression6 = 6;
    function = @"testFunction";
    columnType = WCDB::ColumnType::Integer32;
    select = WCDB::StatementSelect().select(1);
    expressions = {
        1,
        2,
    };
    raiseFunction = WCDB::RaiseFunction().ignore();
    windowFunction = @"testWindowFunction";
    filter = WCDB::Filter().where(1);
    windowDef = WCDB::WindowDef();
    window = @"testWindow";
}

- (void)test_default_constructible
{
    WCDB::Expression constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::Expression().getType(), WCDB::SQL::Type::Expression);
    TestCaseAssertEqual(WCDB::Expression::type, WCDB::SQL::Type::Expression);
}

- (void)test_literal_value
{
    auto testingSQL = WCDB::Expression(literalValue);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"1");
}

- (void)test_bind_parameter
{
    auto testingSQL = WCDB::Expression(bindParameter);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::BindParameter };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"?1");
}

- (void)test_column
{
    auto testingSQL = WCDB::Expression(column).table(table).schema(schema);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testSchema.testTable.testColumn");
}

- (void)test_column_without_schema
{
    auto testingSQL = WCDB::Expression(column).table(table);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"main.testTable.testColumn");
}

- (void)test_column_without_table
{
    auto testingSQL = WCDB::Expression(column);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn");
}

- (void)test_function
{
    auto testingSQL = WCDB::Expression::function(function).distinct().invoke().argument(expression1);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testFunction(DISTINCT 1)");
}

- (void)test_function_with_arguments
{
    auto testingSQL = WCDB::Expression::function(function).distinct().invoke().argument(expression1).argument(expression2);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testFunction(DISTINCT 1, 2)");
}

- (void)test_function_without_distinct
{
    auto testingSQL = WCDB::Expression::function(function).invoke().argument(expression1);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testFunction(1)");
}

- (void)test_function_without_parameter
{
    auto testingSQL = WCDB::Expression::function(function);

    auto testingTypes = { WCDB::SQL::Type::Expression };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testFunction()");
}

- (void)test_function_all
{
    auto testingSQL = WCDB::Expression::function(function).invokeAll();

    auto testingTypes = { WCDB::SQL::Type::Expression };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testFunction(*)");
}

- (void)test_expression_operable
{
    // see ExpressionOperableTests
}

- (void)test_core_function_operable
{
    // see CoreFunctionTests
}

- (void)test_aggregate_function_operable
{
    // see AggregateFunctionTests
}

- (void)test_fts3_function_operable
{
    // see FTS3FunctionTests
}

- (void)test_expressions
{
    auto testingSQL = WCDB::Expression(expressions);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"(1, 2)");
}

- (void)test_cast
{
    auto testingSQL = WCDB::Expression::cast(expression1).as(columnType);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CAST(1 AS INTEGER)");
}

- (void)test_select
{
    auto testingSQL = WCDB::Expression(select);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"(SELECT 1)");
}

- (void)test_exists
{
    auto testingSQL = WCDB::Expression::exists(select);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"EXISTS(SELECT 1)");
}

- (void)test_not_exists
{
    auto testingSQL = WCDB::Expression::notExists(select);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"NOT EXISTS(SELECT 1)");
}

- (void)test_case
{
    auto testingSQL = WCDB::Expression::case_(expression1).when(expression2).then(expression3).else_(expression4);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CASE 1 WHEN 2 THEN 3 ELSE 4 END");
}

- (void)test_case_without_case
{
    auto testingSQL = WCDB::Expression::case_().when(expression2).then(expression3).else_(expression4);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CASE WHEN 2 THEN 3 ELSE 4 END");
}

- (void)test_case_multiple_when_then
{
    auto testingSQL = WCDB::Expression::case_(expression1).when(expression2).then(expression3).when(expression4).then(expression5).else_(expression6);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CASE 1 WHEN 2 THEN 3 WHEN 4 THEN 5 ELSE 6 END");
}

- (void)test_case_without_else
{
    auto testingSQL = WCDB::Expression::case_(expression1).when(expression2).then(expression3);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CASE 1 WHEN 2 THEN 3 END");
}

- (void)test_raise_function
{
    auto testingSQL = WCDB::Expression(raiseFunction);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::RaiseFunction };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"RAISE(IGNORE)");
}

- (void)test_window_function
{
    auto testingSQL = WCDB::Expression::windowFunction(windowFunction).invoke().argument(expression1).over(windowDef);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::WindowDef };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testWindowFunction(1) OVER()");
}

- (void)test_window_function_with_arguments
{
    auto testingSQL = WCDB::Expression::windowFunction(windowFunction).invoke().argument(expression1).argument(expression2).over(windowDef);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::WindowDef };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testWindowFunction(1, 2) OVER()");
}

- (void)test_window_function_without_parameter
{
    auto testingSQL = WCDB::Expression::windowFunction(windowFunction).invoke().over(windowDef);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::WindowDef };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testWindowFunction() OVER()");
}

- (void)test_window_function_all
{
    auto testingSQL = WCDB::Expression::windowFunction(windowFunction).invokeAll().over(windowDef);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::WindowDef };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testWindowFunction(*) OVER()");
}

- (void)test_window_function_with_filter
{
    auto testingSQL = WCDB::Expression::windowFunction(windowFunction).invoke().argument(expression1).filter(filter).over(windowDef);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Filter, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::WindowDef };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testWindowFunction(1) FILTER(WHERE 1) OVER()");
}

- (void)test_window_function_with_name
{
    auto testingSQL = WCDB::Expression::windowFunction(windowFunction).invoke().argument(expression1).over(window);

    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testWindowFunction(1) OVER testWindow");
}

- (void)test_expression_convertible
{
    WCDB::Expression sql(1);
    NSString* expected = @"1";
    TestCaseAssertWINQConvertible(WCDB::Expression, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Expressions, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerm, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerms, sql, expected);
}

- (void)test_float_precision
{
    auto expr = WCDB::Column(@"testColumn") > 123456789.123456;
    TestCaseAssertSQLEqual(expr, @"testColumn > 123456789.123456");
}

- (void)test_expressions_convertible
{
    WCDB::Expressions sqls = {
        1,
        WCDB::Column(@"testColumn"),
    };
    NSString* expected = @"1, testColumn";
    TestCaseAssertWINQConvertible(WCDB::Expressions, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerms, sqls, expected);
}

- (void)test_precedence
{
    TestCaseAssertSQLEqual((!(((column && 1) > 2) - 3)).collate(@"testCollation"), @"(NOT (((testColumn AND 1) > 2) - 3)) COLLATE testCollation");
}

- (void)test_boundary
{
    WCDB::Expression int64Min = std::numeric_limits<int64_t>::min();
    TestCaseAssertSQLEqual(int64Min, @"-9223372036854775808");
    WCDB::Expression int64Max = std::numeric_limits<int64_t>::max();
    TestCaseAssertSQLEqual(int64Max, @"9223372036854775807");

    WCDB::Expression uint64Min = std::numeric_limits<uint64_t>::min();
    TestCaseAssertSQLEqual(uint64Min, @"0");
    WCDB::Expression uint64Max = std::numeric_limits<uint64_t>::max();
    TestCaseAssertSQLEqual(uint64Max, @"18446744073709551615");

    WCDB::Expression enumNSMin = EnumNSTypeMin;
    TestCaseAssertSQLEqual(enumNSMin, @"-9223372036854775808");
    WCDB::Expression enumNSMax = EnumNSTypeMax;
    TestCaseAssertSQLEqual(enumNSMax, @"9223372036854775807");

    WCDB::Expression optionNSMin = OptionNSTypeMin;
    TestCaseAssertSQLEqual(optionNSMin, @"-9223372036854775808");
    WCDB::Expression optionNSMax = OptionNSTypeMax;
    TestCaseAssertSQLEqual(optionNSMax, @"9223372036854775807");

    WCDB::Expression enumMin = EnumMin;
    TestCaseAssertSQLEqual(enumMin, @"-2147483648");
    WCDB::Expression enumMax = EnumMax;
    TestCaseAssertSQLEqual(enumMax, @"2147483647");

    WCDB::Expression enumClassMin = EnumClassType::Min;
    TestCaseAssertSQLEqual(enumClassMin, @"-2147483648");
    WCDB::Expression enumClassMax = EnumClassType::Max;
    TestCaseAssertSQLEqual(enumClassMax, @"2147483647");

    WCDB::LiteralValue numberMax = [NSNumber numberWithUnsignedLongLong:std::numeric_limits<uint64_t>::max()];
    TestCaseAssertSQLEqual(numberMax, @"18446744073709551615");
}

@end
