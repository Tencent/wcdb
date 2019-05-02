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

@interface WithTests : BaseTestCase

@end

@implementation WithTests {
    WCDB::CTETable table1;
    WCDB::StatementSelect select1;
    WCDB::CTETable table2;
    WCDB::StatementSelect select2;
}

- (void)setUp
{
    [super setUp];
    table1 = @"testTable1";
    select1 = WCDB::StatementSelect().select(1);
    table2 = @"testTable2";
    select2 = WCDB::StatementSelect().select(2);
}

- (void)test_default_constructible
{
    WCDB::With constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::With().getType(), WCDB::SQL::Type::WithClause);
    TestCaseAssertEqual(WCDB::With::type, WCDB::SQL::Type::WithClause);
}

- (void)test_with
{
    auto testingSQL = WCDB::With().table(table1).as(select1);

    auto testingTypes = { WCDB::SQL::Type::WithClause, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"WITH testTable1 AS(SELECT 1)");
}

- (void)test_with_recursive
{
    auto testingSQL = WCDB::With().recursive().table(table1).as(select1);

    auto testingTypes = { WCDB::SQL::Type::WithClause, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"WITH RECURSIVE testTable1 AS(SELECT 1)");
}

- (void)test_with_multiple
{
    auto testingSQL = WCDB::With().table(table1).as(select1).table(table2).as(select2);

    auto testingTypes = { WCDB::SQL::Type::WithClause, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"WITH testTable1 AS(SELECT 1), testTable2 AS(SELECT 2)");
}

@end
