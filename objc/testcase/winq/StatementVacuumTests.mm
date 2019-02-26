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

@interface StatementVacuumTests : BaseTestCase

@end

@implementation StatementVacuumTests {
    WCDB::Schema schema;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
}

- (void)test_default_constructible
{
    WCDB::StatementVacuum constructible;
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementVacuum().getType(), WCDB::SQL::Type::VacuumSTMT);
    TestCaseAssertEqual(WCDB::StatementVacuum::type, WCDB::SQL::Type::VacuumSTMT);
}

- (void)test_vacuum
{
    auto testingSQL = WCDB::StatementVacuum().vacuum();

    auto testingTypes = { WCDB::SQL::Type::VacuumSTMT };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"VACUUM");
}

- (void)test_vacuum_with_schema
{
    auto testingSQL = WCDB::StatementVacuum().vacuum(schema);

    auto testingTypes = { WCDB::SQL::Type::VacuumSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"VACUUM testSchema");
}

@end
