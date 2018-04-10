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

@interface TableConstraintTests : WINQTestCase

@end

@implementation TableConstraintTests

- (void)testTableConstraint
{
    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withPrimaryKey(self.class.indexedColumn)
                        .onConflict(WCDB::ConflictClause::NotSet),
                    @"CONSTRAINT testConstraint PRIMARY KEY(testColumn)");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withPrimaryKey(self.class.indexedColumn)
                        .onConflict(WCDB::ConflictClause::Rollback),
                    @"CONSTRAINT testConstraint PRIMARY KEY(testColumn) ON CONFLICT ROLLBACK");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withPrimaryKey(self.class.indexedColumns)
                        .onConflict(WCDB::ConflictClause::NotSet),
                    @"CONSTRAINT testConstraint PRIMARY KEY(testColumn, testColumn2)");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withUnique(self.class.indexedColumn)
                        .onConflict(WCDB::ConflictClause::NotSet),
                    @"CONSTRAINT testConstraint UNIQUE(testColumn)");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withUnique(self.class.indexedColumn)
                        .onConflict(WCDB::ConflictClause::Rollback),
                    @"CONSTRAINT testConstraint UNIQUE(testColumn) ON CONFLICT ROLLBACK");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withUnique(self.class.indexedColumns),
                    @"CONSTRAINT testConstraint UNIQUE(testColumn, testColumn2)");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withChecking(self.class.condition),
                    @"CONSTRAINT testConstraint CHECK(testColumn NOTNULL)");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withForeignKey(self.class.column, self.class.foreignKeyClause),
                    @"CONSTRAINT testConstraint FOREIGN KEY(testColumn) REFERENCES testForeignTable");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withForeignKey(self.class.columns, self.class.foreignKeyClause),
                    @"CONSTRAINT testConstraint FOREIGN KEY(testColumn, testColumn2) REFERENCES testForeignTable");

    //Default
    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withPrimaryKey(self.class.indexedColumn),
                    @"CONSTRAINT testConstraint PRIMARY KEY(testColumn)");

    WINQAssertEqual(WCDB::TableConstraint(self.class.constraintName)
                        .withUnique(self.class.indexedColumn),
                    @"CONSTRAINT testConstraint UNIQUE(testColumn)");
}

@end
