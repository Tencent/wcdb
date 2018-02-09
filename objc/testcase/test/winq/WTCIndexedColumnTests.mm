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

@interface WTCIndexedColumnTests : XCTestCase

@end

@implementation WTCIndexedColumnTests

- (void)testIndexedColumn
{
    std::string columnName = "testColumn1";
    WCDB::Expression expression = WCDB::Expression::ColumnNamed("testColumn2");
    std::string collationName = "testCollation";

    WINQAssertEqual(WCDB::IndexedColumn(columnName), @"testColumn1");

    WINQAssertEqual(WCDB::IndexedColumn(columnName).withOrder(WCDB::Order::ASC), @"testColumn1 ASC");

    WINQAssertEqual(WCDB::IndexedColumn(columnName).withOrder(WCDB::Order::DESC), @"testColumn1 DESC");

    WINQAssertEqual(WCDB::IndexedColumn(columnName).withCollate(collationName), @"testColumn1 COLLATE testCollation");

    WINQAssertEqual(WCDB::IndexedColumn(expression), @"testColumn2");
}

@end
