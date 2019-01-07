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

@interface ForeignKeyTests : WINQTestCase

@end

@implementation ForeignKeyTests {
    NSString* name;
    WCDB::Column column1;
    WCDB::Column column2;
    NSString* match;
}

- (void)setUp
{
    [super setUp];
    name = @"testForeignTable";
    column1 = WCDB::Column(@"testColumn1");
    column2 = WCDB::Column(@"testColumn2");
    match = @"testMatch";
}

- (void)test_default_constructible
{
    WCDB::ForeignKey constructible __attribute((unused));
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::ForeignKey().getType(), WCDB::SQL::Type::ForeignKeyClause);
    TestCaseAssertEqual(WCDB::ForeignKey::type, WCDB::SQL::Type::ForeignKeyClause);
}

- (void)test_foreign_table
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetNull().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_with_column
{
    auto testingSQL = WCDB::ForeignKey().references(name).column(column1).onDeleteSetNull().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable(testColumn1) ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_with_columns
{
    auto testingSQL = WCDB::ForeignKey().references(name).column(column1).column(column2).onDeleteSetNull().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable(testColumn1, testColumn2) ON DELETE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_delete_set_default
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetDefault().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET DEFAULT NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_delete_cascade
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteCascade().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE CASCADE NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_delete_restrict
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteRestrict().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE RESTRICT NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_delete_no_action
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteNoAction().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE NO ACTION NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_update_set_null
{
    auto testingSQL = WCDB::ForeignKey().references(name).onUpdateSetNull().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON UPDATE SET NULL NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_update_set_default
{
    auto testingSQL = WCDB::ForeignKey().references(name).onUpdateSetDefault().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON UPDATE SET DEFAULT NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_update_cascade
{
    auto testingSQL = WCDB::ForeignKey().references(name).onUpdateCascade().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON UPDATE CASCADE NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_update_restrict
{
    auto testingSQL = WCDB::ForeignKey().references(name).onUpdateRestrict().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON UPDATE RESTRICT NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_on_update_no_action
{
    auto testingSQL = WCDB::ForeignKey().references(name).onUpdateNoAction().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON UPDATE NO ACTION NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_match
{
    auto testingSQL = WCDB::ForeignKey().references(name).match(match).notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable MATCH testMatch NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_without_action
{
    auto testingSQL = WCDB::ForeignKey().references(name).notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_with_mutiple_action
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetNull().onUpdateSetDefault().notDeferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET NULL ON UPDATE SET DEFAULT NOT DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_not_deferrable_initially_immediate
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetNull().notDeferrableInitiallyImmediate();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET NULL NOT DEFERRABLE INITIALLY IMMEDIATE");
}

- (void)test_foreign_table_not_deferrable
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetNull().notDeferrable();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET NULL NOT DEFERRABLE");
}

- (void)test_foreign_table_deferrable_initially_deferred
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetNull().deferrableInitiallyDeferred();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED");
}

- (void)test_foreign_table_deferrable_initially_immediate
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetNull().deferrableInitiallyImmediate();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET NULL DEFERRABLE INITIALLY IMMEDIATE");
}

- (void)test_foreign_table_deferrable
{
    auto testingSQL = WCDB::ForeignKey().references(name).onDeleteSetNull().deferrable();

    auto testingTypes = { WCDB::SQL::Type::ForeignKeyClause };
    IterateAssertEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"REFERENCES testForeignTable ON DELETE SET NULL DEFERRABLE");
}

@end
