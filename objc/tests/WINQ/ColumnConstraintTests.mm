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

@interface ColumnConstraintTests : WINQTestCase

@end

@implementation ColumnConstraintTests {
    NSString* name;
    WCDB::Order order;
    WCDB::Conflict conflict;
    WCDB::Expression condition;
    WCDB::LiteralValue defaultValue;
    WCDB::Expression defaultExpression;
    NSString* collation;
    WCDB::ForeignKey foreignKey;
}

- (void)setUp
{
    [super setUp];
    name = @"testColumnConstraint";
    order = WCDB::Order::ASC;
    conflict = WCDB::Conflict::Abort;
    condition = 1;
    defaultValue = 1;
    defaultExpression = 1;
    collation = @"testCollation";
    foreignKey = WCDB::ForeignKey().references(@"testForeignTable");
}

- (void)test_default_constructible
{
    WCDB::ColumnConstraint constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::ColumnConstraint().getType(), WCDB::SQL::Type::ColumnConstraint);
    XCTAssertEqual(WCDB::ColumnConstraint::type, WCDB::SQL::Type::ColumnConstraint);
}

- (void)test_primary_key
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey();

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY");
}

- (void)test_primary_key_without_name
{
    auto testingSQL = WCDB::ColumnConstraint().primaryKey();

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"PRIMARY KEY");
}

- (void)test_primary_key_with_order
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().order(WCDB::Order::ASC);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY ASC");
}

- (void)test_primary_key_with_conflict
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().conflict(conflict);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY ON CONFLICT ABORT");
}

- (void)test_primary_key_with_auto_increment
{
    auto testingSQL = WCDB::ColumnConstraint(name).primaryKey().autoIncrement();

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint PRIMARY KEY AUTOINCREMENT");
}

- (void)test_not_null
{
    auto testingSQL = WCDB::ColumnConstraint(name).notNull();

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint NOT NULL");
}

- (void)test_not_null_with_conflict
{
    auto testingSQL = WCDB::ColumnConstraint(name).notNull().conflict(conflict);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint NOT NULL ON CONFLICT ABORT");
}

- (void)test_unique
{
    auto testingSQL = WCDB::ColumnConstraint(name).unique();

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint UNIQUE");
}

- (void)test_unique_with_conflict
{
    auto testingSQL = WCDB::ColumnConstraint(name).unique().conflict(conflict);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint UNIQUE ON CONFLICT ABORT");
}

- (void)test_check
{
    auto testingSQL = WCDB::ColumnConstraint(name).check(condition);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint CHECK(1)");
}

- (void)test_default_expression
{
    auto testingSQL = WCDB::ColumnConstraint(name).default_(defaultExpression);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint DEFAULT 1");
}

- (void)test_collate
{
    auto testingSQL = WCDB::ColumnConstraint(name).collate(collation);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint COLLATE testCollation");
}

- (void)test_foreign_key
{
    auto testingSQL = WCDB::ColumnConstraint(name).foreignKey(foreignKey);

    auto testingTypes = { WCDB::SQL::Type::ColumnConstraint, WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"CONSTRAINT testColumnConstraint REFERENCES testForeignTable");
}

@end
