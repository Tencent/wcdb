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

@interface WTCColumnConstraintTests : XCTestCase

@end

@implementation WTCColumnConstraintTests

- (void)testColumnConstraint
{
    WCDB::ConflictClause conflictClause = WCDB::ConflictClause::Rollback;
    WCDB::Expression expression = WCDB::Expression::ColumnNamed("testColumn");
    expression = expression.notNull();
    WCDB::ForeignKeyClause foreignKeyClause("testForeignTable");
    std::string constraintName = "testConstraint";

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withPrimaryKey(), @"CONSTRAINT testConstraint PRIMARY KEY");

    WINQAssertEqual(WCDB::ColumnConstraint().withPrimaryKey(), @"PRIMARY KEY");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withPrimaryKey(WCDB::Order::ASC), @"CONSTRAINT testConstraint PRIMARY KEY ASC");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withPrimaryKey(WCDB::Order::DESC), @"CONSTRAINT testConstraint PRIMARY KEY DESC");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withPrimaryKey(WCDB::Order::NotSet, conflictClause), @"CONSTRAINT testConstraint PRIMARY KEY ON CONFLICT ROLLBACK");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withPrimaryKey(WCDB::Order::NotSet, WCDB::ConflictClause::NotSet, true), @"CONSTRAINT testConstraint PRIMARY KEY AUTOINCREMENT");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withNotNull(), @"CONSTRAINT testConstraint NOT NULL");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withNotNull(conflictClause), @"CONSTRAINT testConstraint NOT NULL ON CONFLICT ROLLBACK");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withUnique(), @"CONSTRAINT testConstraint UNIQUE");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withUnique(conflictClause), @"CONSTRAINT testConstraint UNIQUE ON CONFLICT ROLLBACK");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withChecking(expression), @"CONSTRAINT testConstraint CHECK(testColumn NOTNULL)");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withDefault(1), @"CONSTRAINT testConstraint DEFAULT 1");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withCollate("testCollate"), @"CONSTRAINT testConstraint COLLATE testCollate");

    WINQAssertEqual(WCDB::ColumnConstraint().named(constraintName).withForeignKeyClause(foreignKeyClause), @"CONSTRAINT testConstraint REFERENCES testForeignTable");
}

@end
