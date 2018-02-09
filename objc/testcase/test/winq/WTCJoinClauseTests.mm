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

@interface WTCJoinClauseTests : XCTestCase

@end

@implementation WTCJoinClauseTests

- (void)testJoinClause
{
    WCDB::TableOrSubquery tableOrSubquery = "testTable";
    WCDB::TableOrSubquery tableOrSubquery1 = "testTable1";
    WCDB::JoinConstraint joinConstraint1 = WCDB::JoinConstraint().usingColumn("testColumn1");
    WCDB::TableOrSubquery tableOrSubquery2 = "testTable2";
    WCDB::JoinConstraint joinConstraint2 = WCDB::JoinConstraint().usingColumn("testColumn2");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery), @"testTable");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).with(tableOrSubquery1, joinConstraint1), @"testTable, testTable1 USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).join(tableOrSubquery1, joinConstraint1), @"testTable JOIN testTable1 USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).leftOuterJoin(tableOrSubquery1, joinConstraint1), @"testTable LEFT OUTER JOIN testTable1 USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).leftJoin(tableOrSubquery1, joinConstraint1), @"testTable LEFT JOIN testTable1 USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).innerJoin(tableOrSubquery1, joinConstraint1), @"testTable INNER JOIN testTable1 USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).crossJoin(tableOrSubquery1, joinConstraint1), @"testTable CROSS JOIN testTable1 USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).leftOuterJoin(tableOrSubquery1, joinConstraint1), @"testTable LEFT OUTER JOIN testTable1 USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinClause(tableOrSubquery).with(tableOrSubquery1, joinConstraint1).join(tableOrSubquery2, joinConstraint2), @"testTable, testTable1 USING(testColumn1) JOIN testTable2 USING(testColumn2)");
}

@end
