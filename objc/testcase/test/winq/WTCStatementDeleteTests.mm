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

#import "WTCAssert.h"
#import <WINQ/abstract.h>

@interface WTCStatementDeleteTests : XCTestCase

@end

@implementation WTCStatementDeleteTests

- (void)testStatementDelete
{
    WCDB::CTETableName cteTableName("testTable");
    WCDB::StatementSelect statementSelect = WCDB::StatementSelect().select(WCDB::ResultColumn(WCDB::Expression::ColumnNamed("testColumn1")));
    WCDB::WithClause withClause = WCDB::WithClause().with(cteTableName, statementSelect);
    WCDB::QualifiedTableName qualifiedTableName("testTable");
    WCDB::Expression expr = WCDB::Expression(WCDB::Expression::ColumnNamed("testColumn1")).notNull();

    WINQAssertEqual(WCDB::StatementDelete()
                        .deleteFrom(qualifiedTableName)
                        .where(expr),
                    @"DELETE FROM testTable WHERE testColumn1 NOTNULL");

    WINQAssertEqual(WCDB::StatementDelete()
                        .deleteFrom(qualifiedTableName),
                    @"DELETE FROM testTable");

    WINQAssertEqual(WCDB::StatementDelete()
                        .with(withClause)
                        .deleteFrom(qualifiedTableName)
                        .where(expr),
                    @"WITH testTable AS(SELECT testColumn1) DELETE FROM testTable WHERE testColumn1 NOTNULL");
}

@end
