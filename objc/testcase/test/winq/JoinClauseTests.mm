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

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery), @"main.testTable");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .with(tableOrSubquery2),
                    @"main.testTable, main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .with(tableOrSubquery2, joinConstraint),
                    @"main.testTable, main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .join(tableOrSubquery2),
                    @"main.testTable JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .join(tableOrSubquery2, joinConstraint),
                    @"main.testTable JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftJoin(tableOrSubquery2),
                    @"main.testTable LEFT JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable LEFT JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftOuterJoin(tableOrSubquery2),
                    @"main.testTable LEFT OUTER JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .leftOuterJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable LEFT OUTER JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .innerJoin(tableOrSubquery2),
                    @"main.testTable INNER JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .innerJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable INNER JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .crossJoin(tableOrSubquery2),
                    @"main.testTable CROSS JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .crossJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable CROSS JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalJoin(tableOrSubquery2),
                    @"main.testTable NATURAL JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable NATURAL JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftJoin(tableOrSubquery2),
                    @"main.testTable NATURAL LEFT JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable NATURAL LEFT JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftOuterJoin(tableOrSubquery2),
                    @"main.testTable NATURAL LEFT OUTER JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalLeftOuterJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable NATURAL LEFT OUTER JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalInnerJoin(tableOrSubquery2),
                    @"main.testTable NATURAL INNER JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalInnerJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable NATURAL INNER JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalCrossJoin(tableOrSubquery2),
                    @"main.testTable NATURAL CROSS JOIN main.testTable2");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .naturalCrossJoin(tableOrSubquery2, joinConstraint),
                    @"main.testTable NATURAL CROSS JOIN main.testTable2 USING(testColumn)");

    WINQAssertEqual(WCDB::JoinClause(self.class.tableOrSubquery)
                        .with(tableOrSubquery2)
                        .join(tableOrSubquery3, joinConstraint),
                    @"main.testTable, main.testTable2 JOIN main.testTable3 USING(testColumn)");
}

@end
