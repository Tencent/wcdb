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

@interface JoinClauseTests : WINQTestCase

@end

@implementation JoinClauseTests

- (void)testJoinClause
{
    WCDB::TableOrSubquery tableOrSubquery2("testTable2");
    WCDB::TableOrSubquery tableOrSubquery3("testTable3");
    WCDB::JoinConstraint joinConstraint = WCDB::JoinConstraint().usingColumn(self.class.column);

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery), @"testTable");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .with(tableOrSubquery2),
                    @"testTable, testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .with(tableOrSubquery2, joinConstraint),
                    @"testTable, testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .join(tableOrSubquery2),
                    @"testTable JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .join(tableOrSubquery2, joinConstraint),
                    @"testTable JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftJoin(tableOrSubquery2),
                    @"testTable LEFT JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftJoin(tableOrSubquery2, joinConstraint),
                    @"testTable LEFT JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftOuterJoin(tableOrSubquery2),
                    @"testTable LEFT OUTER JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftOuterJoin(tableOrSubquery2, joinConstraint),
                    @"testTable LEFT OUTER JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .innerJoin(tableOrSubquery2),
                    @"testTable INNER JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .innerJoin(tableOrSubquery2, joinConstraint),
                    @"testTable INNER JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .crossJoin(tableOrSubquery2),
                    @"testTable CROSS JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .crossJoin(tableOrSubquery2, joinConstraint),
                    @"testTable CROSS JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalJoin(tableOrSubquery2),
                    @"testTable NATURAL JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalJoin(tableOrSubquery2, joinConstraint),
                    @"testTable NATURAL JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftJoin(tableOrSubquery2),
                    @"testTable NATURAL LEFT JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftJoin(tableOrSubquery2, joinConstraint),
                    @"testTable NATURAL LEFT JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftOuterJoin(tableOrSubquery2),
                    @"testTable NATURAL LEFT OUTER JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftOuterJoin(tableOrSubquery2, joinConstraint),
                    @"testTable NATURAL LEFT OUTER JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalInnerJoin(tableOrSubquery2),
                    @"testTable NATURAL INNER JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalInnerJoin(tableOrSubquery2, joinConstraint),
                    @"testTable NATURAL INNER JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalCrossJoin(tableOrSubquery2),
                    @"testTable NATURAL CROSS JOIN testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalCrossJoin(tableOrSubquery2, joinConstraint),
                    @"testTable NATURAL CROSS JOIN testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .with(tableOrSubquery2)
                        .join(tableOrSubquery3, joinConstraint),
                    @"testTable, testTable2 JOIN testTable3 USING(testColumn)");
}

@end
