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

@interface StatementDeleteTests : WINQTestCase

@end

@implementation StatementDeleteTests

- (void)testStatementDelete
{
    XCTAssertEqual(WCDB::StatementDelete().getType(), WCDB::Statement::Type::Delete);

    WINQAssertEqual(WCDB::StatementDelete()
                    .deleteFrom(self.class.qualifiedTableName)
                    .where(self.class.condition)
                    .orderBy(self.class.orderingTerm)
                    .limit(self.class.limit)
                    .offset(self.class.limitParameter),
                    @"DELETE FROM main.testTable WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementDelete()
                    .deleteFrom(self.class.qualifiedTableName)
                    .where(self.class.condition)
                    .orderBy(self.class.orderingTerm)
                    .limit(self.class.limit, self.class.limitParameter),
                    @"DELETE FROM main.testTable WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1, 2");

    WINQAssertEqual(WCDB::StatementDelete()
                    .deleteFrom(self.class.qualifiedTableName)
                    .where(self.class.condition)
                    .orderBy(self.class.orderingTerm)
                    .limit(self.class.limit),
                    @"DELETE FROM main.testTable WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1");

    WINQAssertEqual(WCDB::StatementDelete()
                    .deleteFrom(self.class.qualifiedTableName)
                    .where(self.class.condition)
                    .orderBy(self.class.orderingTerms)
                    .limit(self.class.limit)
                    .offset(self.class.limitParameter),
                    @"DELETE FROM main.testTable WHERE testColumn NOTNULL ORDER BY testColumn, testColumn2 LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementDelete()
                    .deleteFrom(self.class.qualifiedTableName)
                    .where(self.class.condition)
                    .limit(self.class.limit)
                    .offset(self.class.limitParameter),
                    @"DELETE FROM main.testTable WHERE testColumn NOTNULL LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementDelete()
                    .deleteFrom(self.class.qualifiedTableName)
                    .orderBy(self.class.orderingTerm)
                    .limit(self.class.limit)
                    .offset(self.class.limitParameter),
                    @"DELETE FROM main.testTable ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementDelete()
                    .with(self.class.withClause)
                    .deleteFrom(self.class.qualifiedTableName)
                    .where(self.class.condition)
                    .orderBy(self.class.orderingTerm)
                    .limit(self.class.limit)
                    .offset(self.class.limitParameter),
                    @"WITH testCTETable AS(SELECT testColumn FROM main.testTable) DELETE FROM main.testTable WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");
}

@end
