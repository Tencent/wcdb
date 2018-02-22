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

@interface WTCStatementSelectTests : WTCWINQTestCase

@end

@implementation WTCStatementSelectTests

- (void)testStatementSelect
{
    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit, self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1, 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerms)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn, testColumn2 LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.groups)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn, testColumn2 HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquerys)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable, testTable2 WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.joinClause)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumns)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn, testColumn2 FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .distinct()
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT DISTINCT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .with(self.class.commonTableExpression)
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"WITH testTable AS(SELECT testColumn FROM testTable) SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .with(self.class.commonTableExpressions)
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"WITH testTable AS(SELECT testColumn FROM testTable), testTable2 AS(SELECT testColumn FROM testTable) SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .withRecursive(self.class.commonTableExpression)
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"WITH RECURSIVE testTable AS(SELECT testColumn FROM testTable) SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .withRecursive(self.class.commonTableExpressions)
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"WITH RECURSIVE testTable AS(SELECT testColumn FROM testTable), testTable2 AS(SELECT testColumn FROM testTable) SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL ORDER BY testColumn LIMIT 1 OFFSET 2");
}

- (void)testValues
{
    WINQAssertEqual(WCDB::StatementSelect()
                        .values(self.class.value),
                    @"VALUES(1)");

    WINQAssertEqual(WCDB::StatementSelect()
                        .values(self.class.values),
                    @"VALUES(1, 'testValue')");
}

- (void)testCompoundOperator
{
    WCDB::SelectCore selectCore = WCDB::SelectCore().select(self.class.resultColumn).from(self.class.tableOrSubquery);

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .union_(selectCore)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL UNION SELECT testColumn FROM testTable ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .unionAll(selectCore)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL UNION ALL SELECT testColumn FROM testTable ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .intersect(selectCore)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL INTERSECT SELECT testColumn FROM testTable ORDER BY testColumn LIMIT 1 OFFSET 2");

    WINQAssertEqual(WCDB::StatementSelect()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having)
                        .except(selectCore)
                        .orderBy(self.class.orderingTerm)
                        .limit(self.class.limit)
                        .offset(self.class.limitParameter),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL EXCEPT SELECT testColumn FROM testTable ORDER BY testColumn LIMIT 1 OFFSET 2");
}

@end
