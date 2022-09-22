//
// Created by sanhuazhang on 2019/05/02
//

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

@interface ColumnTests : BaseTestCase

@end

@implementation ColumnTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::Column constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::Column().getType(), WCDB::SQL::Type::Column);
    TestCaseAssertEqual(WCDB::Column::type, WCDB::SQL::Type::Column);
}

- (void)test_column
{
    auto testingSQL = WCDB::Column(@"testColumn");

    auto testingTypes = { WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testColumn");
}

- (void)test_column_with_table_and_schema
{
    auto testingSQL = WCDB::Column(@"testColumn").table(@"testTable").schema(@"testSchema");

    auto testingTypes = { WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"testSchema.testTable.testColumn");
}

- (void)test_column_with_table_without_schema
{
    auto testingSQL = WCDB::Column(@"testColumn").table(@"testTable");

    auto testingTypes = { WCDB::SQL::Type::Column, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"main.testTable.testColumn");
}

- (void)test_rowid
{
    TestCaseAssertSQLEqual(WCDB::Column::rowid(), @"rowid");
}

- (void)test_column_convertible
{
    WCDB::Column sql(@"testColumn");
    NSString* expected = @"testColumn";
    TestCaseAssertWINQConvertible(WCDB::Column, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Columns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Expression, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::Expressions, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumn, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerm, sql, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerms, sql, expected);
}

- (void)test_columns_convertible
{
    WCDB::Columns sqls = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
    NSString* expected = @"testColumn1, testColumn2";
    TestCaseAssertWINQConvertible(WCDB::Columns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::Expressions, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::IndexedColumns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::ResultColumns, sqls, expected);
    TestCaseAssertWINQConvertible(WCDB::OrderingTerms, sqls, expected);
}

@end
