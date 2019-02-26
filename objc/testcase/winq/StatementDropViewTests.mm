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

@interface StatementDropViewTests : BaseTestCase

@end

@implementation StatementDropViewTests {
    WCDB::Schema schema;
    NSString* view;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    view = @"testView";
}

- (void)test_default_constructible
{
    WCDB::StatementDropView constructible;
TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementDropView().getType(), WCDB::SQL::Type::DropViewSTMT);
    TestCaseAssertEqual(WCDB::StatementDropView::type, WCDB::SQL::Type::DropViewSTMT);
}

- (void)test_drop_view
{
    auto testingSQL = WCDB::StatementDropView().dropView(view).schema(schema).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropViewSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP VIEW IF EXISTS testSchema.testView");
}

- (void)test_drop_view_without_if_exists
{
    auto testingSQL = WCDB::StatementDropView().dropView(view).schema(schema);

    auto testingTypes = { WCDB::SQL::Type::DropViewSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP VIEW testSchema.testView");
}

- (void)test_drop_view_without_schema
{
    auto testingSQL = WCDB::StatementDropView().dropView(view).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropViewSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP VIEW IF EXISTS main.testView");
}

@end
