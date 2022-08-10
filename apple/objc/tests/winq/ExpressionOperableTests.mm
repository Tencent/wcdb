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
#include <WCDB/WCTConvertible.h>

@interface ExpressionOperableTests : BaseTestCase

@end

@implementation ExpressionOperableTests {
    WCDB::Expression operand;
    WCDB::Expression expression;
    NSString* collation;
    WCDB::StatementSelect select;
    WCDB::Expressions expressions;
    WCDB::Schema schema;
    NSString* table;
    WCDB::Expression columnExpression;
    NSString* function;
}

- (void)setUp
{
    [super setUp];
    operand = 1;
    expression = 2;
    collation = @"testCollation";
    select = WCDB::StatementSelect().select(1);
    expressions = {
        1,
        2,
    };
    schema = @"testSchema";
    table = @"testTable";
    columnExpression = WCDB::Column(@"testColumn");
    function = @"testFunction";
}

- (void)test_unary_tilde
{
    auto testingSQL = ~columnExpression;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"~testColumn");
}

- (void)test_unary_not
{
    auto testingSQL = !columnExpression;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"NOT testColumn");
}

- (void)test_unary_positive
{
    auto testingSQL = +columnExpression;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"+testColumn");
}

- (void)test_unary_negative
{
    auto testingSQL = -columnExpression;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"-testColumn");
}

- (void)test_unary_is_null
{
    auto testingSQL = columnExpression.isNull();
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn ISNULL");
}

- (void)test_unary_not_null
{
    auto testingSQL = columnExpression.notNull();
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOTNULL");
}

- (void)test_binary_concat
{
    auto testingSQL = columnExpression.concat(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn || 1");
}

- (void)test_binary_multiply
{
    auto testingSQL = columnExpression * operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn * 1");
}

- (void)test_binary_divide
{
    auto testingSQL = columnExpression / operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn / 1");
}

- (void)test_binary_modulo
{
    auto testingSQL = columnExpression % operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn % 1");
}

- (void)test_binary_plus
{
    auto testingSQL = columnExpression + operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn + 1");
}

- (void)test_binary_minus
{
    auto testingSQL = columnExpression - operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn - 1");
}

- (void)test_binary_left_shift
{
    auto testingSQL = columnExpression << operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn << 1");
}

- (void)test_binary_right_shift
{
    auto testingSQL = columnExpression >> operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn >> 1");
}

- (void)test_binary_bitwise_and
{
    auto testingSQL = columnExpression & operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn & 1");
}

- (void)test_binary_bitwise_or
{
    auto testingSQL = columnExpression | operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn | 1");
}

- (void)test_binary_less
{
    auto testingSQL = columnExpression < operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn < 1");
}

- (void)test_binary_less_or_equal
{
    auto testingSQL = columnExpression <= operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn <= 1");
}

- (void)test_binary_greater
{
    auto testingSQL = columnExpression > operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn > 1");
}

- (void)test_binary_greater_or_equal
{
    auto testingSQL = columnExpression >= operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn >= 1");
}

- (void)test_binary_equal
{
    auto testingSQL = columnExpression == operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn == 1");
}

- (void)test_binary_equal_null
{
    NSString* content = nil;
    auto testingSQL = columnExpression == content;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn == NULL");
}

- (void)test_binary_not_equal
{
    auto testingSQL = columnExpression != operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn != 1");
}

- (void)test_binary_is
{
    auto testingSQL = columnExpression.is(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IS 1");
}

- (void)test_binary_is_not
{
    auto testingSQL = columnExpression.isNot(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IS NOT 1");
}

- (void)test_binary_and
{
    auto testingSQL = columnExpression && operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn AND 1");
}

- (void)test_binary_or
{
    auto testingSQL = columnExpression || operand;
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn OR 1");
}

- (void)test_binary_like
{
    auto testingSQL = columnExpression.like(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn LIKE 1");
}

- (void)test_binary_not_like
{
    auto testingSQL = columnExpression.notLike(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT LIKE 1");
}

- (void)test_binary_like_escape
{
    auto testingSQL = columnExpression.like(operand).escape(expression);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn LIKE 1 ESCAPE 2");
}

- (void)test_binary_glob
{
    auto testingSQL = columnExpression.glob(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn GLOB 1");
}

- (void)test_binary_not_glob
{
    auto testingSQL = columnExpression.notGlob(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT GLOB 1");
}

- (void)test_binary_match
{
    auto testingSQL = columnExpression.match(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn MATCH 1");
}

- (void)test_binary_not_match
{
    auto testingSQL = columnExpression.notMatch(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT MATCH 1");
}

- (void)test_binary_regexp
{
    auto testingSQL = columnExpression.regexp(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn REGEXP 1");
}

- (void)test_binary_not_regexp
{
    auto testingSQL = columnExpression.notRegexp(operand);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT REGEXP 1");
}

- (void)test_collate
{
    auto testingSQL = columnExpression.collate(collation);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn COLLATE testCollation");
}

- (void)test_between
{
    auto testingSQL = columnExpression.between(operand, expression);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn BETWEEN 1 AND 2");
}

- (void)test_between_not_between
{
    auto testingSQL = columnExpression.notBetween(operand, expression);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT BETWEEN 1 AND 2");
}

- (void)test_in
{
    auto testingSQL = columnExpression.in();
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN()");
}

- (void)test_not_in
{
    auto testingSQL = columnExpression.notIn();
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT IN()");
}

- (void)test_in_select
{
    auto testingSQL = columnExpression.in(select);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN(SELECT 1)");
}

- (void)test_not_in_select
{
    auto testingSQL = columnExpression.notIn(select);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT IN(SELECT 1)");
}

- (void)test_in_expressions
{
    auto testingSQL = columnExpression.in(expressions);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN(1, 2)");
}

- (void)test_not_in_expressions
{
    auto testingSQL = columnExpression.notIn(expressions);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT IN(1, 2)");
}

- (void)test_in_table
{
    auto testingSQL = columnExpression.inTable(table).schema(schema);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN testSchema.testTable");
}

- (void)test_in_table_without_schema
{
    auto testingSQL = columnExpression.inTable(table);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN main.testTable");
}

- (void)test_not_in_table
{
    auto testingSQL = columnExpression.notInTable(table).schema(schema);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT IN testSchema.testTable");
}

- (void)test_in_function
{
    auto testingSQL = columnExpression.inFunction(function).schema(schema).invoke().arguments(expressions);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN testSchema.testFunction(1, 2)");
}

- (void)test_in_function_without_schema
{
    auto testingSQL = columnExpression.inFunction(function).invoke().arguments(expressions);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN main.testFunction(1, 2)");
}

- (void)test_in_function_without_parameter
{
    auto testingSQL = columnExpression.inFunction(function).schema(schema).invoke();
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn IN testSchema.testFunction()");
}

- (void)test_not_in_function
{
    auto testingSQL = columnExpression.notInFunction(function).schema(schema).invoke().arguments(expressions);
    auto testingTypes = { WCDB::SQL::Type::Expression, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn NOT IN testSchema.testFunction(1, 2)");
}

@end
