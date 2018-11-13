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

@interface StatementReindexTests : WINQTestCase

@end

@implementation StatementReindexTests {
    WCDB::Schema schema;
    NSString* collation;
    NSString* table;
    NSString* index;
}

- (void)setUp
{
    [super setUp];
    collation = @"testCollation";
    schema = @"testSchema";
    table = @"testTable";
    index = @"testIndex";
}

- (void)test_default_constructible
{
    WCDB::StatementReindex constructible __attribute((unused));
}

- (void)test_get_type
{
    XCTAssertEqual(WCDB::StatementReindex().getType(), WCDB::SQL::Type::ReindexSTMT);
    XCTAssertEqual(WCDB::StatementReindex::type, WCDB::SQL::Type::ReindexSTMT);
}

- (void)test_reindex
{
    auto testingSQL = WCDB::StatementReindex().reindex();

    auto testingTypes = { WCDB::SQL::Type::ReindexSTMT };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"REINDEX");
}

- (void)test_reindex_collation
{
    auto testingSQL = WCDB::StatementReindex().reindex().collation(collation);

    auto testingTypes = { WCDB::SQL::Type::ReindexSTMT };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"REINDEX testCollation");
}

- (void)test_reindex_table
{
    auto testingSQL = WCDB::StatementReindex().reindex().schema(schema).table(table);

    auto testingTypes = { WCDB::SQL::Type::ReindexSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"REINDEX testSchema.testTable");
}

- (void)test_reindex_table_without_schema
{
    auto testingSQL = WCDB::StatementReindex().reindex().table(table);

    auto testingTypes = { WCDB::SQL::Type::ReindexSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"REINDEX main.testTable");
}

- (void)test_reindex_index
{
    auto testingSQL = WCDB::StatementReindex().reindex().schema(schema).index(index);

    auto testingTypes = { WCDB::SQL::Type::ReindexSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"REINDEX testSchema.testIndex");
}

- (void)test_reindex_index_without_schema
{
    auto testingSQL = WCDB::StatementReindex().reindex().index(index);

    auto testingTypes = { WCDB::SQL::Type::ReindexSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    SQLAssertEqual(testingSQL, @"REINDEX main.testIndex");
}

@end
