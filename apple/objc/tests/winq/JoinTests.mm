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

@interface JoinTests : BaseTestCase

@end

@implementation JoinTests {
    WCDB::TableOrSubquery tableOrSubquery;
    WCDB::TableOrSubquery tableOrSubquery1;
    WCDB::TableOrSubquery tableOrSubquery2;
}

- (void)setUp
{
    [super setUp];
    tableOrSubquery = @"testTable";
    tableOrSubquery1 = @"testTable1";
    tableOrSubquery2 = @"testTable2";
}

- (void)test_default_constructible
{
    WCDB::Join constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::Join().getType(), WCDB::SQL::Type::JoinClause);
    TestCaseAssertEqual(WCDB::Join::type, WCDB::SQL::Type::JoinClause);
}

- (void)test_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).join(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable JOIN testTable1");
}

- (void)test_join_with
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).with(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable, testTable1");
}

- (void)test_left_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).leftJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable LEFT JOIN testTable1");
}

- (void)test_left_outer_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).leftOuterJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable LEFT OUTER JOIN testTable1");
}

- (void)test_inner_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).innerJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable INNER JOIN testTable1");
}

- (void)test_cross_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).crossJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable CROSS JOIN testTable1");
}

- (void)test_natural_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable NATURAL JOIN testTable1");
}

- (void)test_natural_left_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalLeftJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable NATURAL LEFT JOIN testTable1");
}

- (void)test_natural_left_outer_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalLeftOuterJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable NATURAL LEFT OUTER JOIN testTable1");
}

- (void)test_natural_inner_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalInnerJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable NATURAL INNER JOIN testTable1");
}

- (void)test_natural_cross_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalCrossJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable NATURAL CROSS JOIN testTable1");
}

- (void)test_join_with_constraint
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).join(tableOrSubquery1).on(1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable JOIN testTable1 ON 1");
}

- (void)test_multiple_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).join(tableOrSubquery1).on(1).leftOuterJoin(tableOrSubquery2).on(2);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testTable JOIN testTable1 ON 1 LEFT OUTER JOIN testTable2 ON 2");
}

- (void)test_join_convertible
{
    WCDB::Join sql(WCDB::Join().table(@"testTable"));
    NSString* expected = @"(testTable)";
    TestCaseAssertWINQConvertible(WCDB::TableOrSubquery, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::TablesOrSubqueries, sql, expected);
}

@end
