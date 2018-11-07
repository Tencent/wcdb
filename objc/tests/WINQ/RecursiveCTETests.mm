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

@interface RecursiveCTETests : WINQTestCase

@end

@implementation RecursiveCTETests {
    WCDB::CTETable table;
    WCDB::StatementSelect initialSelect;
    WCDB::StatementSelect recursiveSelect;
}

- (void)setUp
{
    [super setUp];
    table = @"testTable";
    initialSelect = WCDB::StatementSelect().select(1);
    recursiveSelect = WCDB::StatementSelect().select(2);
}

- (void)test_default_constructible
{
    WCDB::RecursiveCTE constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::RecursiveCTE().getType(), WCDB::SQL::Type::RecursiveCTE);
    XCTAssertEqual(WCDB::RecursiveCTE::type, WCDB::SQL::Type::RecursiveCTE);
}

- (void)test_recursive_cte
{
    auto testingSQL = WCDB::RecursiveCTE(table).as(initialSelect).union_(recursiveSelect);

    auto testingTypes = { WCDB::SQL::Type::RecursiveCTE, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testTable AS(SELECT 1 UNION SELECT 2)");
}

- (void)test_recursive_cte_union_all
{
    auto testingSQL = WCDB::RecursiveCTE(table).as(initialSelect).unionAll(recursiveSelect);

    auto testingTypes = { WCDB::SQL::Type::RecursiveCTE, WCDB::SQL::Type::CTETableName, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testTable AS(SELECT 1 UNION ALL SELECT 2)");
}

@end
