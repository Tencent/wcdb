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

@interface UpsertTests : WINQTestCase

@end

@implementation UpsertTests {
    WCDB::IndexedColumn indexedColumn1;
    WCDB::IndexedColumn indexedColumn2;
    WCDB::Expression condition;
    WCDB::Columns column;
    WCDB::Columns columns;
    WCDB::Expression value1;
    WCDB::Expression value2;
    WCDB::Expression updateCondition;
}

- (void)setUp
{
    [super setUp];
    column = WCDB::Column(@"testColumn1");
    columns = {
        WCDB::Column(@"testColumn2"),
        WCDB::Column(@"testColumn3"),
    };
    indexedColumn1 = WCDB::Column(@"testColumn1");
    indexedColumn2 = WCDB::Column(@"testColumn2");
    condition = 1;
    value1 = 2;
    value2 = 3;
    updateCondition = 4;
}

- (void)test_default_constructible
{
    WCDB::Upsert constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::Upsert().getType(), WCDB::SQL::Type::UpsertClause);
    XCTAssertEqual(WCDB::Upsert::type, WCDB::SQL::Type::UpsertClause);
}

- (void)test_upsert
{
    auto testingSQL = WCDB::Upsert().conflict().doNothing();

    auto testingTypes = { WCDB::SQL::Type::UpsertClause };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON CONFLICT DO NOTHING");
}

- (void)test_with_indexed_column
{
    auto testingSQL = WCDB::Upsert().conflict().indexed(indexedColumn1).doNothing();

    auto testingTypes = { WCDB::SQL::Type::UpsertClause, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON CONFLICT(testColumn1) DO NOTHING");
}

- (void)test_with_indexed_columns
{
    auto testingSQL = WCDB::Upsert().conflict().indexed(indexedColumn1).indexed(indexedColumn2).doNothing();

    auto testingTypes = { WCDB::SQL::Type::UpsertClause, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON CONFLICT(testColumn1, testColumn2) DO NOTHING");
}

- (void)test_with_indexed_column_and_condition
{
    auto testingSQL = WCDB::Upsert().conflict().indexed(indexedColumn1).where(condition).doNothing();

    auto testingTypes = { WCDB::SQL::Type::UpsertClause, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON CONFLICT(testColumn1) WHERE 1 DO NOTHING");
}

- (void)test_with_update
{
    auto testingSQL = WCDB::Upsert().conflict().doUpdate().set(column).to(value1);

    auto testingTypes = { WCDB::SQL::Type::UpsertClause, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON CONFLICT DO UPDATE SET testColumn1 = 2");
}

- (void)test_with_update_multiple
{
    auto testingSQL = WCDB::Upsert().conflict().doUpdate().set(column).to(value1).set(columns).to(value2);

    auto testingTypes = { WCDB::SQL::Type::UpsertClause, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Column, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON CONFLICT DO UPDATE SET testColumn1 = 2, (testColumn2, testColumn3) = 3");
}

- (void)test_with_update_condition
{
    auto testingSQL = WCDB::Upsert().conflict().doUpdate().set(column).to(value1).where(updateCondition);

    auto testingTypes = { WCDB::SQL::Type::UpsertClause, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"ON CONFLICT DO UPDATE SET testColumn1 = 2 WHERE 4");
}

@end
