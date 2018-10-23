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

@interface QualifiedTableTests : WINQTestCase

@end

@implementation QualifiedTableTests {
    WCDB::Schema schema;
    NSString* table;
    NSString* alias;
    NSString* index;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    table = @"testTable";
    alias = @"testAlias";
    index = @"testIndex";
}

- (void)test_default_constructible
{
    WCDB::QualifiedTable constructible __attribute((unused));
}

- (void)test_qualified_table
{
    auto testingSQL = WCDB::QualifiedTable(schema, table);

    auto testingTypes = { WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testSchema.testTable");
}

- (void)test_qualified_table_without_schema
{
    auto testingSQL = WCDB::QualifiedTable(table);

    auto testingTypes = { WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"main.testTable");
}

- (void)test_qualified_table_with_alias
{
    auto testingSQL = WCDB::QualifiedTable(schema, table).as(alias);

    auto testingTypes = { WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testSchema.testTable AS testAlias");
}

- (void)test_qualified_table_with_indexed
{
    auto testingSQL = WCDB::QualifiedTable(schema, table).indexed(index);

    auto testingTypes = { WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testSchema.testTable INDEXED BY testIndex");
}

- (void)test_qualified_table_without_index
{
    auto testingSQL = WCDB::QualifiedTable(schema, table).notIndexed();

    auto testingTypes = { WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"testSchema.testTable NOT INDEXED");
}

WCDB::QualifiedTable acceptable(const WCDB::QualifiedTable& qualifiedTable)
{
    return qualifiedTable;
}

- (void)test_convertible
{
    WINQAssertEqual(acceptable("testTable"), @"main.testTable");
    WINQAssertEqual(acceptable(@"testTable"), @"main.testTable");
    WINQAssertEqual(acceptable(std::string("testTable")), @"main.testTable");
}

@end
