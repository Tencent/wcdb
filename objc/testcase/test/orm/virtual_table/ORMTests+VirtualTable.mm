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

#import "ORMTestCase.h"
#import "ORMVirtualTable+WCTTableCoding.h"
#import "ORMVirtualTable.h"
#import "ORMVirtualTableModuleFTS3+WCTTableCoding.h"
#import "ORMVirtualTableModuleFTS3.h"
#import "ORMVirtualTableTokenize+WCTTableCoding.h"
#import "ORMVirtualTableTokenize.h"
#import "ORMVirtualTableTokenizeWCDB+WCTTableCoding.h"
#import "ORMVirtualTableTokenizeWCDB.h"

@interface ORMTests_VirtualTable : ORMTestCase

@end

@implementation ORMTests_VirtualTable

- (void)setUp
{
    [super setUp];

    [_database setTokenizer:WCTTokenizer.name];

    NSString *expectedSQL = [NSString stringWithFormat:@"CREATE VIRTUAL TABLE IF NOT EXISTS %@ USING fts3(tokenize=WCDB, variable INTEGER)", _tableName];
    [_expectedSQLs addObject:expectedSQL];
}

- (void)test_virtual_table
{
    XCTAssertTrue([_database createVirtualTable:_tableName withClass:ORMVirtualTable.class]);
}

- (void)test_virtual_table_tokenize
{
    XCTAssertTrue([_database createVirtualTable:_tableName withClass:ORMVirtualTableTokenize.class]);
}

- (void)test_virtual_table_tokenize_WCDB
{
    XCTAssertTrue([_database createVirtualTable:_tableName withClass:ORMVirtualTableTokenizeWCDB.class]);
}

- (void)test_virtual_table_module_fts3
{
    XCTAssertTrue([_database createVirtualTable:_tableName withClass:ORMVirtualTableModuleFTS3.class]);
}

@end
