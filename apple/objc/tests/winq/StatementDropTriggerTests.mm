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

@interface StatementDropTriggerTests : BaseTestCase

@end

@implementation StatementDropTriggerTests {
    WCDB::Schema schema;
    NSString* trigger;
}

- (void)setUp
{
    [super setUp];
    schema = @"testSchema";
    trigger = @"testTrigger";
}

- (void)test_default_constructible
{
    WCDB::StatementDropTrigger constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementDropTrigger().getType(), WCDB::SQL::Type::DropTriggerSTMT);
    TestCaseAssertEqual(WCDB::StatementDropTrigger::type, WCDB::SQL::Type::DropTriggerSTMT);
}

- (void)test_drop_trigger
{
    auto testingSQL = WCDB::StatementDropTrigger().dropTrigger(trigger).schema(schema).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropTriggerSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP TRIGGER IF EXISTS testSchema.testTrigger");
}

- (void)test_drop_trigger_without_if_exists
{
    auto testingSQL = WCDB::StatementDropTrigger().dropTrigger(trigger).schema(schema);

    auto testingTypes = { WCDB::SQL::Type::DropTriggerSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP TRIGGER testSchema.testTrigger");
}

- (void)test_drop_trigger_without_schema
{
    auto testingSQL = WCDB::StatementDropTrigger().dropTrigger(trigger).ifExists();

    auto testingTypes = { WCDB::SQL::Type::DropTriggerSTMT, WCDB::SQL::Type::Schema };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DROP TRIGGER IF EXISTS main.testTrigger");
}

@end
