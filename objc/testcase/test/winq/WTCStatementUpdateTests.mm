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

using namespace WCDB;

@interface WTCStatementUpdateTests : WTCBaseTestCase

@end

@implementation WTCStatementUpdateTests

- (void)testStatementUpdate
{
    //Give
    std::string table1 = "table1";
    Column column1("column1");
    Column column2("column2");
    
    //Then
    XCTAssertEqual(StatementUpdate().getStatementType(), Statement::Type::Update);
    
    std::pair<const Column, const int> p = {column1, 1};
    StatementUpdate u = StatementUpdate().update(table1).set(p);
    WINQAssertEqual(StatementUpdate().update(table1).set(p), @"UPDATE table1 SET column1=1");
    
    WINQAssertEqual(StatementUpdate().update(table1, Conflict::Replace).set({{column1, 1}, {column2, "test"}}), @"UPDATE OR REPLACE table1 SET column1=1, column2='test'");
    
    WINQAssertEqual(StatementUpdate().update(table1).set({{column1, 1}, {column2, "test"}}), @"UPDATE table1 SET column1=1, column2='test'");
    
    WINQAssertEqual(StatementUpdate().update(table1).set({column1, 1}).where(column1 > 1), @"UPDATE table1 SET column1=1 WHERE (column1 > 1)");
    
    WINQAssertEqual(StatementUpdate().update(table1).set({column1, 1}).orderBy({column1, column2}), @"UPDATE table1 SET column1=1 ORDER BY column1, column2");
    
    WINQAssertEqual(StatementUpdate().update(table1).set({column1, 1}).limit(1), @"UPDATE table1 SET column1=1 LIMIT 1");
    
    WINQAssertEqual(StatementUpdate().update(table1).set({column1, 1}).limit( 1, 2), @"UPDATE table1 SET column1=1 LIMIT 1, 2");
    
    WINQAssertEqual(StatementUpdate().update(table1).set({column1, 1}).limit(1).offset(3), @"UPDATE table1 SET column1=1 LIMIT 1 OFFSET 3");
}

@end
