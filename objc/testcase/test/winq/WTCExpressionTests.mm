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

#import "WTCWINQTestCase.h"

@interface WTCExpressionTests : WTCWINQTestCase

@end

@implementation WTCExpressionTests

- (void)testExpression
{
    WINQAssertEqual(WCDB::Expression(self.class.literalValue), @"1");

    WCDB::BindParameter bindParameter;
    WINQAssertEqual(WCDB::Expression(bindParameter), @"?");

    WCDB::Expression column = WCDB::Expression(self.class.column);

    WINQAssertEqual(WCDB::Expression(column)
                        .as(WCDB::ColumnType::Integer32),
                    @"CAST(testColumn AS INTEGER)");

    WINQAssertEqual(WCDB::Expression(column)
                        .withCollate(self.class.collationName),
                    @"testColumn COLLATE testCollation");

    WINQAssertEqual(WCDB::Expression::Exists(self.class.statementSelect), @"EXISTS(SELECT testColumn FROM testTable)");

    WINQAssertEqual(WCDB::Expression::NotExists(self.class.statementSelect), @"NOT EXISTS(SELECT testColumn FROM testTable)");

    WINQAssertEqual(WCDB::Expression(self.class.statementSelect), @"(SELECT testColumn FROM testTable)");

    WINQAssertEqual(WCDB::Expression(self.class.statementSelect, true), @"NOT(SELECT testColumn FROM testTable)");

    WCDB::RaiseFunction raiseFunction = WCDB::RaiseFunction().withIgnore();
    WINQAssertEqual(WCDB::Expression(raiseFunction), @"RAISE(IGNORE)");
}

- (void)testColumn
{
    WINQAssertEqual(WCDB::Expression(self.class.column), @"testColumn");

    WINQAssertEqual(WCDB::Expression(self.class.column)
                        .withTable(self.class.tableName),
                    @"testTable.testColumn");

    WINQAssertEqual(WCDB::Expression(self.class.column)
                        .withTable(self.class.tableName)
                        .withSchema(self.class.schemaName),
                    @"testSchema.testTable.testColumn");
}

- (void)testUnaryOperation
{
    WCDB::Expression expression = WCDB::Expression(self.class.column);

    WINQAssertEqual(!expression, @"NOT testColumn");
    WINQAssertEqual(~expression, @"~testColumn");
    WINQAssertEqual(+expression, @"+testColumn");
    WINQAssertEqual(-expression, @"-testColumn");
}

- (void)testBinaryOperation
{
    WCDB::Expression expression1 = WCDB::Expression(self.class.column);
    WCDB::Expression expression2 = self.class.literalValue;

    WINQAssertEqual(expression1.concat(expression2), @"testColumn || 1");
    WINQAssertEqual(expression1 * expression2, @"testColumn * 1");
    WINQAssertEqual(expression1 / expression2, @"testColumn / 1");
    WINQAssertEqual(expression1 % expression2, @"testColumn % 1");
    WINQAssertEqual(expression1 + expression2, @"testColumn + 1");
    WINQAssertEqual(expression1 - expression2, @"testColumn - 1");
    WINQAssertEqual(expression1 << expression2, @"testColumn << 1");
    WINQAssertEqual(expression1 >> expression2, @"testColumn >> 1");
    WINQAssertEqual(expression1 & expression2, @"testColumn & 1");
    WINQAssertEqual(expression1 | expression2, @"testColumn | 1");
    WINQAssertEqual(expression1 < expression2, @"testColumn < 1");
    WINQAssertEqual(expression1 <= expression2, @"testColumn <= 1");
    WINQAssertEqual(expression1 > expression2, @"testColumn > 1");
    WINQAssertEqual(expression1 >= expression2, @"testColumn >= 1");
    WINQAssertEqual(expression1 == expression2, @"testColumn == 1");
    WINQAssertEqual(expression1 != expression2, @"testColumn != 1");
    WINQAssertEqual(expression1.is(expression2), @"testColumn IS 1");
    WINQAssertEqual(expression1.isNot(expression2), @"testColumn IS NOT 1");
    WINQAssertEqual(expression1 && expression2, @"testColumn AND 1");
    WINQAssertEqual(expression1 || expression2, @"testColumn OR 1");
}

- (void)testFunction
{
    WCDB::Expression expression1 = WCDB::Expression(self.class.column);
    WCDB::Expression expression2 = WCDB::Expression(self.class.column2);
    std::list<WCDB::Expression> expressions = {expression1, expression2};

    WINQAssertEqual(WCDB::Expression::Function(self.class.functionName, expression1, true), @"testFunction(DISTINCT testColumn)");

    WINQAssertEqual(WCDB::Expression::Function(self.class.functionName, expressions, true), @"testFunction(DISTINCT testColumn, testColumn2)");

    WINQAssertEqual(WCDB::Expression::Function(self.class.functionName, expression1), @"testFunction(testColumn)");

    WINQAssertEqual(WCDB::Expression::Function(self.class.functionName), @"testFunction()");

    WINQAssertEqual(WCDB::Expression::FunctionAll(self.class.functionName), @"testFunction(*)");
}

- (void)testList
{
    WCDB::Expression expression1 = WCDB::Expression(self.class.column);
    WCDB::Expression expression2 = WCDB::Expression(self.class.column2);
    std::list<WCDB::Expression> expressions = {expression1, expression2};

    WINQAssertEqual(WCDB::Expression(expressions), @"(testColumn, testColumn2)");
}

- (void)testPattern
{
    WCDB::Expression expression1(WCDB::Expression(self.class.column));
    WCDB::Expression expression2(WCDB::LiteralValue("a%"));
    WCDB::Expression expression3((WCDB::BindParameter()));

    WINQAssertEqual(expression1
                        .notLike(expression2),
                    @"testColumn NOT LIKE 'a%'");

    WINQAssertEqual(expression1
                        .notLike(expression2)
                        .withEscape(expression3),
                    @"testColumn NOT LIKE 'a%' ESCAPE ?");

    WINQAssertEqual(expression1
                        .notGlob(expression2),
                    @"testColumn NOT GLOB 'a%'");

    WINQAssertEqual(expression1
                        .notGlob(expression2)
                        .withEscape(expression3),
                    @"testColumn NOT GLOB 'a%' ESCAPE ?");

    WINQAssertEqual(expression1
                        .notRegexp(expression2),
                    @"testColumn NOT REGEXP 'a%'");

    WINQAssertEqual(expression1
                        .notRegexp(expression2)
                        .withEscape(expression3),
                    @"testColumn NOT REGEXP 'a%' ESCAPE ?");

    WINQAssertEqual(expression1
                        .notMatch(expression2),
                    @"testColumn NOT MATCH 'a%'");

    WINQAssertEqual(expression1
                        .notMatch(expression2)
                        .withEscape(expression3),
                    @"testColumn NOT MATCH 'a%' ESCAPE ?");

    WINQAssertEqual(expression1
                        .like(expression2),
                    @"testColumn LIKE 'a%'");

    WINQAssertEqual(expression1
                        .like(expression2)
                        .withEscape(expression3),
                    @"testColumn LIKE 'a%' ESCAPE ?");

    WINQAssertEqual(expression1
                        .glob(expression2),
                    @"testColumn GLOB 'a%'");

    WINQAssertEqual(expression1
                        .glob(expression2)
                        .withEscape(expression3),
                    @"testColumn GLOB 'a%' ESCAPE ?");

    WINQAssertEqual(expression1
                        .regexp(expression2),
                    @"testColumn REGEXP 'a%'");

    WINQAssertEqual(expression1
                        .regexp(expression2)
                        .withEscape(expression3),
                    @"testColumn REGEXP 'a%' ESCAPE ?");

    WINQAssertEqual(expression1
                        .match(expression2),
                    @"testColumn MATCH 'a%'");

    WINQAssertEqual(expression1
                        .match(expression2)
                        .withEscape(expression3),
                    @"testColumn MATCH 'a%' ESCAPE ?");
}

- (void)testNull
{
    WCDB::Expression expression = WCDB::Expression(self.class.column);

    WINQAssertEqual(expression.isNull(), @"testColumn ISNULL");
    WINQAssertEqual(expression.notNull(), @"testColumn NOTNULL");
}

- (void)testBetween
{
    WCDB::Expression expression1 = WCDB::Expression(self.class.column);
    WCDB::Expression expression2 = self.class.literalValue;
    WCDB::Expression expression3 = WCDB::LiteralValue(3);

    WINQAssertEqual(expression1
                        .notBetween(expression2, expression3),
                    @"testColumn NOT BETWEEN 1 AND 3");

    WINQAssertEqual(expression1
                        .between(expression2, expression3),
                    @"testColumn BETWEEN 1 AND 3");
}

- (void)testIn
{
    WCDB::Expression expression = WCDB::Expression(self.class.column);
    WCDB::Expression expression1 = self.class.literalValue;
    WCDB::Expression expression2 = WCDB::LiteralValue(2);
    std::list<WCDB::Expression> expressions = {expression1, expression2};

    WINQAssertEqual(expression
                        .notIn(self.class.statementSelect),
                    @"testColumn NOT IN(SELECT testColumn FROM testTable)");

    WINQAssertEqual(expression
                        .notIn(expression1),
                    @"testColumn NOT IN(1)");

    WINQAssertEqual(expression
                        .notIn(expressions),
                    @"testColumn NOT IN(1, 2)");

    WINQAssertEqual(expression
                        .in(self.class.statementSelect),
                    @"testColumn IN(SELECT testColumn FROM testTable)");

    WINQAssertEqual(expression
                        .in(expression1),
                    @"testColumn IN(1)");

    WINQAssertEqual(expression
                        .in(expressions),
                    @"testColumn IN(1, 2)");

    WINQAssertEqual(expression
                        .notInTable(self.class.tableName),
                    @"testColumn NOT IN testTable");

    WINQAssertEqual(expression
                        .notInTable(self.class.schemaName, self.class.tableName),
                    @"testColumn NOT IN testSchema.testTable");

    WINQAssertEqual(expression
                        .inTable(self.class.tableName),
                    @"testColumn IN testTable");

    WINQAssertEqual(expression
                        .inTable(self.class.schemaName, self.class.tableName),
                    @"testColumn IN testSchema.testTable");

    WINQAssertEqual(expression
                        .notInFunction(self.class.functionName, expression1),
                    @"testColumn NOT IN testFunction(1)");

    WINQAssertEqual(expression
                        .notInFunction(self.class.functionName, expressions),
                    @"testColumn NOT IN testFunction(1, 2)");

    WINQAssertEqual(expression
                        .notInFunction(self.class.schemaName, self.class.functionName, expression1),
                    @"testColumn NOT IN testSchema.testFunction(1)");

    WINQAssertEqual(expression
                        .notInFunction(self.class.schemaName, self.class.functionName, expressions),
                    @"testColumn NOT IN testSchema.testFunction(1, 2)");

    WINQAssertEqual(expression
                        .inFunction(self.class.functionName, expression1),
                    @"testColumn IN testFunction(1)");

    WINQAssertEqual(expression
                        .inFunction(self.class.functionName, expressions),
                    @"testColumn IN testFunction(1, 2)");

    WINQAssertEqual(expression
                        .inFunction(self.class.schemaName, self.class.functionName, expression1),
                    @"testColumn IN testSchema.testFunction(1)");

    WINQAssertEqual(expression
                        .inFunction(self.class.schemaName, self.class.functionName, expressions),
                    @"testColumn IN testSchema.testFunction(1, 2)");
}

- (void)testCase
{
    WCDB::Expression caseExpression = WCDB::Expression(self.class.column);
    WCDB::Expression when1 = WCDB::Expression(self.class.column2) > WCDB::Expression(WCDB::LiteralValue(0));
    WCDB::Expression then1 = WCDB::LiteralValue(1);
    WCDB::Expression when2 = WCDB::Expression(WCDB::Column("testColumn3")) < WCDB::Expression(WCDB::LiteralValue(0));
    WCDB::Expression then2 = WCDB::LiteralValue(2);
    WCDB::Expression elseExpression = WCDB::LiteralValue(3);

    WINQAssertEqual(WCDB::Expression::Case(caseExpression)
                        .whenAndThen(when1, then1)
                        .else_(elseExpression),
                    @"CASE testColumn WHEN testColumn2 > 0 THEN 1 ELSE 3 END");

    WINQAssertEqual(WCDB::Expression::Case(caseExpression)
                        .whenAndThen(when1, then1),
                    @"CASE testColumn WHEN testColumn2 > 0 THEN 1 END");

    WINQAssertEqual(WCDB::Expression::Case(caseExpression)
                        .whenAndThen(when1, then1)
                        .whenAndThen(when2, then2),
                    @"CASE testColumn WHEN testColumn2 > 0 THEN 1 WHEN testColumn3 < 0 THEN 2 END");

    WINQAssertEqual(WCDB::Expression::Case()
                        .whenAndThen(when1, then1),
                    @"CASE WHEN testColumn2 > 0 THEN 1 END");
}

@end
