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

@interface ColumnDefTests : BaseTestCase

@end

@implementation ColumnDefTests {
    WCDB::Column column;
    WCDB::ColumnType columnType;
    WCDB::ColumnConstraint columnConstraint1;
    WCDB::ColumnConstraint columnConstraint2;
}

- (void)setUp
{
    [super setUp];
    column = WCDB::Column(@"testColumn");
    columnType = WCDB::ColumnType::Integer32;
    columnConstraint1 = WCDB::ColumnConstraint().check(1);
    columnConstraint2 = WCDB::ColumnConstraint().check(2);
}

- (void)test_default_constructible
{
    WCDB::ColumnDef constructible __attribute((unused));
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::ColumnDef().getType(), WCDB::SQL::Type::ColumnDef);
    TestCaseAssertEqual(WCDB::ColumnDef::type, WCDB::SQL::Type::ColumnDef);
}

- (void)test_column_def
{
    auto testingSQL = WCDB::ColumnDef(column, columnType);

    auto testingTypes = { WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn INTEGER");
}

- (void)test_column_def_without_type
{
    auto testingSQL = WCDB::ColumnDef(column);

    auto testingTypes = { WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn");
}

- (void)test_column_def_with_constraint
{
    auto testingSQL = WCDB::ColumnDef(column, columnType).constraint(columnConstraint1);

    auto testingTypes = { WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn INTEGER CHECK(1)");
}

- (void)test_column_def_with_constraints
{
    auto testingSQL = WCDB::ColumnDef(column, columnType).constraint(columnConstraint1).constraint(columnConstraint2);

    auto testingTypes = { WCDB::SQL::Type::ColumnDef, WCDB::SQL::Type::Column, WCDB::SQL::Type::ColumnConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::ColumnConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn INTEGER CHECK(1) CHECK(2)");
}

@end
