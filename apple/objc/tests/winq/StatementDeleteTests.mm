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

@interface StatementDeleteTests : BaseTestCase

@end

@implementation StatementDeleteTests {
    WCDB::CommonTableExpression with;
    WCDB::QualifiedTable table;
    WCDB::Expression condition;
    WCDB::OrderingTerms orderingTerms;
    WCDB::Expression limit;
    WCDB::Expression limitParameter;
}

- (void)setUp
{
    [super setUp];
    with = WCDB::CommonTableExpression(@"testTable").as(WCDB::StatementSelect().select(1));
    table = @"testTable";
    condition = 2;
    orderingTerms = {
        WCDB::Column(@"testColumn1"),
        WCDB::Column(@"testColumn2"),
    };
    limit = 3;
    limitParameter = 4;
}

- (void)test_default_constructible
{
    WCDB::StatementDelete constructible;
    TestCaseAssertFalse(constructible.syntax().isValid());
    TestCaseAssertIterateEqual(constructible, std::list<WCDB::Syntax::Identifier::Type>());
    TestCaseAssertTrue(constructible.getDescription().empty());
}

- (void)test_get_type
{
    TestCaseAssertEqual(WCDB::StatementDelete().getType(), WCDB::SQL::Type::DeleteSTMT);
    TestCaseAssertEqual(WCDB::StatementDelete::type, WCDB::SQL::Type::DeleteSTMT);
}

- (void)test_delete
{
    auto testingSQL = WCDB::StatementDelete().deleteFrom(table).where(condition).orders(orderingTerms).limit(limit).offset(limitParameter);

    auto testingTypes = { WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DELETE FROM testTable WHERE 2 ORDER BY testColumn1, testColumn2 LIMIT 3 OFFSET 4");
}

- (void)test_delete_with
{
    auto testingSQL = WCDB::StatementDelete().with(with).deleteFrom(table).where(condition).orders(orderingTerms).limit(limit).offset(limitParameter);

    auto testingTypes = { WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::CommonTableExpression, WCDB::SQL::Type::SelectSTMT, WCDB::SQL::Type::SelectCore, WCDB::SQL::Type::ResultColumn, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"WITH testTable AS(SELECT 1) DELETE FROM testTable WHERE 2 ORDER BY testColumn1, testColumn2 LIMIT 3 OFFSET 4");
}

- (void)test_delete_without_condition
{
    auto testingSQL = WCDB::StatementDelete().deleteFrom(table).orders(orderingTerms).limit(limit).offset(limitParameter);

    auto testingTypes = { WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DELETE FROM testTable ORDER BY testColumn1, testColumn2 LIMIT 3 OFFSET 4");
}

- (void)test_delete_without_orders
{
    auto testingSQL = WCDB::StatementDelete().deleteFrom(table).where(condition).limit(limit).offset(limitParameter);

    auto testingTypes = { WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DELETE FROM testTable WHERE 2 LIMIT 3 OFFSET 4");
}

- (void)test_delete_with_length
{
    auto testingSQL = WCDB::StatementDelete().deleteFrom(table).where(condition).orders(orderingTerms).limit(limit, limitParameter);

    auto testingTypes = { WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DELETE FROM testTable WHERE 2 ORDER BY testColumn1, testColumn2 LIMIT 3, 4");
}

- (void)test_delete_without_offset
{
    auto testingSQL = WCDB::StatementDelete().deleteFrom(table).where(condition).orders(orderingTerms).limit(limit);

    auto testingTypes = { WCDB::SQL::Type::DeleteSTMT, WCDB::SQL::Type::QualifiedTableName, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::OrderingTerm, WCDB::SQL::Type::Expression, WCDB::SQL::Type::Column, WCDB::SQL::Type::Expression, WCDB::SQL::Type::LiteralValue };
    TestCaseAssertIterateEqual(testingSQL, testingTypes);
    TestCaseAssertSQLEqual(testingSQL, @"DELETE FROM testTable WHERE 2 ORDER BY testColumn1, testColumn2 LIMIT 3");
}

@end
