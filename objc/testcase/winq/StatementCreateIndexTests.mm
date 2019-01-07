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

@interface StatementCreateIndexTests : BaseTestCase

@end

@implementation StatementCreateIndexTests {
    WCDB::Schema schema;
    NSString* index;
    NSString* table;
    WCDB::IndexedColumn indexedColumn1;
    WCDB::IndexedColumn indexedColumn2;
    WCDB::Expression condition;
}

- (void)setUp
{
    [super setUp];

    schema = @"testSchema";
    index = @"testIndex";
    table = @"testTable";
    indexedColumn1 = WCDB::Column(@"testColumn1");
    indexedColumn2 = WCDB::Column(@"testColumn2");
    condition = 1;
}

- (void)test_default_constructible
{
    WCDB::StatementCreateIndex constructible __attribute((unused));
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementCreateIndex().getType(), WCDB::SQL::Type::CreateIndexSTMT);
    TestCaseAssertEqual(WCDB::StatementCreateIndex::type, WCDB::SQL::Type::CreateIndexSTMT);
}

- (void)test_create_index
{
    auto testingSQL = WCDB::StatementCreateIndex().createIndex(index).schema(schema).table(table).indexed(indexedColumn1);

    auto testingTypes = { WCDB::SQL::Type::CreateIndexSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE INDEX testSchema.testIndex ON testTable(testColumn1)");
}

- (void)test_create_multi_index
{
    auto testingSQL = WCDB::StatementCreateIndex().createIndex(index).schema(schema).table(table).indexed(indexedColumn1).indexed(indexedColumn2);

    auto testingTypes = { WCDB::SQL::Type::CreateIndexSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE INDEX testSchema.testIndex ON testTable(testColumn1, testColumn2)");
}

- (void)test_create_unique_index
{
    auto testingSQL = WCDB::StatementCreateIndex().createIndex(index).schema(schema).unique().table(table).indexed(indexedColumn1);

    auto testingTypes = { WCDB::SQL::Type::CreateIndexSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE UNIQUE INDEX testSchema.testIndex ON testTable(testColumn1)");
}

- (void)test_create_index_if_not_exists
{
    auto testingSQL = WCDB::StatementCreateIndex().createIndex(index).schema(schema).ifNotExists().table(table).indexed(indexedColumn1);

    auto testingTypes = { WCDB::SQL::Type::CreateIndexSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE INDEX IF NOT EXISTS testSchema.testIndex ON testTable(testColumn1)");
}

- (void)test_create_index_without_schema
{
    auto testingSQL = WCDB::StatementCreateIndex().createIndex(index).table(table).indexed(indexedColumn1);

    auto testingTypes = { WCDB::SQL::Type::CreateIndexSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE INDEX main.testIndex ON testTable(testColumn1)");
}

- (void)test_create_index_with_condition
{
    auto testingSQL = WCDB::StatementCreateIndex().createIndex(index).schema(schema).table(table).indexed(indexedColumn1).where(condition);

    auto testingTypes = { WCDB::SQL::Type::CreateIndexSTMT, WCDB::SQL::Type::Schema, WCDB::SQL::Type::IndexedColumn, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"CREATE INDEX testSchema.testIndex ON testTable(testColumn1) WHERE 1");
}

@end
