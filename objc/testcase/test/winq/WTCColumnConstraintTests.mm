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

@interface WTCColumnConstraintTests : WTCWINQTestCase

@end

@implementation WTCColumnConstraintTests

- (void)testPrimaryKey
{
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withPrimaryKey(WCDB::Order::NotSet, WCDB::ConflictClause::NotSet, false),
                    @"CONSTRAINT testConstraint PRIMARY KEY");

    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withPrimaryKey(WCDB::Order::ASC, WCDB::ConflictClause::NotSet, false),
                    @"CONSTRAINT testConstraint PRIMARY KEY ASC");

    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withPrimaryKey(WCDB::Order::DESC, WCDB::ConflictClause::NotSet, false),
                    @"CONSTRAINT testConstraint PRIMARY KEY DESC");

    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withPrimaryKey(WCDB::Order::NotSet, WCDB::ConflictClause::Rollback, false),
                    @"CONSTRAINT testConstraint PRIMARY KEY ON CONFLICT ROLLBACK");

    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withPrimaryKey(WCDB::Order::NotSet, WCDB::ConflictClause::NotSet, true),
                    @"CONSTRAINT testConstraint PRIMARY KEY AUTOINCREMENT");

    WINQAssertEqual(WCDB::ColumnConstraint()
                        .withPrimaryKey(WCDB::Order::NotSet, WCDB::ConflictClause::NotSet, false),
                    @"PRIMARY KEY");

    //Default
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withPrimaryKey(),
                    @"CONSTRAINT testConstraint PRIMARY KEY");
}

- (void)testNotNull
{
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withNotNull(WCDB::ConflictClause::NotSet),
                    @"CONSTRAINT testConstraint NOT NULL");

    //Default
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withNotNull(),
                    @"CONSTRAINT testConstraint NOT NULL");
}

- (void)testUnique
{
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withUnique(WCDB::ConflictClause::NotSet),
                    @"CONSTRAINT testConstraint UNIQUE");

    //Default
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withUnique(),
                    @"CONSTRAINT testConstraint UNIQUE");
}

- (void)testCheck
{
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withChecking(self.class.condition),
                    @"CONSTRAINT testConstraint CHECK(testColumn NOTNULL)");
}

- (void)testDefault
{
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withDefault(self.class.literalValue),
                    @"CONSTRAINT testConstraint DEFAULT 1");

    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withDefault(self.class.value),
                    @"CONSTRAINT testConstraint DEFAULT(1)");
}

- (void)testCollate
{
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withCollate(self.class.collationName),
                    @"CONSTRAINT testConstraint COLLATE testCollation");
}

- (void)testForeignKey
{
    WINQAssertEqual(WCDB::ColumnConstraint()
                        .named(self.class.constraintName)
                        .withForeignKeyClause(self.class.foreignKeyClause),
                    @"CONSTRAINT testConstraint REFERENCES testForeignTable");
}

@end
