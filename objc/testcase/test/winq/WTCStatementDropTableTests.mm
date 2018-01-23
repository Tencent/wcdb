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

@interface WTCStatementDropTableTests : WTCBaseTestCase

@end

@implementation WTCStatementDropTableTests

- (void)testStatementDropTable
{
    std::string table1 = "table1";

    XCTAssertEqual(WCDB::StatementDropTable().getStatementType(), WCDB::Statement::Type::DropTable);

    WINQAssertEqual(WCDB::StatementDropTable().drop(table1), @"DROP TABLE IF EXISTS table1");

    WINQAssertEqual(WCDB::StatementDropTable().drop(table1, false), @"DROP TABLE table1");
}

@end
