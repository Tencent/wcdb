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

#import "WTCAssert.h"
#import <WINQ/abstract.h>

@interface WTCExpressionTests : XCTestCase

@end

@implementation WTCExpressionTests

- (void)testExpression
{
    WCDB::LiteralValue literalValue = 1;
    WCDB::BindParameter bindParameter;
    WCDB::ColumnType columnType = WCDB::ColumnType::Integer32;
    std::string columnName = "testColumn";
    std::string collateName = "testCollate";
    WCDB::RaiseFunction raiseFunction;
    raiseFunction.withIgnore();
    WCDB::Expression expression = WCDB::Expression::ColumnNamed("testColumn");
    WCDB::ResultColumn resultColumn(expression);
    WCDB::StatementSelect selectSTMT = WCDB::StatementSelect().select(resultColumn);

    WINQAssertEqual(WCDB::Expression(literalValue), @"1");

    WINQAssertEqual(WCDB::Expression(bindParameter), @"?");

    WINQAssertEqual(WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn")).as(columnType), @"CAST(testColumn AS INTEGER)");

    WINQAssertEqual(WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn")).withCollate("testCollate"), @"testColumn COLLATE testCollate");

    WINQAssertEqual(WCDB::Expression::Exists(selectSTMT), @"EXISTS(SELECT testColumn)");

    WINQAssertEqual(WCDB::Expression::NotExists(selectSTMT), @"NOT EXISTS(SELECT testColumn)");

    WINQAssertEqual(WCDB::Expression(selectSTMT), @"(SELECT testColumn)");

    WINQAssertEqual(WCDB::Expression(selectSTMT, true), @"NOT(SELECT testColumn)");

    WINQAssertEqual(WCDB::Expression(raiseFunction), @"RAISE(IGNORE)");
}

- (void)testColumn
{
    std::string schemeName = "testSchema";
    std::string tableName = "testTable";
    std::string columnName = "testColumn";

    WINQAssertEqual(WCDB::Expression(WCDB::Expression::ColumnNamed(columnName)), @"testColumn");

    WINQAssertEqual(WCDB::Expression(WCDB::Expression::ColumnNamed(columnName).withTable(tableName)), @"testTable.testColumn");

    WINQAssertEqual(WCDB::Expression(WCDB::Expression::ColumnNamed(columnName).withTable(tableName).withSchema(schemeName)), @"testSchema.testTable.testColumn");
}

- (void)testUnaryOperation
{
    WCDB::Expression expression = WCDB::Expression::ColumnNamed("testColumn");

    WINQAssertEqual(!expression, @"NOT testColumn");
    WINQAssertEqual(~expression, @"~testColumn");
    WINQAssertEqual(+expression, @"+testColumn");
    WINQAssertEqual(-expression, @"-testColumn");
}

- (void)testBinaryOperation
{
    WCDB::Expression expression1 = WCDB::Expression::ColumnNamed("testColumn1");
    WCDB::Expression expression2 = WCDB::LiteralValue(1);

    WINQAssertEqual(expression1.concat(expression2), @"testColumn1 || 1");
    WINQAssertEqual(expression1 * expression2, @"testColumn1 * 1");
    WINQAssertEqual(expression1 / expression2, @"testColumn1 / 1");
    WINQAssertEqual(expression1 % expression2, @"testColumn1 % 1");
    WINQAssertEqual(expression1 + expression2, @"testColumn1 + 1");
    WINQAssertEqual(expression1 - expression2, @"testColumn1 - 1");
    WINQAssertEqual(expression1 << expression2, @"testColumn1 << 1");
    WINQAssertEqual(expression1 >> expression2, @"testColumn1 >> 1");
    WINQAssertEqual(expression1 & expression2, @"testColumn1 & 1");
    WINQAssertEqual(expression1 | expression2, @"testColumn1 | 1");
    WINQAssertEqual(expression1 < expression2, @"testColumn1 < 1");
    WINQAssertEqual(expression1 <= expression2, @"testColumn1 <= 1");
    WINQAssertEqual(expression1 > expression2, @"testColumn1 > 1");
    WINQAssertEqual(expression1 >= expression2, @"testColumn1 >= 1");
    WINQAssertEqual(expression1 == expression2, @"testColumn1 == 1");
    WINQAssertEqual(expression1 != expression2, @"testColumn1 != 1");
    WINQAssertEqual(expression1.is(expression2), @"testColumn1 IS 1");
    WINQAssertEqual(expression1.isNot(expression2), @"testColumn1 IS NOT 1");
    WINQAssertEqual(expression1 && expression2, @"testColumn1 AND 1");
    WINQAssertEqual(expression1 || expression2, @"testColumn1 OR 1");
}

- (void)testFunction
{
    WCDB::Expression expression1(WCDB::Expression::ColumnNamed("testColumn1"));
    WCDB::Expression expression2(WCDB::Expression::ColumnNamed("testColumn2"));
    std::list<WCDB::Expression> expressions = {expression1, expression2};
    std::string functionName = "testFunction";

    WINQAssertEqual(WCDB::Expression::Function(functionName, expression1, true), @"testFunction(DISTINCT testColumn1)");

    WINQAssertEqual(WCDB::Expression::Function(functionName, expressions, true), @"testFunction(DISTINCT testColumn1, testColumn2)");

    WINQAssertEqual(WCDB::Expression::Function(functionName, expression1), @"testFunction(testColumn1)");

    WINQAssertEqual(WCDB::Expression::Function(functionName), @"testFunction()");

    WINQAssertEqual(WCDB::Expression::FunctionAll(functionName), @"testFunction(*)");
}

- (void)testList
{
    WCDB::Expression expression1(WCDB::Expression::ColumnNamed("testColumn1"));
    WCDB::Expression expression2(WCDB::Expression::ColumnNamed("testColumn2"));
    std::list<WCDB::Expression> expressions = {expression1, expression2};

    WINQAssertEqual(WCDB::Expression(expressions), @"(testColumn1, testColumn2)");
}

- (void)testPattern
{
    WCDB::Expression expression1(WCDB::Expression::ColumnNamed("testColumn"));
    WCDB::Expression expression2(WCDB::LiteralValue("a%"));
    WCDB::Expression expression3((WCDB::BindParameter()));

    WINQAssertEqual(expression1.notLike(expression2), @"testColumn NOT LIKE 'a%'");

    WINQAssertEqual(expression1.notLike(expression2).withEscape(expression3), @"testColumn NOT LIKE 'a%' ESCAPE ?");

    WINQAssertEqual(expression1.notGlob(expression2), @"testColumn NOT GLOB 'a%'");

    WINQAssertEqual(expression1.notGlob(expression2).withEscape(expression3), @"testColumn NOT GLOB 'a%' ESCAPE ?");

    WINQAssertEqual(expression1.notRegexp(expression2), @"testColumn NOT REGEXP 'a%'");

    WINQAssertEqual(expression1.notRegexp(expression2).withEscape(expression3), @"testColumn NOT REGEXP 'a%' ESCAPE ?");

    WINQAssertEqual(expression1.notMatch(expression2), @"testColumn NOT MATCH 'a%'");

    WINQAssertEqual(expression1.notMatch(expression2).withEscape(expression3), @"testColumn NOT MATCH 'a%' ESCAPE ?");

    WINQAssertEqual(expression1.like(expression2), @"testColumn LIKE 'a%'");

    WINQAssertEqual(expression1.like(expression2).withEscape(expression3), @"testColumn LIKE 'a%' ESCAPE ?");

    WINQAssertEqual(expression1.glob(expression2), @"testColumn GLOB 'a%'");

    WINQAssertEqual(expression1.glob(expression2).withEscape(expression3), @"testColumn GLOB 'a%' ESCAPE ?");

    WINQAssertEqual(expression1.regexp(expression2), @"testColumn REGEXP 'a%'");

    WINQAssertEqual(expression1.regexp(expression2).withEscape(expression3), @"testColumn REGEXP 'a%' ESCAPE ?");

    WINQAssertEqual(expression1.match(expression2), @"testColumn MATCH 'a%'");

    WINQAssertEqual(expression1.match(expression2).withEscape(expression3), @"testColumn MATCH 'a%' ESCAPE ?");
}

- (void)testNull
{
    WCDB::Expression expression(WCDB::Expression::ColumnNamed("testColumn"));

    WINQAssertEqual(expression.isNull(), @"testColumn ISNULL");

    WINQAssertEqual(expression.notNull(), @"testColumn NOTNULL");
}

- (void)testBetween
{
    WCDB::Expression expression1(WCDB::Expression::ColumnNamed("testColumn"));
    WCDB::Expression expression2(WCDB::LiteralValue(1));
    WCDB::Expression expression3(WCDB::LiteralValue(3));

    WINQAssertEqual(expression1.notBetween(expression2, expression3), @"testColumn NOT BETWEEN 1 AND 3");

    WINQAssertEqual(expression1.between(expression2, expression3), @"testColumn BETWEEN 1 AND 3");
}

- (void)testIn
{
    WCDB::Expression expression = WCDB::Expression::ColumnNamed("testColumn");
    WCDB::ResultColumn resultColumn(expression);
    WCDB::StatementSelect selectSTMT = WCDB::StatementSelect().select(resultColumn);
    WCDB::Expression expression1(WCDB::LiteralValue(1));
    WCDB::Expression expression2(WCDB::LiteralValue(2));
    std::list<WCDB::Expression> expressions = {expression1, expression2};
    std::string tableName = "testTable";
    std::string tableFunctionName = "testFunction";
    std::string schemaName = "testSchema";

    WINQAssertEqual(expression.notIn(selectSTMT), @"testColumn NOT IN(SELECT testColumn)");

    WINQAssertEqual(expression.notIn(expression1), @"testColumn NOT IN(1)");

    WINQAssertEqual(expression.notIn(expressions), @"testColumn NOT IN(1, 2)");

    WINQAssertEqual(expression.in(selectSTMT), @"testColumn IN(SELECT testColumn)");

    WINQAssertEqual(expression.in(expression1), @"testColumn IN(1)");

    WINQAssertEqual(expression.in(expressions), @"testColumn IN(1, 2)");

    WINQAssertEqual(expression.notInTable(tableName), @"testColumn NOT IN testTable");

    WINQAssertEqual(expression.notInTable(schemaName, tableName), @"testColumn NOT IN testSchema.testTable");

    WINQAssertEqual(expression.inTable(tableName), @"testColumn IN testTable");

    WINQAssertEqual(expression.inTable(schemaName, tableName), @"testColumn IN testSchema.testTable");

    WINQAssertEqual(expression.notInFunction(tableFunctionName, expression1), @"testColumn NOT IN testFunction(1)");

    WINQAssertEqual(expression.notInFunction(tableFunctionName, expressions), @"testColumn NOT IN testFunction(1, 2)");

    WINQAssertEqual(expression.notInFunction(schemaName, tableFunctionName, expression1), @"testColumn NOT IN testSchema.testFunction(1)");

    WINQAssertEqual(expression.notInFunction(schemaName, tableFunctionName, expressions), @"testColumn NOT IN testSchema.testFunction(1, 2)");

    WINQAssertEqual(expression.inFunction(tableFunctionName, expression1), @"testColumn IN testFunction(1)");

    WINQAssertEqual(expression.inFunction(tableFunctionName, expressions), @"testColumn IN testFunction(1, 2)");

    WINQAssertEqual(expression.inFunction(schemaName, tableFunctionName, expression1), @"testColumn IN testSchema.testFunction(1)");

    WINQAssertEqual(expression.inFunction(schemaName, tableFunctionName, expressions), @"testColumn IN testSchema.testFunction(1, 2)");
}

- (void)testCase
{
    WCDB::Expression caseExpression = WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn1"));
    WCDB::Expression when1 = WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn2")) > WCDB::Expression(WCDB::LiteralValue(0));
    WCDB::Expression then1 = WCDB::LiteralValue(1);
    WCDB::Expression when2 = WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn3")) < WCDB::Expression(WCDB::LiteralValue(0));
    WCDB::Expression then2 = WCDB::LiteralValue(2);
    WCDB::Expression elseExpression = WCDB::LiteralValue(3);

    WINQAssertEqual(WCDB::Expression::Case(caseExpression).whenAndThen(when1, then1).else_(elseExpression), @"CASE testColumn1 WHEN testColumn2 > 0 THEN 1 ELSE 3 END");

    WINQAssertEqual(WCDB::Expression::Case(caseExpression).whenAndThen(when1, then1), @"CASE testColumn1 WHEN testColumn2 > 0 THEN 1 END");

    WINQAssertEqual(WCDB::Expression::Case(caseExpression).whenAndThen(when1, then1).whenAndThen(when2, then2), @"CASE testColumn1 WHEN testColumn2 > 0 THEN 1 WHEN testColumn3 < 0 THEN 2 END");

    WINQAssertEqual(WCDB::Expression::Case().whenAndThen(when1, then1), @"CASE WHEN testColumn2 > 0 THEN 1 END");
}

@end
