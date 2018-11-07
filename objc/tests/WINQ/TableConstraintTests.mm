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

@interface TableConstraintTests : WINQTestCase

@end

@implementation TableConstraintTests {
    NSString* name;
    WCDB::IndexedColumns indexedColumns;
    WCDB::Conflict conflict;
    WCDB::Expression condition;
    WCDB::Columns columns;
    WCDB::ForeignKey foreignKey;
}

- (void)setUp
{
    [super setUp];
    name = @"testTableConstraint";
    columns = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
    condition = 1;
    indexedColumns = columns;
    conflict = WCDB::Conflict::Abort;
    foreignKey = WCDB::ForeignKey().references(@"testForeignTable");
}

- (void)test_default_constructible
{
    WCDB::TableConstraint constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::TableConstraint().getType(), WCDB::SQL::Type::TableConstraint);
    XCTAssertEqual(WCDB::TableConstraint::type, WCDB::SQL::Type::TableConstraint);
}

- (void)test_primary_key
{
    auto testingSQL = WCDB::TableConstraint(name).primaryKey(indexedColumns);

    auto testingTypes = { WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CONSTRAINT testTableConstraint PRIMARY KEY(testColumn1, testColumn2)");
}

- (void)test_primary_key_without_name
{
    auto testingSQL = WCDB::TableConstraint().primaryKey(indexedColumns);

    auto testingTypes = { WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"PRIMARY KEY(testColumn1, testColumn2)");
}

- (void)test_primary_key_with_conflict
{
    auto testingSQL = WCDB::TableConstraint(name).primaryKey(indexedColumns).conflict(conflict);

    auto testingTypes = { WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CONSTRAINT testTableConstraint PRIMARY KEY(testColumn1, testColumn2) ON CONFLICT ABORT");
}

- (void)test_unique
{
    auto testingSQL = WCDB::TableConstraint(name).unique(indexedColumns);

    auto testingTypes = { WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CONSTRAINT testTableConstraint UNIQUE(testColumn1, testColumn2)");
}

- (void)test_unique_with_conflict
{
    auto testingSQL = WCDB::TableConstraint(name).unique(indexedColumns).conflict(conflict);

    auto testingTypes = { WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CONSTRAINT testTableConstraint UNIQUE(testColumn1, testColumn2) ON CONFLICT ABORT");
}

- (void)test_check
{
    auto testingSQL = WCDB::TableConstraint(name).check(condition);

    auto testingTypes = { WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CONSTRAINT testTableConstraint CHECK(1)");
}

- (void)test_foreign_key
{
    auto testingSQL = WCDB::TableConstraint(name).foreignKey(columns, foreignKey);

    auto testingTypes = { WCDB::SQL::Type::TableConstraint, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CONSTRAINT testTableConstraint FOREIGN KEY(testColumn1, testColumn2) REFERENCES testForeignTable");
}

@end
