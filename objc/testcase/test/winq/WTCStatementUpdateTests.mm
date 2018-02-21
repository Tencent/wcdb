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

#import "WTCWINQTestCase.h"

@interface WTCStatementUpdateTests : WTCWINQTestCase

@end

@implementation WTCStatementUpdateTests

- (void)testStatementUpdate
{
    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit, self.class.limitParameter),
                    @"UPDATE testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1, 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit),
                    @"UPDATE testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerms)
                        .limit(self.class.limit, self.class.limitParameter),
                    @"UPDATE testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn, testColumn2 LIMIT 1, 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE testTable SET testColumn = 1 WHERE testColumn NOTNULL LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE testTable SET testColumn = 1 ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnNames, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE testTable SET testColumn, testColumn2 = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .set(self.class.columnName2, self.class.value2)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE testTable SET testColumn = 1, testColumn2 = 'testValue' WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .updateOrRollback(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE OR ROLLBACK testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .updateOrAbort(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE OR ABORT testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .updateOrReplace(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE OR REPLACE testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .updateOrFail(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE OR FAIL testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .updateOrIgnore(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"UPDATE OR IGNORE testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementUpdate()
                        .with(self.class.withClause)
                        .update(self.class.qualifiedTableName)
                        .set(self.class.columnName, self.class.value)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"WITH testTable AS(SELECT testColumn FROM testTable) UPDATE testTable SET testColumn = 1 WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");
}

@end
