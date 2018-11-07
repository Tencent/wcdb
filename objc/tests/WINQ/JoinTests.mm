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

@interface JoinTests : WINQTestCase

@end

@implementation JoinTests {
    WCDB::TableOrSubquery tableOrSubquery;
    WCDB::TableOrSubquery tableOrSubquery1;
    WCDB::JoinConstraint joinConstraint1;
    WCDB::TableOrSubquery tableOrSubquery2;
    WCDB::JoinConstraint joinConstraint2;
}

- (void)setUp
{
    [super setUp];
    tableOrSubquery = @"testTable";
    tableOrSubquery1 = @"testTable1";
    joinConstraint1 = WCDB::JoinConstraint().on(1);
    tableOrSubquery2 = @"testTable2";
    joinConstraint2 = WCDB::JoinConstraint().on(2);
}

- (void)test_default_constructible
{
    WCDB::Join constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::Join().getType(), WCDB::SQL::Type::JoinClause);
    XCTAssertEqual(WCDB::Join::type, WCDB::SQL::Type::JoinClause);
}

- (void)test_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).join(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable JOIN main.testTable1");
}

- (void)test_join_with
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).with(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable, main.testTable1");
}

- (void)test_left_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).leftJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable LEFT JOIN main.testTable1");
}

- (void)test_left_outer_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).leftOuterJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable LEFT OUTER JOIN main.testTable1");
}

- (void)test_inner_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).innerJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable INNER JOIN main.testTable1");
}

- (void)test_cross_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).crossJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable CROSS JOIN main.testTable1");
}

- (void)test_natural_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable NATURAL JOIN main.testTable1");
}

- (void)test_natural_left_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalLeftJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable NATURAL LEFT JOIN main.testTable1");
}

- (void)test_natural_left_outer_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalLeftOuterJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable NATURAL LEFT OUTER JOIN main.testTable1");
}

- (void)test_natural_inner_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalInnerJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable NATURAL INNER JOIN main.testTable1");
}

- (void)test_natural_cross_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).naturalCrossJoin(tableOrSubquery1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable NATURAL CROSS JOIN main.testTable1");
}

- (void)test_join_with_constraint
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).join(tableOrSubquery1).constraint(joinConstraint1);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable JOIN main.testTable1 ON 1");
}

- (void)test_multiple_join
{
    auto testingSQL = WCDB::Join().table(tableOrSubquery).join(tableOrSubquery1).constraint(joinConstraint1).leftOuterJoin(tableOrSubquery2).constraint(joinConstraint2);

    auto testingTypes = { WCDB::SQL::Type::JoinClause, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::TableOrSubquery, WCDB::SQL::Type::Schema, WCDB::SQL::Type::JoinConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable JOIN main.testTable1 ON 1 LEFT OUTER JOIN main.testTable2 ON 2");
}

- (void)test_join_convertible
{
    WCDB::Join sql(WCDB::Join().table(@"testTable"));
    NSString* expected = @"(main.testTable)";
    WINQConvertibleTest(WCDB::TableOrSubquery, sql, expected);
    WINQConvertibleTest(WCDB::TablesOrSubqueries, sql, expected);
}

@end
