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

@interface WTCJoinConstraintTests : XCTestCase

@end

@implementation WTCJoinConstraintTests

- (void)testJoinConstraint
{
    std::string columnName1 = "testColumn1";
    std::string columnName2 = "testColumn2";
    WCDB::Expression expression = WCDB::Expression::ColumnNamed(columnName1);
    std::list<std::string> columnNames = {columnName1, columnName2};

    WINQAssertEqual(WCDB::JoinConstraint(), @"");

    WINQAssertEqual(WCDB::JoinConstraint().on(expression), @"ON testColumn1");

    WINQAssertEqual(WCDB::JoinConstraint().usingColumn(columnName1), @"USING(testColumn1)");

    WINQAssertEqual(WCDB::JoinConstraint().usingColumns(columnNames), @"USING(testColumn1, testColumn2)");
}

@end
