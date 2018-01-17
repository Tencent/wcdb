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
#import "WTCBaseTestCase.h"

@interface WTCJoinClauseTests : WTCBaseTestCase

@end

@implementation WTCJoinClauseTests

- (WCDB::JoinClause)generateJoinClause
{
    return WCDB::JoinClause("testJoinClauseTable");
}

- (void)testJoinClause
{
    WINQAssertEqual([self generateJoinClause].join("testJoinClauseTable2", WCDB::JoinClause::Type::Left), @"testJoinClauseTable LEFT JOIN testJoinClauseTable2");
    
    WINQAssertEqual([self generateJoinClause].naturalJoin("testJoinClauseTable2", WCDB::JoinClause::Type::LeftOuter), @"testJoinClauseTable NATURAL LEFT OUTER JOIN testJoinClauseTable2");
    
    WINQAssertEqual([self generateJoinClause].join("testJoinClauseTable2", WCDB::JoinClause::Type::Inner), @"testJoinClauseTable INNER JOIN testJoinClauseTable2");
    
    WINQAssertEqual([self generateJoinClause].naturalJoin("testJoinClauseTable2", WCDB::JoinClause::Type::Cross), @"testJoinClauseTable NATURAL CROSS JOIN testJoinClauseTable2");
    
    WINQAssertEqual([self generateJoinClause].on(1), @"testJoinClauseTable ON 1");
    
    WINQAssertEqual([self generateJoinClause].usingColumns({WCDB::Column("testColumn1"), WCDB::Column("testColumn2")}), @"testJoinClauseTable USING testColumn1, testColumn2");
}

@end
