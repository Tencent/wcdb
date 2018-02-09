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

@interface WTCQualifiedTableNameTests : XCTestCase

@end

@implementation WTCQualifiedTableNameTests

- (void)testQualifiedTableName
{
    std::string schemaName = "testSchema";
    std::string tableName = "testTable";
    std::string indexName = "testIndex";

    WINQAssertEqual(WCDB::QualifiedTableName(tableName), @"testTable");

    WINQAssertEqual(WCDB::QualifiedTableName(tableName).indexedBy(indexName), @"testTable INDEXED BY testIndex");

    WINQAssertEqual(WCDB::QualifiedTableName(tableName).notIndexed(), @"testTable NOT INDEXED");

    WINQAssertEqual(WCDB::QualifiedTableName(tableName).withSchema(schemaName), @"testSchema.testTable");
}

@end
