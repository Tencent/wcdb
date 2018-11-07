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

@interface StatementCreateViewTests : WINQTestCase

@end

@implementation StatementCreateViewTests {
    WCDB::Schema schema;
    NSString* name;
    WCDB::Columns columns;
    WCDB::StatementSelect select;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    name = @"testView";
    columns = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
    select = WCDB::StatementSelect().select(1);
}

- (void)test_default_constructible
{
    WCDB::StatementCreateView constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::StatementCreateView().getType(), WCDB::SQL::Type::CreateViewSTMT);
    XCTAssertEqual(WCDB::StatementCreateView::type, WCDB::SQL::Type::CreateViewSTMT);
}

- (void)test_create_view
{
    auto testingSQL = WCDB::StatementCreateView().createView(name).as(select);

    auto testingTypes = { WCDB::SQL::Type::CreateViewSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE VIEW main.testView AS SELECT 1");
}

- (void)test_create_temp_view
{
    auto testingSQL = WCDB::StatementCreateView().createView(name).temp().as(select);

    auto testingTypes = { WCDB::SQL::Type::CreateViewSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE TEMP VIEW main.testView AS SELECT 1");
}

- (void)test_create_view_if_not_exists
{
    auto testingSQL = WCDB::StatementCreateView().createView(name).ifNotExists().as(select);

    auto testingTypes = { WCDB::SQL::Type::CreateViewSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE VIEW IF NOT EXISTS main.testView AS SELECT 1");
}

- (void)test_create_view_with_schema
{
    auto testingSQL = WCDB::StatementCreateView().createView(name).schema(schema).as(select);

    auto testingTypes = { WCDB::SQL::Type::CreateViewSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE VIEW testSchema.testView AS SELECT 1");
}

- (void)test_create_view_with_columns
{
    auto testingSQL = WCDB::StatementCreateView().createView(name).columns(columns).as(select);

    auto testingTypes = { WCDB::SQL::Type::CreateViewSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"CREATE VIEW main.testView(testColumn1, testColumn2) AS SELECT 1");
}

@end
