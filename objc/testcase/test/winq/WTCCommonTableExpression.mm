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

@interface WTCCommonTableExpression : XCTestCase

@end

@implementation WTCCommonTableExpression

- (void)testCommonTableExpression
{
    WCDB::ResultColumn resultColumn(WCDB::Expression::ColumnNamed("testColumn1"));
    WCDB::StatementSelect selectSTMT = WCDB::StatementSelect().select(resultColumn);
    std::string columnName = "testColumn2";
    std::list<std::string> columnNames = {columnName, "testColumn3"};
    std::string tableName = "testTable";

    WINQAssertEqual(WCDB::CommonTableExpression(tableName).byAddingColumn(columnName).as(selectSTMT), @"testTable(testColumn2) AS(SELECT testColumn1)");

    WINQAssertEqual(WCDB::CommonTableExpression(tableName).byAddingColumns(columnNames).as(selectSTMT), @"testTable(testColumn2, testColumn3) AS(SELECT testColumn1)");

    WINQAssertEqual(WCDB::CommonTableExpression(tableName).as(selectSTMT), @"testTable AS(SELECT testColumn1)");
}

@end
