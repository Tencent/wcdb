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

@interface StatementDropIndexTests : WINQTestCase

@end

@implementation StatementDropIndexTests {
    WCDB::Schema schema;
    NSString* index;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    index = @"testIndex";
}

- (void)test_default_constructible
{
    WCDB::StatementDropIndex constructible __attribute((unused));
}

- (void)test_drop_index
{
    auto testingSQL = WCDB::StatementDropIndex().dropIndex(index).schema(schema).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropIndexSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"DROP INDEX IF EXISTS testSchema.testIndex");
}

- (void)test_drop_index_without_if_exists
{
    auto testingSQL = WCDB::StatementDropIndex().dropIndex(index).schema(schema);

    auto testingTypes = { WCDB::SQL::Type::DropIndexSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"DROP INDEX testSchema.testIndex");
}

- (void)test_drop_index_without_schema
{
    auto testingSQL = WCDB::StatementDropIndex().dropIndex(index).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropIndexSTMT, WCDB::SQL::Type::Schema };
    IterateAssertEqual(testingSQL, testingTypes);
    WINQAssertEqual(testingSQL, @"DROP INDEX IF EXISTS main.testIndex");
}

@end
