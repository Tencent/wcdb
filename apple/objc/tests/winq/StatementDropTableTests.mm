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

@interface StatementDropTableTests : BaseTestCase

@end

@implementation StatementDropTableTests {
    WCDB::Schema schema;
    NSString* table;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    table = @"testTable";
}

- (void)test_default_constructible
{
    WCDB::StatementDropTable constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementDropTable().getType(), WCDB::SQL::Type::DropTableSTMT);
    TestCaseAssertEqual(WCDB::StatementDropTable::type, WCDB::SQL::Type::DropTableSTMT);
}

- (void)test_drop_table
{
    auto testingSQL = WCDB::StatementDropTable().dropTable(table).schema(schema).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP TABLE IF EXISTS testSchema.testTable");
}

- (void)test_drop_table_without_if_exists
{
    auto testingSQL = WCDB::StatementDropTable().dropTable(table).schema(schema);

    auto testingTypes = { WCDB::SQL::Type::DropTableSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP TABLE testSchema.testTable");
}

- (void)test_drop_table_without_schema
{
    auto testingSQL = WCDB::StatementDropTable().dropTable(table).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropTableSTMT };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP TABLE IF EXISTS testTable");
}

@end
