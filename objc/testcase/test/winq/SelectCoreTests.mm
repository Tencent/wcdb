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

@interface SelectCoreTests : WINQTestCase

@end

@implementation SelectCoreTests

- (void)testSelectCore
{
    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.groups)
                        .having(self.class.having),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn, testColumn2 HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquery)
                        .groupBy(self.class.group)
                        .having(self.class.having),
                    @"SELECT testColumn FROM testTable GROUP BY testColumn HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .from(self.class.tableOrSubquerys)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having),
                    @"SELECT testColumn FROM testTable, testTable2 WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .from(self.class.joinClause)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having),
                    @"SELECT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having),
                    @"SELECT testColumn WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumns)
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having),
                    @"SELECT testColumn, testColumn2 FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .select(self.class.resultColumn)
                        .distinct()
                        .from(self.class.tableOrSubquery)
                        .where(self.class.condition)
                        .groupBy(self.class.group)
                        .having(self.class.having),
                    @"SELECT DISTINCT testColumn FROM testTable WHERE testColumn NOTNULL GROUP BY testColumn HAVING testColumn NOTNULL");

    WINQAssertEqual(WCDB::SelectCore()
                        .values(self.class.value),
                    @"VALUES(1)");

    WINQAssertEqual(WCDB::SelectCore()
                        .values(self.class.values),
                    @"VALUES(1, 'testValue')");
}

@end
