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

@interface WTCTableConstraintTests : XCTestCase

@end

@implementation WTCTableConstraintTests

- (void)testTableConstraint
{
    std::string name = "testConstraint";
    std::string columnName1 = "testColumn1";
    std::string columnName2 = "testColumn2";
    std::string columnName3 = "testColumn3";
    std::list<std::string> columnNames = {columnName1, columnName2};
    WCDB::IndexedColumn indexedColumn1 = WCDB::IndexedColumn(columnName1);
    std::list<WCDB::IndexedColumn> indexedColumns = {indexedColumn1, WCDB::IndexedColumn(columnName2)};
    WCDB::Expression expression = WCDB::Expression(WCDB::Expression::ColumnNamed(columnName3)).notNull();
    WCDB::ConflictClause conflictClause = WCDB::ConflictClause::Rollback;
    WCDB::ForeignKeyClause foreignKeyClause = WCDB::ForeignKeyClause("testForeignTable");

    WINQAssertEqual(WCDB::TableConstraint(name).withPrimaryKey(indexedColumn1), @"CONSTRAINT testConstraint PRIMARY KEY(testColumn1)");

    WINQAssertEqual(WCDB::TableConstraint(name).withPrimaryKey(indexedColumn1, conflictClause), @"CONSTRAINT testConstraint PRIMARY KEY(testColumn1) ON CONFLICT ROLLBACK");

    WINQAssertEqual(WCDB::TableConstraint(name).withPrimaryKey(indexedColumns), @"CONSTRAINT testConstraint PRIMARY KEY(testColumn1, testColumn2)");

    WINQAssertEqual(WCDB::TableConstraint(name).withUnique(indexedColumn1), @"CONSTRAINT testConstraint UNIQUE(testColumn1)");

    WINQAssertEqual(WCDB::TableConstraint(name).withChecking(expression), @"CONSTRAINT testConstraint CHECK(testColumn3 NOTNULL)");

    WINQAssertEqual(WCDB::TableConstraint(name).withForeignKey(columnName1, foreignKeyClause), @"CONSTRAINT testConstraint FOREIGN KEY(testColumn1) REFERENCES testForeignTable");

    WINQAssertEqual(WCDB::TableConstraint(name).withForeignKey(columnNames, foreignKeyClause), @"CONSTRAINT testConstraint FOREIGN KEY(testColumn1, testColumn2) REFERENCES testForeignTable");
}

@end
