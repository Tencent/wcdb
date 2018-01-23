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

@interface WTCStatementDeleteTests : WTCBaseTestCase

@end

@implementation WTCStatementDeleteTests

- (WCDB::StatementDelete)generateStatementDelete
{
    std::string table1 = "table1";
    return WCDB::StatementDelete().deleteFrom(table1);
}

- (void)testStatementDelete
{
    //Give
    WCDB::Column column1("column1");
    WCDB::Column column2("column2");

    //Then
    XCTAssertEqual(WCDB::StatementDelete().getStatementType(), WCDB::Statement::Type::Delete);

    WINQAssertEqual([self generateStatementDelete].where(column1 > 1), @"DELETE FROM table1 WHERE (column1 > 1)");

    WINQAssertEqual([self generateStatementDelete].orderBy({column1, column2}), @"DELETE FROM table1 ORDER BY column1, column2");

    WINQAssertEqual([self generateStatementDelete].limit(1), @"DELETE FROM table1 LIMIT 1");

    WINQAssertEqual([self generateStatementDelete].limit(1, 2), @"DELETE FROM table1 LIMIT 1, 2");

    WINQAssertEqual([self generateStatementDelete].limit(1).offset(3), @"DELETE FROM table1 LIMIT 1 OFFSET 3");

    WINQAssertEqual([self generateStatementDelete].where(column1 > 1).orderBy({column1, column2}).limit(1).offset(2), @"DELETE FROM table1 WHERE (column1 > 1) ORDER BY column1, column2 LIMIT 1 OFFSET 2");
}

@end
