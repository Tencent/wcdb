//
// Created by sanhuazhang on 2019/08/30
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

@interface StatementExplainTests : BaseTestCase

@end

@implementation StatementExplainTests {
    WCDB::StatementSelect select;
    WCDB::StatementPragma pragma;
}

- (void)setUp
{
    [super setUp];
    select = WCDB::StatementSelect().select(1);
    pragma = WCDB::StatementPragma().pragma(WCDB::Pragma::integrityCheck());
}

- (void)test_default_constructible
{
    WCDB::StatementExplain constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementExplain().getType(), WCDB::SQL::Type::ExplainSTMT);
    TestCaseAssertEqual(WCDB::StatementExplain::type, WCDB::SQL::Type::ExplainSTMT);
}

- (void)test_explain
{
    auto testingSQL = WCDB::StatementExplain().explain(select);

    auto testingTypes = { WCDB::SQL::Type::ExplainSTMT, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"EXPLAIN SELECT 1");
}

- (void)test_explain_query_plan
{
    auto testingSQL = WCDB::StatementExplain().explainQueryPlan(pragma);

    auto testingTypes = { WCDB::SQL::Type::ExplainSTMT, WCDB::SQL::Type::PragmaSTMT, WCDB::SQL::Type::Pragma };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"EXPLAIN QUERY PLAN PRAGMA integrity_check");
}

@end
