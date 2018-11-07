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

@interface ColumnTests : WINQTestCase

@end

@implementation ColumnTests {
}

- (void)setUp
{
    [super setUp];
}

- (void)test_default_constructible
{
    WCDB::Column constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::Column().getType(), WCDB::SQL::Type::Column);
    XCTAssertEqual(WCDB::Column::type, WCDB::SQL::Type::Column);
}

- (void)test_column
{
    auto testingSQL = WCDB::Column(@"testColumn");

    auto testingTypes = { WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testColumn");
}

- (void)test_rowid
{
    WINQAssertEqual(WCDB::Column::rowid(), @"rowid");
}

- (void)test_column_convertible
{
    WCDB::Column sql(@"testColumn");
    NSString* expected = @"testColumn";
    WINQConvertibleTest(WCDB::Column, sql, expected);
    WINQConvertibleTest(WCDB::Columns, sql, expected);
    WINQConvertibleTest(WCDB::Expression, sql, expected);
    WINQConvertibleTest(WCDB::Expressions, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumn, sql, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumn, sql, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerm, sql, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sql, expected);
}

- (void)test_columns_convertible
{
    WCDB::Columns sqls = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
    NSString* expected = @"testColumn1, testColumn2";
    WINQConvertibleTest(WCDB::Columns, sqls, expected);
    WINQConvertibleTest(WCDB::Expressions, sqls, expected);
    WINQConvertibleTest(WCDB::IndexedColumns, sqls, expected);
    WINQConvertibleTest(WCDB::ResultColumns, sqls, expected);
    WINQConvertibleTest(WCDB::OrderingTerms, sqls, expected);
}

@end
