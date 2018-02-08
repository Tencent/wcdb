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

@interface WTCCTETableName : XCTestCase

@end

@implementation WTCCTETableName

- (void)testCTETableName
{
    std::string tableName = "testTable";
    std::string columnName = "testColumn1";
    std::list<std::string> columnNames = {columnName, "testColumn2"};

    WINQAssertEqual(WCDB::CTETableName(tableName).byAddingColumnName(columnName), @"testTable(testColumn1)");

    WINQAssertEqual(WCDB::CTETableName(tableName).byAddingColumnNames(columnNames), @"testTable(testColumn1, testColumn2)");

    WINQAssertEqual(WCDB::CTETableName(tableName), @"testTable");
}

@end
